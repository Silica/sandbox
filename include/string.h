#pragma once

#include <Windows.h>
#include <string>
#include <algorithm>
#include <vector>

struct String
{
	typedef std::string string;
	typedef std::wstring wstring;
	static DWORD cpy(char *dest, const string &src, size_t size)
	{
		#ifdef _DEBUG
		if (dest == NULL)
			printf("copy to null:%s\n", src.c_str());
		#endif
		strncpy(dest, src.c_str(), size);
		return src.size() < size ? src.size() : size;
	}
	static DWORD cpy(wchar_t *dest, const wstring &src, size_t size)
	{
		#ifdef _DEBUG
		if (dest == NULL)
			printf("copy to null:%S\n", src.c_str());
		#endif
		wcsncpy(dest, src.c_str(), size);
		return src.size() < size ? src.size() : size;
	}

	template<class C>
	static std::basic_string<C> tstr(const string &s)
	{
		return s;
	}
	template<>
	static std::basic_string<wchar_t> tstr(const string &s)
	{
		auto size = s.size() + 1;
		wstring ws(size, 0);
		size = MultiByteToWideChar(CP_ACP, 0, s.c_str(), s.size(), &ws[0], size);
		ws.resize(size);
		return ws;
	}
	template<class C>
	static std::basic_string<C> tstr(const wstring &s)
	{
		return s;
	}
	template<>
	static std::basic_string<char> tstr(const wstring &s)
	{
		auto size = s.size() * sizeof(wchar_t) + 1;
		string str(size, 0);
		size = WideCharToMultiByte(CP_ACP, 0, s.c_str(), s.size(), &str[0], size, 0, 0);
		str.resize(size);
		return str;
	}
	template<class C>
	static std::basic_string<C> tstr(const char *s)
	{
		return tstr<C>(string(s));
	}
	template<class C>
	static std::basic_string<C> tstr(const wchar_t *s)
	{
		return tstr<C>(wstring(s));
	}
	template<class C>
	static std::basic_string<C> tstr(char *s)
	{
		return tstr<C>(string(s));
	}
	template<class C>
	static std::basic_string<C> tstr(wchar_t *s)
	{
		return tstr<C>(wstring(s));
	}
	template<class C, class T>
	static std::basic_string<C> tstr(T t)
	{
		return std::to_string(t);
	}

	template<class S>
	static S trim_tmp(const S &s, const S &c)
	{
		auto begin = s.find_first_not_of(c);
		if (begin == S::npos)
			return S();
		auto end = s.find_last_not_of(c);
		return s.substr(begin, end - begin + 1);
	}
	static string trim(const string &s, const string &c = " \t\r\n")
	{
		return trim_tmp(s, c);
	}
	static wstring trim(const wstring &s, const wstring &c = L" \t\r\n")
	{
		return trim_tmp(s, c);
	}

	template<class S>
	static S tolower_tmp(const S &s)
	{
		S d = s;
		std::transform(s.begin(), s.end(), d.begin(), ::tolower);
		return d;
	}
	static string tolower(const string &s)
	{
		return tolower_tmp(s);
	}
	static wstring tolower(const wstring &s)
	{
		return tolower_tmp(s);
	}

	template<class S>
	static S replace_tmp(const S &s, const S &o, const S &n, bool i = false, bool g = true)
	{
		auto sm = s;
		auto sl = i ? tolower(s) : s;
		auto ol = i ? tolower(o) : o;

		S::size_type p = 0;
		while ((p = sl.find(ol, p)) != S::npos)
		{
			sm.replace(p, o.size(), n);
			sl.replace(p, ol.size(), n);
			if (!g)
				break;
			p += n.size();
		}

		return sm;
	}
	static string replace(const string &s, const string &oldstr, const string &newstr, bool ignorecase = false, bool repeat = true)
	{
		return replace_tmp(s, oldstr, newstr, ignorecase, repeat);
	}
	static wstring replace(const wstring &s, const wstring &oldstr, const wstring &newstr, bool ignorecase = false, bool repeat = true)
	{
		return replace_tmp(s, oldstr, newstr, ignorecase, repeat);
	}

	static bool starts_with(const string &s1, const string &s2)
	{
		if (s1.size() < s2.size())
			return false;
		return s1.substr(0, s2.size()) == s2;
	}

	static std::vector<string> split(const string &str, const string &sep = "\n")
	{
		std::vector<string> list;

		auto slen = sep.size();
		string::size_type start = 0;
		auto pos = str.find(sep);
		while (pos != string::npos)
		{
			list.push_back(str.substr(start, pos - start));
			start = pos + slen;
			pos = str.find(sep, start);
		}
		list.push_back(str.substr(start));
		return list;
	}

	template<class T>
	static string concat(T s)
	{
		return tstr<char>(s);
	}
	template<class T, class... Args>
	static string concat(T s, Args... args)
	{
		return tstr<char>(s) + concat(args...);
	}
};
