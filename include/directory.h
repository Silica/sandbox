#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <functional>

#include "path.h"

struct Directory
{
	typedef std::string string;
	typedef std::wstring wstring;
	typedef std::vector<string> slist;
	typedef std::vector<wstring> wslist;
	typedef std::vector<WIN32_FIND_DATAA> infolist;
	typedef std::vector<WIN32_FIND_DATAW> winfolist;

	static bool Exists(const string &d)
	{
		return PathIsDirectoryA(d.c_str());
	}
	static bool Exists(const wstring &d)
	{
		return PathIsDirectoryW(d.c_str());
	}

	template<class R, class T>
	static R GetBase(const string &d, const T &f)
	{
		R result;
		WIN32_FIND_DATAA data;
		HANDLE h = FindFirstFileA(d.c_str(), &data);
		if (h == INVALID_HANDLE_VALUE)
			return result;

		do
		{
			f(data, result);
		} while (FindNextFile(h, &data));
		FindClose(h);

		return result;
	}
	template<class R, class T>
	static R GetBase(const wstring &d, const T &f)
	{
		R result;
		WIN32_FIND_DATAW data;
		HANDLE h = FindFirstFileW(d.c_str(), &data);
		if (h == INVALID_HANDLE_VALUE)
			return result;

		do
		{
			f(data, result);
		} while (FindNextFileW(h, &data));
		FindClose(h);

		return result;
	}

	static slist GetFiles(const string &d, const string &s = "*")
	{
		return GetBase<slist>(Path::Combine(d, s), [](WIN32_FIND_DATAA &data, slist &list)
		{
			if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				list.push_back(data.cFileName);
		});
	}
	static slist GetDirectories(const string &d, const string &s = "*")
	{
		return GetBase<slist>(Path::Combine(d, s), [](WIN32_FIND_DATAA &data, slist &list)
		{
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (data.cFileName[0] != '.'))
				list.push_back(data.cFileName);
		});
	}
	static wslist GetFiles(const wstring &d, const wstring &s = L"*")
	{
		return GetBase<wslist>(Path::Combine(d, s), [](WIN32_FIND_DATAW &data, wslist &list)
		{
			if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				list.push_back(data.cFileName);
		});
	}
	static wslist GetDirectories(const wstring &d, const wstring &s = L"*")
	{
		return GetBase<wslist>(Path::Combine(d, s), [](WIN32_FIND_DATAW &data, wslist &list)
		{
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (data.cFileName[0] != '.'))
				list.push_back(data.cFileName);
		});
	}

	static infolist GetFileInfos(const string &d, const string&s = "*")
	{
		return GetBase<infolist>(Path::Combine(d, s), [](WIN32_FIND_DATAA &data, infolist &list)
		{
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (data.cFileName[0] != '.'))
				list.push_back(data);
		});
	}
	static infolist GetDirectoryInfos(const string &d, const string&s = "*")
	{
		return GetBase<infolist>(Path::Combine(d, s), [](WIN32_FIND_DATAA &data, infolist &list)
		{
			if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				list.push_back(data);
		});
	}
	static winfolist GetFileInfos(const wstring &d, const wstring&s = L"*")
	{
		return GetBase<winfolist>(Path::Combine(d, s), [](WIN32_FIND_DATAW &data, winfolist &list)
		{
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (data.cFileName[0] != '.'))
				list.push_back(data);
		});
	}
	static winfolist GetDirectoryInfos(const wstring &d, const wstring&s = L"*")
	{
		return GetBase<winfolist>(Path::Combine(d, s), [](WIN32_FIND_DATAW &data, winfolist &list)
		{
			if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				list.push_back(data);
		});
	}

	static BOOL Delete(const string &d, bool recursive = false)
	{
		if (recursive)
		{
			auto files = GetFiles(d);
			for (auto &f : files)
			{
				DeleteFileA(Path::Combine(d, f).c_str());
			}
			auto dirs = GetDirectories(d);
			for (auto &n : dirs)
			{
				Delete(Path::Combine(d, n).c_str(), true);
			}
		}
		return RemoveDirectoryA(d.c_str());
	}
	static BOOL Delete(const wstring &d, bool recursive = false)
	{
		if (recursive)
		{
			auto files = GetFiles(d);
			for (auto &f : files)
			{
				DeleteFileW(Path::Combine(d, f).c_str());
			}
			auto dirs = GetDirectories(d);
			for (auto &n : dirs)
			{
				Delete(Path::Combine(d, n).c_str(), true);
			}
		}
		return RemoveDirectoryW(d.c_str());
	}
};
