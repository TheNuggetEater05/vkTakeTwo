#include "je_app.hpp"

#include <stdexcept>
#include <cstdio>

namespace je
{
	// Initialize EVERYTHING. JeAPP is the king of hell (vulkan) :)
	JEApp::JEApp(const char* title, uint32_t width, uint32_t height)
	{
		initializeSDL();
		_jeWindow = std::make_unique<JEWindow>(title, width, height);
		_jeInstance = std::make_unique<JEInstance>();

		// Give some instance to window so we can create our surface
		_jeWindow->setInstance(_jeInstance->instance());
		_jeSurface = _jeWindow->surface();

		// Gotta put this guy down here cus he needs the instance too
		_jeDevice = std::make_unique<JEDevice>(_jeInstance->instance(), _jeWindow->surface());

		// NOW WE'RE GETTING TO THE MEAT AND POTATOES BABY!!!
		// Create our renderer and hand it all of our lovely vulkan objects
		_jeRenderer = std::make_unique<JERenderer>(*_jeDevice, *_jeWindow);
	}

	JEApp::~JEApp()
	{
		destroySDL();
	}

	// The powerhouse of the cell
	void JEApp::run()
	{
		SDL_Event e;

		while (_shouldClose == false)
		{
			while (SDL_PollEvent(&e))
			{
				if (e.type == SDL_EVENT_QUIT)
					_shouldClose = true;
			}
		}
	}

	// Prays to the SDL gods to initialize, then bitches if it doesn't work
	void JEApp::initializeSDL() const
	{
		if (SDL_Init(SDL_INIT_VIDEO) != false)
			printf(SDL_GetError());

		if (!SDL_Vulkan_LoadLibrary(0))
			throw std::runtime_error("Failed to load vulkan");
	}

	void JEApp::destroySDL()
	{
		SDL_DestroyWindow(_jeWindow->window());
		SDL_Vulkan_UnloadLibrary();
		SDL_Quit();
	}
}