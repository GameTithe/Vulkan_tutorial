#pragma once
#include "Instance.h"
#include "map"
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>

class Instance;


class PhysicalDevice
{
public:
	PhysicalDevice(Instance& instance, VkPhysicalDevice physicalDevice);
	~PhysicalDevice();

	template<typename T>
	T getExtensionFeatures(VkStructureType type)
	{
		// We cannot request extension features if the physical device properties 2 instance extension isn't enabled
		if (!instance.inEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
		{
			throw std::runtime_error("Couldn't request feature from device as " + std::string(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) +
				" isn't enabled!");
		}

		// Get the extension features
		T features{ type };
		VkPhysicalDeviceFeatures2KHR physical_device_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR };
		physical_device_features.pNext = &features;
		vkGetPhysicalDeviceFeatures2(handle, &physical_device_features);
	}

	template <typename T>
	T& add_extension_features(VkStructureType type)
	{
		// We cannot request extension features if the physical device properties 2 instance extension isn't enabled
		if (!instance.inEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
		{
			throw std::runtime_error("Couldn't request feature from device as " + std::string(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) +
				" isn't enabled!");
		}

		// Add an (empty) extension features into the map of extension features
		auto [it, added] = extension_features.insert({ type, std::make_shared<T>(T{type}) });
		if (added)
		{
			// if it was actually added, also add it to the structure chain
			if (last_requested_extension_feature)
			{
				static_cast<T*>(it->second.get())->pNext = last_requested_extension_feature;
			}
			last_requested_extension_feature = it->second.get();
		}

		return *static_cast<T*>(it->second.get());
	}

	template <typename Feature>
	VkBool32 request_optional_feature(VkStructureType type, VkBool32 Feature::* flag, std::string const& featureName, std::string const& flagName)
	{
		VkBool32 supported = get_extension_features<Feature>(type).*flag;
		if (supported)
		{
			add_extension_features<Feature>(type).*flag = true;
		}
		else
		{
			std::cout << "Requested optional feature <{}::{}> is not supported " << featureName << flagName;
		}
		return supported;
	}

	template <typename Feature>
	void request_required_feature(VkStructureType type, VkBool32 Feature::* flag, std::string const& featureName, std::string const& flagName)
	{
		if (get_extension_features<Feature>(type).*flag)
		{
			add_extension_features<Feature>(type).*flag = true;
		}
		else
		{
			throw std::runtime_error(std::string("Requested required feature <") + featureName + "::" + flagName + "> is not supported");
		}
	}



private:
	 

	// Handle to the Vulkan physical device
	VkPhysicalDevice handle{ VK_NULL_HANDLE };
	Instance& instance;
	 
	// The features that this GPU supports
	VkPhysicalDeviceFeatures features{};

	// The extensions that this GPU supports
	std::vector<VkExtensionProperties> device_extensions;

	// The GPU properties
	VkPhysicalDeviceProperties properties;

	// The GPU memory properties
	VkPhysicalDeviceMemoryProperties memory_properties;

	// The GPU queue family properties
	std::vector<VkQueueFamilyProperties> queue_family_properties;

	// The features that will be requested to be enabled in the logical device
	VkPhysicalDeviceFeatures requested_features{};


	// The extension feature pointer
	void* last_requested_extension_feature{ nullptr };

	// Holds the extension feature structures, we use a map to retain an order of requested structures
	std::map<VkStructureType, std::shared_ptr<void>> extension_features;


	bool high_priority_graphics_queue{};

};

#define REQUEST_OPTIONAL_FEATURE(gpu, Feature, type, flag) gpu.request_optional_feature<Feature>(type, &Feature::flag, #Feature, #flag)
#define REQUEST_REQUIRED_FEATURE(gpu, Feature, type, flag) gpu.request_required_feature<Feature>(type, &Feature::flag, #Feature, #flag)







