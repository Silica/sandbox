#pragma once

#include <string>
#include <map>

#include "include/string.h"

class Config
{
public:
	typedef std::string string;
	std::map<string, string> config;
	Config()
	{
		config["default"] = "value";

		#ifdef _DEBUG
		config["logfile"] = "1";
		config["console"] = "1";
		config["loglevel"] = "5";
		#endif

		config["username"] = "SANDBOX";
		config["computername"] = "SANDBOX";
		config["reg"] = "registry";

		config["drive"] = "C:";
		config["virtual_user_directory"] = "C:\\Users\\SANDBOX";

		config["verifyversioninfo"] = "1";
	}
	static Config Read(const string &file)
	{
		Config cfg;
		auto h = CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h == INVALID_HANDLE_VALUE)
			return cfg;
		DWORD filesize = GetFileSize(h, NULL);
		string buffer(filesize, 0);
		DWORD NumberOfBytesRead;
		ReadFile(h, &buffer[0], filesize, &NumberOfBytesRead, NULL);
		buffer.resize(NumberOfBytesRead);
		auto list = String::split(buffer);

		for (auto &line : list)
		{
			auto x = String::split(line, "=");
			if (x.size() < 2)
				continue;
			cfg.config[String::tolower(String::trim(x[0]))] = String::trim(x[1]);
		}

		return cfg;
	}
	static DWORD parseNum(const string &s)
	{
		if (String::starts_with(s, "0x"))
		{
			return std::stoi(s.substr(2), nullptr, 16);
		}
		else
		{
			return std::stoi(s);
		}
	}
	bool exists(const string &s)
	{
		return config.count(String::tolower(s));
	}
	DWORD dword(const string &s, DWORD def = 0)
	{
		auto l = String::tolower(s);
		if (config.count(l))
		{
			return parseNum(config[l]);
		}
		return def;
	}
	int Int(const string &s, int def = 0)
	{
		auto l = String::tolower(s);
		if (config.count(l))
		{
			return parseNum(config[l]);
		}
		return def;
	}
	string str(const string &s, const string &def = "")
	{
		auto l = String::tolower(s);
		if (config.count(l))
		{
			return config[l];
		}
		return def;
	}
	std::wstring wstr(const string &s, const std::wstring &def = L"")
	{
		auto l = String::tolower(s);
		if (config.count(l))
		{
			return String::tstr<wchar_t>(config[l]);
		}
		return def;
	}
};
