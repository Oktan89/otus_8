#pragma once
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>  
#include <stdexcept>
#include <set>

namespace po = boost::program_options;

class exitOptionsProgramm : public std::exception {};

class Options
{
    po::options_description desc;//("Options bayan");
    //po::positional_options_description p;
    po::variables_map vm;

public:
    Options() : desc("Options bayan")
    {
        setOptions();
    }
    
    void parseCommandLine(int argc, char* argv[])
    {
        try{
            po::store(po::parse_command_line(argc, argv, desc), vm);
            po::notify(vm);
        }
        catch(std::exception &e)
        {
            std::cout << e.what() << std::endl;
            throw exitOptionsProgramm();
        }
        
        if(argc < 2)
            printHelp();
    }

    void printHelp()
    {
        std::cout << "Utility for detecting duplicate files" << std::endl;
        std::cout << "Usage: example bayan [OPTION]... -s [directory scan]... -e [directory to exclude]...\n";
        std::cout << "\t  or   bayan [OPTION]... -s [directory scan]... -m [mask of file...]\n";
        std::cout << "\t  or   bayan [OPTION]... -m [mask of file...]\n";
        std::cout << "\t  or   bayan [OPTION]... -e [directory to exclude]...\n";
        std::cout << desc << std::endl;
        throw exitOptionsProgramm();
    }

    void print()
    {
        if (vm.count("help"))
        {
            printHelp();
        }
        if (vm.count("scan-dir"))
        {
            const std::vector<std::string> &vec = vm["scan-dir"].as<std::vector<std::string>>();
            std::cout <<"Directory to scan :\n\t";
            std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(std::cout, "\n\t")); 
            std::cout << '\r';
        }
        if (vm.count("excl-dir"))
        {
            const std::vector<std::string> &vec = vm["excl-dir"].as<std::vector<std::string>>();
            std::cout <<"Directory to exclude :\n\t";
            std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(std::cout, "\n\t")); 
            std::cout << '\r';
        }
        if (vm.count("level-scan"))
        {
            std::cout << "Level scan :" << vm["level-scan"].as<int>() << std::endl;
        }
        if (vm.count("file-size"))
        {
            std::cout << "The minimum file size from: " << vm["file-size"].as<std::size_t>() << " byte"<< std::endl;
        }
        if (vm.count("file-mask"))
        {
            std::cout << "Masks of file names: " << vm["file-mask"].as<std::string>() << std::endl;
        }
        if (vm.count("block-size"))
        {
            std::cout << "The size of block to read file: " << vm["block-size"].as<std::size_t>() << " byte"<< std::endl;
        }
        std::cout << std::endl;
    }

    std::vector<boost::filesystem::path> getScanPath() const
    {
        if(vm.count("scan-dir"))
        {
            // auto vec = vm["scan-dir"].as<std::vector<std::string>>();

            std::vector<boost::filesystem::path> vec_path;
            for(auto &p: vm["scan-dir"].as<std::vector<std::string>>())
            {
                vec_path.emplace_back(p);
            }

            return vec_path;
        }
            
        return std::vector<boost::filesystem::path>();
    }

    std::set<boost::filesystem::path> getExcludePath() const
    {
        
        if(vm.count("excl-dir"))
        {
            auto vec = vm["excl-dir"].as<std::vector<std::string>>();
            std::set<boost::filesystem::path> map_excl;
            
            for(auto &p: vec)
            {
               map_excl.insert(p);
            }

            return map_excl;
        }
        
        return std::set<boost::filesystem::path>();
    }

    bool level() const
    {
        return vm["level-scan"].as<int>();
    }

    std::uintmax_t getFileSize() const
    {
        return vm["file-size"].as<std::uintmax_t>();
    }
    
    std::string getMask() const
    {   std::string mask = vm["file-mask"].as<std::string>();
        boost::algorithm::to_lower(mask);
        return mask;
    }

private:
    void setOptions()
    {
        desc.add_options()
            ("help,h", "this help message")
            ("scan-dir,s", po::value<std::vector<std::string>>()->composing(),
                "directories to scan (there may be several)")
            ("excl-dir,e", po::value<std::vector<std::string>>()->composing(),
                "directories to exclude from scanning (there may be several)")
            ("level-scan,l", po::value<int>()->default_value(0),
                "scan level-scan (1 - for all directories, 0 - only the specified directory without nested ones)")
            ("file-size,f", po::value<std::uintmax_t>()->default_value(1), 
                "the minimum file size, by default, all files larger than 1 byte are checked.")
            ("block-size,b", po::value<std::size_t>()->default_value(10), 
                "the size of the block in bytes used to read files.")
            ("file-mask,m", po::value<std::string>()->default_value("*"),
            "masks of file names allowed for comparison (case-insensitive)");

    }

};