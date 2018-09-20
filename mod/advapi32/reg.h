static LONG WINAPI RegOpenKeyEx(
	HKEY hKey,
	const C *lpSubKey,
	DWORD ulOptions,
	REGSAM samDesired,
	PHKEY phkResult
)
{
	auto x = info<LONG>(add_suffix<C>("RegOpenKeyEx"), hKey, lpSubKey, ulOptions, samDesired, phkResult);

	auto rpath = lpSubKey ? Path::Combine(GetDirectoryName(hKey), String::tstr<wchar_t>(lpSubKey)) : GetDirectoryName(hKey);
	auto p = replace_invalid(rpath);
	if (!DirectoryExists(p))
	{
		NativeRegistry n(rpath);
		if (n.result)
		{
			x.l(6, rpath, " error native key result=", n.result);
			return x.result = n.result;
		}
	}
	if (phkResult)
	{
		*phkResult = (HKEY)new RegistryKey(rpath);
		x.l(6, rpath, " HKEY=", FuncInfo::hex(*phkResult));
	}
	else
	{
		x.l(6, rpath, " phkResult=NULL");
	}

	return x.result = ERROR_SUCCESS;
}
static LONG WINAPI RegOpenKey(
	HKEY hKey,
	const C *lpSubKey,
	PHKEY phkResult
)
{
	auto x = info<LONG>(add_suffix<C>("RegOpenKey"), hKey, lpSubKey, phkResult);
	return x.result = RegOpenKeyEx(hKey, lpSubKey, 0, 0, phkResult);
}

static LONG WINAPI RegOpenCurrentUser(
	REGSAM samDesired,
	PHKEY phkResult
)
{
	auto x = info<LONG>("RegOpenCurrentUser", samDesired, phkResult);
	return x.result = RegOpenKeyEx(HKEY_CURRENT_USER, NULL, 0, samDesired, phkResult);
}

static LONG WINAPI RegOpenUserClassesRoot(
	HANDLE hToken,
	DWORD dwOptions,
	REGSAM samDesired,
	PHKEY phkResult
)
{
	auto x = info<LONG>("RegOpenUserClassesRoot", hToken, dwOptions, samDesired, phkResult);
	return x.result = RegOpenKeyEx(HKEY_CLASSES_ROOT, NULL, 0, samDesired, phkResult);
}

static LONG WINAPI RegCloseKey(HKEY hKey)
{
	auto x = info<LONG>("RegCloseKey", hKey);
	if (hKey >= HKEY_CLASSES_ROOT)
	{
		return x.result = ERROR_SUCCESS;
	}
	if (isNativeHKey(hKey))
	{
		x.l(5, "native HKEY?:", FuncInfo::hex(hKey));
		return x.result = orig(RegCloseKey)(hKey);
	}
	RegistryKey *rk = (RegistryKey*)hKey;
	delete rk;
	return x.result = ERROR_SUCCESS;
}

static LONG WINAPI RegFlushKey(HKEY hKey)
{
	auto x = info<LONG>("RegFlushKey", hKey);
	return x.result = ERROR_SUCCESS;
}

