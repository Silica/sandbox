#include <Windows.h>
#include <TlHelp32.h>
#include <DbgHelp.h>
#include <delayimp.h>
#include <ShlObj.h>

#include <string>
#include <map>

#pragma comment(lib, "DbgHelp")
#pragma comment(lib, "shell32")
#pragma comment(lib, "advapi32")

#include "include/string.h"
#include "include/path.h"
#include "include/directory.h"

#ifdef _WIN64
typedef unsigned long long pointer_size_uint;
#else
typedef unsigned long pointer_size_uint;
#endif

template<class T> T *orig(T *m)
{
	#ifdef _DEBUG
	if (!mod_to_orig.count(m))
	{
		log(-1, "orig error:", FuncInfo::hex(m));
		return m;
	}
	#endif
	return (T*)mod_to_orig[m];
}

#include "utility/config.h"
#include "utility/cnvpath.h"
#include "utility/saveerror.h"
#include "utility/funcinfo.h"

Config config;
HANDLE hlog = NULL;
std::string cnv<char>::sandbox;
std::string cnv<char>::drive;
std::string cnv<char>::vuser;
std::string cnv<char>::suser;
std::string cnv<char>::mutex;
std::wstring cnv<wchar_t>::sandbox;
std::wstring cnv<wchar_t>::drive;
std::wstring cnv<wchar_t>::vuser;
std::wstring cnv<wchar_t>::suser;
std::wstring cnv<wchar_t>::mutex;
int loglevel = 0;

std::string exename32;
std::string dllname32;
std::string exename64;
std::string dllname64;

void initname(const std::wstring &name)
{
	auto basename = String::replace(String::tstr<char>(name), "64", "");
	exename32 = basename + ".exe";
	dllname32 = basename + ".dll";
	exename64 = basename + "64.exe";
	dllname64 = basename + "64.dll";
}

void log(int level, const std::string &str)
{
	SaveError e;

	if (loglevel < level)
		return;

	auto s = String::trim(str);
	auto th = GetCurrentThreadId();

	printf("log:%X:%s\n", th, s.c_str());

	if (hlog && hlog != INVALID_HANDLE_VALUE)
	{
		s = FuncInfo::hex(th) + ":" + s + "\r\n";
		DWORD size = s.size();
		WriteFile(hlog, s.c_str(), s.size(), &size, 0);
	}
}
template<class... Args>
static void log(int loglevel, Args... args)
{
	log(loglevel, String::concat(args...));
}

std::string GetLastErrorString()
{
	std::string msg(MAX_PATH, 0);
	auto size = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, &msg[0], MAX_PATH, NULL);
	msg.resize(size);
	return msg;
}

void moduleIAT(HMODULE m, void *dest, void *src)
{
	ULONG size;
	PIMAGE_IMPORT_DESCRIPTOR imp = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(m, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size);
	if (imp)
	{
		while (imp->FirstThunk)
		{
			PIMAGE_THUNK_DATA td = (PIMAGE_THUNK_DATA)((BYTE*)m + imp->FirstThunk);
			while (td->u1.Function)
			{
				PROC *p = (PROC*)&td->u1.Function;
				if (*p == src)
				{
					DWORD d;
					VirtualProtect(p, sizeof(p), PAGE_EXECUTE_READWRITE, &d);
					*p = (PROC)dest;
					VirtualProtect(p, sizeof(p), d, &d);
				}
				td++;
			}
			imp++;
		}
	}
	/*PImgDelayDescr dd = (PImgDelayDescr)ImageDirectoryEntryToData(m, TRUE, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT, &size);
	if (dd)
	{
		while (dd->rvaIAT)
		{
			PIMAGE_THUNK_DATA td = (PIMAGE_THUNK_DATA)((BYTE*)m + dd->rvaIAT);
			while (td->u1.Function)
			{
				PROC *p = (PROC*)&td->u1.Function;
				if (*p == src)
				{
					DWORD d;
					VirtualProtect(p, sizeof(p), PAGE_EXECUTE_READWRITE, &d);
					*p = (PROC)dest;
					VirtualProtect(p, sizeof(p), d, &d);
				}
				td++;
			}
			dd++;
		}
	}*/
}

void IAT(void *dest, void *src)
{
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if (h < 0)
	{
		log(0, "error:CreateToolhelp32Snapshot:", GetLastErrorString());
		return;
	}

	MODULEENTRY32 me;
	me.dwSize = sizeof(me);

	BOOL b = Module32First(h, &me);
	while (b)
	{
		moduleIAT(me.hModule, dest, src);
		b = Module32Next(h, &me);
	}

	CloseHandle(h);
}

