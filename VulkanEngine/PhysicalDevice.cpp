#include "PhysicalDevice.h"
#include <iostream>

PhysicalDevice::PhysicalDevice(Instance& instance, VkPhysicalDevice physicalDevice) : instance(instance), handle(physicalDevice)
{

	vkGetPhysicalDeviceFeatures(physicalDevice, &features);
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory_properties);

	std::cout << "Found GPU: " << properties.deviceName;
}

PhysicalDevice::~PhysicalDevice()
{
}
