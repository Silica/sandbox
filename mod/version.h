template<class C> struct Version
{
	typedef std::basic_string<C> string;

	static BOOL WINAPI GetFileVersionInfo(
		const C *lptstrFilename,
		DWORD dwHandle,
		DWORD dwLen,
		LPVOID lpData
	)
	{
		auto x = info<BOOL>(add_suffix<C>("GetFileVersionInfo"), lptstrFilename, dwHandle, dwLen, lpData);
		auto ep = cnv<C>::getExists(lptstrFilename);
		x.l(5, lptstrFilename, " > ", ep);
		x.result = orig(GetFileVersionInfo)(ep.c_str(), dwHandle, dwLen, lpData);
		return x.result;
	}
	static BOOL WINAPI GetFileVersionInfoEx(
		DWORD dwFlags,
		const C *lpwstrFilename,
		DWORD dwHandle,
		DWORD dwLen,
		LPVOID lpData
	)
	{
		auto x = info<BOOL>(add_suffix<C>("GetFileVersionInfoEx"), dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
		auto ep = cnv<C>::getExists(lpwstrFilename);
		x.l(5, lpwstrFilename, " > ", ep);
		x.result = orig(GetFileVersionInfoEx)(dwFlags, ep.c_str(), dwHandle, dwLen, lpData);
		return x.result;
	}
	static DWORD WINAPI GetFileVersionInfoSize(
		const C *lptstrFilename,
		LPDWORD lpdwHandle
	)
	{
		auto x = info<DWORD>(add_suffix<C>("GetFileVersionInfoSize"), lptstrFilename, lpdwHandle);
		auto ep = cnv<C>::getExists(lptstrFilename);
		x.l(5, lptstrFilename, " > ", ep);
		x.result = orig(GetFileVersionInfoSize)(ep.c_str(), lpdwHandle);
		return x.result;
	}
	static DWORD WINAPI GetFileVersionInfoSizeEx(
		DWORD dwFlags,
		const C *lpwstrFilename,
		LPDWORD lpdwHandle
	)
	{
		auto x = info<DWORD>(add_suffix<C>("GetFileVersionInfoSizeEx"), dwFlags, lpwstrFilename, lpdwHandle);
		auto ep = cnv<C>::getExists(lpwstrFilename);
		x.l(5, lpwstrFilename, " > ", ep);
		x.result = orig(GetFileVersionInfoSizeEx)(dwFlags, ep.c_str(), lpdwHandle);
		return x.result;
	}

	static BOOL WINAPI VerQueryValue(
		const LPVOID pBlock,
		const C *lpSubBlock,
		LPVOID *lplpBuffer,
		PUINT puLen
	)
	{
		auto x = info<BOOL>(add_suffix<C>("VerQueryValue"), pBlock, lpSubBlock, lplpBuffer, puLen);
		x.l(9, "log only");
		return x.result = orig(VerQueryValue)(pBlock, lpSubBlock, lplpBuffer, puLen);
	}

	static void Attach()
	{
		HMODULE version = LoadLibraryA("version");
		//Api-ms-win-core-version-l1-1-0?
		reg(version, add_suffix<C>("GetFileVersionInfo"), GetFileVersionInfo);
		reg(version, add_suffix<C>("GetFileVersionInfoEx"), GetFileVersionInfoEx);
		reg(version, add_suffix<C>("GetFileVersionInfoSize"), GetFileVersionInfoSize);
		reg(version, add_suffix<C>("GetFileVersionInfoSizeEx"), GetFileVersionInfoSizeEx);
		reg(version, add_suffix<C>("VerQueryValue"), VerQueryValue);
	}
};
