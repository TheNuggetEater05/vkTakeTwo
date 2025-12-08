#pragma once
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <memory>

#include "je_window.hpp"
#include "je_instance.hpp"
#include "je_device.hpp"

namespace je
{
	class JEApp
	{
	public:
		JEApp(const char* title, uint32_t width, uint32_t height);
		~JEApp();

		JEApp (const JEApp&) = delete;
		JEApp& operator= (const JEApp&) = delete;

		void run();
	private:
		// All classes made as unique_ptrs so that they manage their own memory
		std::unique_ptr<JEWindow> _jeWindow = nullptr;
		VkSurfaceKHR _jeSurface = nullptr;
		std::unique_ptr<JEInstance> _jeInstance = nullptr;
		std::unique_ptr<JEDevice> _jeDevice = nullptr;

		bool _shouldClose = false;

		void initializeSDL() const;
		void destroySDL();
	};
}