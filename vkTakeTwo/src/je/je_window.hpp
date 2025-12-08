#pragma once

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace je
{
	class JEWindow
	{
	public:
		JEWindow(const char* title, uint32_t w, uint32_t h);
		~JEWindow();

		JEWindow (const JEWindow&) = delete;
		JEWindow& operator= (const JEWindow&) = delete;

		SDL_Window* window() { return _window; };
		VkSurfaceKHR surface() { return _surface; };

		void setInstance(VkInstance instance) { _instance = instance; createSurface(); };
	private:
		SDL_Window* _window = nullptr;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;

		VkInstance _instance = VK_NULL_HANDLE;

		void createWindow(const char* title, uint32_t w, uint32_t h);
		void createSurface();
	};
}