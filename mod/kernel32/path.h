static BOOL WINAPI SetCurrentDirectory(const C *lpPathName)
{
	auto x = info<BOOL>(add_suffix<C>("SetCurrentDirectory"), lpPathName);
	auto sp = cnv<C>::sp(lpPathName);
	x.l(2, lpPathName, " > ", sp);
	return x.result = orig(SetCurrentDirectory)(sp.c_str());
}

static DWORD WINAPI GetCurrentDirectory(DWORD nBufferLength, C *lpBuffer)
{
	auto x = info<DWORD>(add_suffix<C>("GetCurrentDirectory"), nBufferLength, lpBuffer);
	string buf(MAX_PATH, 0);
	auto size = orig(GetCurrentDirectory)(MAX_PATH, &buf[0]);
	buf.resize(size);
	if (size > MAX_PATH)
		buf.resize(orig(GetCurrentDirectory)(size, &buf[0]));
	SaveError s;
	auto vp = cnv<C>::vp(buf);
	x.l(5, buf, " > ", vp);
	if (nBufferLength <= vp.size())
		x.result = vp.size() + 1;
	else
		x.result = String::cpy(lpBuffer, vp, nBufferLength);
	return x.result;
}

static UINT WINAPI GetTempPath(DWORD nBufferLength, C *lpBuffer)
{
	auto x = info<UINT>(add_suffix<C>("GetTempPath"), nBufferLength, lpBuffer);
	if (config.exists("TEMP"))
	{
		auto s = String::tstr<C>(config.str("TEMP"));
		if (nBufferLength <= s.size())
			x.result = s.size() + 1;
		else
			x.result = String::cpy(lpBuffer, s, nBufferLength);
	}
	else
		x.result = orig(GetTempPath)(nBufferLength, lpBuffer);
	return x.result;
}

static UINT WINAPI GetWindowsDirectory(C *lpBuffer, UINT uSize)
{
	auto x = info<UINT>(add_suffix<C>("GetWindowsDirectory"), lpBuffer, uSize);
	return x.result = orig(GetWindowsDirectory)(lpBuffer, uSize);
}

static UINT WINAPI GetSystemWindowsDirectory(C *lpBuffer, UINT uSize)
{
	auto x = info<UINT>(add_suffix<C>("GetSystemWindowsDirectory"), lpBuffer, uSize);
	return x.result = orig(GetSystemWindowsDirectory)(lpBuffer, uSize);
}

static UINT WINAPI GetSystemDirectory(C *lpBuffer, UINT uSize)
{
	auto x = info<UINT>(add_suffix<C>("GetSystemDirectory"), lpBuffer, uSize);
	return x.result = orig(GetSystemDirectory)(lpBuffer, uSize);
}

static UINT WINAPI GetSystemWow64Directory(C *lpBuffer, UINT uSize)
{
	auto x = info<UINT>(add_suffix<C>("GetSystemWow64Directory"), lpBuffer, uSize);
	return x.result = orig(GetSystemWow64Directory)(lpBuffer, uSize);
}

static DWORD WINAPI GetModuleFileName(
	HMODULE hModule,
	C *lpFilename,
	DWORD nSize
)
{
	auto x = info<DWORD>(add_suffix<C>("GetModuleFileName"), hModule, lpFilename, nSize);
	string buf(MAX_PATH, 0);
	x.result = orig(GetModuleFileName)(hModule, &buf[0], MAX_PATH);
	SaveError s;
	if (x.result == 0)
	{
		x.l(5, "failed!");
		return x.result;
	}
	buf.resize(x.result);
	auto vp = cnv<C>::vp(buf);
	x.l(5, buf, " > ", vp);
	x.result = String::cpy(lpFilename, vp, nSize);
	if (x.result == nSize)
		lpFilename[nSize - 1] = 0;
	return x.result;
}

