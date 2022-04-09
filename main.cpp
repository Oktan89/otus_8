#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "options.h"

namespace fs = boost::filesystem;

struct HumanReadable {
    std::uintmax_t size{};
  private: friend
    std::ostream& operator<<(std::ostream& os, HumanReadable hr) {
        int i{};
        double mantissa = hr.size;
        for (; mantissa >= 1024.; mantissa /= 1024., ++i) { }
        mantissa = std::ceil(mantissa * 10.) / 10.;
        os << mantissa << "BKMGTPE"[i];
        return i == 0 ? os : os << "B (" << hr.size << ')';
    }
};

void recursive_dir(const fs::path& path, const std::set<boost::filesystem::path>& epath, bool level, std::size_t file_size);

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

    auto scan_path = opt.getScanPath();
    auto excl_path = opt.getExcludePath();

    for(auto &path : scan_path)   
    {
        if(fs::exists(path))
        {
            recursive_dir(path, excl_path, opt.level(), opt.getFileSize());
        }
        else
        {
            std::cout << "path error " << path << std::endl;
        }
    }
        
   
    
    return 0;
}

//Рекурсивно обходим все дириктории и выводим содержащиеся в них файлы с уровнем 0 (все директории)
void recursive_dir(const fs::path &path, const std::set<boost::filesystem::path>& epath, bool level, std::uintmax_t file_size)
{
    if (fs::is_directory(path))
    {
        // std::cout << path << " is directory" << std::endl;
        for (auto &dir : fs::directory_iterator(path))
            if (fs::is_regular_file(dir.path()))
            {
                if(file_size <= fs::file_size(dir.path()))
                    std::cout << dir.path() << " \tsize file----> :" << HumanReadable{fs::file_size(dir.path())} << std::endl;
            }
            else if (fs::is_directory(dir.path()) && level)
            {
                auto it = epath.find(dir.path().filename());
                if(it == epath.end())
                    recursive_dir(dir.path(), epath, level, file_size);
            }
    }
}