#include <iostream>

#include "Revolver.h"
#include <Windows.h>

typedef int(__cdecl* TestFunction)();

struct Balls {
	int v = 5;
};

int main()
{
	std::cout << "App Started" << std::endl;

	Revolver revolver;
	bool success = revolver.AddModule("HotReload");
	revolver.LinkModuleFiles("HotReload", "c:/dev/cpp/revolver/testing/HotReload");

	if (!success)
	{
		std::cout << "NO LIBRARY FOUND!" << std::endl;
	}

	TestFunction test = revolver.GetFunction<TestFunction>("HotReload", "test");
	std::cout << test() << std::endl;
	
	while (true)
	{
		//TestFunction test = revolver.GetFunction<TestFunction>("HotReload", "test");
		//std::cout << test() << std::endl;
	}
}
