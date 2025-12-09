#include "je_window.hpp"

#include <stdexcept>

namespace je
{
	JEWindow::JEWindow(const char* title, uint32_t w, uint32_t h) : _width(w), _height(h)
	{
		createWindow(title, w, h);
	}

	JEWindow::~JEWindow()
	{
		SDL_Vulkan_DestroySurface(_instance, _surface, nullptr);
		SDL_DestroyWindow(_window);
	}

	void JEWindow::createWindow(const char* title, uint32_t w, uint32_t h)
	{
		_window = SDL_CreateWindow(title, w, h, SDL_WINDOW_VULKAN);
	}
	void JEWindow::createSurface()
	{
		if (!SDL_Vulkan_CreateSurface(_window, _instance, nullptr, &_surface))
			throw std::runtime_error("Failed to create surface");
	}
}