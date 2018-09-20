static HRESULT STDAPICALLTYPE SHGetFolderPath(
	HWND hwnd,
	int csidl,
	HANDLE hToken,
	DWORD dwFlags,
	C *pszPath
)
{
	auto x = info<HRESULT>(add_suffix<C>("SHGetFolderPath"), hwnd, csidl, hToken, dwFlags, pszPath);
	x.result = orig(SHGetFolderPath)(hwnd, csidl & ~CSIDL_FLAG_MASK, hToken, dwFlags, pszPath);
	SaveError s;
	if (x.result != S_OK)
	{
		x.l(7, csidl & ~CSIDL_FLAG_MASK, " = error!");
		return x.result;
	}
	auto vp = cnv<C>::vp(pszPath);
	x.l(7, csidl & ~CSIDL_FLAG_MASK, " = ", pszPath, " > ", vp);
	if (csidl & CSIDL_FLAG_CREATE)
	{
		auto sp = cnv<C>::sp(pszPath);
		x.l(3, "create:", sp);
		orig(SHCreateDirectoryEx)(hwnd, sp.c_str(), NULL);
	}
	String::cpy(pszPath, vp, vp.size() + 1);
	return x.result;
}

static HRESULT WINAPI SHGetFolderPathAndSubDir(
	HWND hwnd,
	int csidl,
	HANDLE hToken,
	DWORD dwFlags,
	const C *pszSubDir,
	C *pszPath
)
{
	auto x = info<HRESULT>(add_suffix<C>("SHGetFolderPathAndSubDir"), hwnd, csidl, hToken, dwFlags, pszSubDir, pszPath);
	C spath[MAX_PATH];
	x.result = SHGetFolderPath(hwnd, csidl, hToken, dwFlags, spath);
	auto path = Path::Combine(spath, pszSubDir);
	x.l(8, pszSubDir, " > ", path);
	String::cpy(pszPath, path, MAX_PATH);
	return x.result;
}

static BOOL WINAPI SHGetSpecialFolderPath(
	HWND hwnd,
	C *pszPath,
	int csidl,
	BOOL fCreate
)
{
	auto x = info<BOOL>(add_suffix<C>("SHGetSpecialFolderPath"), hwnd, pszPath, csidl, fCreate);
	return x.result = SHGetFolderPath(hwnd, csidl | (fCreate ? CSIDL_FLAG_CREATE : 0), NULL, 0, pszPath);
}

static HRESULT WINAPI SHGetSpecialFolderLocation(
	HWND hwnd,
	int csidl,
	PIDLIST_ABSOLUTE *ppidl
)
{
	auto x = info<HRESULT>(add_suffix<C>("SHGetSpecialFolderLocation"), hwnd, csidl, ppidl);
	x.result = orig(SHGetSpecialFolderLocation)(hwnd, csidl, ppidl);
	SaveError s;
	return x.result;
}

static HRESULT WINAPI SHGetFolderLocation(
	HWND hwnd,
	int csidl,
	HANDLE hToken,
	DWORD dwFlags,
	PIDLIST_ABSOLUTE *ppidl
)
{
	auto x = info<HRESULT>(add_suffix<C>("SHGetFolderLocation"), hwnd, csidl, hToken, dwFlags, ppidl);
	x.result = orig(SHGetFolderLocation)(hwnd, csidl, hToken, dwFlags, ppidl);
	SaveError s;
	return x.result;
}

static HRESULT WINAPI SHGetFolderPathEx(
	REFKNOWNFOLDERID rfid,
	DWORD dwFlags,
	HANDLE hToken,
	LPWSTR pszPath, // only W
	UINT cchPath
)
{
	auto x = info<HRESULT>(add_suffix<C>("SHGetFolderPathEx"), rfid, dwFlags, hToken, pszPath, cchPath);
	x.result = orig(SHGetFolderPathEx)(rfid, dwFlags, hToken, pszPath, cchPath);
	SaveError s;
	if (x.result == S_OK)
	{
		auto vp = cnv<wchar_t>::vp(pszPath);
		x.l(7, FuncInfo::guid(rfid), " = ", pszPath, " > ", vp);
		String::cpy(pszPath, vp, cchPath);
	}
	else
	{
		x.l(2, FuncInfo::guid(rfid), " = error!");
	}
	return x.result;
}

