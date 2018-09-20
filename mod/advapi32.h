#include <shlobj.h>
#include "utility/registry.h"

template<class C> struct Advapi32
{
	typedef std::basic_string<C> string;
	struct NativeRegistry
	{
		HKEY key;
		LONG result;
		NativeRegistry(const std::wstring &s)
		{
			key = NULL;
			std::wstring sub;
			HKEY hk;
			std::wstring::size_type pos;
			if ((pos = s.find(L"HKEY_LOCAL_MACHINE")) != std::wstring::npos)
			{
				hk = HKEY_LOCAL_MACHINE;
				sub = s.substr(pos + 19);
			}
			else if ((pos = s.find(L"HKEY_CLASSES_ROOT")) != std::wstring::npos)
			{
				hk = HKEY_CLASSES_ROOT;
				sub = s.substr(pos + 18);
			}
			else if ((pos = s.find(L"HKEY_CURRENT_CONFIG")) != std::wstring::npos)
			{
				hk = HKEY_CURRENT_CONFIG;
				sub = s.substr(pos + 20);
			}
			else
			{
				result = ERROR_FILE_NOT_FOUND;
				return;
			}

			result = orig(Advapi32<wchar_t>::RegOpenKeyEx)(hk, sub.c_str(), 0, KEY_READ, &key);
		}
		~NativeRegistry()
		{
			orig(Advapi32<wchar_t>::RegCloseKey)(key);
		}
	};

	static bool isNativeHKey(HKEY hKey)
	{
		return (hKey >= HKEY_CLASSES_ROOT) || !((pointer_size_uint)hKey >> 16);
	}

	static std::wstring GetDirectoryName(HKEY hKey)
	{
		std::wstring rpath;
		if (hKey >= HKEY_CLASSES_ROOT)
		{
			switch ((DWORD)hKey)
			{
			case HKEY_CLASSES_ROOT:
				rpath = Path::Combine(cnv<wchar_t>::sandbox, config.wstr("reg"), L"HKEY_CLASSES_ROOT");
				break;
			case HKEY_CURRENT_CONFIG:
				rpath = Path::Combine(cnv<wchar_t>::sandbox, config.wstr("reg"), L"HKEY_CURRENT_CONFIG");
				break;
			case HKEY_CURRENT_USER:
				rpath = Path::Combine(cnv<wchar_t>::sandbox, config.wstr("reg"), L"HKEY_CURRENT_USER");
				break;
			case HKEY_LOCAL_MACHINE:
				rpath = Path::Combine(cnv<wchar_t>::sandbox, config.wstr("reg"), L"HKEY_LOCAL_MACHINE");
				break;
			case HKEY_USERS:
				rpath = Path::Combine(cnv<wchar_t>::sandbox, config.wstr("reg"), L"HKEY_USERS");
				break;
			default:
				rpath = Path::Combine(cnv<wchar_t>::sandbox, config.wstr("reg"), L"_");
				break;
			}
		}
		else if ((pointer_size_uint)hKey >> 16)
		{
			rpath = ((RegistryKey*)hKey)->path;
		}
		else
		{
			log(1, "native HKEY?:", FuncInfo::hex(hKey));
			rpath = Path::Combine(cnv<wchar_t>::sandbox, config.wstr("reg"), L"_");
		}
		return rpath;
	}

	static std::wstring replace_invalid(const std::wstring &path)
	{
		std::wstring str = path;
		const std::wstring invalid = L":/*?\"<>|";
		auto pos = str.find_first_of(invalid, 2);
		while (pos != std::wstring::npos)
		{
			str[pos] = L'$';
			pos = str.find_first_of(invalid, pos + 1);
		}
		return str;
	}

	static bool DirectoryExists(const std::wstring &s)
	{
		auto attr = orig(Kernel32<wchar_t>::GetFileAttributes)(s.c_str());
		return (attr != -1 && (attr & FILE_ATTRIBUTE_DIRECTORY));
	}

	#include "advapi32/reg.h"
	#include "advapi32/security.h"

	static void Attach()
	{
		HMODULE advapi32 = LoadLibraryA("advapi32");
		{
			auto lib = std::string("api-ms-win-core-localregistry-") + config.str("api-ms-win-core-localregistry", "");
			HMODULE registry = ::LoadLibraryA(lib.c_str());
			log(7, "try to load ", lib);
			if (!registry)
			{
				registry = advapi32;
			}
			else
			{
				log(7, "use ", lib);
			}
			reg(registry, add_suffix<C>("RegOpenKeyEx"), RegOpenKeyEx);
			reg(registry, add_suffix<C>("RegOpenKey"), RegOpenKey);
			reg(registry, "RegOpenCurrentUser", RegOpenCurrentUser);
			reg(registry, "RegOpenUserClassesRoot", RegOpenUserClassesRoot);
			reg(registry, "RegCloseKey", RegCloseKey);
			reg(registry, "RegFlushKey", RegFlushKey);

			reg(registry, add_suffix<C>("RegCreateKeyEx"), RegCreateKeyEx);
			reg(registry, add_suffix<C>("RegCreateKey"), RegCreateKey);
			reg(registry, add_suffix<C>("RegDeleteKeyEx"), RegDeleteKeyEx);
			reg(registry, add_suffix<C>("RegDeleteKey"), RegDeleteKey);
			reg(registry, add_suffix<C>("RegDeleteValue"), RegDeleteValue);

			reg(registry, add_suffix<C>("RegEnumKeyEx"), RegEnumKeyEx);
			reg(registry, add_suffix<C>("RegEnumKey"), RegEnumKey);
			reg(registry, add_suffix<C>("RegEnumValue"), RegEnumValue);
			reg(registry, add_suffix<C>("RegQueryValueEx"), RegQueryValueEx);
			reg(registry, add_suffix<C>("RegQueryValue"), RegQueryValue);
			reg(registry, add_suffix<C>("RegSetValueEx"), RegSetValueEx);
			reg(registry, add_suffix<C>("RegSetValue"), RegSetValue);
			reg(registry, add_suffix<C>("RegQueryInfoKey"), RegQueryInfoKey);

			reg(registry, add_suffix<C>("RegQueryMultipleValues"), RegQueryMultipleValues);
			reg(registry, add_suffix<C>("RegSaveKey"), RegSaveKey);
			reg(registry, add_suffix<C>("RegRestoreKey"), RegRestoreKey);
			reg(registry, add_suffix<C>("RegLoadKey"), RegLoadKey);
			reg(registry, add_suffix<C>("RegReplaceKey"), RegReplaceKey);
			reg(registry, add_suffix<C>("RegUnLoadKey"), RegUnLoadKey);
			reg(registry, "RegNotifyChangeKeyValue", RegNotifyChangeKeyValue);
			reg(registry, "RegOverridePredefKey", RegOverridePredefKey);
			reg(registry, add_suffix<C>("RegConnectRegistry"), RegConnectRegistry);
		}

		{
			auto lib = std::string("api-ms-win-security-base-") + config.str("api-ms-win-security-base", "L1-1-0");
			HMODULE security = ::LoadLibraryA(lib.c_str());
			log(7, "try to load ", lib);
			if (!security)
			{
				security = advapi32;
			}
			else
			{
				log(7, "use ", lib);
			}
			reg(security, add_suffix<C>("GetFileSecurity"), GetFileSecurity);
			reg(security, add_suffix<C>("SetFileSecurity"), SetFileSecurity);
		}
	}
};
//GetUserName
//CreateProcessAsUser
//CreateProcessWithLogonW
//CreateProcessWithTokenW
//CreateService
