static LSTATUS WINAPI SHSetValue(
	HKEY hkey,
	const C *pszSubKey,
	const C *pszValue,
	DWORD dwType,
	LPCVOID pvData,
	DWORD cbData
)
{
	auto x = info<LSTATUS>(add_suffix<C>("SHSetValue"), hkey, pszSubKey, pszValue, dwType, pvData, cbData);
	HKEY key;
	x.result = Advapi32<C>::RegCreateKeyEx(hkey, pszSubKey, 0, NULL, 0, KEY_WRITE, NULL, &key, NULL);
	if (x.result == ERROR_SUCCESS)
	{
		Advapi32<C>::RegSetValueEx(key, pszValue, 0, dwType, (BYTE*)pvData, cbData);
		Advapi32<C>::RegCloseKey(key);
	}
	return x.result;
}

static LSTATUS WINAPI SHDeleteKey(
	HKEY hkey,
	const C *pszSubKey
)
{
	auto x = info<LSTATUS>(add_suffix<C>("SHDeleteKey"), hkey, pszSubKey);
	return x.result = Advapi32<C>::RegDeleteKeyEx(hkey, pszSubKey, 0, 0);
}

static LSTATUS WINAPI SHDeleteValue(
	HKEY hkey,
	const C *pszSubKey,
	const C *pszValue
)
{
	auto x = info<LSTATUS>(add_suffix<C>("SHDeleteValue"), hkey, pszSubKey, pszValue);
	HKEY key;
	x.result = Advapi32<C>::RegOpenKeyEx(hkey, pszSubKey, 0, KEY_WRITE, &key);
	if (x.result == ERROR_SUCCESS)
	{
		Advapi32<C>::RegDeleteValue(key, pszValue);
		Advapi32<C>::RegCloseKey(key);
	}
	return x.result;
}

/*
SHCopyKey
SHGetValue
SHRegOpenUSKey
SHRegWriteUSValue
SHRegSetUSValue
SHRegSetPath
*/