static HRESULT WINAPI SHGetKnownFolderPath(
	REFKNOWNFOLDERID rfid,
	DWORD dwFlags,
	HANDLE hToken,
	PWSTR *ppszPath // only W
)
{
	auto x = info<HRESULT>(add_suffix<C>("SHGetKnownFolderPath"), rfid, dwFlags, hToken, ppszPath);
	x.result = orig(SHGetKnownFolderPath)(rfid, dwFlags, hToken, ppszPath);
	SaveError s;
	if (x.result == S_OK)
	{
		auto vp = cnv<wchar_t>::vp(*ppszPath);
		x.l(7, FuncInfo::guid(rfid), " = ", *ppszPath, " > ", vp);
		auto l = wcslen(*ppszPath);
		if (l < vp.size())
		{
			auto p = CoTaskMemRealloc(*ppszPath, (vp.size() + 1) * sizeof(wchar_t));
			if (p)
			{
				x.l(7, "CoTaskMemRealloc:", FuncInfo::hex(*ppszPath), " to ", FuncInfo::hex(p));
				*ppszPath = (PWSTR)p;
			}
			else
			{
				p = CoTaskMemAlloc((vp.size() + 1) * sizeof(wchar_t));
				if (p)
				{
					CoTaskMemFree(*ppszPath);
					*ppszPath = (PWSTR)p;
				}
				else
				{
					vp.resize(l);
				}
			}
		}
		String::cpy(*ppszPath, vp, vp.size() + 1);
	}
	else
	{
		x.l(2, FuncInfo::guid(rfid), " = error!");
	}
	return x.result;
}

static HRESULT WINAPI SHGetKnownFolderIDList(
	REFKNOWNFOLDERID rfid,
	DWORD dwFlags,
	HANDLE hToken,
	PIDLIST_ABSOLUTE *ppidl
)
{
	auto x = info<HRESULT>(add_suffix<C>("SHGetKnownFolderIDList"), rfid, dwFlags, hToken, ppidl);
	return x.result = orig(SHGetKnownFolderIDList)(rfid, dwFlags, hToken, ppidl);
}

static BOOL WINAPI SHGetPathFromIDList(
	PCIDLIST_ABSOLUTE pidl,
	C *pszPath
)
{
	auto x = info<BOOL>(add_suffix<C>("SHGetPathFromIDList"), pidl, pszPath);
	x.result = orig(SHGetPathFromIDList)(pidl, pszPath);
	SaveError s;
	if (x.result)
	{
		auto vp = cnv<C>::vp(pszPath);
		x.l(7, pszPath, " > ", vp);
		String::cpy(pszPath, vp, vp.size() + 1);
	}
	else
	{
		x.l(2, "error!");
	}
	return x.result;
}

static BOOL WINAPI SHGetPathFromIDListEx(
	PCIDLIST_ABSOLUTE pidl,
	PWSTR pszPath, // only W
	DWORD cchPath,
	GPFIDL_FLAGS uOpts
)
{
	auto x = info<BOOL>(add_suffix<C>("SHGetPathFromIDListEx"), pidl, pszPath, cchPath, uOpts);
	x.result = orig(SHGetPathFromIDListEx)(pidl, pszPath, cchPath, uOpts);
	SaveError s;
	if (x.result)
	{
		auto vp = cnv<wchar_t>::vp(pszPath);
		x.l(7, pszPath, " > ", vp);
		String::cpy(pszPath, vp, cchPath);
	}
	else
	{
		x.l(2, "error!");
	}
	return x.result;
}

static HRESULT WINAPI SHILCreateFromPath(
	PCWSTR pszPath, // only W
	PIDLIST_ABSOLUTE *ppidl,
	DWORD *rgfInOut
)
{
	auto x = info<HRESULT>(add_suffix<C>("SHILCreateFromPath"), pszPath, ppidl, rgfInOut);
	auto f = cnv<wchar_t>::getExists(pszPath);
	return x.result = orig(SHILCreateFromPath)(f.c_str(), ppidl, rgfInOut);
}

static PIDLIST_ABSOLUTE WINAPI ILCreateFromPath(
	const C *pszPath
)
{
	auto x = info<PIDLIST_ABSOLUTE>(add_suffix<C>("ILCreateFromPath"), pszPath);
	auto f = cnv<C>::getExists(pszPath);
	return x.result = orig(ILCreateFromPath)(f.c_str());
}

static PIDLIST_ABSOLUTE WINAPI SHSimpleIDListFromPath(
	PCWSTR pszPath
)
{
	auto x = info<PIDLIST_ABSOLUTE>(add_suffix<C>("SHSimpleIDListFromPath"), pszPath);
	auto f = cnv<wchar_t>::getExists(pszPath);
	return x.result = orig(SHSimpleIDListFromPath)(f.c_str());
}

static HRESULT WINAPI SHGetDesktopFolder(
	IShellFolder **ppshf
)
{
	auto x = info<HRESULT>("SHGetDesktopFolder", ppshf);
	x.l(-1, "unimplemented");
	return x.result = orig(SHGetDesktopFolder)(ppshf);
}
//SHBrowseForFolder
