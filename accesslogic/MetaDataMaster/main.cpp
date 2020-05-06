#include <iostream>
#include "filesystem.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    FileSystem file(argv[1]);
    file.init();
    cout << "Hello world!" << endl;
    return 0;
}