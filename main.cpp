#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "options.h"



int main(int argc, char*argv[])
{
    Options opt;
    try
    {
        opt.parseCommandLine(argc, argv);
        opt.print();
    }
    catch (exitOptionsProgramm &e)
    {
        return 0;
    }

     const auto scan_patch = opt.getScanPatch();   
     boost::filesystem::path test(scan_patch[0]);
     std::cout << test.generic_path() << std::endl;
    return 0;
}
