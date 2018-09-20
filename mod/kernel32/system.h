static VOID WINAPI GetSystemInfo(LPSYSTEM_INFO lpSystemInfo)
{
	auto x = info<int>("GetSystemInfo", lpSystemInfo);
	orig(GetSystemInfo)(lpSystemInfo);
	lpSystemInfo->dwOemId;
	lpSystemInfo->wProcessorArchitecture;
	lpSystemInfo->wReserved;
	lpSystemInfo->dwPageSize;
	lpSystemInfo->lpMinimumApplicationAddress;
	lpSystemInfo->lpMaximumApplicationAddress;
	lpSystemInfo->dwActiveProcessorMask;
	lpSystemInfo->dwNumberOfProcessors;
	lpSystemInfo->dwProcessorType;
	lpSystemInfo->dwAllocationGranularity;
	lpSystemInfo->wProcessorLevel;
	lpSystemInfo->wProcessorRevision;
}

static void WINAPI GetNativeSystemInfo(LPSYSTEM_INFO lpSystemInfo)
{
	auto x = info<int>("GetNativeSystemInfo", lpSystemInfo);
	orig(GetNativeSystemInfo)(lpSystemInfo);
}

static VOID WINAPI GetSystemTime(LPSYSTEMTIME lpSystemTime)
{
	auto x = info<int>("GetSystemTime", lpSystemTime);
	orig(GetSystemTime)(lpSystemTime);
	SaveError s;
	lpSystemTime->wYear = config.dword("Time_wYear", lpSystemTime->wYear);
	lpSystemTime->wMonth = config.dword("Time_wMonth", lpSystemTime->wMonth);
	lpSystemTime->wDayOfWeek = config.dword("Time_wDayOfWeek", lpSystemTime->wDayOfWeek);
	lpSystemTime->wDay = config.dword("Time_wDay", lpSystemTime->wDay);
	lpSystemTime->wHour = config.dword("Time_wHour", lpSystemTime->wHour);
	lpSystemTime->wMinute = config.dword("Time_wMinute", lpSystemTime->wMinute);
	lpSystemTime->wSecond = config.dword("Time_wSecond", lpSystemTime->wSecond);
	lpSystemTime->wMilliseconds = config.dword("Time_wMilliseconds", lpSystemTime->wMilliseconds);
}

static VOID WINAPI GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
	auto x = info<int>("GetLocalTime", lpSystemTime);
	orig(GetLocalTime)(lpSystemTime);
	SaveError s;
	lpSystemTime->wYear = config.dword("Time_wYear", lpSystemTime->wYear);
	lpSystemTime->wMonth = config.dword("Time_wMonth", lpSystemTime->wMonth);
	lpSystemTime->wDayOfWeek = config.dword("Time_wDayOfWeek", lpSystemTime->wDayOfWeek);
	lpSystemTime->wDay = config.dword("Time_wDay", lpSystemTime->wDay);
	lpSystemTime->wHour = config.dword("Time_wHour", lpSystemTime->wHour);
	lpSystemTime->wMinute = config.dword("Time_wMinute", lpSystemTime->wMinute);
	lpSystemTime->wSecond = config.dword("Time_wSecond", lpSystemTime->wSecond);
	lpSystemTime->wMilliseconds = config.dword("Time_wMilliseconds", lpSystemTime->wMilliseconds);
}

static VOID WINAPI GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime)
{
	auto x = info<int>("GetSystemTimeAsFileTime", lpSystemTimeAsFileTime);
	orig(GetSystemTimeAsFileTime)(lpSystemTimeAsFileTime);
	SaveError s;
	lpSystemTimeAsFileTime->dwLowDateTime = config.dword("dwLowDateTime", lpSystemTimeAsFileTime->dwLowDateTime);
	lpSystemTimeAsFileTime->dwHighDateTime = config.dword("dwHighDateTime", lpSystemTimeAsFileTime->dwHighDateTime);
}

static BOOL WINAPI GetVersionEx(OSVersionInfo *lpVersionInfo)
{
	auto x = info<BOOL>(add_suffix<C>("GetVersionEx"), lpVersionInfo);
	x.result = orig(GetVersionEx)(lpVersionInfo);
	SaveError s;
	lpVersionInfo->dwMajorVersion = config.dword("dwMajorVersion", lpVersionInfo->dwMajorVersion);
	lpVersionInfo->dwMinorVersion = config.dword("dwMinorVersion", lpVersionInfo->dwMinorVersion);
	if (lpVersionInfo->dwOSVersionInfoSize == sizeof(OSVERSIONINFO))
		return x.result;
	lpVersionInfo->wProductType = config.dword("wProductType", lpVersionInfo->wProductType);
	return x.result;
}

static DWORD WINAPI GetVersion()
{
	auto x = info<DWORD>("GetVersion");
	if (config.exists("GetVersion"))
		x.result = config.dword("GetVersion");
	else
		x.result = orig(GetVersion)();
	return x.result;
}

