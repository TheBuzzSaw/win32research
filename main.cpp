#include "Window.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
    XPG::Window* window = new XPG::Window;

    window->DisplayVersion();
    cerr << "displayed version\n";
    window->Run();
    delete window;
    return 0;
}
