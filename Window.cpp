#include "Window.h"
#include <iostream>
#include <cassert>

namespace XPG
{
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms633570(v=vs.85).aspx#designing_proc

    const char* const ClassName = "XPG";
    const char* const Title = "XPG Reborn";

    Window::Window()
    {
        mRotation = 0.0f;

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

        mDeviceContext = GetDC(mWindowHandle);
        SetupContext();
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
            if (!DestroyWindow(mWindowHandle))
            {
                std::cerr << "error on DestroyWindow\n";
            }
        }

        UnregisterClass(ClassName, mInstanceHandle);
        ShowCursor(TRUE);
    }

    void Window::DisplayVersion()
    {
        const char* version = (const char*)glGetString(GL_VERSION);
        std::cerr << "Version: " << version << std::endl;
    }

    void Window::Run()
    {
        OnLoad();
        OnResize();

        ShowWindow(mWindowHandle, SW_SHOW);
        UpdateWindow(mWindowHandle);

        BOOL result;
        MSG msg;

        while ((result = GetMessage(&msg, NULL, 0, 0)) != 0)
        {
            if (result == -1)
            {
                std::cerr << "error on GetMessage\n";
                break;
            }
            else
            {
                //TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        if (msg.message == WM_QUIT)
            std::cerr << "WM_QUIT\n";

        ShowWindow(mWindowHandle, SW_HIDE);
        //Sleep(1000);
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
        case WM_CREATE:
            std::cerr << "WM_CREATE\n";
            break;

        case WM_CLOSE:
            std::cerr << "WM_CLOSE\n";
            PostQuitMessage(0);
            return 0;
            break;

        case WM_SIZE:
            OnResize();
            break;

        case WM_PAINT:
            OnPaint();
            break;

        case WM_DESTROY:
            std::cerr << "WM_DESTROY\n";
            //PostQuitMessage(0);
            break;

        case WM_TIMER:
            //std::cerr << "TIMER\n";
            mRotation += 0.5f;
            if (mRotation > 180.0f) mRotation -= 360.0f;
            OnPaint();
            break;

        default:
            break;
        }

        return DefWindowProc(inWindowHandle, inMessage, inW, inL);
    }

    void Window::SetupContext()
    {
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

        GLenum e = glewInit();

        if (e == GLEW_OK)
        {
            int attributes[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 1,
                0, 0
                };

            if (wglewIsSupported("WGL_ARB_create_context") == 1)
            {
                HGLRC rc = wglCreateContextAttribsARB(mDeviceContext, NULL,
                    attributes);

                if (rc)
                {
                    wglMakeCurrent(NULL, NULL);
                    wglDeleteContext(mRenderContext);
                    mRenderContext = rc;
                    wglMakeCurrent(mDeviceContext, mRenderContext);
                }
            }
        }
    }

    void Window::OnLoad()
    {
        SetTimer(mWindowHandle, 0, 25, NULL);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    }

    void Window::OnResize()
    {
        RECT r;
        GetWindowRect(mWindowHandle, &r);

        int width = r.right - r.left;
        int height = r.bottom - r.top;

        OnResize(width, height);
    }

    void Window::OnResize(int inWidth, int inHeight)
    {
        glViewport(0, 0, inWidth, inHeight);
        glMatrixMode(GL_PROJECTION);

        float ratio = float(inWidth) / float(inHeight);
        const float range = 2.0f;

        glLoadIdentity();
        glOrtho(-range * ratio, range * ratio, -range, range, -range, range);

        glMatrixMode(GL_MODELVIEW);
    }

    void Window::OnPaint()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();
        glRotatef(mRotation, 0.0f, 0.0f, 1.0f);

        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex2f(0.0f, 1.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
        glEnd();

        SwapBuffers();
    }
}
