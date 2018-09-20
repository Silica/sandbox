static DWORD WINAPI GetLogicalDrives()
{
	auto x = info<DWORD>("GetLogicalDrives");
	if (config.exists("GetLogicalDrives"))
		x.result = config.dword("GetLogicalDrives");
	else
		x.result = orig(GetLogicalDrives)();
	return x.result;
}

static DWORD WINAPI GetLogicalDriveStrings(
	DWORD nBufferLength,
	C *lpBuffer
)
{
	auto x = info<DWORD>(add_suffix<C>("GetLogicalDriveStrings"));
	auto drives = GetLogicalDrives();
	DWORD p = 0;
	for (int i = 0; i < 26; i++)
	{
		if (drives & (1 << i))
		{
			if (p < (nBufferLength - 3))
			{
				lpBuffer[p++] = C('a' + i);
				lpBuffer[p++] = C(':');
				lpBuffer[p++] = C('\\');
				lpBuffer[p++] = 0;
			}
			else
			{
				p += 4;
			}
		}
	}
	if (p <= nBufferLength)
		lpBuffer[p] = 0;
	return x.result = p;
}

static UINT WINAPI GetDriveType(const C *lpRootPathName)
{
	auto x = info<UINT>(add_suffix<C>("GetDriveType"), lpRootPathName);
	x.result = orig(GetDriveType)(lpRootPathName);
	return x.result;
	//todo
}

static BOOL WINAPI GetDiskFreeSpace(
	const C *lpRootPathName,
	LPDWORD lpSectorsPerCluster,
	LPDWORD lpBytesPerSector,
	LPDWORD lpNumberOfFreeClusters,
	LPDWORD lpTotalNumberOfClusters
)
{
	auto x = info<BOOL>(add_suffix<C>("GetDiskFreeSpace"), lpRootPathName, lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters);
	if (lpRootPathName)
	{
		auto sp = cnv<C>::sp(lpRootPathName);
		x.result = orig(GetDiskFreeSpace)(sp.c_str(), lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters);
	}
	else
	{
		x.result = orig(GetDiskFreeSpace)(lpRootPathName, lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters);
	}
	return x.result;
}

static BOOL WINAPI GetDiskFreeSpaceEx(
	const C *lpDirectoryName,
	PULARGE_INTEGER lpFreeBytesAvailable,
	PULARGE_INTEGER lpTotalNumberOfBytes,
	PULARGE_INTEGER lpTotalNumberOfFreeBytes
)
{
	auto x = info<BOOL>(add_suffix<C>("GetDiskFreeSpaceEx"), lpDirectoryName, lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
	if (lpDirectoryName)
	{
		auto sp = cnv<C>::sp(lpDirectoryName);
		x.result = orig(GetDiskFreeSpaceEx)(sp.c_str(), lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
	}
	else
	{
		x.result = orig(GetDiskFreeSpaceEx)(lpDirectoryName, lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
	}
	return x.result;
}

static BOOL WINAPI GetVolumeInformation(
	const C *lpRootPathName,
	C *lpVolumeNameBuffer,
	DWORD nVolumeNameSize,
	LPDWORD lpVolumeSerialNumber,
	LPDWORD lpMaximumComponentLength,
	LPDWORD lpFileSystemFlags,
	LPTSTR lpFileSystemNameBuffer,
	DWORD nFileSystemNameSize
)
{
	auto x = info<BOOL>(add_suffix<C>("GetVolumeInformation"), lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
	x.l(-1, "unimplemented");
	x.result = orig(GetVolumeInformation)(lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
	x.l(6, "lpVolumeNameBuffer=", lpVolumeNameBuffer);
	return x.result;
}

static BOOL WINAPI GetVolumePathName(
	C *lpszFileName,
	C *lpszVolumePathName,
	DWORD cchBufferLength
)
{
	auto x = info<BOOL>(add_suffix<C>("GetVolumePathName"), lpszFileName, lpszVolumePathName, cchBufferLength);
	x.result = orig(GetVolumePathName)(lpszFileName, lpszVolumePathName, cchBufferLength);
	x.l(6, "lpszVolumePathName=", lpszVolumePathName);
	return x.result;
}
//SetVolumeLabel
