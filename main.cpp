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

    Identical ident(opt);
    ident.scanOverlapFilesSize();
    std::cout << "Count overlap files size: " << ident.getCountOverlapFiles() << std::endl;
    
    ident.searchIdentical();

    
    
    return 0;
}