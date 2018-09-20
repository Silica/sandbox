namespace FuncInfo
{
	template<class R>
	struct Info
	{
		std::string name;
		R result;
		Info(const std::string &n, const std::string &args = "")
		{
			name = n;
			result = R();
			log(9, name, "(", args, "){\n");
		}
		void l(int loglevel, const std::string &str)
		{
			log(loglevel, name, ":", str);
		}
		template<class... Args>
		void l(int ll, Args... args)
		{
			l(ll, String::concat(args...));
		}
		~Info()
		{
			log(9, name, "} result=", hex(result, sizeof(R) * 2));
		}
	};

	std::string hex(pointer_size_uint h, int d = sizeof(pointer_size_uint) * 2)
	{
		std::string s;
		for (int i = 0; i < d; i++)
		{
			int n = (h >> (i*4)) & 0xF;
			if (n < 10)
				s = char('0' + n) + s;
			else
				s = char('A' + n - 10) + s;
		}
		return s;
	}
	std::string hex(const void *p, int d = sizeof(void*) * 2)
	{
		return hex((pointer_size_uint)p, d);
	}

	std::string guid(const GUID &g)
	{
		return
			hex(g.Data1, 8) + "-" +
			hex(g.Data2, 4) + "-" +
			hex(g.Data3, 4) + "-" +
			hex(g.Data4[0], 2) +
			hex(g.Data4[1], 2) + "-" +
			hex(g.Data4[2], 2) +
			hex(g.Data4[3], 2) +
			hex(g.Data4[4], 2) +
			hex(g.Data4[5], 2) +
			hex(g.Data4[6], 2) +
			hex(g.Data4[7], 2)
		;
	}

	std::string Arg(int i)
	{
		return std::to_string(i);
	}

	std::string Arg(UINT u)
	{
		return hex(u);
	}

	std::string Arg(DWORD d)
	{
		return hex(d);
	}

	std::string Arg(DWORDLONG d)
	{
		return hex(d);
	}

	std::string Arg(const char *s)
	{
		if ((pointer_size_uint)s >> 16)
		{
			return String::concat(hex(s), "=", s);
		}
		else
		{
			return hex(s);
		}
	}

	std::string Arg(const wchar_t *s)
	{
		if ((pointer_size_uint)s >> 16)
		{
			return String::concat(hex(s), "=", s);
		}
		else
		{
			return hex(s);
		}
	}

	std::string Arg(char *s)
	{
		return hex(s);
	}

	std::string Arg(wchar_t *s)
	{
		return hex(s);
	}

	std::string Arg(const void *p)
	{
		return hex(p);
	}

	std::string Arg(const GUID &g)
	{
		return guid(g);
	}
	std::string Arg(const GUID *g)
	{
		if (g == NULL)
			return "(NULL)";
		else
			return guid(*g);
	}

	template<class T>
	std::string Arguments(T t)
	{
		return Arg(t);
	}
	template<class T, class... Args>
	std::string Arguments(T t, Args... args)
	{
		return Arg(t) + "," + Arguments(args...);
	}
}

template<class R, class... Args>
FuncInfo::Info<R> info(const std::string &n, Args... args)
{
	return FuncInfo::Info<R>(n, FuncInfo::Arguments(args...));
}

template<class R>
FuncInfo::Info<R> info(const std::string &n)
{
	return FuncInfo::Info<R>(n, "");
}
