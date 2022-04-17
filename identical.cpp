#include "identical.h"
#include <boost/algorithm/string.hpp>  
#include <list>
#include <algorithm>
#include <unordered_set>
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

void Identical::scanOverlapFilesSize()
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

void Identical::searchIdentical()
{
    for(auto &index : map_eq_index)
    {
        auto it = all_files.find(index);
        if(it != all_files.end())
        {
            printOverlap(it->second);
            // for(auto &path: it->second)
            // {
            //     try{
            //         std::cout << path << " size: "<< HumanReadable{fs::file_size(path)} <<std::endl;
            //         searchHash(path, fs::file_size(path));
            //     }
            //     catch(fs::filesystem_error &e)
            //     {
            //         std::cout << "File "<< path <<" possibly deleted by the operating system:" << e.what() << std::endl;
            //     }
                
            // }
        }
        //std::cout << std::endl;
    }
}

void Identical::searchHash(const fs::path &patch, [[maybe_unused]] uintmax_t size)
{
    char *test = new char[_opt.getBlockSize()];  
    
    memset(test, 0, _opt.getBlockSize());

    std::ifstream file(patch.string(), std::ios::binary);

    file.read(test, _opt.getBlockSize());
  
    std::cout << file.tellg() << std::endl;
    for(std::size_t i =0; i < _opt.getBlockSize(); ++i)
        std::cout << std::hex << (int)test[i] <<" ";
    std::cout<<std::endl;
    std::cout << "crc32 " << getcrc32(test, _opt.getBlockSize()) << std::endl;;
    delete[] test;    
}

void Identical::printOverlap(const std::vector<fs::path> &vec)
{
    std::vector<std::unique_ptr<std::ifstream>> files;
    files.reserve(vec.size());

    for(std::size_t i = 0; i < vec.size(); ++i)
        files.emplace_back(std::make_unique<std::ifstream>(vec[i].string(), std::ios::binary));

    std::size_t size_b = _opt.getBlockSize();
    std::unique_ptr<char[]> block(new char[size_b]);
    memset(block.get(), 0, size_b);   
    

    std::unordered_map<std::size_t, uint32_t> hash_block;
    std::unordered_set<std::size_t> equal_files;
    hash_block.reserve(vec.size());

    bool start = true;
    while(start)
    {
        for(std::size_t i = 0; i < files.size(); ++i)
        {
            if(files[i]->is_open())
            {
                files[i]->read(block.get(), size_b);
                hash_block[i] = getcrc32(block.get(), size_b);
            }
        }

        for(std::size_t i = 0; i < hash_block.size(); ++i)
        {
            for(std::size_t j = hash_block.size()-1; j > i; --j)
            {
                auto itone = hash_block.find(i);
                auto ittwo = hash_block.find(j);
                if(itone != hash_block.end() && ittwo != hash_block.end())
                {
                    if(itone->second == ittwo->second)
                    {
                        equal_files.insert(i);
                        equal_files.insert(j);
                    }
                }
            }
        }

        start = (equal_files.empty())? false : true;

        for(std::size_t i = 0; i < files.size(); ++i)
        {
            auto it = equal_files.find(i);
            if(it == equal_files.end())
            {
                files[i]->close();
            }
        }

        // for(auto &&cr : hash_block)
        //     std::cout << std::hex << cr.second <<" ";
        // std::cout << std::endl;
             
        if (files.begin()->get()->eof()) 
        {
            // std::cout << "stream state is eof\n";
            for(std::size_t i = 0; i < vec.size(); ++i)
            {
                auto it = equal_files.find(i);
                if(it != equal_files.end())
                {
                    std::cout << vec[i] << " size: "<< HumanReadable{fs::file_size(vec[i])} <<std::endl;
                }
            }
            std::cout << std::endl;
            start = false;
        }
        else
        {
            memset(block.get(), 0, size_b);  
            hash_block.clear();
            equal_files.clear();  
        } 
    }
}

uint32_t Identical::getcrc32(const char* block, std::size_t size_b)
{
    boost::crc_32_type result;
    result.process_bytes(block, size_b);
    return result.checksum();
}

bool Identical::mask_matching(const fs::path &path) const
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