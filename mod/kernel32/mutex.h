static HANDLE WINAPI CreateMutex(
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL bInitialOwner,
	const C *lpName
)
{
	auto x = info<HANDLE>(add_suffix<C>("CreateMutex"), lpMutexAttributes, bInitialOwner, lpName);
	if (lpName)
	{
		std::string s = String::tstr<char>(lpName);
		if (!String::starts_with(s, "Local"))
		{
			auto n = lpName + cnv<C>::mutex;
			return x.result = orig(CreateMutex)(lpMutexAttributes, bInitialOwner, n.c_str());
		}
	}
	return x.result = orig(CreateMutex)(lpMutexAttributes, bInitialOwner, lpName);
}
static HANDLE WINAPI CreateMutexEx(
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	const C *lpName,
	DWORD dwFlags,
	DWORD dwDesiredAccess
)
{
	auto x = info<HANDLE>(add_suffix<C>("CreateMutexEx"), lpMutexAttributes, lpName, dwFlags, dwDesiredAccess);
	if (lpName)
	{
		std::string s = String::tstr<char>(lpName);
		if (!String::starts_with(s, "Local"))
		{
			auto n = lpName + cnv<C>::mutex;
			return x.result = orig(CreateMutexEx)(lpMutexAttributes, n.c_str(), dwFlags, dwDesiredAccess);
		}
	}
	return x.result = orig(CreateMutexEx)(lpMutexAttributes, lpName, dwFlags, dwDesiredAccess);
}
static HANDLE WINAPI OpenMutex(
	DWORD dwDesiredAccess,
	BOOL bInheritHandle,
	const C *lpName
)
{
	auto x = info<HANDLE>(add_suffix<C>("OpenMutex"), dwDesiredAccess, bInheritHandle, lpName);
	if (lpName)
	{
		std::string s = String::tstr<char>(lpName);
		if (!String::starts_with(s, "Local"))
		{
			auto n = lpName + cnv<C>::mutex;
			return x.result = orig(OpenMutex)(dwDesiredAccess, bInheritHandle, n.c_str());
		}
	}
	return x.result = orig(OpenMutex)(dwDesiredAccess, bInheritHandle, lpName);
}
