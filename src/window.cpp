#include "common.h"
#include "application.h"
#include "string.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// The Window class
/////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////
// static ptr to the one and only Window

Window* Window::pWnd_;

/////////////////////////////////////////////////////////////////////////////////////////////
// Constructor:
// 1. Registers window class
// 2. Creates window
// 3. Shows window
// 4. saves 'this' to global variable for future reference
// 5. rolls the message loop

Window::Window(HINSTANCE hInstance, const char* appName, const char* windowTitle, Application* pApp)
{
    const int cxSize = 200;
    const int cySize = 100;

    pWnd_ = this;
        // save pointer to Window object
    pApp_ = pApp;
        // save pointer to Application object
    hInstance_ = hInstance;
    title_ = windowTitle;
    hrs_ = mins_ = secs_ = 0;

    WNDCLASSEX wndclass;
    wndclass.cbSize        = sizeof(wndclass);
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = (WNDPROC)WndProcStub;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;    // window belongs to this process
    wndclass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wndclass.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);
    wndclass.lpszClassName = appName;
    wndclass.hIconSm       = NULL;
    RegisterClassEx(&wndclass);
        // class registered

    CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW, appName, windowTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,    // window style
        CW_USEDEFAULT, CW_USEDEFAULT,
        cxSize, cySize, NULL, NULL, hInstance, NULL);
        // window created, handle saved in hwnd_


    // position in bottom-right corner, make always on top
    int cxPos = GetSystemMetrics(SM_CXFULLSCREEN) - cxSize - 150;
    int cyPos = GetSystemMetrics(SM_CYFULLSCREEN) - cySize - 140;
    SetWindowPos(hwnd_, HWND_TOPMOST, cxPos, cyPos, cxSize, cySize, SWP_SHOWWINDOW);
    // make the window transparent
    SetLayeredWindowAttributes(hwnd_, 0, 255, LWA_ALPHA);


    memset(&lf_, 0, sizeof(lf_));
    lf_.lfHeight = 50;
    lf_.lfQuality = PROOF_QUALITY;
    lf_.lfCharSet = ANSI_CHARSET;
    strcpy(lf_.lfFaceName, "Times New Roman");


    SetTimer(hwnd_, 1, 1000, NULL);

    MessageLoop();

}


/////////////////////////////////////////////////////////////////////////////////////////////
// Destructor

Window::~Window()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////
// The message pump

int Window::MessageLoop()
{
    HACCEL haccel = LoadAccelerators(hInstance_, MAKEINTRESOURCE(IDR_ACCELERATOR1));

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (0==TranslateAccelerator(hwnd_, haccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Window procedure stub -- just reroutes to the actual Window instance
// we do this because this is a static procedure

LRESULT Window::WndProcStub(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return pWnd_->WndProc(hwnd, message, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// The actual window procedure

LRESULT Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            OnCreate(hwnd);
            return 0;

        case WM_PAINT:
            OnPaint();
            return 0; 

        case WM_CHAR:
            OnChar((char)wParam);
            return 0;

        case WM_TIMER:
            OnTimer();
            return 0;

        case WM_LBUTTONDOWN:
            OnLButtonDown((short)LOWORD(lParam), (short)HIWORD(lParam));
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam)==ID_SELECT_TIMEOUT) SetTimeout();
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// WM_PAINT

void Window::OnPaint()
{
    PAINTSTRUCT ps;
    RECT rc;
    HDC hdc = BeginPaint(hwnd_, &ps);
    SelectObject(hdc, CreateFontIndirect(&lf_));
    SetBkMode(hdc, TRANSPARENT);

    char buf[50];
    wsprintf(buf, "Shutdown in %.2d:%.2d:%.2d", hrs_, mins_, secs_);
    GetClientRect(hwnd_, &rc);
    DrawText(hdc, buf+12, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    
    // window title
    if (hrs_ | mins_ | secs_)
        SetWindowText(hwnd_, buf);
    else
        SetWindowText(hwnd_, title_);

    // line under menu
    MoveToEx(hdc, rc.left+3, rc.top, NULL);
    LineTo(hdc, rc.right-3, rc.top);

    DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
    EndPaint (hwnd_, &ps);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// WM_CREATE

void Window::OnCreate(HWND hwnd)
{
    hwnd_ = hwnd;
    ::PostMessage(hwnd_, WM_COMMAND, ID_SELECT_TIMEOUT, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// WM_CHAR

void Window::OnChar(char c)
{
    if (c==27) DestroyWindow(hwnd_);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// WM_LBUTTONDOWN

void Window::OnLButtonDown(int x, int y)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// WM_TIMER

void Window::OnTimer()
{
    if (hrs_ | mins_ | secs_)
    {
        if (--secs_ < 0) {
            secs_ = 59;
            if (--mins_ < 0) {
                mins_ = 59;
                if (--hrs_ < 0) {
                    hrs_ = 0;
                }
            }
        }
        InvalidateRect(hwnd_, NULL, TRUE);

        if (hrs_ | mins_ | secs_) return;

        Suspend();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////
// sleep

void Window::Suspend()
{
    static bool privileges_adjusted = false;

    if (!privileges_adjusted) 
    {
        HANDLE hToken;
        BOOL success = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
        if (!success)
            return;

        TOKEN_PRIVILEGES tp;
        success = LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid);
        if (!success)
            return;

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        success = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);
        if (!success)
            return;

        privileges_adjusted = true;
    }

    SetSystemPowerState(TRUE, FALSE);

    // exit when power is restored
    PostQuitMessage(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// sleep

INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

void Window::SetTimeout()
{
    int result = (int) DialogBox(hInstance_, MAKEINTRESOURCE(IDD_TIMEOUT), hwnd_, DialogProc);
    if (result) {
        hrs_ = result / 60;
        mins_ = result % 60;
        secs_ = 0;
        InvalidateRect(hwnd_, NULL, TRUE);
    }
}
