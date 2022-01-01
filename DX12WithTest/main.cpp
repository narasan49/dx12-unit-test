#include "pch.h"
#include "../modules/core/core.h"

int main()
{
	develop::DebugPrint(L"Show window test.");

	auto app = module::Application::instance();
	app->initialize();
	app->run();
	app->finalize();
}