static DWORD WINAPI GetFullPathName(
	const C *lpFileName,
	DWORD nBufferLength,
	C *lpBuffer,
	C **lpFilePart
)
{
	auto x = info<DWORD>(add_suffix<C>("GetFullPathName"), lpFileName, nBufferLength, lpBuffer, lpFilePart);
	C *filepart;
	string buf(MAX_PATH, 0);
	x.result = orig(GetFullPathName)(lpFileName, MAX_PATH, &buf[0], &filepart);
	if (x.result == 0)
	{
		SaveError s;
		x.l(6, "failed!");
		return x.result;
	}
	buf.resize(x.result);
	if (x.result > MAX_PATH)
		buf.resize(orig(GetFullPathName)(lpFileName, MAX_PATH, &buf[0], &filepart));
	SaveError s;
	auto vp = cnv<C>::vp(buf);
	x.l(6, lpFileName, " > ", buf, " > ", vp);

	if (!lpBuffer || nBufferLength <= vp.size())
		return x.result = vp.size() + 1;
	if (lpFilePart && filepart)
	{
		*lpFilePart = lpBuffer + (vp.size() - buf.size()) + (filepart - &buf[0]);
	}
	return x.result = String::cpy(lpBuffer, vp, nBufferLength);
}

static DWORD WINAPI GetFinalPathNameByHandle(
	HANDLE hFile,
	C *lpszFilePath,
	DWORD cchFilePath,
	DWORD dwFlags
)
{
	auto x = info<DWORD>(add_suffix<C>("GetFinalPathNameByHandle"), hFile, lpszFilePath, cchFilePath, dwFlags);
	string buf(MAX_PATH, 0);
	x.result = orig(GetFinalPathNameByHandle)(hFile, &buf[0], MAX_PATH, dwFlags);
	buf.resize(x.result);
	if (x.result > MAX_PATH)
		buf.resize(orig(GetFinalPathNameByHandle)(hFile, &buf[0], x.result + 1, dwFlags));
	SaveError s;
	auto vp = cnv<C>::vp(buf);
	x.l(6, buf, " > ", vp);
	String::cpy(lpszFilePath, vp, cchFilePath);
	return x.result = vp.size();
}

static DWORD WINAPI SearchPath(
	const C *lpPath,
	const C *lpFileName,
	const C *lpExtension,
	DWORD nBufferLength,
	C *lpBuffer,
	C **lpFilePart
)
{
	auto x = info<DWORD>(add_suffix<C>("SearchPath"), lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
	auto file = cnv<C>::sp(lpFileName);
	string buf(MAX_PATH, 0);
	C *filepart;
	if (lpPath)
	{
		auto sp = cnv<C>::sp(lpPath);
		x.result = orig(SearchPath)(sp.c_str(), file.c_str(), lpExtension, nBufferLength, &buf[0], &filepart);
		if (!x.result)
		{
			auto cp = cnv<C>::cp(lpPath);
			if (sp != cp)
				x.result = orig(SearchPath)(cp.c_str(), file.c_str(), lpExtension, nBufferLength, &buf[0], &filepart);
		}
	}
	else
	{
		x.result = orig(SearchPath)(lpPath, file.c_str(), lpExtension, nBufferLength, &buf[0], &filepart);
	}
	SaveError s;

	if (x.result)
	{
		buf.resize(x.result);
		string vp = cnv<C>::vp(buf);
		if (!lpBuffer || nBufferLength <= vp.size())
			return vp.size() + 1;
		if (lpBuffer)
		{
			x.result = String::cpy(lpBuffer, vp, nBufferLength);
			if (lpFilePart && filepart)
			{
				*lpFilePart = lpBuffer + (vp.size() - buf.size()) + (filepart - &buf[0]);
			}
		}
		x.l(6, lpFileName, " > ", buf, " > ", vp);
	}
	return x.result;
}

static DWORD WINAPI GetShortPathName(
	const C *lpszLongPath,
	C *lpszShortPath,
	DWORD cchBuffer
)
{
	auto x = info<DWORD>(add_suffix<C>("GetShortPathName"), lpszLongPath, lpszShortPath, cchBuffer);
	x.result = orig(GetShortPathName)(lpszLongPath, lpszShortPath, cchBuffer);
	x.l(-1, "unimplemented:", lpszLongPath, " > ", lpszShortPath);
	return x.result;
}

static DWORD WINAPI GetLongPathName(
	const C *lpszShortPath,
	C *lpszLongPath,
	DWORD cchBuffer
)
{
	auto x = info<DWORD>(add_suffix<C>("GetLongPathName"), lpszShortPath, lpszLongPath, cchBuffer);
	x.result = orig(GetLongPathName)(lpszShortPath, lpszLongPath, cchBuffer);
	SaveError s;
	x.l(-1, "unimplemented:", lpszShortPath, " > ", lpszLongPath);
	return x.result;
}
