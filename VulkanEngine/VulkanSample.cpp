#include "VulkanSample.h"

#include "Common.h"  


bool VulkanSample::prepare(const ApplicationOptions& options)
{
	if (!Parent::prepare(options))
	{
		return false;
	}
	LOGI("Initializing Vulkan sample");

	//TODO
	//�Ʒ� �����ϰ�, vulkanSample��ӹ��� ������Ʈ ����� ��,
	// �װɷ� app���۽�Ű�� �Ǵµ�
	//volkInit

	//createing vulkan instance

	//getting valid vulkan surface

	// Createing vulkan device
}

std::unique_ptr<Instance> VulkanSample::createInstance()
{
	return std::make_unique<Instance>(getName(), get_instance_extensions(), get_instance_layers(), get_layer_settings(), api_version);
}

std::unordered_map<const char*, bool> const& VulkanSample::get_instance_extensions() const
{
	return instance_extensions;
}

std::unordered_map<const char*, bool> const& VulkanSample::get_instance_layers() const
{

	return instance_layers;

}

 
std::vector<VkLayerSettingEXT> const& VulkanSample::get_layer_settings() const
{
	
	return reinterpret_cast<std::vector<VkLayerSettingEXT> const&>(layer_settings); 
}