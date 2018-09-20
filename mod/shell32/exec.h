static bool isExecutableExt(const string &s)
{
	auto ext = String::tolower(Path::GetExtension(s));
	if (ext == String::tstr<C>(".exe"))
		return true;
	return false;
}

static BOOL WINAPI ShellExecuteEx(ShellExecuteInfo *pExecInfo)
{
	auto x = info<BOOL>(add_suffix<C>("ShellExecuteEx"), pExecInfo);
	x.l(0, pExecInfo->lpFile);
	if (pExecInfo->lpVerb)
		x.l(2, "verb:", pExecInfo->lpVerb);
	if (pExecInfo->lpParameters)
		x.l(2, "param:", pExecInfo->lpParameters);
	if (pExecInfo->lpDirectory)
		x.l(2, "dir:", pExecInfo->lpDirectory);
	auto file = cnv<C>::getExists(pExecInfo->lpFile);
	string directory;

	bool flag = false;
	if (!pExecInfo->lpVerb)
	{
		flag = true;
	}
	else
	{
		auto verb = String::tolower(String::tstr<char>(pExecInfo->lpVerb));
		if (verb == "open" || verb == "")
		{
			flag = true;
		}
		else if (verb == "runas")
		{
			DWORD bt;
			orig(Kernel32<C>::GetBinaryType)(file.c_str(), &bt);
			auto newf = Path::Combine(cnv<C>::sandbox, String::tstr<C>(bt == SCS_64BIT_BINARY ? exename64 : exename32));
			string param = pExecInfo->lpParameters;
			param = String::tstr<C>("\"") + file + String::tstr<C>("\" ") + directory + param;

			if (pExecInfo->lpDirectory)
			{
				directory = cnv<C>::sp(pExecInfo->lpDirectory);
				directory = String::tstr<C>("\"/D") + directory + String::tstr<C>("\" ");
			}

			ShellExecuteInfo ei = *pExecInfo;
			ei.lpFile = newf.c_str();
			ei.lpParameters = param.c_str();
			x.l(1, ei.lpFile, " ", ei.lpParameters);
			x.result = orig(ShellExecuteEx)(&ei);
			SaveError s;
			x.l(7, "result=", x.result, " hInstApp = ", (int)ei.hInstApp, " hProcess = ", (int)ei.hProcess);
			if (x.result)
			{
				WaitForSingleObject(ei.hProcess, INFINITE);
				DWORD code;
				GetExitCodeProcess(ei.hProcess, &code);
				pExecInfo->hInstApp = ei.hInstApp;
				pExecInfo->hProcess = (HANDLE)code;
			}
			return x.result;
		}
	}

	if (flag)
	{
		C path[MAX_PATH];
		auto ext_exe = String::tstr<C>(".exe");
		auto result = orig(Kernel32<C>::SearchPath)(NULL, file.c_str(), ext_exe.c_str(), MAX_PATH, path, NULL);
		if (!result && pExecInfo->lpDirectory)
		{
			directory = cnv<C>::sp(pExecInfo->lpDirectory);
			result = orig(Kernel32<C>::SearchPath)(directory.c_str(), file.c_str(), ext_exe.c_str(), MAX_PATH, path, NULL);
		}
		SaveError s;
		if (result > 0 && isExecutableExt(path))
		{
			if (pExecInfo->lpDirectory)
			{
				directory = cnv<C>::sp(pExecInfo->lpDirectory);
			}
			else
			{
				directory = Path::GetDirectoryName(path);
			}

			Kernel32<C>::StartupInfo si;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof(pi));
			string command = String::tstr<C>("\"") + path + String::tstr<C>("\"");
			if (pExecInfo->lpParameters)
			{
				command += C(' ');
				command += pExecInfo->lpParameters;
			}
			x.result = Kernel32<C>::CreateProcess(path, &command[0], NULL, NULL, false,
				NORMAL_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE,
				NULL, directory.c_str(), &si, &pi
			);
			pExecInfo->hProcess = pi.hProcess;
			pExecInfo->hInstApp = x.result ? (HINSTANCE)100 : (HINSTANCE)(GetLastError() & 0x1F);
			return x.result;
		}
	}
	auto origf = pExecInfo->lpFile;
	auto origd = pExecInfo->lpDirectory;
	pExecInfo->lpFile = file.c_str();
	if (pExecInfo->lpDirectory)
	{
		directory = cnv<C>::sp(pExecInfo->lpDirectory);
		pExecInfo->lpDirectory = directory.c_str();
	}
	x.result = orig(ShellExecuteEx)(pExecInfo);
	pExecInfo->lpFile = origf;
	pExecInfo->lpDirectory = origd;
	return x.result;
}

static HINSTANCE WINAPI ShellExecute(
	HWND hwnd,
	const C *lpVerb,
	const C *lpFile,
	const C *lpParameters,
	const C *lpDirectory,
	INT nShowCmd
)
{
	auto x = info<HINSTANCE>(add_suffix<C>("ShellExecute"), hwnd, lpVerb, lpFile, lpParameters, lpDirectory, nShowCmd);
	ShellExecuteInfo si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.lpVerb = lpVerb;
	si.lpFile = lpFile;
	si.lpParameters = lpParameters;
	si.lpDirectory = lpDirectory;
	si.nShow = nShowCmd;
	ShellExecuteEx(&si);
	return x.result = si.hInstApp;
}
//FindExecutable
