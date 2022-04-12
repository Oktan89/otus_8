#include "identical.h"
#include <boost/algorithm/string.hpp>  
#include <cctype>
//Оставить только path остальное можно вызывать сразу из класса!!!
void Identical::recursive_dir(const fs::path &path, const set_path &epath, bool level, std::size_t file_size)
{
    if (fs::is_directory(path) && !fs::is_symlink(path))
    {
        for (auto &dir : fs::directory_iterator(path))
            if (fs::is_regular_file(dir.path()))
            {
                auto f_size = fs::file_size(dir.path());
                if (file_size <= f_size && mask_matching(dir.path()))
                {
                    std::cout << dir.path() << " \tsize file: " << HumanReadable{f_size} << std::endl;
                    const auto [it, ok] = all_files.insert({f_size, std::vector<fs::path>{dir.path()}});
                    if (!ok)
                    {
                        it->second.push_back(dir.path());
                        map_eq_index.emplace(f_size);
                    }
                }
            }
            else if (fs::is_directory(dir.path()) && level)
            {
                auto it = epath.find(dir.path().filename());
                if (it == epath.end())
                {
                    recursive_dir(dir.path(), epath, level, file_size);
                }
            }
    }
}

void Identical::scanFiles()
{

    for(auto &path : _opt.getScanPath())   
    {
        if(fs::exists(path))
        {
            recursive_dir(path, _opt.getExcludePath(), _opt.level(), _opt.getFileSize());
        }
        else
        {
            std::cout << "Search path "<< path <<" not found." << std::endl;
        }
    }
}

void Identical::printIdentical() const
{
    for(auto &index : map_eq_index)
    {
        auto it = all_files.find(index);
        if(it != all_files.end())
        {
            for(auto &path: it->second)
            {
                try{
                    std::cout << path << " size: "<< HumanReadable{fs::file_size(path)} <<std::endl;
                }
                catch(fs::filesystem_error &e)
                {
                    std::cout << "File "<< path <<" delete OS:" << e.what() << std::endl;
                }
                
            }
        }
        std::cout << std::endl;
    }
}

bool Identical::mask_matching([[maybe_unused]]const fs::path &path) const
{

    std::string dest_path = path.filename().string();
    boost::algorithm::to_lower(dest_path);
  
    const char* ptr = _opt.getMask().c_str();
    std::size_t index{0};
    auto next = [&ptr, &index](){++ptr; ++index;};

    for (;;)
    {
        switch (*ptr)
        {
            case '*':
                
                index = dest_path.find_first_of(*(++ptr), index);
                break;
            
            case '\0': 
            
                return true;

            case '?':
                try
                {
                    dest_path.at(index);
                }
                catch(std::out_of_range const& exc) 
                {
                    return false;
                }
                next();
                if(*ptr == '\0' && dest_path.size() > index)
                        return false;
                break;

            default:
                if(dest_path[index] == *ptr)
                {
                    next();
                    if(*ptr == '\0' && dest_path.size() > index)
                        return false;
                }
                else return false;
                break;
        }
    }
}