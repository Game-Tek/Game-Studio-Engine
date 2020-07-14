#pragma once
#include "ByteEngine/Game/System.h"
#include "ByteEngine/Game/GameInstance.h"

#include <GTSL/DataSizes.h>
#include "RenderTypes.h"

namespace GTSL {
	class Window;
}

class RenderSystem : public System
{
public:
	RenderSystem() = default;

	const char* GetName() const override { return "RenderSystem"; }

	void Initialize(const InitializeInfo& initializeInfo) override;
	void Shutdown() override;
	
	struct InitializeRendererInfo
	{
		GTSL::Window* Window{ 0 };
	};
	void InitializeRenderer(const InitializeRendererInfo& initializeRenderer);
	
	void UpdateWindow(GTSL::Window& window);

	struct ScratchMemoryAllocationInfo
	{
		DeviceMemory* DeviceMemory = nullptr;
		uint32 Size = 0;
		uint32 MemoryType = 0;
		uint32* Offset = nullptr;
		void** Data = nullptr;
	};
	void AllocateScratchMemory(ScratchMemoryAllocationInfo& memoryAllocationInfo);
	
	RenderDevice* GetRenderDevice() { return &renderDevice; }
	CommandBuffer* GetTransferCommandBuffer() { return &transferCommandBuffers[index]; }
	
private:
	RenderDevice renderDevice;
	Surface surface;
	RenderContext renderContext;
	
	GTSL::Extent2D renderArea;
	
	GTSL::Vector<GTSL::Id64> renderGroups;

	RenderPass renderPass;
	GTSL::Array<ImageView, 3> swapchainImages;
	GTSL::Array<Semaphore, 3> imageAvailableSemaphore;
	GTSL::Array<Semaphore, 3> renderFinishedSemaphore;
	GTSL::Array<Fence, 3> inFlightFences;
	
	GTSL::Array<CommandBuffer, 3> commandBuffers;
	GTSL::Array<CommandPool, 3> commandPools;
	
	GTSL::Array<CommandPool, 3> transferCommandPools;
	GTSL::Array<CommandBuffer, 3> transferCommandBuffers;
	
	GTSL::Array<FrameBuffer, 3> frameBuffers;

	GTSL::Array<GTSL::RGBA, 3> clearValues;

	Queue graphicsQueue;

	uint8 index = 0;

	uint32 swapchainPresentMode{ 0 };
	uint32 swapchainFormat{ 0 };
	uint32 swapchainColorSpace{ 0 };
	
	void render(const GameInstance::TaskInfo& taskInfo);

	void printError(const char* message, RenderDevice::MessageSeverity messageSeverity) const;

	struct MakeScratchAllocationInfo
	{
		DeviceMemory* DeviceMemory = nullptr;
		uint32 Size = 0;
		uint32* Offset = nullptr;
		void** Data = nullptr;
	};
	void allocateScratchMemoryBlock();
	
	struct LocalMemoryBlock
	{
		static constexpr GTSL::Byte ALLOCATION_SIZE{ GTSL::MegaByte(128) };

		void Free(const RenderDevice& renderDevice, const GTSL::AllocatorReference& allocatorReference);

		bool TryAllocate(const MakeScratchAllocationInfo& makeAllocationInfo) const;

		DeviceMemory DeviceMemory;

		struct Allocation
		{
			uint8 DeviceAllocationIndex = 0;
			uint32 Size = 0;
			uint32 Offset = 0;
		};
		GTSL::Vector<Allocation> allocations;

		uint32 UnusableSize = 0;
	};
	GTSL::Array<LocalMemoryBlock, 32> localMemoryBlocks;

	struct ScratchMemoryBlock
	{
		static constexpr GTSL::Byte ALLOCATION_SIZE{ GTSL::MegaByte(128) };
		
		ScratchMemoryBlock() = default;
		
		void AllocateDeviceMemory(const RenderDevice& renderDevice, const GTSL::AllocatorReference& allocatorReference);
		void Free(const RenderDevice& renderDevice, const GTSL::AllocatorReference& allocatorReference);

		bool TryAllocate(const MakeScratchAllocationInfo& makeAllocationInfo) const;
		void Allocate(MakeScratchAllocationInfo& makeAllocationInfo, const GTSL::AllocatorReference& allocatorReference);

	private:
		DeviceMemory deviceMemory;
		void* mappedMemory = nullptr;

		struct Allocation
		{
			uint32 Size = 0;
			uint32 Offset = 0;
			bool InUse = true;
		};
		GTSL::Vector<Allocation> allocations;

		uint32 UnusableSize = 0;
	};
	GTSL::Array<ScratchMemoryBlock, 32> scratchMemoryBlocks;
};
