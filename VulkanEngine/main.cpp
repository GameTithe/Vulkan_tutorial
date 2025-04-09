//#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <map>
#include <optional>
#include <set>


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
 
//validation
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else 
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
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


void DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


class HelloTringleApplication
{
public:
	void run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	// GLFW window
	GLFWwindow* window;

	//vulkan instance
	VkInstance instance;

	//debug callback messanger
	VkDebugUtilsMessengerEXT debugMessenger;

	//physical device
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	 
	//queue family
	std::optional<uint32_t> graphicsFamily;

	//logical device
	VkDevice device;

	//Queue
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	//surface
	VkSurfaceKHR surface;


	bool checkValidationLayerSupport()
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
	void initWindow() {

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

	void initVulkan() {
		createInstance();
		setupDebugMessenger();

		createSurface();

		pickPhysicalDevice();
		createLogicalDevice(); 
	}
	 
	//surface
	void createSurface()
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}

	//logical device
	void createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		
		//queue info
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies =
		{ indices.graphicsFamily.value(), indices.presentFamily.value() };
		 
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1; 
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}
		
		//features
		VkPhysicalDeviceFeatures deviceFeatures{};

		//create logical device
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures; 
		createInfo.enabledExtensionCount = 0;

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}

		else
		{
			createInfo.enabledLayerCount = 0;
		}
		

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &graphicsQueue);

	}

	//queue family
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;
		
		//Logic to find graphics queue family indices to populate struct with
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
				
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

				if (presentSupport)
				{
					indices.presentFamily = i;
				}
			} 

			if (indices.isComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	} 

	//physical device
	void pickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (isDeviceSuitable(device))
			{
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	bool isDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = findQueueFamilies(device);

		return indices.isComplete();
	}

	void mainLoop()
	{
		/*while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}*/
		
	}

	void cleanup()
	{
		if (enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);

		vkDestroyInstance(instance, nullptr);
		
		glfwDestroyWindow(window);
		glfwTerminate(); 
	} 

	void createInstance()
	{
		if (enableValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		 

		std::vector<const char*> glfwExtensions = getRequiredExtensions();//  glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size() );
		createInfo.ppEnabledExtensionNames = glfwExtensions.data();

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}


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
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
		
		// InstanceExtension
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

	std::vector<const char*> getRequiredExtensions()
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
	
	// Debug
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
	 
	void setupDebugMessenger()
	{
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};

		populateDebugMessengerCreateInfo(createInfo);
		 
		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}
	
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
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

};

int main()
{
	HelloTringleApplication app;
	
	try
	{
		app.run();
	}

	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;

		return EXIT_FAILURE;
	} 

	return EXIT_SUCCESS;
}