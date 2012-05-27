#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>

namespace XPG
{
    class Window
    {
        public:
            Window();
            virtual ~Window();

            void DisplayVersion();
            void Run();
            void Update();
            inline void SwapBuffers() { ::SwapBuffers(mDeviceContext); }

        protected:
        private:
            static LRESULT CALLBACK SetupCallback(HWND inWindowHandle,
                UINT inMessage, WPARAM inW, LPARAM inL);
            static LRESULT CALLBACK WndProc(HWND inWindowHandle, UINT inMessage,
                WPARAM inW, LPARAM inL);

            LRESULT CALLBACK WindowProcess(HWND inWindowHandle, UINT inMessage,
                WPARAM inW, LPARAM inL);

            void SetupLegacyContext();
            void SetupCoreContext();
            void OnLoad();
            void OnResize();
            void OnResize(int inWidth, int inHeight);
            void OnPaint();

            LPTSTR mClassName;
            HINSTANCE mInstanceHandle;
            HWND mWindowHandle;
            HDC mDeviceContext;
            HGLRC mRenderContext;
    };
}

#endif
