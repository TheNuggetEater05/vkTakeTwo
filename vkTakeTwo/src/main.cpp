#include "je/core/je_app.hpp"

#include <iostream>

constexpr int WND_WIDTH = 1920 / 2;
constexpr int WND_HEIGHT = 1080 / 2;

int main(int argc, char** argv)
{
	je::JEApp jeApp{"vkTakeTwo", WND_WIDTH, WND_HEIGHT};

	try
	{
		jeApp.run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << "\n";
	}
	return 0;
}