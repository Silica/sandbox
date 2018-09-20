template<class C> struct User32
{
	typedef std::basic_string<C> string;

	static HANDLE WINAPI LoadImage(
		HINSTANCE hinst,
		const C *lpszName,
		UINT uType,
		int cxDesired,
		int cyDesired,
		UINT fuLoad
	)
	{
		auto x = info<HANDLE>(add_suffix<C>("LoadImage"), hinst, lpszName, uType, cxDesired, cyDesired, fuLoad);
		if ((pointer_size_uint)lpszName >> 16)
		{
			auto f = cnv<C>::getExists(lpszName);
			x.result = orig(LoadImage)(hinst, f.c_str(), uType, cxDesired, cyDesired, fuLoad);
		}
		else
		{
			x.result = orig(LoadImage)(hinst, lpszName, uType, cxDesired, cyDesired, fuLoad);
		}
		return x.result;
	}

	static HCURSOR WINAPI LoadCursorFromFile(
		const C *lpFileName
	)
	{
		auto x = info<HCURSOR>(add_suffix<C>("LoadCursorFromFile"), lpFileName);
		if ((pointer_size_uint)lpFileName >> 16)
		{
			auto f = cnv<C>::getExists(lpFileName);
			x.result = orig(LoadCursorFromFile)(f.c_str());
		}
		else
		{
			x.result = orig(LoadCursorFromFile)(lpFileName);
		}
		return x.result;
	}

	static BOOL WINAPI SystemParametersInfo(
		UINT uiAction,
		UINT uiParam,
		PVOID pvParam,
		UINT fWinIni
	)
	{
		auto x = info<BOOL>(add_suffix<C>("SystemParametersInfo"), uiAction, uiParam, pvParam, fWinIni);
		return x.result = orig(SystemParametersInfo)(uiAction, uiParam, pvParam, fWinIni);
	}

	static void Attach()
	{
		HMODULE user32 = LoadLibraryA("user32");
		reg(user32, add_suffix<C>("LoadImage"), LoadImage);
		reg(user32, add_suffix<C>("LoadCursorFromFile"), LoadCursorFromFile);
		//reg(user32, add_suffix<C>("SystemParametersInfo"), SystemParametersInfo);
	}
};
