#pragma once
#include <cstdint>
#include <string>
#include <vector>

class Window
{
public:
	struct Extent
	{
		uint32_t width;
		uint32_t height;
	};

	struct Properties
	{
		std::string title = "";
		bool        resizable = true;
		Extent      extent = { 1280, 720 };

	};

	Window(const Properties& properties);
	virtual ~Window() = default;
	
	virtual std::vector<const char*> get_required_surface_extensions() const = 0;
	Extent resize(const Extent & newExtent);

protected:
		
	Properties properties;
	
};

