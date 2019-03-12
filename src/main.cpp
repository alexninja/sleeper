#include "common.h"
#include "application.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int)
{
    Application theApp(hInstance, "Sleeper", "The sleep machine");
}
