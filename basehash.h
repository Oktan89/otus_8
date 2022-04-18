#pragma once
#include <string>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>

class BaseHash
{
public:
    virtual std::string getHash(const char* block, std::size_t size_b) = 0;
    ~BaseHash() = default;
};

class bayan_boost_CRC32 : public BaseHash
{
public:
    std::string getHash(const char* block, std::size_t size_b) override
    {
        boost::crc_32_type result;
        result.process_bytes(block, size_b);
        return std::to_string(result.checksum());
    }
    ~bayan_boost_CRC32() = default;
private:
    
};

class bayan_boost_md5 : public BaseHash
{    
public:
    std::string getHash(const char* block, std::size_t size_b) override
    {
        boost::uuids::detail::md5 md5;
        boost::uuids::detail::md5::digest_type result;  
        std::string str_md5;
        md5.process_bytes(block, size_b);
        md5.get_digest(result);
        const auto char_digest = reinterpret_cast<const char*>(&result);
        boost::algorithm::hex(char_digest,char_digest+sizeof(boost::uuids::detail::md5::digest_type), std::back_inserter(str_md5));
        return str_md5;
    }
    ~bayan_boost_md5() = default;
private:

};