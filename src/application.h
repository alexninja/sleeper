#include "window.h"
#pragma warning (disable:4355) // don't warn about passing 'this' in constructor

class Application
{
public:
    Application(HINSTANCE hInstance, const char* name, const char* title)
        : wnd(hInstance, name, title, this) {}

private:
    Window wnd;


public:
    // interaction methods here
};