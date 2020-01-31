#pragma once

#include "Core.h"

#include "RAPI/Vulkan/VulkanBase.h"

#include "VKDespcriptorSet.h"

MAKE_VK_HANDLE(VkDescriptorPool)

struct VkDescriptorPoolCreateInfo;

struct VKDescriptorPoolCreator final : VKObjectCreator<VkDescriptorPool>
{
	VKDescriptorPoolCreator(VKDevice* _Device, const VkDescriptorPoolCreateInfo* _VkDPCI);
};

struct VkDescriptorSetAllocateInfo;

class VKDescriptorPool final : public VKObject<VkDescriptorPool>
{
public:
	VKDescriptorPool(const VKDescriptorPoolCreator& _VKDPC) : VKObject<VkDescriptorPool>(_VKDPC)
	{
	}

	~VKDescriptorPool();

	void AllocateDescriptorSets(const VkDescriptorSetAllocateInfo* _VkDSAI, VkDescriptorSet* _DescriptorSets) const;
};
