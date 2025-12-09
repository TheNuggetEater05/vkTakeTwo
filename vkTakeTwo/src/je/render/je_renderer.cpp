#include "je_renderer.hpp"

namespace je
{
	JERenderer::JERenderer(JEDevice& device, JEWindow& window) : _jeDevice(device), _jeWindow(window)
	{
		_jeSwapchain = std::make_unique<JESwapchain>(device, window);
	}

	JERenderer::~JERenderer()
	{

	}


}