static LONG WINAPI RegCreateKeyEx(
	HKEY hKey,
	const C *lpSubKey,
	DWORD Reserved,
	const C *lpClass,
	DWORD dwOptions,
	REGSAM samDesired,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY phkResult,
	LPDWORD lpdwDisposition
)
{
	auto x = info<LONG>(add_suffix<C>("RegCreateKeyEx"), hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	auto rpath = Path::Combine(GetDirectoryName(hKey), String::tstr<wchar_t>(lpSubKey));
	x.l(3, rpath);
	*phkResult = (HKEY)new RegistryKey(rpath);
	auto p = replace_invalid(rpath);
	if (DirectoryExists(p))
	{
		if (lpdwDisposition)
		{
			*lpdwDisposition = REG_OPENED_EXISTING_KEY;
		}
		x.result = ERROR_SUCCESS;
	}
	else
	{
		if (lpdwDisposition)
		{
			*lpdwDisposition = REG_CREATED_NEW_KEY;
		}
		x.result = SHCreateDirectory(NULL, p.c_str());
	}
	return x.result;
}
static LONG WINAPI RegCreateKey(
	HKEY hKey,
	const C *lpSubKey,
	PHKEY phkResult
)
{
	auto x = info<LONG>(add_suffix<C>("RegCreateKey"), hKey, lpSubKey, phkResult);
	return x.result = RegCreateKeyEx(hKey, lpSubKey, NULL, NULL, 0, 0, NULL, phkResult, NULL);
}

static LONG WINAPI RegDeleteKeyEx(
	HKEY hKey,
	const C *lpSubKey,
	REGSAM samDesired,
	DWORD Reserved
)
{
	auto x = info<LONG>(add_suffix<C>("RegDeleteKeyEx"), hKey, lpSubKey, samDesired, Reserved);
	auto rpath = Path::Combine(GetDirectoryName(hKey), String::tstr<wchar_t>(lpSubKey));
	auto p = replace_invalid(rpath);
	return x.result = !Directory::Delete(p, true);
}
static LONG WINAPI RegDeleteKey(
	HKEY hKey,
	const C *lpSubKey
)
{
	auto x = info<LONG>(add_suffix<C>("RegDeleteKey"), hKey, lpSubKey);
	auto rpath = Path::Combine(GetDirectoryName(hKey), String::tstr<wchar_t>(lpSubKey));
	auto p = replace_invalid(rpath);
	return x.result = !Directory::Delete(p, true);
}

static LONG WINAPI RegDeleteValue(
	HKEY hKey,
	const C *lpValueName
)
{
	auto x = info<LONG>(add_suffix<C>("RegDeleteValue"), hKey, lpValueName);
	auto rpath = Path::Combine(GetDirectoryName(hKey), String::tstr<wchar_t>(lpValueName));
	auto p = replace_invalid(rpath);
	return x.result = !orig(Kernel32<wchar_t>::DeleteFile)(p.c_str());
}

static LONG WINAPI RegEnumKeyEx(
	HKEY hKey,
	DWORD dwIndex,
	C *lpName,
	LPDWORD lpcName,
	LPDWORD lpReserved,
	C *lpClass,
	LPDWORD lpcClass,
	PFILETIME lpftLastWriteTime
)
{
	auto x = info<LONG>(add_suffix<C>("RegEnumKeyEx"), hKey, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime);
	auto rpath = GetDirectoryName(hKey);
	x.l(6, rpath);
	auto p = replace_invalid(rpath);
	if (!DirectoryExists(p))
	{
		NativeRegistry n(rpath);
		if (n.result == ERROR_SUCCESS)
		{
			x.result = orig(RegEnumKeyEx)(n.key, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime);
			x.l(5, "open native registry:", rpath);
			return x.result;
		}
		return x.result = n.result;
	}

	auto dirs = Directory::GetDirectoryInfos(p);
	if (dwIndex >= dirs.size())
		return x.result = ERROR_NO_MORE_ITEMS;

	if (lpftLastWriteTime)
	{
		*lpftLastWriteTime = dirs[dwIndex].ftLastWriteTime;
	}

	*lpcName = String::cpy(lpName, String::tstr<C>(dirs[dwIndex].cFileName), *lpcName);

	return x.result = ERROR_SUCCESS;
}
static LONG WINAPI RegEnumKey(
	HKEY hKey,
	DWORD dwIndex,
	C *lpName,
	DWORD cchName
)
{
	auto x = info<LONG>(add_suffix<C>("RegEnumKey"), hKey, dwIndex, lpName, cchName);
	return x.result = RegEnumKeyEx(hKey, dwIndex, lpName, &cchName, NULL, NULL, NULL, NULL);
}

static LONG WINAPI RegEnumValue(
	HKEY hKey,
	DWORD dwIndex,
	C *lpValueName,
	LPDWORD lpcValueName,
	LPDWORD lpReserved,
	LPDWORD lpType,
	LPBYTE lpData,
	LPDWORD lpcbData
)
{
	auto x = info<LONG>(add_suffix<C>("RegEnumValue"), hKey, dwIndex, lpValueName, lpcValueName, lpReserved, lpType, lpData, lpcbData);
	auto rpath = GetDirectoryName(hKey);
	x.l(6, rpath);
	auto p = replace_invalid(rpath);
	if (!DirectoryExists(p))
	{
		NativeRegistry n(rpath);
		if (n.result == ERROR_SUCCESS)
		{
			x.result = orig(RegEnumValue)(n.key, dwIndex, lpValueName, lpcValueName, lpReserved, lpType, lpData, lpcbData);
			x.l(5, "open native registry:", rpath);
			return x.result;
		}
		return x.result = n.result;
	}

	auto files = Directory::GetFiles(p);
	if (dwIndex >= files.size())
		return x.result = ERROR_NO_MORE_ITEMS;

	*lpcValueName = String::cpy(lpValueName, String::tstr<C>(files[dwIndex]), *lpcValueName);

	auto s = String::tstr<C>(files[dwIndex]);
	return x.result = RegQueryValueEx(hKey, s.c_str(), NULL, lpType, lpData, lpcbData);
}
static LONG WINAPI RegQueryValueEx(
	HKEY hKey,
	const C *lpValueName,
	LPDWORD lpReserved,
	LPDWORD lpType,
	LPBYTE lpData,
	LPDWORD lpcbData
)
{
	auto x = info<LONG>(add_suffix<C>("RegQueryValueEx"), hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
	const C *valuename = lpValueName;
	string defaultname = String::tstr<C>("@");
	if (!lpValueName || lpValueName[0] == 0)
	{
		valuename = defaultname.c_str();
	}
	auto rpath = GetDirectoryName(hKey);
	if (isNativeHKey(hKey))
	{
		x.result = orig(RegQueryValueEx)(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
		x.l(5, "native HKEY?:", FuncInfo::hex(hKey));
		return x.result;
	}
	auto f = replace_invalid(Path::Combine(rpath, String::tstr<wchar_t>(valuename)));
	x.l(6, f);

	auto h = orig(Kernel32<wchar_t>::CreateFile)(f.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		NativeRegistry n(rpath);
		if (n.result == ERROR_SUCCESS)
		{
			x.result = orig(RegQueryValueEx)(n.key, lpValueName, lpReserved, lpType, lpData, lpcbData);
			x.l(5, "open native registry:", rpath);
			return x.result;
		}
		return x.result = n.result;
	}

	DWORD dw;
	DWORD dwType;
	ReadFile(h, &dwType, sizeof(DWORD), &dw, NULL);
	if (lpType)
	{
		*lpType = dwType;
	}
	if (lpData || lpcbData)
	{
		DWORD size;
		ReadFile(h, &size, sizeof(DWORD), &dw, NULL);
		if ((sizeof(C) == sizeof(char)) && (dwType == REG_SZ || dwType == REG_MULTI_SZ || dwType == REG_EXPAND_SZ))
		{
			std::wstring ws(size, 0);
			ReadFile(h, &ws[0], size, &size, NULL);
			ws.resize(size);
			auto s = String::tstr<char>(ws);
			size = s.size() + 1;
			if (lpData)
			{
				if (*lpcbData < size)
				{
					size = *lpcbData;
				}
				String::cpy((char*)lpData, s, size);
			}
			*lpcbData = size;
		}
		else if (lpData)
		{
			if (*lpcbData < size)
				size = *lpcbData;
			ReadFile(h, lpData, size, &size, NULL);
			*lpcbData = size;
		}
		else if (lpcbData)
		{
			*lpcbData = size;
		}
	}
	CloseHandle(h);

	return x.result = ERROR_SUCCESS;
}
static LONG WINAPI RegQueryValue(
	HKEY hKey,
	const C *lpSubKey,
	C *lpData,
	PLONG lpcbData
)
{
	auto x = info<LONG>(add_suffix<C>("RegQueryValue"), hKey, lpSubKey, lpData, lpcbData);
	return x.result = RegQueryValueEx(hKey, lpSubKey, NULL, NULL, (LPBYTE)lpData, (LPDWORD)lpcbData);
}

static LONG WINAPI RegSetValueEx(
	HKEY hKey,
	const C *lpValueName,
	DWORD Reserved,
	DWORD dwType,
	CONST BYTE *lpData,
	DWORD cbData
)
{
	auto x = info<LONG>(add_suffix<C>("RegSetValueEx"), hKey, lpValueName, Reserved, dwType, lpData, cbData);
	const C *valuename = lpValueName;
	string defaultname = String::tstr<C>("@");
	if (!lpValueName || lpValueName[0] == 0)
	{
		valuename = defaultname.c_str();
	}
	auto rpath = GetDirectoryName(hKey);
	auto f = replace_invalid(Path::Combine(rpath, String::tstr<wchar_t>(valuename)));
	x.l(3, f);

	auto p = replace_invalid(rpath);
	if (!DirectoryExists(p))
		SHCreateDirectory(NULL, p.c_str());
	auto h = orig(Kernel32<wchar_t>::CreateFile)(f.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
		return x.result = ERROR_ACCESS_DENIED;

	DWORD dw;
	auto r = WriteFile(h, &dwType, sizeof(DWORD), &dw, NULL);

	if ((sizeof(C) == sizeof(char)) && (dwType == REG_SZ || dwType == REG_MULTI_SZ || dwType == REG_EXPAND_SZ))
	{
		//cut with cbData?
		auto ws = String::tstr<wchar_t>((C*)lpData);
		DWORD size = (ws.size() + 1) * sizeof(wchar_t);
		r &= WriteFile(h, &size, sizeof(DWORD), &dw, NULL);
		r &= WriteFile(h, ws.c_str(), size, &dw, NULL);
	}
	else
	{
		r &= WriteFile(h, &cbData, sizeof(DWORD), &dw, NULL);
		r &= WriteFile(h, lpData, cbData, &dw, NULL);
	}

	if (!r)
		x.result = GetLastError();
	else
		x.result = ERROR_SUCCESS;

	CloseHandle(h);

	return x.result;
}
static LONG WINAPI RegSetValue(
	HKEY hKey,
	const C *lpSubKey,
	DWORD dwType,
	const C *lpData,
	DWORD cbData
)
{
	auto x = info<LONG>(add_suffix<C>("RegSetValue"), hKey, lpSubKey, dwType, lpData, cbData);
	return x.result = RegSetValueEx(hKey, lpSubKey, 0, dwType, (const BYTE*)lpData, cbData);
}

static LONG WINAPI RegQueryInfoKey(
	HKEY hKey,
	C *lpClass,
	LPDWORD lpcClass,
	LPDWORD lpReserved,
	LPDWORD lpcSubKeys,
	LPDWORD lpcMaxSubKeyLen,
	LPDWORD lpcMaxClassLen,
	LPDWORD lpcValues,
	LPDWORD lpcMaxValueNameLen,
	LPDWORD lpcMaxValueLen,
	LPDWORD lpcbSecurityDescriptor,
	PFILETIME lpftLastWriteTime
)
{
	auto x = info<LONG>(add_suffix<C>("RegQueryInfoKey"), hKey, lpClass, lpcClass, lpReserved, lpcSubKeys, lpcMaxSubKeyLen, lpcMaxClassLen, lpcValues, lpcMaxValueNameLen, lpcMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime);
	auto rpath = GetDirectoryName(hKey);
	x.l(6, rpath);
	auto p = replace_invalid(rpath);
	if (!DirectoryExists(p))
	{
		NativeRegistry n(rpath);
		if (n.result == ERROR_SUCCESS)
		{
			auto result = orig(RegQueryInfoKey)(n.key, lpClass, lpcClass, lpReserved, lpcSubKeys, lpcMaxSubKeyLen, lpcMaxClassLen, lpcValues, lpcMaxValueNameLen, lpcMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime);
			x.l(5, "open native registry result=", result);
			return result;
		}
		return n.result;
	}

	auto dirs = Directory::GetDirectoryInfos(p);
	if (lpcSubKeys)
		*lpcSubKeys = dirs.size();
	if (lpcMaxSubKeyLen)
	{
		*lpcMaxSubKeyLen = 0;
		for (auto &d : dirs)
		{
			auto len = std::wstring(d.cFileName).size() + 1;
			if (*lpcMaxSubKeyLen < len)
				*lpcMaxSubKeyLen = len;
		}
	}
	auto files = Directory::GetFileInfos(p);
	if (lpcValues)
		*lpcValues = files.size();
	if (lpcMaxValueNameLen)
		*lpcMaxValueNameLen = 0;
	if (lpcMaxValueLen)
		*lpcMaxValueLen = 0;
	if (lpftLastWriteTime)
	{
		lpftLastWriteTime->dwHighDateTime = 0;
		lpftLastWriteTime->dwLowDateTime = 0;
	}
	for (auto &f : files)
	{
		if (lpcMaxValueNameLen)
		{
			auto l = std::wstring(f.cFileName).size() + 1;
			if (*lpcMaxValueNameLen < l)
				*lpcMaxValueNameLen = l;
		}
		if (lpcMaxValueLen)
		{
			if (*lpcMaxValueLen < f.nFileSizeLow)
				*lpcMaxValueLen = f.nFileSizeLow;
		}
		if (lpftLastWriteTime)
		{
			if (lpftLastWriteTime->dwHighDateTime < f.ftLastWriteTime.dwHighDateTime)
			{
				lpftLastWriteTime->dwHighDateTime = f.ftLastWriteTime.dwHighDateTime;
				lpftLastWriteTime->dwLowDateTime = f.ftLastWriteTime.dwLowDateTime;
			}
			else if (lpftLastWriteTime->dwHighDateTime == f.ftLastWriteTime.dwHighDateTime && lpftLastWriteTime->dwLowDateTime < f.ftLastWriteTime.dwLowDateTime)
			{
				lpftLastWriteTime->dwLowDateTime = f.ftLastWriteTime.dwLowDateTime;
			}
		}
	}

	if (dirs.empty() && files.empty())
		return x.result = ERROR_FILE_NOT_FOUND;

	return x.result = ERROR_SUCCESS;
}

static LONG WINAPI RegQueryMultipleValues(
	HKEY hKey,
	PVALENT val_list,
	DWORD num_vals,
	C *lpValueBuf,
	LPDWORD ldwTotsize
)
{
	log(9, add_suffix<C>("RegQueryMultipleValues"));
	return ERROR_CANTREAD;
}

static LONG WINAPI RegSaveKey(
	HKEY hKey,
	const C *lpFile,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	log(9, add_suffix<C>("RegSaveKey"));
	return ERROR_FILE_NOT_FOUND;
}

static LONG WINAPI RegRestoreKey(
	HKEY hKey,
	const C *lpFile,
	DWORD dwFlags
)
{
	log(9, add_suffix<C>("RegRestoreKey"));
	return ERROR_FILE_NOT_FOUND;
}

static LONG WINAPI RegLoadKey(
	HKEY hKey,
	const C *lpSubKey,
	const C *lpFile
)
{
	log(9, add_suffix<C>("RegLoadKey"));
	return ERROR_FILE_NOT_FOUND;
}

static LONG WINAPI RegReplaceKey(
	HKEY hKey,
	const C *lpSubKey,
	const C *lpNewFile,
	const C *lpOldFile
)
{
	log(9, add_suffix<C>("RegReplaceKey"));
	return ERROR_FILE_NOT_FOUND;
}

static LONG WINAPI RegUnLoadKey(
	HKEY hKey,
	const C *lpSubKey
)
{
	log(9, add_suffix<C>("RegUnLoadKey"));
	return ERROR_FILE_NOT_FOUND;
}

static LONG WINAPI RegNotifyChangeKeyValue(
	HKEY hKey,
	BOOL bWatchSubtree,
	DWORD dwNotifyFilter,
	HANDLE hEvent,
	BOOL fAsynchronous
)
{
	log(9, "RegNotifyChangeKeyValue");
	return ERROR_FILE_NOT_FOUND;
}

static LONG WINAPI RegOverridePredefKey(
	HKEY hKey,
	HKEY hNewHKey
)
{
	log(9, "RegOverridePredefKey");
	return ERROR_FILE_NOT_FOUND;
}

static LONG WINAPI RegConnectRegistry(
	const C *lpMachineName,
	HKEY hKey,
	PHKEY phkResult
)
{
	log(9, add_suffix<C>("RegConnectRegistry"));
	return ERROR_FILE_NOT_FOUND;
}
