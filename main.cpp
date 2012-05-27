#include "Window.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
    XPG::Window w;
    cout << "Begin..." << endl;
    w.Run();
    cout << "Done!" << endl;
    return 0;
}
