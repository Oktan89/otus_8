#pragma once
#include <map>
#include <boost/filesystem.hpp>
#include <vector>
#include <set>
#include "options.h"
namespace fs = boost::filesystem;

class Identical
{
    using set_path = std::set<boost::filesystem::path>;
    using map_eq_files = std::map<uintmax_t, std::vector<fs::path>>;

    map_eq_files all_files;
    std::set<uintmax_t> map_eq_index;

    const Options &_opt;

public:

    Identical(const Options& opt) : _opt(opt){}
    
    void scanFiles();

    void printIdentical() const;

    std::size_t getCountOverlapFiles() const noexcept {return map_eq_index.size();}
    
private:
    void recursive_dir(const fs::path &path, const set_path &epath, bool level, std::size_t file_size);
    
    struct HumanReadable
    {
        std::uintmax_t size{};

    private:
        friend std::ostream &operator<<(std::ostream &os, HumanReadable hr)
        {
            int i{};
            double mantissa = hr.size;
            for (; mantissa >= 1024.; mantissa /= 1024., ++i){}
            mantissa = std::ceil(mantissa * 10.) / 10.;
            os << mantissa << "BKMGTPE"[i];
            return i == 0 ? os : os << "B (" << hr.size << ')';
        }
    };
};