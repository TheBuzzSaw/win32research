#include "Window.h"
#include <iostream>
#include <cassert>

namespace XPG
{
    const char* const ClassName = "XPG";
    const char* const Title = "XPG Reborn";

    VOID CALLBACK TimerProc(HWND inWindowHandle, UINT inMessage, UINT_PTR inId,
        DWORD inTime)
    {
        std::cout << "TIMER" << std::endl;
        ShowWindow(inWindowHandle, SW_SHOWNORMAL);
        //KillTimer(inWindowHandle, inId);
    }

    Window::Window()
    {
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

        mWindowHandle = CreateWindowEx(dwExStyle, ClassName, "XPG Reborn",
                               dwStyle, CW_USEDEFAULT, 0, 640,
                               480, NULL, NULL, mInstanceHandle, this);

        if (!mWindowHandle)
        {
            std::cerr << "error on CreateWindowEx\n";
        }

        SetupDeviceContext();
    }

    Window::~Window()
    {
        if (mRenderContext)
        {
            wglMakeCurrent(mDeviceContext, 0);
            wglDeleteContext(mRenderContext);
        }

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

        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);

        ShowWindow(mWindowHandle, SW_SHOW);
        UpdateWindow(mWindowHandle);

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
            OnPaint();
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            break;
        }

        return DefWindowProc(inWindowHandle, inMessage, inW, inL);
    }

    void Window::SetupDeviceContext()
    {
        mDeviceContext = GetDC(mWindowHandle);

        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL |
            PFD_DRAW_TO_WINDOW;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 32;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int nPixelFormat = ChoosePixelFormat(mDeviceContext, &pfd);
        if (nPixelFormat == 0)
        {
            std::cerr << "failed ChoosePixelFormat\n";
            return;
        }

        if (!SetPixelFormat(mDeviceContext, nPixelFormat, &pfd))
        {
            std::cerr << "failed SetPixelFormat\n";
            return;
        }

        mRenderContext = wglCreateContext(mDeviceContext);
        wglMakeCurrent(mDeviceContext, mRenderContext);
    }

    void Window::OnPaint()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SwapBuffers();
    }
}
