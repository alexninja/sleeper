#include "common.h"
class Application;

class Window
{
public:
    Window(HINSTANCE, const char*, const char*, Application*);
    ~Window();

private:
    int MessageLoop();
    LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
    static LRESULT WndProcStub(HWND, UINT, WPARAM, LPARAM);

private:
    static Window* pWnd_;
    Application* pApp_;
    HINSTANCE hInstance_;
    HWND hwnd_;
    const char* title_;
    int hrs_, mins_, secs_;
    LOGFONT lf_;
    HWND GetHWND() { return hwnd_; }

// properties
public:
    //__declspec(property(get=GetHWND)) HWND hwnd;

private:
    void Suspend();
    void SetTimeout();

// window methods
private:
    void OnCreate(HWND);
    void OnPaint();
    void OnChar(char);
    void OnLButtonDown(int, int);
    void OnTimer();

};