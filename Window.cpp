#include "Window.h"
#include <iostream>
#include <cassert>

namespace XPG
{
    const char* const ClassName = "XPG";
    const char* const Title = "XPG Reborn";
    static Window* theWindow = NULL;
    LPCTSTR HelloString = TEXT("Hello, World!");

    VOID CALLBACK TimerProc(HWND inWindowHandle, UINT inMessage, UINT_PTR inId,
                            DWORD inTime)
    {
        std::cout << "TIMER" << std::endl;
        ShowWindow(inWindowHandle, SW_SHOWNORMAL);
        //KillTimer(inWindowHandle, inId);
    }

    Window::Window()
    {
        theWindow = this;
        mInstanceHandle = GetModuleHandle(NULL);

        WNDCLASSEX windowClass;
        memset(&windowClass, 0, sizeof(windowClass));
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        windowClass.lpfnWndProc = (WNDPROC)SetupCallback;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = sizeof(Window*);
        windowClass.hInstance = mInstanceHandle;
        windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = NULL;
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = ClassName;
        windowClass.hIconSm = NULL;

        if (!RegisterClassEx(&windowClass))
        {
            std::cerr << "error on RegisterClass\n";
            return;
        }

        DWORD dwExStyle = WS_EX_APPWINDOW;
        DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        dwExStyle |= WS_EX_WINDOWEDGE;
        dwStyle |= WS_OVERLAPPEDWINDOW;

        mWindowHandle = CreateWindowEx(dwExStyle, ClassName, Title,
                               dwStyle, CW_USEDEFAULT, 0, 640,
                               480, NULL, NULL, mInstanceHandle, this);

        if (!mWindowHandle)
        {
            std::cerr << "error on CreateWindowEx\n";
        }

        mDeviceContext = GetDC(mWindowHandle);

        ShowWindow(mWindowHandle, SW_SHOW);
        UpdateWindow(mWindowHandle);
    }

    Window::~Window()
    {
        if (mDeviceContext)
        {
            ReleaseDC(mWindowHandle, mDeviceContext);
        }

        if (mWindowHandle)
        {
            DestroyWindow(mWindowHandle);
        }

        UnregisterClass(ClassName, mInstanceHandle);
        ShowCursor(TRUE);
    }

    void Window::Run()
    {
        SetTimer(mWindowHandle, 0, 1000, TimerProc);

        BOOL result;
        MSG msg;

        while ((result = GetMessage(&msg, NULL, 0, 0)) != 0)
        {
            if (result != -1)
            {
                //TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    LRESULT CALLBACK Window::SetupCallback(HWND inWindowHandle, UINT inMessage,
        WPARAM inW, LPARAM inL)
    {
        std::cerr << "SetupProc\n";

        LRESULT outResult;

        if (inMessage == WM_NCCREATE)
        {
            LPCREATESTRUCT cs = (LPCREATESTRUCT)inL;
            SetWindowLongPtr(inWindowHandle, GWL_USERDATA,
                (long)cs->lpCreateParams);
            SetWindowLongPtr(inWindowHandle, GWL_WNDPROC, (long)WndProc);
            outResult = WndProc(inWindowHandle, inMessage, inW, inL);
        }
        else
        {
            outResult = DefWindowProc(inWindowHandle, inMessage, inW, inL);
        }

        return outResult;
    }

    LRESULT CALLBACK Window::WndProc(HWND inWindowHandle, UINT inMessage,
        WPARAM inW, LPARAM inL)
    {
        Window* window = (Window*)GetWindowLongPtr(inWindowHandle,
            GWL_USERDATA);

        assert(window != NULL);

        return window->WindowProcess(inWindowHandle, inMessage, inW, inL);
    }

    LRESULT CALLBACK Window::WindowProcess(HWND inWindowHandle, UINT inMessage,
        WPARAM inW, LPARAM inL)
    {
        switch (inMessage)
        {
        case WM_PAINT:
            std::cerr << "paint!\n";
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            break;
        }

        return DefWindowProc(inWindowHandle, inMessage, inW, inL);
    }
}
