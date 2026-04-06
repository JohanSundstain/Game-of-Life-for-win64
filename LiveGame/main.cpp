#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

#include "UsersExceptions.h"
#include "Initializer.h"

int main(int32_t argc, char** argv)
{
    try
    {
        Initializer in(argc, argv);
        in.init();
    }
    catch (Error& e)
    {
        std::cerr << e.what() << std::endl;
    }
  
    return 0;
}