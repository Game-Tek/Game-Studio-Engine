#include "AnimationResourceManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.hpp>
#include <assimp/postprocess.h>
#include <GTSL/Buffer.hpp>
#include <GTSL/DataSizes.h>
#include <GTSL/Filesystem.h>

static GTSL::Matrix4 assimpMatrixToMatrix(const aiMatrix4x4 assimpMatrix)
{
	return GTSL::Matrix4(
		assimpMatrix.a1, assimpMatrix.a2, assimpMatrix.a3, assimpMatrix.a4,
		assimpMatrix.b1, assimpMatrix.b2, assimpMatrix.b3, assimpMatrix.b4,
		assimpMatrix.c1, assimpMatrix.c2, assimpMatrix.c3, assimpMatrix.c4,
		assimpMatrix.d1, assimpMatrix.d2, assimpMatrix.d3, assimpMatrix.d4
	);
}

static Id assimpStringToId(const aiString& aiString) {
	return Id(GTSL::Range<const utf8*>(aiString.length + 1, aiString.data));
}

static GTSL::Vector3 aiVector3DToVector(const aiVector3D assimpVector) {
	return GTSL::Vector3(assimpVector.x, assimpVector.y, assimpVector.z);
}

static GTSL::Quaternion aiQuaternionToQuaternion(const aiQuaternion assimpQuaternion) {
	return GTSL::Quaternion(assimpQuaternion.x, assimpQuaternion.y, assimpQuaternion.z, assimpQuaternion.w);
}

AnimationResourceManager::AnimationResourceManager(): ResourceManager("AnimationResourceManager")
{
	initializePackageFiles(GetResourcePath(GTSL::StaticString<32>("Animations"), GTSL::ShortString<32>("bepkg")));

	auto aa = GetResourcePath(GTSL::StaticString<64>("*.fbx"));
	
	GTSL::FileQuery fileQuery(aa);
	
	while (fileQuery.DoQuery()) {
		GTSL::File animationFile; animationFile.Open(GetResourcePath(fileQuery.GetFileNameWithExtension()), GTSL::File::AccessMode::READ);
		GTSL::Buffer<BE::TAR> buffer(animationFile.GetSize(), 16, GetTransientAllocator());
		animationFile.Read(buffer.GetBufferInterface());

		SkeletonData skeletonData;
		AnimationData animationData;
		
		GTSL::Buffer<BE::TAR> skeletonDataBuffer; skeletonDataBuffer.Allocate(GTSL::Byte(GTSL::KiloByte(8)), 16, GetTransientAllocator());
		GTSL::Buffer<BE::TAR> animationDataBuffer; skeletonDataBuffer.Allocate(GTSL::Byte(GTSL::KiloByte(8)), 16, GetTransientAllocator());
		
		loadSkeleton(buffer, skeletonData, skeletonDataBuffer);
		loadAnimation(buffer, animationData, animationDataBuffer);
	}
}

void AnimationResourceManager::loadSkeleton(const GTSL::Range<const byte*> sourceBuffer, SkeletonData& skeletonData,
                                            GTSL::Buffer<BE::TAR>& meshDataBuffer)
{
	Assimp::Importer importer;
	const auto* const scene = importer.ReadFileFromMemory(sourceBuffer.begin(), sourceBuffer.Bytes(),
		aiProcess_Triangulate | aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices, "fbx");

	if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
	{
		BE_LOG_ERROR(importer.GetErrorString())
		return;
	}

	aiMesh* mesh = scene->mMeshes[0];

	skeletonData.Bones.Initialize(mesh->mNumBones, GetPersistentAllocator());
	skeletonData.BonesMap.Initialize(mesh->mNumBones, GetPersistentAllocator());
	
	for (uint32 b = 0; b < mesh->mNumBones; ++b)
	{
		const auto& assimpBone = mesh->mBones[b];
		
		skeletonData.BonesMap.Emplace(assimpStringToId(assimpBone->mName), b);
		auto& bone = skeletonData.Bones.EmplaceBack();

		for (uint32 w = 0; w < assimpBone->mNumWeights; ++w) {
			bone.AffectedBone[w] = assimpBone->mWeights[w].mVertexId;
			bone.EffectIntensity[w] = assimpBone->mWeights[w].mWeight;
		}

		bone.Offset = assimpMatrixToMatrix(mesh->mBones[b]->mOffsetMatrix);
	}
}

void AnimationResourceManager::loadAnimation(const GTSL::Range<const byte*> sourceBuffer, AnimationData& animationData,
	GTSL::Buffer<BE::TAR>& meshDataBuffer)
{
	Assimp::Importer importer;
	const auto* const scene = importer.ReadFileFromMemory(sourceBuffer.begin(), sourceBuffer.Bytes(),
		aiProcess_Triangulate | aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices);

	if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) { return; }

	aiMesh* mesh = scene->mMeshes[0];

	animationData.Frames.Initialize(static_cast<uint32>(scene->mAnimations[0]->mDuration), GetPersistentAllocator());

	GTSL::Vector<Id, BE::TAR> boneNames(128, GetTransientAllocator());

	for (uint32 b = 0; b < mesh->mNumBones; ++b)
	{
		const auto& assimpBone = mesh->mBones[b];
		auto name = assimpStringToId(assimpBone->mName);
		boneNames.EmplaceBack(name);
	}
	
	for (uint32 a = 0; a < scene->mNumAnimations; ++a)
	{
		auto& assimpAnimation = scene->mAnimations[a]; //a single animation, e.g. "walk", "run", "shoot"
		auto animationName = assimpStringToId(assimpAnimation->mName);

		AnimationData animation;

		BE_ASSERT(assimpAnimation->mDuration - ((float64)(uint64)assimpAnimation->mDuration) != 0.0,
			"Animation is not round number");
		animation.FrameCount = static_cast<uint64>(assimpAnimation->mDuration);

		animation.FPS = static_cast<float32>(assimpAnimation->mTicksPerSecond == 0.0
			? 30.0
			: assimpAnimation->mTicksPerSecond);

		for (uint32 c = 0; c < assimpAnimation->mNumChannels; ++c) {
			BE_ASSERT(assimpStringToId(assimpAnimation->mChannels[c]->mNodeName) == boneNames[c], "Order doesn't match");
		}

		for (uint32 frameIndex = 0; frameIndex < animation.FrameCount; ++frameIndex)
		{
			auto& frame = animation.Frames.EmplaceBack();
			frame.Bones.Initialize(assimpAnimation->mNumChannels, GetPersistentAllocator());
			
			for (uint32 b = 0; b < assimpAnimation->mNumChannels; ++b)
			{
				const auto& assimpChannel = assimpAnimation->mChannels[b];

				auto nodeName = assimpStringToId(assimpChannel->mNodeName);

				if (assimpChannel->mNumPositionKeys != assimpChannel->mNumRotationKeys != assimpChannel->mNumScalingKeys)
				{
					BE_LOG_WARNING("Number of keys doesn't match");
				}

				auto& bone = frame.Bones.EmplaceBack();

				BE_ASSERT(
					assimpChannel->mPositionKeys[frameIndex].mTime ==
					assimpChannel->mRotationKeys[frameIndex].mTime ==
					assimpChannel->mScalingKeys[frameIndex].mTime, "Time doesn't match");

				bone.Position = aiVector3DToVector(assimpChannel->mPositionKeys[frameIndex].mValue);
				bone.Rotation = aiQuaternionToQuaternion(assimpChannel->mRotationKeys[frameIndex].mValue);
				bone.Scale = aiVector3DToVector(assimpChannel->mScalingKeys[frameIndex].mValue);
			}
		}
	}
}
