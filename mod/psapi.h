template<class C> struct Psapi
{
	typedef std::basic_string<C> string;

	static DWORD WINAPI GetModuleBaseName(
		HANDLE hProcess,
		HMODULE hModule,
		C *lpBaseName,
		DWORD nSize
	)
	{
		auto x = info<DWORD>(add_suffix<C>("GetModuleBaseName"), hProcess, hModule, lpBaseName, nSize);
		x.result = orig(GetModuleBaseName)(hProcess, hModule, lpBaseName, nSize);
		SaveError s;
		if (x.result)
		{
			auto vp = cnv<C>::vp(lpBaseName);
			x.l(7, lpBaseName, " > ", vp);
			x.result = String::cpy(lpBaseName, vp, nSize);
		}
		return x.result;
	}

	static DWORD WINAPI GetModuleFileNameEx(
		HANDLE hProcess,
		HMODULE hModule,
		C *lpFilename,
		DWORD nSize
	)
	{
		auto x = info<DWORD>(add_suffix<C>("GetModuleFileNameEx"), hProcess, hModule, lpFilename, nSize);
		x.result = orig(GetModuleFileNameEx)(hProcess, hModule, lpFilename, nSize);
		SaveError s;
		if (x.result)
		{
			auto vp = cnv<C>::vp(lpFilename);
			x.l(7, lpFilename, " > ", vp);
			x.result = String::cpy(lpFilename, vp, nSize);
		}
		return x.result;
	}

	static DWORD WINAPI GetMappedFileName(
		HANDLE hProcess,
		LPVOID lpv,
		C *lpFilename,
		DWORD nSize
	)
	{
		auto x = info<DWORD>(add_suffix<C>("GetMappedFileName"), hProcess, lpv, lpFilename, nSize);
		x.result = orig(GetMappedFileName)(hProcess, lpv, lpFilename, nSize);
		SaveError s;
		if (x.result)
		{
			auto vp = cnv<C>::vp(lpFilename);
			x.l(7, lpFilename, " > ", vp);
			x.result = String::cpy(lpFilename, vp, nSize);
		}
		return x.result;
	}

	static void Attach()
	{
		if (config.dword("psapi"))
		{
			HMODULE psapi = LoadLibraryA("psapi");
			reg(psapi, add_suffix<C>("GetModuleBaseName"), GetModuleBaseName);
			reg(psapi, add_suffix<C>("GetModuleFileNameEx"), GetModuleFileNameEx);
			reg(psapi, add_suffix<C>("GetMappedFileName"), GetMappedFileName);
		}
		else
		{
			HMODULE kernel32 = GetModuleHandleA("kernel32");
			reg(kernel32, add_suffix<C>("K32GetModuleBaseName"), GetModuleBaseName);
			reg(kernel32, add_suffix<C>("K32GetModuleFileNameEx"), GetModuleFileNameEx);
			reg(kernel32, add_suffix<C>("K32GetMappedFileName"), GetMappedFileName);
		}
	}
};
//GetProcessImageFileName
