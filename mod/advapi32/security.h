static BOOL WINAPI GetFileSecurity(
	const C *lpFileName,
	SECURITY_INFORMATION RequestedInformation,
	PSECURITY_DESCRIPTOR pSecurityDescriptor,
	DWORD nLength,
	LPDWORD lpnLengthNeeded
)
{
	auto x = info<BOOL>(add_suffix<C>("GetFileSecurity"), lpFileName, RequestedInformation, pSecurityDescriptor, nLength, lpnLengthNeeded);
	auto f = cnv<C>::getExists(lpFileName);
	x.l(9, lpFileName, " > ", f);
	return x.result = orig(GetFileSecurity)(f.c_str(), RequestedInformation, pSecurityDescriptor, nLength, lpnLengthNeeded);
}

static BOOL WINAPI SetFileSecurity(
	const C *lpFileName,
	SECURITY_INFORMATION SecurityInformation,
	PSECURITY_DESCRIPTOR pSecurityDescriptor
)
{
	auto x = info<BOOL>(add_suffix<C>("SetFileSecurity"), lpFileName, SecurityInformation, pSecurityDescriptor);
	auto f = cnv<C>::sp(lpFileName);
	x.l(9, lpFileName, " > ", f);
	return x.result = orig(SetFileSecurity)(f.c_str(), SecurityInformation, pSecurityDescriptor);
}
