static HANDLE WINAPI CreateFile(
	const C *lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
)
{
	auto x = info<HANDLE>(add_suffix<C>("CreateFile"), lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	if (!(dwDesiredAccess & GENERIC_WRITE)
		&& (dwCreationDisposition != CREATE_NEW && dwCreationDisposition != CREATE_ALWAYS && dwCreationDisposition != TRUNCATE_EXISTING)
	)
	{
		auto ep = cnv<C>::getExists(lpFileName);
		x.l(6, lpFileName, " > ", ep);
		x.result = orig(CreateFile)(ep.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		return x.result;
	}
	else
	{
		auto sp = cnv<C>::sp(lpFileName);
		x.l(3, lpFileName, " > ", sp);
		x.result = orig(CreateFile)(sp.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		return x.result;
	}
}

static BOOL WINAPI MoveFile(
	const C *lpExistingFileName,
	const C *lpNewFileName
)
{
	auto x = info<BOOL>(add_suffix<C>("MoveFile"), lpExistingFileName, lpNewFileName);
	auto ep = cnv<C>::sp(lpExistingFileName);
	auto np = cnv<C>::sp(lpNewFileName);
	x.l(3, lpExistingFileName, " > ", ep, " > ", np);
	x.result = orig(MoveFile)(ep.c_str(), np.c_str());
	return x.result;
}

static BOOL WINAPI MoveFileEx(
	const C *lpExistingFileName,
	const C *lpNewFileName,
	DWORD dwFlags
)
{
	auto x = info<BOOL>(add_suffix<C>("MoveFileEx"), lpExistingFileName, lpNewFileName, dwFlags);
	string exf;
	string newf;
	if (lpExistingFileName)
	{
		exf = cnv<C>::sp(lpExistingFileName);
		lpExistingFileName = exf.c_str();
	}
	if (lpNewFileName)
	{
		newf = cnv<C>::sp(lpNewFileName);
		lpNewFileName = newf.c_str();
	}
	x.l(3, exf, " > ", newf);
	x.result = orig(MoveFileEx)(lpExistingFileName, lpNewFileName, dwFlags);
	return x.result;
}

static BOOL WINAPI CopyFile(
	const C *lpExistingFileName,
	const C *lpNewFileName,
	BOOL bFailIfExists
)
{
	auto x = info<BOOL>(add_suffix<C>("CopyFile"), lpExistingFileName, lpNewFileName, bFailIfExists);
	auto ef = cnv<C>::getExists(lpExistingFileName);
	auto nf = cnv<C>::sp(lpNewFileName);
	x.l(3, lpExistingFileName, " > ", ef, " > ", nf);
	x.result = orig(CopyFile)(ef.c_str(), nf.c_str(), bFailIfExists);
	return x.result;
}

static BOOL WINAPI CopyFileEx(
	const C *lpExistingFileName,
	const C *lpNewFileName,
	LPPROGRESS_ROUTINE lpProgressRoutine,
	LPVOID lpData,
	LPBOOL pbCancel,
	DWORD dwCopyFlags
)
{
	auto x = info<BOOL>(add_suffix<C>("CopyFileEx"), lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);
	auto ef = cnv<C>::getExists(lpExistingFileName);
	auto nf = cnv<C>::sp(lpNewFileName);
	x.l(3, lpExistingFileName, " > ", ef, " > ", nf);
	x.result = orig(CopyFileEx)(ef.c_str(), nf.c_str(), lpProgressRoutine, lpData, pbCancel, dwCopyFlags);
	return x.result;
}

static BOOL WINAPI DeleteFile(
	const C *lpFileName
)
{
	auto x = info<BOOL>(add_suffix<C>("DeleteFile"), lpFileName);
	auto sp = cnv<C>::sp(lpFileName);
	x.l(3, lpFileName, " > ", sp);
	x.result = orig(DeleteFile)(sp.c_str());
	return x.result;
}

static BOOL WINAPI ReplaceFile(
	const C *lpReplacedFileName,
	const C *lpReplacementFileName,
	const C *lpBackupFileName,
	DWORD dwReplaceFlags,
	LPVOID lpExclude,
	LPVOID lpReserved
)
{
	auto x = info<BOOL>(add_suffix<C>("ReplaceFile"), lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);
	x.l(-1, "unimplemented");
	//The backup file, replaced file, and replacement file must all reside on the same volume.
	auto from = cnv<C>::sp(lpReplacedFileName);
	auto to = cnv<C>::sp(lpReplacementFileName);
	string backup;
	if (lpBackupFileName)
	{
		backup = cnv<C>::sp(lpBackupFileName);
		lpBackupFileName = backup.c_str();
	}
	x.l(3, from, " > ", to, " : ", backup);
	x.result = orig(ReplaceFile)(from.c_str(), to.c_str(), lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);
	return x.result;
}

static BOOL WINAPI CreateDirectory(
	const C *lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	auto x = info<BOOL>(add_suffix<C>("CreateDirectory"), lpPathName, lpSecurityAttributes);
	if (lpPathName)
	{
		auto sp = cnv<C>::sp(lpPathName);
		x.l(3, lpPathName, " > ", sp);
		x.result = orig(CreateDirectory)(sp.c_str(), lpSecurityAttributes);
	}
	else
	{
		x.l(3, "NULL");
		x.result = orig(CreateDirectory)(lpPathName, lpSecurityAttributes);
	}
	return x.result;
}

static BOOL WINAPI CreateDirectoryEx(
	const C *lpTemplateDirectory,
	const C *lpNewDirectory,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	auto x = info<BOOL>(add_suffix<C>("CreateDirectoryEx"), lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);
	auto td = cnv<C>::getExists(lpTemplateDirectory);
	auto nd = cnv<C>::sp(lpNewDirectory);
	x.l(3, lpNewDirectory, " > ", nd);
	x.result = orig(CreateDirectoryEx)(td.c_str(), nd.c_str(), lpSecurityAttributes);
	return x.result;
}

static BOOL WINAPI RemoveDirectory(
	const C *lpPathName
)
{
	auto x = info<BOOL>(add_suffix<C>("RemoveDirectory"), lpPathName);
	auto sp = cnv<C>::sp(lpPathName);
	x.l(3, lpPathName, " > ", sp);
	x.result = orig(RemoveDirectory)(sp.c_str());
	return x.result;
}

static HANDLE WINAPI FindFirstFile(
	const C *lpFileName,
	LPWIN32_FIND_DATA lpFindFileData
)
{
	auto x = info<HANDLE>(add_suffix<C>("FindFirstFile"), lpFileName, lpFindFileData);
	auto sp = cnv<C>::sp(lpFileName);
	x.result = orig(FindFirstFile)(sp.c_str(), lpFindFileData);
	if (x.result != INVALID_HANDLE_VALUE)
	{
		x.l(6, lpFileName, " > ", sp);
	}
	else
	{
		auto cp = cnv<C>::cp(lpFileName);
		x.result = orig(FindFirstFile)(cp.c_str(), lpFindFileData);
		if (x.result != INVALID_HANDLE_VALUE)
			x.l(6, lpFileName, " > ", cp);
		else
			x.l(6, lpFileName, " failed!");
	}
	return x.result;
}

static HANDLE WINAPI FindFirstFileEx(
	const C *lpFileName,
	FINDEX_INFO_LEVELS fInfoLevelId,
	LPVOID lpFindFileData,
	FINDEX_SEARCH_OPS fSearchOp,
	LPVOID lpSearchFilter,
	DWORD dwAdditionalFlags
)
{
	auto x = info<HANDLE>(add_suffix<C>("FindFirstFileEx"), lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
	auto sp = cnv<C>::sp(lpFileName);
	x.result = orig(FindFirstFileEx)(sp.c_str(), fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
	if (x.result != INVALID_HANDLE_VALUE)
	{
		x.l(6, lpFileName, " > ", sp);
	}
	else
	{
		auto cp = cnv<C>::cp(lpFileName);
		x.result = orig(FindFirstFileEx)(cp.c_str(), fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
		if (x.result != INVALID_HANDLE_VALUE)
			x.l(6, lpFileName, " > ", cp);
		else
			x.l(6, lpFileName, " failed!");
	}
	return x.result;
}

static BOOL WINAPI SetFileAttributes(const C *lpFileName, DWORD dwFileAttributes)
{
	auto x = info<BOOL>(add_suffix<C>("SetFileAttributes"), lpFileName, dwFileAttributes);
	auto sp = cnv<C>::sp(lpFileName);
	x.l(3, lpFileName, " > ", sp);
	x.result = orig(SetFileAttributes)(sp.c_str(), dwFileAttributes);
	return x.result;
}

static DWORD WINAPI GetFileAttributes(const C *lpFileName)
{
	auto x = info<DWORD>(add_suffix<C>("GetFileAttributes"), lpFileName);
	auto f = cnv<C>::getExists(lpFileName);
	x.l(7, lpFileName, " > ", f);
	x.result = orig(GetFileAttributes)(f.c_str());
	return x.result;
}

static DWORD WINAPI GetFileAttributesEx(
	const C *lpFileName,
	GET_FILEEX_INFO_LEVELS fInfoLevelId,
	LPVOID lpFileInformation
)
{
	auto x = info<DWORD>(add_suffix<C>("GetFileAttributesEx"), lpFileName, fInfoLevelId, lpFileInformation);
	if (lpFileName)
	{
		auto f = cnv<C>::getExists(lpFileName);
		x.l(7, lpFileName, " > ", f);
		x.result = orig(GetFileAttributesEx)(f.c_str(), fInfoLevelId, lpFileInformation);
	}
	else
	{
		x.result = orig(GetFileAttributesEx)(lpFileName, fInfoLevelId, lpFileInformation);
	}
	return x.result;
}

static HFILE WINAPI _lopen(
	LPCSTR lpPathName,
	int iReadWrite
)
{
	auto x = info<HFILE>("_lopen", lpPathName, iReadWrite);
	auto ep = cnv<char>::getExists(lpPathName);
	x.result = orig(_lopen)(ep.c_str(), iReadWrite);
	return x.result;
}
static HFILE WINAPI OpenFile(
	LPCSTR lpFileName,
	LPOFSTRUCT lpReOpenBuff,
	UINT uStyle
)
{
	auto x = info<HFILE>("OpenFile", lpFileName, lpReOpenBuff, uStyle);
	auto ep = cnv<char>::getExists(lpFileName);
	x.result = orig(OpenFile)(ep.c_str(), lpReOpenBuff, uStyle);
	return x.result;
}

static UINT WINAPI GetTempFileName(
	const C *lpPathName,
	const C *lpPrefixString,
	UINT uUnique,
	C *lpTempFileName
)
{
	auto x = info<UINT>(add_suffix<C>("GetTempFileName"), lpPathName, lpPrefixString, uUnique, lpTempFileName);
	auto sp = cnv<C>::sp(lpPathName);
	x.result = orig(GetTempFileName)(sp.c_str(), lpPrefixString, uUnique, lpTempFileName);
	x.l(7, lpPathName, " > ", sp, " > ", lpTempFileName);
	return x.result;
}

/*
CopyFileTransactedA
CopyFileTransactedW
CreateDirectoryTransactedA
CreateDirectoryTransactedW
CreateFileTransactedA
CreateFileTransactedW
DeleteFileTransactedA
DeleteFileTransactedW
FindFirstFileNameTransactedW
FindFirstFileNameW
FindFirstFileTransactedA
FindFirstFileTransactedW
MoveFileTransactedA
MoveFileTransactedW
RemoveDirectoryTransactedA
RemoveDirectoryTransactedW

FindFirstStreamTransactedW

GetCompressedFileSizeA
GetCompressedFileSizeTransactedA
GetCompressedFileSizeTransactedW
GetCompressedFileSizeW

GetFileAttributesTransactedA
GetFileAttributesTransactedW
SetFileAttributesTransactedA
SetFileAttributesTransactedW

GetLongPathNameTransactedA
GetLongPathNameTransactedW

MoveFileWithProgress

FindFirstChangeNotification

FindFirstStreamW?
*/