static BOOL WINAPI VerifyVersionInfo(
	OSVersionInfo *lpVersionInformation,
	DWORD dwTypeMask,
	DWORDLONG dwlConditionMask
)
{
	auto x = info<BOOL>(add_suffix<C>("VerifyVersionInfo"), lpVersionInformation, dwTypeMask, dwlConditionMask);
	if (config.exists("VerifyVersionInfo"))
		x.result = config.dword("VerifyVersionInfo");
	else
		x.result = orig(VerifyVersionInfo)(lpVersionInformation, dwTypeMask, dwlConditionMask);
	return x.result;
}

static BOOL WINAPI GetProductInfo(
	DWORD dwOSMajorVersion,
	DWORD dwOSMinorVersion,
	DWORD dwSpMajorVersion,
	DWORD dwSpMinorVersion,
	PDWORD pdwReturnedProductType
)
{
	auto x = info<BOOL>("GetProductInfo", dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
	if (config.exists("GetProductInfo"))
	{
		*pdwReturnedProductType = config.dword("GetProductInfo");
		x.result = true;
	}
	else
	{
		x.result = orig(GetProductInfo)(dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
	}
	return x.result;
}

static BOOL WINAPI GetLogicalProcessorInformation(
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
	PDWORD ReturnLength
)
{
	auto x = info<BOOL>("GetLogicalProcessorInformation", Buffer, ReturnLength);
	x.result = orig(GetLogicalProcessorInformation)(Buffer, ReturnLength);
	SaveError s;
	x.l(-1, "unimplemented");
	return x.result;
}
static BOOL WINAPI GetLogicalProcessorInformationEx(
	LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Buffer,
	PDWORD ReturnedLength
)
{
	auto x = info<BOOL>("GetLogicalProcessorInformation", RelationshipType, Buffer, ReturnedLength);
	x.result = orig(GetLogicalProcessorInformationEx)(RelationshipType, Buffer, ReturnedLength);
	SaveError s;
	x.l(-1, "unimplemented");
	return x.result;
}

static BOOL WINAPI IsProcessorFeaturePresent(
	DWORD ProcessorFeature
)
{
	auto x = info<BOOL>("IsProcessorFeaturePresent", ProcessorFeature);
	return x.result = orig(IsProcessorFeaturePresent)(ProcessorFeature);
}

static DWORD WINAPI GetEnvironmentVariable(
	const C *lpName,
	C *lpBuffer,
	DWORD nSize
)
{
	auto x = info<DWORD>(add_suffix<C>("GetEnvironmentVariable"), lpName, lpBuffer, nSize);
	string buf(nSize + MAX_PATH, 0);
	x.result = orig(GetEnvironmentVariable)(lpName, &buf[0], nSize + MAX_PATH);
	SaveError s;
	buf.resize(x.result);
	if (x.result)
	{
		auto vp = cnv<C>::vp(buf);
		x.l(5, lpName, " = ", buf, " > ", vp);
		x.result = String::cpy(lpBuffer, vp, nSize);
		if (nSize <= vp.size())
			x.result = vp.size() + 1;
	}
	else
	{
		x.l(6, lpName, " is not exists");
	}
	return x.result;
}

static DWORD WINAPI ExpandEnvironmentStrings(
	const C *lpSrc,
	C *lpDst,
	DWORD nSize
)
{
	auto x = info<DWORD>(add_suffix<C>("ExpandEnvironmentStrings"), lpSrc, lpDst, nSize);
	string buf(nSize + MAX_PATH, 0);
	x.result = orig(ExpandEnvironmentStrings)(lpSrc, &buf[0], nSize);
	SaveError s;
	buf.resize(x.result);
	if (x.result)
	{
		auto vp = cnv<C>::vp(buf);
		x.l(5, lpSrc, " > ", buf, " > ", vp);
		x.result = String::cpy(lpDst, vp, nSize);
		if (nSize <= vp.size())
			x.result = vp.size() + 1;
	}
	else
	{
		x.l(6, "failed!");
	}
	return x.result;
}

static BOOL WINAPI GetComputerName(
	C *lpBuffer,
	LPDWORD lpnSize
)
{
	auto x = info<BOOL>(add_suffix<C>("GetComputerName"), lpBuffer, lpnSize);
	return x.result = orig(GetComputerName)(lpBuffer, lpnSize);
}

static BOOL WINAPI GetComputerNameEx(
	COMPUTER_NAME_FORMAT NameType,
	C *lpBuffer,
	LPDWORD nSize
)
{
	auto x = info<BOOL>(add_suffix<C>("GetComputerNameEx"), NameType, lpBuffer, nSize);
	return x.result = orig(GetComputerNameEx)(NameType, lpBuffer, nSize);
}

//GetCurrentPackage**?
//GetUserName -> advapi32
//GetUserNameEx -> secur32
//GetComputerObjectName -> secur32
//GetCurrentHwProfile -> advapi32
//https://msdn.microsoft.com/ja-jp/library/cc429371.aspx
