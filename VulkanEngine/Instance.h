#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

class PhysicalDevice;

class Instance
{
public:

	Instance();

	~Instance();
	VkInstance getHandle() const;

private:
	VkInstance handle{ VK_NULL_HANDLE };
	VkDebugUtilsMessengerEXT debugMessenger{ VK_NULL_HANDLE };


	bool checkValidationLayerSupport();
	 
	std::vector<const char*> getRequiredExtensions();
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};

