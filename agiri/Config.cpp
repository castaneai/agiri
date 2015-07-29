#include "precompiled_header.hpp"
#include "Config.hpp"

Config::Config(const string_t& filePath)
    : m_configFilePath(filePath)
{}

Config::~Config()
{}

const int Config::getInt(const string_t& sectionName, const string_t& keyName, const int& defaultValue = 0) const
{
    return GetPrivateProfileInt(sectionName.c_str(), keyName.c_str(), defaultValue, m_configFilePath.c_str());
}

const string_t Config::getString(const string_t& sectionName, const string_t& keyName, const string_t& defaultValue = string_t()) const
{
    TCHAR result[1024];
    GetPrivateProfileString(sectionName.c_str(), keyName.c_str(), defaultValue.c_str(), result, sizeof(result), m_configFilePath.c_str());
    return std::move(string_t(result));
}