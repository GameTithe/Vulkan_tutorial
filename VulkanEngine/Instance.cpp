#include "Instance.h"
 
#include <iostream> 
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>  


#include "Common.h"
#include "PhysicalDevice.h"


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger
)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
		"vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}

	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}


static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else 
const bool enableValidationLayers = true;
#endif

namespace
{
	bool enable_extension(const char* requested_extension,
		const std::vector<VkExtensionProperties>& available_extensions,
		std::vector<const char*>& enabled_extensions)
	{ 
		bool is_available =
			std::ranges::any_of(available_extensions,
				[&requested_extension](auto const& available_extension) { return strcmp(requested_extension, available_extension.extensionName) == 0; });
		
		if (is_available)
		{
			bool is_already_enabled =
				std::ranges::any_of(enabled_extensions,
					[&requested_extension](auto const& enabled_extension) { return strcmp(requested_extension, enabled_extension) == 0; });
			
			if (!is_already_enabled)
			{
				std::cout << "Extension {} available, enabling it" << requested_extension;
				enabled_extensions.emplace_back(requested_extension);
			}
		}
		else
		{
			std::cout << "Extension {} not available" << requested_extension;
		}

		return is_available;
	}


	bool enable_layer(const char* requested_layer,
		const std::vector<VkLayerProperties>& available_layers,
		std::vector<const char*>& enabled_layers)
	{
		bool is_available =
			std::ranges::any_of(available_layers,
				[&requested_layer](auto const& available_layer) { return strcmp(requested_layer, available_layer.layerName) == 0; });
		if (is_available)
		{
			bool is_already_enabled =
				std::ranges::any_of(enabled_layers,
					[&requested_layer](auto const& enabled_layer) { return strcmp(requested_layer, enabled_layer) == 0; });
			if (!is_already_enabled)
			{
				std::cout << "Layer {} available, enabling it" << requested_layer;
				enabled_layers.emplace_back(requested_layer);
			}
		}
		else
		{
			std::cout << "Layer {} not available" << requested_layer;
		}


		return is_available;
	}
}

Instance::Instance(const std::string& application_name,
	const std::unordered_map<const char*, bool>& requested_extenstions,
	const std::unordered_map<const char*, bool>& requested_layers,
	const std::vector<VkLayerSettingEXT>& required_layer_settings, uint32_t api_verison)

{ 
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	// InstanceExtension
	checkInstanceExtension();

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Vulkan";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	 
	auto extensions = getRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	//Debug
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	// VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); 
	if (vkCreateInstance(&createInfo, nullptr, &handle) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}


	setupDebugMessenger();

	queryGpus();
}
 

Instance::~Instance()
{
	if (enableValidationLayers)
	{
		if(debugCallback != VK_NULL_HANDLE)
			DestroyDebugUtilsMessengerEXT(handle, debugMessenger, nullptr);
	}

	if (handle != VK_NULL_HANDLE)
	{
		vkDestroyInstance(handle, nullptr);
	}
}

VkInstance Instance::getHandle() const
{
	return handle;
}

bool Instance::inEnabled(const char* extension) const
{
	//return std::ranges::find_if();
	return false;
}

bool Instance::checkValidationLayerSupport()
{ 
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);

	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerProperties.layerName, layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
			return false;
	}

		return true; 
}

std::vector<const char*> Instance::getRequiredExtensions()
{								   
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void Instance::setupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};

	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(handle, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void Instance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType =
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	createInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	createInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	createInfo.pfnUserCallback = debugCallback;
}

void Instance::checkInstanceExtension()
{
	uint32_t instanceExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);

	std::cout << "available extensions: \n";

	std::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions.data());

	for (const auto& extension : instanceExtensions)
	{
		std::cout << '\t' << extension.extensionName << std::endl;
	}
}

void Instance::queryGpus()
{	
	// Querying valid physical devices on the machine
	uint32_t physical_device_count{ 0 };
	vkEnumeratePhysicalDevices(handle, &physical_device_count, nullptr);

	if (physical_device_count < 1)
	{
		throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
	}

	std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
	vkEnumeratePhysicalDevices(handle, &physical_device_count, physical_devices.data());

	// Create gpus wrapper objects from the VkPhysicalDevice's
	for (auto& physical_device : physical_devices)
	{
		gpus.push_back(std::make_unique<PhysicalDevice>(*this, physical_device));
	}
}

//bool isDeviceSuitable(VkPhysicalDevice device)
//{
//	QueueFamilyIndices indices = findQueueFamilies(device);
//
//	bool extensionSupported = checkDeviceExtensionSupport(device);
//
//	//swapchain
//	bool swapChainAdequate = false;
//	if (extensionSupported)
//	{
//		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
//		swapChainAdequate = !swapChainSupport.formats.empty() &&
//			!swapChainSupport.presentModes.empty();
//	}
//
//	VkPhysicalDeviceFeatures supportedFeatures;
//	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
//
//	return indices.isComplete() && extensionSupported
//		&& swapChainAdequate && supportedFeatures.samplerAnisotropy;
//}