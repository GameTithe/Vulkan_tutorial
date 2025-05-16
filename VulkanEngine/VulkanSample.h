
#include "Application.h"
#include "Instance.h"
#include "Common.h" 
 class VulkanSample : public Application
{
	 using Parent = Application;	
public:
	VulkanSample() = default;
	~VulkanSample() override;

	//using InstanceType = typename Instance;

	virtual bool prepare(const ApplicationOptions& options) override;

	 
	void finish() override;
	bool resize(uint32_t width, uint32_t height) override;

	uint32_t api_version = VK_API_VERSION_1_0; 
protected:
	virtual std::unique_ptr<Instance> createInstance();

private:

	std::unordered_map<const char*, bool> const& get_instance_extensions() const;

	std::unordered_map<const char*, bool> const& get_instance_layers() const;

	std::vector<VkLayerSettingEXT> const& get_layer_settings() const;


private:
	std::unordered_map<const char*, bool> instance_extensions;
	std::unordered_map<const char*, bool> instance_layers;
	std::vector<VkLayerSettingEXT> layer_settings;


};



