#pragma once

#include <windows.h>
#include <shlwapi.h>
#include <string>

#pragma comment(lib, "shlwapi")

struct Path
{
	typedef std::string string;
	typedef std::wstring wstring;
	static string GetDirectoryName(const string &path)
	{
		string str = path;
		PathRemoveFileSpecA(&str[0]);
		return string(str.c_str());
	}
	static wstring GetDirectoryName(const wstring &path)
	{
		wstring str = path;
		PathRemoveFileSpecW(&str[0]);
		return wstring(str.c_str());
	}

	static string GetFileName(const string &path)
	{
		auto p = PathFindFileNameA(path.c_str());
		return string(p);
	}
	static wstring GetFileName(const wstring &path)
	{
		auto p = PathFindFileNameW(path.c_str());
		return wstring(p);
	}

	static string GetFileNameWithoutExtension(const string &path)
	{
		string p = PathFindFileNameA(path.c_str());
		PathRemoveExtensionA(&p[0]);
		return string(p.c_str());
	}
	static wstring GetFileNameWithoutExtension(const wstring &path)
	{
		wstring p = PathFindFileNameW(path.c_str());
		PathRemoveExtensionW(&p[0]);
		return wstring(p.c_str());
	}

	static string GetExtension(const string &path)
	{
		auto p = PathFindExtensionA(path.c_str());
		return string(p ? p : "");
	}
	static wstring GetExtension(const wstring &path)
	{
		auto p = PathFindExtensionW(path.c_str());
		return wstring(p ? p : L"");
	}

	static string GetFullPath(const string &path)
	{
		string fullpath(MAX_PATH, 0);
		auto result = GetFullPathNameA(path.c_str(), MAX_PATH, &fullpath[0], NULL);
		if (result < MAX_PATH)
		{
			fullpath.resize(result);
		}
		else
		{
			fullpath.reserve(result);
			result = GetFullPathNameA(path.c_str(), result, &fullpath[0], NULL);
			fullpath.resize(result);
		}
		return fullpath;
	}
	static wstring GetFullPath(const wstring &path)
	{
		wstring fullpath(MAX_PATH, 0);
		auto result = GetFullPathNameW(path.c_str(), MAX_PATH, &fullpath[0], NULL);
		if (result < MAX_PATH)
		{
			fullpath.resize(result);
		}
		else
		{
			fullpath.reserve(result);
			result = GetFullPathNameW(path.c_str(), result, &fullpath[0], NULL);
			fullpath.resize(result);
		}
		return fullpath;
	}

	static string Combine(const string &path1, const string &path2)
	{
		string s(path1.size() + path2.size() + MAX_PATH, 0);
		PathCombineA(&s[0], path1.c_str(), path2.c_str());
		s.resize(strlen(&s[0]));
		return s;
	}
	static wstring Combine(const wstring &path1, const wstring &path2)
	{
		wstring s(path1.size() + path2.size() + MAX_PATH, 0);
		PathCombineW(&s[0], path1.c_str(), path2.c_str());
		s.resize(wcslen(&s[0]));
		return s;
	}
	template<class... Args>
	static string Combine(const string &path1, const string &path2, Args... args)
	{
		return Combine(Combine(path1, path2), args...);
	}
	template<class... Args>
	static wstring Combine(const wstring &path1, const wstring &path2, Args... args)
	{
		return Combine(Combine(path1, path2), args...);
	}

	static string RemoveBackslash(const string &path)
	{
		string s = path;
		PathRemoveBackslashA(&s[0]);
		s.resize(strlen(&s[0]));
		return s;
	}
	static wstring RemoveBackslash(const wstring &path)
	{
		wstring s = path;
		PathRemoveBackslashW(&s[0]);
		s.resize(wcslen(&s[0]));
		return s;
	}
	static string AddBackslash(const string &path)
	{
		string s = path;
		s.resize(s.size() + 1);
		PathAddBackslashA(&s[0]);
		s.resize(strlen(&s[0]));
		return s;
	}
	static wstring AddBackslash(const wstring &path)
	{
		wstring s = path;
		s.resize(s.size() + 1);
		PathAddBackslashW(&s[0]);
		s.resize(wcslen(&s[0]));
		return s;
	}
};