std::map<void*, void*> orig_to_mod;
std::map<void*, void*> mod_to_orig;

bool reg(HMODULE h, const std::string name, void *m)
{
	void *o = GetProcAddress(h, name.c_str());
	if (o)
	{
		#ifdef _DEBUG
		if (o == m)
		{
			log(1, "reg original function?:", name);
		}
		#endif
		orig_to_mod[o] = m;
		mod_to_orig[m] = o;
		IAT(m, o);
		log(9, "reg:", name, " mod=", FuncInfo::hex(m), " orig=", FuncInfo::hex(o));
	}
	else
	{
		log(1, "reg error:", name);
	}
	return o;
}

template<class T> std::string add_suffix(const char *name)
{
	std::string str = name;
	str += "A";
	return str;
}
template<> std::string add_suffix<wchar_t>(const char *name)
{
	std::string str = name;
	str += "W";
	return str;
}

struct vae
{
	HANDLE p;
	void *m;
	DWORD s;
	vae(HANDLE h, DWORD size)
	{
		p = h;
		s = size;
		m = VirtualAllocEx(h, NULL, size, MEM_COMMIT, PAGE_READWRITE);
	}
	~vae()
	{
		if (m)
			VirtualFreeEx(p, m, s, MEM_DECOMMIT);
	}
};

int inject(HANDLE ph, const std::string &dllpath)
{
	vae v(ph, dllpath.size() + 1);
	if (!v.m)
	{
		log(0, "error:VirtualAllocEx:", GetLastErrorString());
		return 1;
	}
	if (!WriteProcessMemory(ph, v.m, dllpath.c_str(), dllpath.size() + 1, NULL))
	{
		log(0, "error:WriteProcessMemory:", GetLastErrorString());
		return 2;
	}

	HMODULE kernel32 = GetModuleHandleA("kernel32");
	FARPROC loadlibrary = orig(GetProcAddress)(kernel32, "LoadLibraryA");

	HANDLE h = CreateRemoteThread(ph, NULL, 0, (LPTHREAD_START_ROUTINE)loadlibrary, v.m, 0, NULL);
	if (!h)
	{
		auto e = GetLastError();
		log(0, "error:CreateRemoteThread:", GetLastErrorString());
		if (e == 5)
		{
			return 64;
		}
		else
		{
			return 3;
		}
	}
	WaitForSingleObject(h, INFINITE);
	log(0, "hook success");
	CloseHandle(h);

	return 0;
}

void initval(const std::wstring &modulepath)
{
	cnv<wchar_t>::sandbox = Path::GetDirectoryName(modulepath);
	cnv<wchar_t>::sandbox = Path::RemoveBackslash(cnv<wchar_t>::sandbox);
	cnv<char>::sandbox = String::tstr<char>(cnv<wchar_t>::sandbox);

	config = Config::Read(Path::Combine(cnv<char>::sandbox, "sandbox.cfg"));

	loglevel = config.Int("loglevel");

	cnv<char>::drive = config.str("drive", "C:");
	cnv<wchar_t>::drive = config.wstr("drive", L"C:");
	cnv<char>::vuser = Path::RemoveBackslash(config.str("virtual_user_directory", "C:\\Users\\SANDBOX"));
	cnv<wchar_t>::vuser = config.wstr("virtual_user_directory", L"C:\\Users\\SANDBOX");
	char path[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path);
	cnv<char>::suser = Path::RemoveBackslash(path);
	cnv<wchar_t>::suser = String::tstr<wchar_t>(cnv<char>::suser);
	cnv<char>::mutex = String::replace(config.str("mutex", cnv<char>::sandbox), "\\", "_");
	cnv<wchar_t>::mutex = String::tstr<wchar_t>(cnv<char>::mutex);

	SetEnvironmentVariable("SANDBOX", cnv<char>::sandbox.c_str());
	//SetEnvironmentVariable("USERNAME", config.str("USERNAME").c_str());
	//SetEnvironmentVariable("COMPUTERNAME", config.str("COMPUTERNAME").c_str());
	log(0, "sandbox:", cnv<char>::sandbox);
}

