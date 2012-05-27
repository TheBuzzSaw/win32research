#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>

namespace XPG
{
    class Window
    {
        public:
            Window();
            virtual ~Window();

            void Run();
            void Update();

        protected:
        private:
            static LRESULT CALLBACK SetupCallback(HWND inWindowHandle,
                UINT inMessage, WPARAM inW, LPARAM inL);
            static LRESULT CALLBACK WndProc(HWND inWindowHandle, UINT inMessage,
                WPARAM inW, LPARAM inL);

            LRESULT CALLBACK WindowProcess(HWND inWindowHandle, UINT inMessage,
                WPARAM inW, LPARAM inL);

            LPTSTR mClassName;
            HINSTANCE mInstanceHandle;
            HWND mWindowHandle;
            HDC mDeviceContext;
    };
}

#endif
