static BOOL WINAPI PathFileExists(const C *pszPath)
{
	auto x = info<BOOL>(add_suffix<C>("PathFileExists"), pszPath);
	auto sp = cnv<C>::sp(pszPath);
	x.result = orig(PathFileExists)(sp.c_str());
	if (x.result)
	{
		SaveError s;
		x.l(7, pszPath, " > ", sp);
		return x.result;
	}
	auto cp = cnv<C>::cp(pszPath);
	x.result = orig(PathFileExists)(cp.c_str());
	if (x.result)
	{
		SaveError s;
		x.l(7, pszPath, " > ", cp);
		return x.result;
	}
	SaveError s;
	x.l(7, pszPath, " > false");
	return x.result;
}

static BOOL WINAPI PathIsDirectory(const C *pszPath)
{
	auto x = info<BOOL>(add_suffix<C>("PathIsDirectory"), pszPath);
	auto sp = cnv<C>::sp(pszPath);
	x.result = orig(PathIsDirectory)(sp.c_str());
	if (x.result)
	{
		SaveError s;
		x.l(7, pszPath, " > ", sp);
		return x.result;
	}
	auto cp = cnv<C>::cp(pszPath);
	x.result = orig(PathIsDirectory)(cp.c_str());
	SaveError s;
	if (x.result)
	{
		x.l(7, pszPath, " > ", cp);
	}
	else
	{
		x.l(7, pszPath, " > false");
	}
	return x.result;
}
