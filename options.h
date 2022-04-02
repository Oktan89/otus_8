#pragma once
#include <iostream>
#include <boost/program_options.hpp>
#include <stdexcept>

namespace po = boost::program_options;

class exitOptionsProgramm : public std::exception {};

class Options
{
    int level;
    po::options_description desc;//("Options bayan");
    po::positional_options_description p;
    po::variables_map vm;

public:
    Options() : desc("Options bayan")
    {
        setOptions();
    }
    
    void parseCommandLine(int argc, char* argv[])
    {
        // po::store(po::parse_command_line(argc, argv, desc), vm);
        po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);
        po::notify(vm);
        if(argc < 2)
            throw exitOptionsProgramm();
    }

    void printHelp()
    {
        std::cout << "Utility for detecting duplicate files" << std::endl;
        std::cout << "Usage: example [OPTION]... --scan_dir [dir]...\n";
        std::cout << "  or   example [OPTION] -s [dir] --excl_dir [dir]...\n";
        std::cout << desc << std::endl;
    }

    void print()
    {
        if (vm.count("help") || vm.empty())
        {
            std::cout << desc << std::endl;
            throw exitOptionsProgramm();
        }
        if (vm.count("scan_dir"))
        {
            const std::vector<std::string> &vec = vm["scan_dir"].as<std::vector<std::string>>();
            std::cout <<"Directory to scan :";
            std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(std::cout, " ")); 
            std::cout << std::endl;
        }
        if (vm.count("excl_dir"))
        {
            const std::vector<std::string> &vec = vm["excl_dir"].as<std::vector<std::string>>();
            std::cout <<"Directory to scan :";
            std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(std::cout, " ")); 
            std::cout << std::endl;
        }
        std::cout << "Level :" << level << std::endl;
    }
private:
    void setOptions()
    {
        desc.add_options()
            ("help,h", "this help message")
            ("scan_dir,s", po::value<std::vector<std::string>>(),
                "directories to scan (there may be several)")
            ("excl_dir,e", po::value<std::vector<std::string>>(),
                "directories to exclude from scanning (there may be several)")
            ("level_scan,l", po::value<int>(&level)->implicit_value(1)->default_value(0),
                "scan level (1 - for all directories, 0 - only the specified directory without nested ones)");
        po::positional_options_description p;
        //p.add("scan_dir", argc);
        p.add("excl_dir", -1);
    }

};