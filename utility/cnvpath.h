enum class PathType
{
	Sandbox,
	Virtual,
	System,
	Others,
};
template<class C>
struct cnv
{
	typedef std::basic_string<C> string;
	static string sandbox;
	static string drive;
	static string vuser;
	static string suser;
	static string mutex;
	static string sp(const string &path)
	{
		#ifdef NO_REDIRECT_USERHOME
		return String::replace(path, drive, sandbox, true, false);
		#else
		auto str = String::replace(path, suser, vuser, true, false);
		return String::replace(str, drive, sandbox, true, false);
		#endif
	}
	static string vp(const string &path)
	{
		#ifdef NO_REDIRECT_USERHOME
		return String::replace(path, sandbox, drive, true, false);
		#else
		auto str = String::replace(path, suser, vuser, true, false);
		return String::replace(str, sandbox, drive, true, false);
		#endif
	}
	static string cp(const string &path)
	{
		#ifdef NO_REDIRECT_USERHOME
		return String::replace(path, sandbox, drive, true, false);
		#else
		auto str = String::replace(path, sandbox, drive, true, false);
		return String::replace(str, vuser, suser, true, false);
		#endif
	}
	static bool is(const string &path, PathType pt)
	{
		auto lp = String::tolower(path);
		switch (pt)
		{
		case PathType::Sandbox:
			return (lp.find(sandbox) != string::npos);
		case PathType::Virtual:
			if (lp.find(suser) != string::npos)
				return false;
			return (lp.find(drive) != string::npos)
		case PathType::System:
			if (lp.find(vuser) != string::npos)
				return false;
			return (lp.find(drive) != string::npos)
		case Others:
			if (lp.find(sandbox) != string::npos)
				return false;
			if (lp.find(drive) != string::npos)
				return false;
			return true;
		}
		return false;
	}
	static string to(const string &path, PathType pt)
	{
		switch (pt)
		{
		case PathType::Sandbox:
			return sp(path);
		case PathType::Virtual:
			return vp(path);
		case PathType::System:
			return cp(path);
		}
		return path;
	}
	static string getExists(const string &path)
	{
		auto sandbox_path = to(path, PathType::Sandbox);
		if (orig(Kernel32<C>::GetFileAttributes)(sandbox_path.c_str()) != -1)
		{
			return sandbox_path;
		}
		auto sys_path = to(path, PathType::System);
		auto attr = orig(Kernel32<C>::GetFileAttributes)(sys_path.c_str());
		if (attr != -1)
		{
			if (attr & FILE_ATTRIBUTE_DIRECTORY)
			{
				log(1, "getExists:CreateSandboxDirectory:", sandbox_path);
				Shell32<C>::SHCreateDirectoryEx(NULL, sandbox_path.c_str(), NULL);
			}
			else
			{
				auto d = Path::GetDirectoryName(sandbox_path);
				auto dattr = orig(Kernel32<C>::GetFileAttributes)(d.c_str());
				if (dattr == -1)
				{
					log(1, "getExists:CreateSandboxDirectory:", d);
					Shell32<C>::SHCreateDirectoryEx(NULL, d.c_str(), NULL);
				}
			}
			return sys_path;
		}
		return path;
	}
};
