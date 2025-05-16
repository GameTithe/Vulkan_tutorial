#pragma once
#include <cstdint>
#include <functional>

class Window;
 
struct ApplicationOptions
{
	Window* window{ nullptr };
};
class Application
{

public:
	Application() {};
	virtual ~Application() = default;

	virtual bool prepare(const ApplicationOptions& options);
	

	virtual void finish();
	virtual bool resize(const uint32_t width, const uint32_t height);
	 

	const std::string& getName() const;
	void setName(const std::string& name);

protected:
	Window* window{ nullptr };
private:
	std::string name{};
};

