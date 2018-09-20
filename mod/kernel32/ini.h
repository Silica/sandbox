static DWORD WINAPI GetPrivateProfileString(
	const C *lpAppName,
	const C *lpKeyName,
	const C *lpDefault,
	C *lpReturnedString,
	DWORD nSize,
	const C *lpFileName
)
{
	auto x = info<DWORD>(add_suffix<C>("GetPrivateProfileString"), lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
	auto f = cnv<C>::getExists(lpFileName);
	return x.result = orig(GetPrivateProfileString)(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, f.c_str());
}

static UINT WINAPI GetPrivateProfileInt(
	const C *lpAppName,
	const C *lpKeyName,
	INT nDefault,
	const C *lpFileName
)
{
	auto x = info<UINT>(add_suffix<C>("GetPrivateProfileInt"), lpAppName, lpKeyName, nDefault, lpFileName);
	auto f = cnv<C>::getExists(lpFileName);
	return x.result = orig(GetPrivateProfileInt)(lpAppName, lpKeyName, nDefault, f.c_str());
}
static BOOL WINAPI GetPrivateProfileStruct(
	const C *lpszSection,
	const C *lpszKey,
	LPVOID lpStruct,
	UINT uSizeStruct,
	const C *szFile
)
{
	auto x = info<BOOL>(add_suffix<C>("GetPrivateProfileStruct"), lpszSection, lpszKey, lpStruct, uSizeStruct, szFile);
	auto f = cnv<C>::getExists(szFile);
	return x.result = orig(GetPrivateProfileStruct)(lpszSection, lpszKey, lpStruct, uSizeStruct, f.c_str());
}
static DWORD WINAPI GetPrivateProfileSection(
	const C *lpAppName,
	C *lpReturnedString,
	DWORD nSize,
	const C *lpFileName
)
{
	auto x = info<DWORD>(add_suffix<C>("GetPrivateProfileSection"), lpAppName, lpReturnedString, nSize, lpFileName);
	auto f = cnv<C>::getExists(lpFileName);
	return x.result = orig(GetPrivateProfileSection)(lpAppName, lpReturnedString, nSize, f.c_str());
}
static DWORD WINAPI GetPrivateProfileSectionNames(
	C *lpszReturnBuffer,
	DWORD nSize,
	const C *lpFileName
)
{
	auto x = info<DWORD>(add_suffix<C>("GetPrivateProfileSectionNames"), lpszReturnBuffer, nSize, lpFileName);
	auto f = cnv<C>::getExists(lpFileName);
	return x.result = orig(GetPrivateProfileSectionNames)(lpszReturnBuffer, nSize, f.c_str());
}

static BOOL WINAPI WritePrivateProfileString(
	const C *lpAppName,
	const C *lpKeyName,
	const C *lpString,
	const C *lpFileName
)
{
	auto x = info<BOOL>(add_suffix<C>("WritePrivateProfileString"), lpAppName, lpKeyName, lpString, lpFileName);
	auto f = cnv<C>::sp(lpFileName);
	return x.result = orig(WritePrivateProfileString)(lpAppName, lpKeyName, lpString, f.c_str());
}

static BOOL WINAPI WritePrivateProfileStruct(
	const C *lpszSection,
	const C *lpszKey,
	LPVOID lpStruct,
	UINT uSizeStruct,
	const C *szFile
)
{
	auto x = info<BOOL>(add_suffix<C>("WritePrivateProfileStruct"), lpszSection, lpszKey, lpStruct, uSizeStruct, szFile);
	auto f = cnv<C>::sp(szFile);
	return x.result = orig(WritePrivateProfileStruct)(lpszSection, lpszKey, lpStruct, uSizeStruct, f.c_str());
}

static BOOL WINAPI WritePrivateProfileSection(
	const C *lpAppName,
	const C *lpString,
	const C *lpFileName
)
{
	auto x = info<BOOL>(add_suffix<C>("WritePrivateProfileSection"), lpAppName, lpString, lpFileName);
	auto f = cnv<C>::sp(lpFileName);
	return x.result = orig(WritePrivateProfileSection)(lpAppName, lpString, f.c_str());
}

static DWORD WINAPI GetProfileString(
	const C *lpAppName,
	const C *lpKeyName,
	const C *lpDefault,
	C *lpReturnedString,
	DWORD nSize
)
{
	auto x = info<DWORD>(add_suffix<C>("GetProfileString"), lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize);
	C buf[MAX_PATH];
	orig(GetWindowsDirectory)(buf, MAX_PATH);
	auto f = cnv<C>::getExists(Path::Combine(buf, String::tstr<C>("Win.ini")));
	return x.result = orig(GetPrivateProfileString)(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, f.c_str());
}

static UINT WINAPI GetProfileInt(
	const C *lpAppName,
	const C *lpKeyName,
	INT nDefault
)
{
	auto x = info<UINT>(add_suffix<C>("GetProfileString"), lpAppName, lpKeyName, nDefault);
	C buf[MAX_PATH];
	orig(GetWindowsDirectory)(buf, MAX_PATH);
	auto f = cnv<C>::getExists(Path::Combine(buf, String::tstr<C>("Win.ini")));
	return x.result = orig(GetPrivateProfileInt)(lpAppName, lpKeyName, nDefault, f.c_str());
}

static DWORD WINAPI GetProfileSection(
	const C *lpAppName,
	C *lpReturnedString,
	DWORD nSize
)
{
	auto x = info<DWORD>(add_suffix<C>("GetProfileString"), lpAppName, lpReturnedString, nSize);
	C buf[MAX_PATH];
	orig(GetWindowsDirectory)(buf, MAX_PATH);
	auto f = cnv<C>::getExists(Path::Combine(buf, String::tstr<C>("Win.ini")));
	return x.result = orig(GetPrivateProfileSection)(lpAppName, lpReturnedString, nSize, f.c_str());
}

static BOOL WINAPI WriteProfileString(
	const C *lpAppName,
	const C *lpKeyName,
	const C *lpString
)
{
	auto x = info<BOOL>(add_suffix<C>("WriteProfileString"), lpAppName, lpKeyName, lpString);
	C buf[MAX_PATH];
	orig(GetWindowsDirectory)(buf, MAX_PATH);
	auto f = cnv<C>::sp(Path::Combine(buf, String::tstr<C>("Win.ini")));
	return x.result = orig(WritePrivateProfileString)(lpAppName, lpKeyName, lpString, f.c_str());
}

static BOOL WINAPI WriteProfileSection(
	const C *lpAppName,
	const C *lpString
)
{
	auto x = info<BOOL>(add_suffix<C>("WriteProfileSection"), lpAppName, lpString);
	C buf[MAX_PATH];
	orig(GetWindowsDirectory)(buf, MAX_PATH);
	auto f = cnv<C>::sp(Path::Combine(buf, String::tstr<C>("Win.ini")));
	return x.result = orig(WritePrivateProfileSection)(lpAppName, lpString, f.c_str());
}
