static INT WINAPI LZOpenFile(
	const C *lpFileName,
	LPOFSTRUCT lpReOpenBuf,
	WORD wStyle
)
{
	auto x = info<INT>(add_suffix<C>("LZOpenFile"), lpFileName, lpReOpenBuf, wStyle);
	string f;
	if (wStyle == OF_READ)
		f = cnv<C>::getExists(lpFileName).c_str();
	else
		f = cnv<C>::sp(lpFileName).c_str();
	x.l(6, lpFileName, " > ", f);
	return x.result = orig(LZOpenFile)(f.c_str(), lpReOpenBuf, wStyle);
}
