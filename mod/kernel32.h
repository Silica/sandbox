template<class C> struct Kernel32
{
	typedef std::basic_string<C> string;
	template<class T>
	struct subtypes
	{
		typedef OSVERSIONINFOEXA OSVersionInfo;
		typedef STARTUPINFOA StartupInfo;
	};
	template<>
	struct subtypes<wchar_t>
	{
		typedef OSVERSIONINFOEXW OSVersionInfo;
		typedef STARTUPINFOW StartupInfo;
	};
	typedef typename subtypes<C>::OSVersionInfo OSVersionInfo;
	typedef typename subtypes<C>::StartupInfo StartupInfo;

	#include "kernel32/file.h"
	#include "kernel32/drive.h"
	#include "kernel32/path.h"
	#include "kernel32/system.h"
	#include "kernel32/ini.h"
	#include "kernel32/process.h"
	#include "kernel32/lz.h"
	#include "kernel32/mutex.h"

	static void Attach()
	{
		HMODULE kernel32 = ::GetModuleHandleA("kernel32");
		{
			auto lib = std::string("api-ms-win-core-file-") + config.str("api-ms-win-core-file", "L1-1-0");
			HMODULE file = ::LoadLibraryA(lib.c_str());
			log(7, "try to load ", lib);
			if (!file)
			{
				file = kernel32;
			}
			else
			{
				log(7, "use ", lib);
			}
			reg(file, add_suffix<C>("CreateFile"), CreateFile);
			reg(kernel32, add_suffix<C>("MoveFile"), MoveFile);
			reg(file, add_suffix<C>("MoveFileEx"), MoveFileEx);
			reg(file, add_suffix<C>("CopyFile"), CopyFile);
			reg(file, add_suffix<C>("CopyFileEx"), CopyFileEx);
			reg(file, add_suffix<C>("DeleteFile"), DeleteFile);
			reg(file, add_suffix<C>("ReplaceFile"), ReplaceFile);
			reg(file, add_suffix<C>("CreateDirectory"), CreateDirectory);
			reg(file, add_suffix<C>("CreateDirectoryEx"), CreateDirectoryEx);
			reg(file, add_suffix<C>("RemoveDirectory"), RemoveDirectory);
			reg(file, add_suffix<C>("FindFirstFile"), FindFirstFile);
			reg(file, add_suffix<C>("FindFirstFileEx"), FindFirstFileEx);
			reg(file, add_suffix<C>("SetFileAttributes"), SetFileAttributes);
			reg(file, add_suffix<C>("GetFileAttributes"), GetFileAttributes);
			reg(file, add_suffix<C>("GetFileAttributesEx"), GetFileAttributesEx);
			reg(file, add_suffix<C>("GetTempFileName"), GetTempFileName);

			reg(file, "GetLogicalDrives", GetLogicalDrives);
			reg(file, add_suffix<C>("GetLogicalDriveStrings"), GetLogicalDriveStrings);
			reg(file, add_suffix<C>("GetDriveType"), GetDriveType);
			reg(file, add_suffix<C>("GetDiskFreeSpace"), GetDiskFreeSpace);
			reg(file, add_suffix<C>("GetDiskFreeSpaceEx"), GetDiskFreeSpaceEx);
			reg(file, add_suffix<C>("GetVolumePathName"), GetVolumePathName);
		}
		reg(kernel32, "_lopen", _lopen);
		reg(kernel32, "OpenFile", OpenFile);

		reg(kernel32, add_suffix<C>("GetPrivateProfileString"), GetPrivateProfileString);
		reg(kernel32, add_suffix<C>("GetPrivateProfileInt"), GetPrivateProfileInt);
		reg(kernel32, add_suffix<C>("GetPrivateProfileStruct"), GetPrivateProfileStruct);
		reg(kernel32, add_suffix<C>("GetPrivateProfileSection"), GetPrivateProfileSection);
		reg(kernel32, add_suffix<C>("GetPrivateProfileSectionNames"), GetPrivateProfileSectionNames);
		reg(kernel32, add_suffix<C>("WritePrivateProfileString"), WritePrivateProfileString);
		reg(kernel32, add_suffix<C>("WritePrivateProfileStruct"), WritePrivateProfileStruct);
		reg(kernel32, add_suffix<C>("WritePrivateProfileSection"), WritePrivateProfileSection);
		reg(kernel32, add_suffix<C>("GetProfileString"), GetProfileString);
		reg(kernel32, add_suffix<C>("GetProfileInt"), GetProfileInt);
		reg(kernel32, add_suffix<C>("GetProfileSection"), GetProfileSection);
		reg(kernel32, add_suffix<C>("WriteProfileString"), WriteProfileString);
		reg(kernel32, add_suffix<C>("WriteProfileSection"), WriteProfileSection);

		{
			auto lib = std::string("api-ms-win-core-processenvironment-") + config.str("api-ms-win-core-processenvironment", "L1-1-0");
			HMODULE processenvironment = ::LoadLibraryA(lib.c_str());
			log(7, "try to load ", lib);
			if (!processenvironment)
			{
				processenvironment = kernel32;
			}
			else
			{
				log(7, "use ", lib);
			}
			reg(processenvironment, add_suffix<C>("SetCurrentDirectory"), SetCurrentDirectory);
			reg(processenvironment, add_suffix<C>("GetCurrentDirectory"), GetCurrentDirectory);
			if (!reg(processenvironment, add_suffix<C>("GetTempPath"), GetTempPath))
				reg(kernel32, add_suffix<C>("GetTempPath"), GetTempPath);
			reg(processenvironment, add_suffix<C>("GetWindowsDirectory"), GetWindowsDirectory);
			reg(processenvironment, add_suffix<C>("GetSystemWindowsDirectory"), GetSystemWindowsDirectory);
			reg(processenvironment, add_suffix<C>("GetSystemDirectory"), GetSystemDirectory);
			if (!reg(processenvironment, add_suffix<C>("GetSystemWow64Directory"), GetSystemWow64Directory))
				reg(kernel32, add_suffix<C>("GetSystemWow64Directory"), GetSystemWow64Directory);
		}

		reg(kernel32, add_suffix<C>("GetModuleFileName"), GetModuleFileName);
		reg(kernel32, add_suffix<C>("GetFullPathName"), GetFullPathName);
		reg(kernel32, add_suffix<C>("GetFinalPathNameByHandle"), GetFinalPathNameByHandle);

		reg(kernel32, add_suffix<C>("SearchPath"), SearchPath);

		reg(kernel32, add_suffix<C>("GetShortPathName"), GetShortPathName);
		reg(kernel32, add_suffix<C>("GetLongPathName"), GetLongPathName);

		{
			auto lib = std::string("api-ms-win-core-sysinfo-") + config.str("api-ms-win-core-sysinfo", "L1-1-0");
			HMODULE sysinfo = ::LoadLibraryA(lib.c_str());
			log(7, "try to load ", lib);
			if (!sysinfo)
			{
				sysinfo = kernel32;
			}
			else
			{
				log(7, "use ", lib);
			}
			reg(sysinfo, "GetSystemInfo", GetSystemInfo);
			if (!reg(sysinfo, "GetNativeSystemInfo", GetNativeSystemInfo))
				reg(kernel32, "GetNativeSystemInfo", GetNativeSystemInfo);
			reg(sysinfo, "GetSystemTime", GetSystemTime);
			reg(sysinfo, "GetLocalTime", GetLocalTime);
			reg(sysinfo, "GetSystemTimeAsFileTime", GetSystemTimeAsFileTime);
			reg(sysinfo, add_suffix<C>("GetVersionEx"), GetVersionEx);
			reg(sysinfo, "GetVersion", GetVersion);
			reg(kernel32, add_suffix<C>("VerifyVersionInfo"), VerifyVersionInfo);
			if (!reg(sysinfo, "GetProductInfo", GetProductInfo))
				reg(kernel32, "GetProductInfo", GetProductInfo);
			reg(sysinfo, "GetLogicalProcessorInformation", GetLogicalProcessorInformation);
			reg(sysinfo, "GetLogicalProcessorInformationEx", GetLogicalProcessorInformationEx);
			if (!reg(sysinfo, "IsProcessorFeaturePresent", IsProcessorFeaturePresent))
				reg(kernel32, "IsProcessorFeaturePresent", IsProcessorFeaturePresent);
			reg(sysinfo, add_suffix<C>("GetEnvironmentVariable"), GetEnvironmentVariable);
			reg(sysinfo, add_suffix<C>("ExpandEnvironmentStrings"), ExpandEnvironmentStrings);
			reg(sysinfo, add_suffix<C>("GetComputerName"), GetComputerName);
			reg(sysinfo, add_suffix<C>("GetComputerNameEx"), GetComputerNameEx);
		}

		reg(kernel32, add_suffix<C>("LZOpenFile"), LZOpenFile);

		{
			auto lib = std::string("api-ms-win-core-processthreads-") + config.str("api-ms-win-core-processthreads", "L1-1-0");
			HMODULE processthreads = ::LoadLibraryA(lib.c_str());
			log(7, "try to load ", lib);
			if (!processthreads)
			{
				processthreads = kernel32;
			}
			else
			{
				log(7, "use ", lib);
			}
			reg(processthreads, add_suffix<C>("CreateProcess"), CreateProcess);
		}

		reg(kernel32, add_suffix<C>("GetStartupInfo"), GetStartupInfo);
		reg(kernel32, "WinExec", WinExec);
		reg(kernel32, add_suffix<C>("GetBinaryType"), GetBinaryType);

		//debug
		reg(kernel32, "IsDebuggerPresent", IsDebuggerPresent);

		//misc
		reg(kernel32, "IsWow64Process", IsWow64Process);

		//api-ms-win-core-synch
		reg(kernel32, add_suffix<C>("CreateMutex"), CreateMutex);
		reg(kernel32, add_suffix<C>("CreateMutexEx"), CreateMutexEx);
		reg(kernel32, add_suffix<C>("OpenMutex"), OpenMutex);
	}
};
