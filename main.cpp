#include "Window.h"
#include <iostream>
using namespace std;

// The Future:
// http://veelck.wordpress.com/2008/11/28/multithread-texture-loading-in-opengl/

int main(int argc, char** argv)
{
    XPG::Window window;

    window.DisplayVersion();
    cerr << "displayed version\n";
    window.Run();
    return 0;
}
