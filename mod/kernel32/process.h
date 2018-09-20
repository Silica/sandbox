static BOOL WINAPI CreateProcess(
	const C *lpApplicationName,
	C *lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	const C *lpCurrentDirectory,
	StartupInfo *lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	auto x = info<BOOL>(add_suffix<C>("CreateProcess"), lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	string sname;
	const C *name = lpApplicationName;
	string scommand;
	C *command = lpCommandLine;
	string scd;
	const C *cd = lpCurrentDirectory;
	if (lpApplicationName)
	{
		sname = cnv<C>::getExists(lpApplicationName);
		name = sname.c_str();
		x.l(0, "App:", lpApplicationName, " > ", sname);
	}
	if (lpCommandLine)
	{
		scommand = cnv<C>::sp(lpCommandLine);
		command = &scommand[0];
		x.l(0, "cmd:", lpCommandLine, " > ", scommand);
	}
	if (lpCurrentDirectory)
	{
		scd = cnv<C>::getExists(lpCurrentDirectory);
		cd = scd.c_str();
		x.l(4, "dir:", lpCurrentDirectory, " > ", scd);
	}
	bool suspend = dwCreationFlags & CREATE_SUSPENDED;
	dwCreationFlags |= CREATE_SUSPENDED;
	/*#ifdef _DEBUG
	dwCreationFlags |= CREATE_NEW_CONSOLE;
	#endif*/
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	x.result = orig(CreateProcess)(name, command, NULL, NULL, bInheritHandles, dwCreationFlags, lpEnvironment, cd, lpStartupInfo, lpProcessInformation ? lpProcessInformation : &pi);
	if (!x.result)
	{
		SaveError s;
		x.l(0, "failed!");
		x.l(0, GetLastErrorString());
		return x.result;
	}

	HANDLE p = lpProcessInformation ? lpProcessInformation->hProcess : pi.hProcess;
	BOOL wow64 = true;
	orig(IsWow64Process)(p, &wow64);
	BOOL cwow64 = true;
	orig(IsWow64Process)(GetCurrentProcess(), &cwow64);

	if (wow64 == cwow64)
	{
		if (!inject(p, Path::Combine(cnv<char>::sandbox, wow64 ? dllname32 : dllname64)))
		{
			if (!suspend)
				ResumeThread(lpProcessInformation ? lpProcessInformation->hThread : pi.hThread);
		}
		else
		{
			TerminateProcess(p, 0xFF);
			return 0;
		}
	}
	else
	{
		auto pid = GetProcessId(p);
		auto exe = Path::Combine(cnv<char>::sandbox, wow64 ? exename32 : exename64);
		auto command = String::tstr<wchar_t>(String::concat(exe, " /P", pid));
		x.l(0, "cross command:", command);
		STARTUPINFOW si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION spi;
		ZeroMemory(&spi, sizeof(spi));
		if (orig(Kernel32<wchar_t>::CreateProcess)(NULL, &command[0], NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &spi))
		{
			WaitForSingleObject(spi.hProcess, INFINITE);
			if (!suspend)
				ResumeThread(lpProcessInformation ? lpProcessInformation->hThread : pi.hThread);
		}
		else
		{
			x.l(0, "cross command:", GetLastErrorString());
			TerminateProcess(p, 0xFF);
			return 0;
		}
	}

	return x.result;
}

static VOID WINAPI GetStartupInfo(StartupInfo *lpStartupInfo)
{
	auto x = info<int>(add_suffix<C>("GetStartupInfo"), lpStartupInfo);
	return orig(GetStartupInfo)(lpStartupInfo);
}

static UINT WINAPI WinExec(
	LPCSTR lpCmdLine,
	UINT uCmdShow
)
{
	auto x = info<UINT>("WinExec", lpCmdLine, uCmdShow);
	auto path = std::string("\"") + Path::Combine(cnv<char>::sandbox, exename32) + "\" " + cnv<char>::getExists(lpCmdLine);
	x.l(0, path);
	return x.result = orig(WinExec)(path.c_str(), uCmdShow);
}

static HINSTANCE WINAPI LoadLibraryEx(
	const C *lpLibFileName,
	HANDLE hFile,
	DWORD dwFlags
)
{
	auto x = info<HINSTANCE>(add_suffix<C>("LoadLibraryEx"), lpLibFileName, hFile, dwFlags);

	auto p = cnv<C>::getExists(lpLibFileName);
	bool loaded = orig(GetModuleHandle)(p.c_str());
	x.result = orig(LoadLibraryEx)(p.c_str(), hFile, dwFlags);
	if (x.result)
	{
		SaveError s;
		x.l(7, p, " = success");
	}
	else
	{
		x.l(8, p, " = ", GetLastErrorString());
		auto lib = Path::GetFileName(lpLibFileName);
		if (p != lib)
		{
			loaded = orig(GetModuleHandle)(lib.c_str());
			x.result = orig(LoadLibraryEx)(lib.c_str(), hFile, dwFlags);
			SaveError s;
			if (x.result)
			{
				x.l(7, lib, " = success");
			}
			else
			{
				x.l(8, lib, " = ", GetLastErrorString());
			}
		}
	}
	SaveError s;
	if (x.result && !loaded)
	{
		for (auto &y : orig_to_mod)
		{
			if (config.dword("LoadLibrary"))
			{
				// Loaded module imports other module
				IAT(y.second, y.first);
			}
			else
			{
				// but, many dll loaded, too heavy
				moduleIAT(x.result, y.second, y.first);
			}
		}
	}
	return x.result;
}

static HMODULE WINAPI LoadLibrary(const C *lpFileName)
{
	auto x = info<HMODULE>(add_suffix<C>("LoadLibrary"), lpFileName);
	return LoadLibraryEx(lpFileName, 0, 0);
}

static HMODULE WINAPI GetModuleHandle(const C *lpModuleName)
{
	auto x = info<HMODULE>(add_suffix<C>("GetModuleHandle"), lpModuleName);
	if (!lpModuleName)
	{
		return x.result = orig(GetModuleHandle)(lpModuleName);
	}
	auto rp = cnv<C>::getExists(lpModuleName);
	x.result = orig(GetModuleHandle)(rp.c_str());
	if (!x.result)
	{
		auto lib = Path::GetFileName(lpModuleName);
		x.result = orig(GetModuleHandle)(lib.c_str());
	}
	return x.result;
}

static FARPROC WINAPI GetProcAddress(
	HMODULE hModule,
	LPCSTR lpProcName
)
{
	auto x = info<FARPROC>("GetProcAddress", hModule, lpProcName);
	C modulepath[MAX_PATH];
	orig(GetModuleFileName)(hModule, modulepath, MAX_PATH);
	if ((pointer_size_uint)lpProcName >> 16)
	{
		x.l(4, modulepath, ".", lpProcName);
	}
	else
	{
		x.l(4, modulepath, ".", (pointer_size_uint)lpProcName & 0xFFFF);
	}
	void *o = orig(GetProcAddress)(hModule, lpProcName);
	if (o && orig_to_mod.count(o))
		x.result = (FARPROC)orig_to_mod[o];
	else
		x.result = (FARPROC)o;
	return x.result;
}

static BOOL WINAPI IsWow64Process(
	HANDLE hProcess,
	PBOOL Wow64Process
)
{
	auto x = info<BOOL>("IsWow64Process", hProcess, Wow64Process);
	if (config.exists("IsWow64Process") && hProcess && GetCurrentProcess() == hProcess && Wow64Process)
	{
		*Wow64Process = config.dword("IsWow64Process", 0);
		x.result = true;
	}
	else
	{
		x.result = orig(IsWow64Process)(hProcess, Wow64Process);
	}
	return x.result;
}

static BOOL WINAPI IsDebuggerPresent()
{
	auto x = info<BOOL>("IsDebuggerPresent");
	return x.result = false;
}

static BOOL WINAPI GetBinaryType(
	const C *lpApplicationName,
	LPDWORD lpBinaryType
)
{
	auto x = info<BOOL>(add_suffix<C>("GetBinaryType"), lpApplicationName, lpBinaryType);
	auto f = cnv<C>::getExists(lpApplicationName);
	x.result = orig(GetBinaryType)(f.c_str(), lpBinaryType);
	x.l(7, lpApplicationName, " > ", f, " lpBinaryType=", *lpBinaryType);
	return x.result;
}
//LoadModule
//CreateProcessAsUserW