void Init(HMODULE module)
{
	wchar_t modulepath[MAX_PATH];
	GetModuleFileNameW(module, modulepath, MAX_PATH);

	initname(Path::GetFileNameWithoutExtension(modulepath));
	initval(modulepath);

	if (config.exists("TEMP"))
	{
		SetEnvironmentVariable("TEMP", config.str("TEMP").c_str());
		SetEnvironmentVariable("TMP", config.str("TEMP").c_str());
		config.config["temp"] = Path::AddBackslash(config.str("TEMP"));
	}

	if (config.dword("logfile"))
	{
		char exepath[MAX_PATH];
		GetModuleFileNameA(NULL, exepath, sizeof(exepath));
		char logfile[MAX_PATH];
		SYSTEMTIME st;
		GetLocalTime(&st);
		auto f = Path::GetFileName(exepath);
		sprintf(logfile, "%s_%.2d-%.2d-%.2d_%.2d%.2d%.2d.log", f.c_str(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		auto p = Path::Combine(cnv<char>::sandbox, logfile);
		hlog = CreateFileA(p.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	}

	if (config.dword("console"))
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout); 
	}
}

void Final()
{
	log(0, "detach sandbox");
	if (hlog && hlog != INVALID_HANDLE_VALUE)
		CloseHandle(hlog);
}

#include "mod/kernel32.h"
#include "mod/advapi32.h"
#include "mod/shell32.h"
#include "mod/ole32.h"
#include "mod/user32.h"
#include "mod/shlwapi.h"
#include "mod/imagehlp.h"
#include "mod/version.h"
#include "mod/psapi.h"
#include "mod/winmm.h"
#include "mod/msi.h"
#include "mod/msvcrt.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		Init(hinstDLL);
		Kernel32<char>::Attach();
		Kernel32<wchar_t>::Attach();
		Advapi32<char>::Attach();
		Advapi32<wchar_t>::Attach();
		Shell32<char>::Attach();
		Shell32<wchar_t>::Attach();
		Ole32::Attach();
		User32<char>::Attach();
		User32<wchar_t>::Attach();
		Shlwapi<char>::Attach();
		Shlwapi<wchar_t>::Attach();
		Imagehlp::Attach();
		Version<char>::Attach();
		Version<wchar_t>::Attach();
		Psapi<char>::Attach();
		Psapi<wchar_t>::Attach();
		Winmm<char>::Attach();
		Winmm<wchar_t>::Attach();
		Msi<char>::Attach();
		Msi<wchar_t>::Attach();
		MSVCRT::Attach();
		{
			auto kernel32 = GetModuleHandle("kernel32");
			auto lib = std::string("api-ms-win-core-libraryloader-") + config.str("api-ms-win-core-libraryloader", "L1-1-0");
			auto libraryloader = LoadLibrary(lib.c_str());
			if (!libraryloader)
				libraryloader = kernel32;
			else
				log(7, "load ", lib);

			if (!reg(libraryloader, "LoadLibraryA", Kernel32<char>::LoadLibrary))
				reg(kernel32, "LoadLibraryA", Kernel32<char>::LoadLibrary);
			if (!reg(libraryloader, "LoadLibraryW", Kernel32<wchar_t>::LoadLibrary))
				reg(kernel32, "LoadLibraryA", Kernel32<char>::LoadLibrary);
			reg(libraryloader, "LoadLibraryExA", Kernel32<char>::LoadLibraryEx);
			reg(libraryloader, "LoadLibraryExW", Kernel32<wchar_t>::LoadLibraryEx);
			reg(libraryloader, "GetModuleHandleA", Kernel32<char>::GetModuleHandle);
			reg(libraryloader, "GetModuleHandleW", Kernel32<wchar_t>::GetModuleHandle);
			reg(libraryloader, "GetProcAddress", Kernel32<char>::GetProcAddress);
		}
		break;
	case DLL_PROCESS_DETACH:
		Final();
		for (auto &x : orig_to_mod)
		{
			IAT(x.first, x.second);
		}
		break;
	}

	return TRUE;
}

