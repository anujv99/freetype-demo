
#include <application.h>

using namespace engine;

#include "ftinspect.h"

int main() {
	Application::CreateInst();

	Application::Ref().PushLayer(new FTInspect());
	
	Application::Ref().Run();
	
	Application::Ref().DestroyInst();
}
