#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>

class PhysicalDevice;

class Instance
{
public: 
	//Instance();
	Instance(const std::string& application_name,
			const std::unordered_map<const char*, bool> &requested_extenstions,
			const std::unordered_map<const char*, bool> &requested_layers,
			const std::vector<VkLayerSettingEXT>		&required_layer_settings,
			uint32_t									api_verison);
	~Instance();
	VkInstance getHandle() const;

	bool inEnabled(const char* extension) const;
private:
	VkInstance handle{ VK_NULL_HANDLE };
	VkDebugUtilsMessengerEXT debugMessenger{ VK_NULL_HANDLE };


	bool checkValidationLayerSupport();
	 
	std::vector<const char*> getRequiredExtensions();
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void checkInstanceExtension();
	
	void queryGpus();
	//void isDeviceSuitable();

	std::vector<std::unique_ptr<PhysicalDevice>> gpus;

};

