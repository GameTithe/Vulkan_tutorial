#include "Application.h"
#include <cassert>
#include "Common.h"

bool Application::prepare(const ApplicationOptions& options)
{
	assert(options.window != nullptr && "Window must be valid");
	
	//TODO 
	//debug info

	window = options.window;
	return true;
}

void Application::finish()
{
}

bool Application::resize(const uint32_t width, const uint32_t height)
{
	return true;
}

const std::string& Application::getName() const
{
	return name;
}

void Application::setName(const std::string& name_)
{
	name = name_;
}
