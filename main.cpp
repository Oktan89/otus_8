#include <iostream>
#include <boost/program_options.hpp>
#include "options.h"



int main(int argc, char*argv[])
{
    Options opt;
    try
    {
        opt.parseCommandLine(argc, argv);
    }
    catch (std::exception &e)
    {
        opt.printHelp();
        return 0;
    }

    opt.print();
    return 0;
}
