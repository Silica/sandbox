static int WINAPI SHCreateDirectoryEx(
	HWND hwnd,
	const C *pszPath,
	const SECURITY_ATTRIBUTES *psa
)
{
	auto x = info<int>(add_suffix<C>("SHCreateDirectoryEx"), hwnd, pszPath, psa);
	auto sp = cnv<C>::sp(pszPath);
	x.l(3, pszPath, " > ", sp);
	x.result = orig(SHCreateDirectoryEx)(hwnd, sp.c_str(), psa);
	return x.result;
}

static int WINAPI SHCreateDirectory(
	HWND hwnd,
	PCWSTR pszPath // only W
)
{
	auto x = info<int>("SHCreateDirectory", hwnd, pszPath);
	return x.result = Shell32<wchar_t>::SHCreateDirectoryEx(hwnd, pszPath, NULL);
}

static HRESULT WINAPI SHPathPrepareForWrite(
	HWND hwnd,
	IUnknown *punkEnableModless,
	const C *pszPath,
	DWORD dwFlags
)
{
	auto x = info<HRESULT>(add_suffix<C>("SHCreateDirectoryEx"), hwnd, punkEnableModless, pszPath, dwFlags);
	auto sp = cnv<C>::sp(pszPath);
	return x.result = orig(SHPathPrepareForWrite)(hwnd, punkEnableModless, sp.c_str(), dwFlags);
}

static DWORD_PTR WINAPI SHGetFileInfo(
	const C *pszPath,
	DWORD dwFileAttributes,
	ShellFileInfo *psfi,
	UINT cbFileInfo,
	UINT uFlags
)
{
	auto x = info<DWORD_PTR>(add_suffix<C>("SHGetFileInfo"), pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);
	auto f = cnv<C>::getExists(pszPath);
	return x.result = orig(SHGetFileInfo)(f.c_str(), dwFileAttributes, psfi, cbFileInfo, uFlags);
}

static int WINAPI SHFileOperation(ShellFileOperation *lpFileOp)
{
	auto x = info<int>(add_suffix<C>("SHFileOperation"), lpFileOp);
	ShellFileOperation sfo = *lpFileOp;
	string from = cnv<C>::getExists(lpFileOp->pFrom);
	sfo.pFrom = from.c_str();
	string to;
	if (lpFileOp->pTo)
	{
		to = cnv<C>::sp(lpFileOp->pTo);
		sfo.pTo = to.c_str();
		x.l(3, lpFileOp->wFunc, ":", lpFileOp->pFrom, " > ", from, " > ", lpFileOp->pTo, " > ", to);
	}
	else
	{
		x.l(3, lpFileOp->wFunc, ":", lpFileOp->pFrom, " > ", from);
	}
	x.result = orig(SHFileOperation)(&sfo);
	lpFileOp->fAnyOperationsAborted = sfo.fAnyOperationsAborted;
	lpFileOp->hNameMappings = sfo.hNameMappings;
	return x.result;
}
//SHGetDiskFreeSpaceEx
//SHGetNewLinkInfo
