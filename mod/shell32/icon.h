static HICON WINAPI ExtractIcon(
	HINSTANCE hInst,
	const C *lpszExeFileName,
	UINT nIconIndex
)
{
	auto x = info<HICON>(add_suffix<C>("ExtractIcon"), hInst, lpszExeFileName, nIconIndex);
	auto ep = cnv<C>::getExists(lpszExeFileName);
	x.l(7, lpszExeFileName, " > ", ep);
	return x.result = orig(ExtractIcon)(hInst, ep.c_str(), nIconIndex);
}

static UINT WINAPI ExtractIconEx(
	const C *lpszFile,
	int nIconIndex,
	HICON *phiconLarge,
	HICON *phiconSmall,
	UINT nIcons
)
{
	auto x = info<UINT>(add_suffix<C>("ExtractIconEx"), lpszFile, nIconIndex, phiconLarge, phiconSmall, nIcons);
	auto ep = cnv<C>::getExists(lpszFile);
	x.l(7, lpszFile, " > ", ep);
	return x.result = orig(ExtractIconEx)(ep.c_str(), nIconIndex, phiconLarge, phiconSmall, nIcons);
}
