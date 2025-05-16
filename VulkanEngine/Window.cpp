#include "Window.h"

Window::Window(const Properties& properties) : properties(properties)
{
}

Window::Extent Window::resize(const Extent& newExtent)
{ 
	if (properties.resizable)
	{
		properties.extent.height = newExtent.height;
		properties.extent.width = newExtent.width;
	}
	
	return properties.extent;
}
