#pragma once

#include <vulkan/vulkan.h>

namespace je
{
	class JEInstance
	{
	public:
		JEInstance();
		~JEInstance();

		JEInstance(const JEInstance&) = delete;
		JEInstance& operator= (const JEInstance&) = delete;

		VkInstance& instance() { return _instance; };

	private:
		VkInstance _instance;

		void createInstance();
	};
}