void InitEnv()
{
	DllMain(NULL, DLL_PROCESS_ATTACH, NULL);

	char path[MAX_PATH];
	GetTempPath(MAX_PATH, path);
	SHCreateDirectoryEx(NULL, path, NULL);

	SHGetFolderPath(NULL, CSIDL_ALTSTARTUP | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_ALTSTARTUP | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_FAVORITES | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_WINDOWS | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_SYSTEM | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_SYSTEMX86 | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_PROFILE | CSIDL_FLAG_CREATE, NULL, 0, path);

	HKEY key;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", 0, NULL, 0, 0, NULL, &key, NULL);
	RegCloseKey(key);
	RegCreateKeyEx(HKEY_CLASSES_ROOT, "CLSID", 0, NULL, 0, 0, NULL, &key, NULL);
	RegCloseKey(key);

	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILESX86 | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES_COMMON | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES_COMMONX86 | CSIDL_FLAG_CREATE, NULL, 0, path);

	SHGetFolderPath(NULL, CSIDL_ADMINTOOLS | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_INTERNET_CACHE | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_CDBURN_AREA | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COOKIES | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_DESKTOP | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_FAVORITES | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_FONTS | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_HISTORY | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_MYMUSIC | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_MYPICTURES | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_MYVIDEO | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_NETHOOD | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_PRINTHOOD | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_PROGRAMS | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_RECENT | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_SENDTO | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_STARTMENU | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_STARTUP | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_TEMPLATES | CSIDL_FLAG_CREATE, NULL, 0, path);

	SHGetFolderPath(NULL, CSIDL_COMMON_ADMINTOOLS | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_DOCUMENTS | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_PROGRAMS | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_STARTMENU | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_STARTUP | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_TEMPLATES | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_MUSIC | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_PICTURES | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_VIDEO | CSIDL_FLAG_CREATE, NULL, 0, path);
	SHGetFolderPath(NULL, CSIDL_COMMON_OEM_LINKS | CSIDL_FLAG_CREATE, NULL, 0, path);

	{
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		orig(Kernel32<char>::CreateProcess)(NULL, "cmd /c mklink /j \"Users\\SANDBOX\\Application Data\" Users\\SANDBOX\\AppData\\Roaming", NULL, NULL, false, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi);
	}
	DllMain(NULL, DLL_PROCESS_DETACH, NULL);
}

std::wstring getcmdparam(int i, int argc, wchar_t **argv)
{
	std::wstring cp = L"";
	for (; i < argc; i++)
	{
		cp += L"\"";
		cp += argv[i];
		cp += L"\" ";
	}
	return cp;
}

int wmain(int argc, wchar_t **argv)
{
	if (argc == 1)
	{
		InitEnv();
		return 0;
	}

	wchar_t exepath[MAX_PATH];
	ZeroMemory(exepath, MAX_PATH * sizeof(wchar_t));
	GetModuleFileNameW(NULL, exepath, sizeof(exepath));
	initname(Path::GetFileNameWithoutExtension(exepath));
	initval(exepath);
	auto exedir = Path::GetDirectoryName(exepath);

	mod_to_orig[GetProcAddress] = GetProcAddress;

	bool suspend = false;
	int target_id = 0;
	std::wstring targetdir;

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == L'/')
		{
			switch (argv[i][1])
			{
			case L'd':
			case L'D':
				targetdir = argv[i];
				break;
			case L'p':
			case L'P':
			{
				int pid = std::stoi(argv[i] + 2);
				log(0, "pid=", pid);
				HANDLE p = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
				if (p)
				{
					BOOL wow64;
					IsWow64Process(p, &wow64);
					inject(p, Path::Combine(String::tstr<char>(cnv<wchar_t>::sandbox), wow64 ? dllname32 : dllname64));
					CloseHandle(p);
				}
				return pid;
			}
			case L's':
			case L'S':
				log(0, "process continue suspended");
				suspend = true;
				break;
			default:
				log(0, "unknown option");
			}
		}
		else if (!target_id)
		{
			target_id = i;
			break;
		}
	}

	if (target_id == 0)
	{
		log(0, "no target exe");
		return 0;
	}

	wchar_t targetpath[MAX_PATH];
	auto spr = SearchPathW(NULL, argv[target_id], L".exe", MAX_PATH, targetpath, NULL);
	if (!spr)
	{
		log(0, GetLastErrorString());
		return 0;
	}
	if (targetdir.empty())
		targetdir = Path::GetDirectoryName(targetpath);
	log(0, "target:", targetpath);

	auto cp = getcmdparam(target_id, argc, argv);
	log(0, "command:", cp);

	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	mod_to_orig[Kernel32<wchar_t>::CreateProcess] = CreateProcessW;
	mod_to_orig[Kernel32<wchar_t>::GetFileAttributes] = GetFileAttributesW;
	mod_to_orig[Kernel32<wchar_t>::IsWow64Process] = IsWow64Process;

	auto result = Kernel32<wchar_t>::CreateProcess(
		targetpath, &cp[0],
		NULL, NULL, false,
		NORMAL_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE | (suspend ? CREATE_SUSPENDED : 0),
		NULL, targetdir.c_str(), &si, &pi
	);
	if (!result)
	{
		auto e = GetLastError();
		if (e == 740 || e == 5)
		{
			ShellExecuteW(NULL, L"runas", exepath, &cp[0], targetdir.c_str(), SW_SHOWNORMAL);
		}
		else
		{
			log(0, GetLastErrorString());
		}
		return 0;
	}

	return GetProcessId(pi.hProcess);
}
