#pragma once
#include "precompiled_header.hpp"

class Config final
{
    NO_DEFAULT_CONSTRUCTOR(Config)
public:
    Config(const string_t& filePath);
    virtual ~Config();
    const int getInt(const string_t& sectionName, const string_t& keyName, const int& defaultValue) const;
    const string_t getString(const string_t& sectionName, const string_t& keyName, const string_t& defaultValue) const;
private:
    const string_t& m_configFilePath;
};