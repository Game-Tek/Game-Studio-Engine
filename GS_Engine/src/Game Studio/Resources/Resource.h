#pragma once

#include "Core.h"

#include "Object.h"

#include "Containers/FString.h"
#include "Containers/Id.h"

using ResourceHeaderType = uint64;
using ResourceSegmentType = uint64;

template<typename T>
void SerializeFVector(OutStream& outStream, FVector<T>& vector)
{
	outStream.Write(vector.getLength());

	for (uint_64 i = 0; i < vector.getLength(); ++i)
	{
		outStream << vector[i];
	}
}

template<typename T>
void operator<<(OutStream& outStream, FVector<T>& vector)
{
	outStream.Write(vector.getLength());

	for (uint_64 i = 0; i < vector.getLength(); ++i)
	{
		outStream << vector[i];
	}
}

template<typename T>
void operator>>(InStream& inStream, FVector<T>& vector)
{
	uint_64 length = 0;

	inStream.Read(&length);

	vector.forceRealloc(length);
	vector.resize(length);

	for (uint_64 i = 0; i < length; ++i)
	{
		inStream >> vector[i];
	}
}

template<typename T>
void DeserializeFVector(InStream& inStream, FVector<T>& vector)
{
	uint_64 length = 0;
	
	inStream.Read(&length);

	vector.resize(length);
	
	for (uint_64 i = 0; i < length; ++i)
	{
		inStream >> vector[i];
	}
}

class ResourceData
{
	friend class Resource;

public:
	FString ResourceName;
	
	ResourceData() = default;

	virtual ~ResourceData()
	{
	}

	virtual void** WriteTo(size_t _Index, size_t _Bytes) = 0;

	virtual void Load(InStream& InStream_);
	virtual void Write(OutStream& OutStream_);
	
	const FString& GetResourceName() const { return ResourceName; }
};

struct LoadResourceData
{
	FString FullPath;
	class ResourceManager* Caller = nullptr;
};

/**
 * \brief Base class representation of all types of resources that can be loaded into the engine.
 */
class GS_API Resource : public Object
{
	friend class ResourceManager;
	
	uint16 References = 0;

	Id resourceName;
	
	void IncrementReferences() { ++References; }
	void DecrementReferences() { --References; }
	[[nodiscard]] uint16 GetReferenceCount() const { return References; }
	
	virtual bool LoadResource(const LoadResourceData& LRD_) = 0;
	virtual void LoadFallbackResource(const FString& _FullPath) = 0;
	
	//Must return the extension name for the extension type.
	[[nodiscard]] virtual const char* GetResourceTypeExtension() const = 0;
	
public:
	Resource() = default;

	virtual ~Resource() = default;
};

struct ResourceElementDescriptor
{
	uint64 Bytes = 0;
	//void* Data = nullptr;
};

struct SaveResourceElementDescriptor
{
	SaveResourceElementDescriptor(ResourceSegmentType _Bytes, void* _Data) : Bytes(_Bytes), Data(_Data)
	{
	}

	ResourceSegmentType Bytes = 0;
	void* Data = nullptr;
};