#include <iostream>
#include "options.h"
#include "identical.h"


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

    Identical test(opt);
    test.scanOverlapFilesSize();
    std::cout << "Count overlap files size: " << test.getCountOverlapFiles() << std::endl;
    
    test.searchIdentical();

    
    
    return 0;
}