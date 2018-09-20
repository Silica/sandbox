#pragma comment(lib, "ole32")
struct Ole32
{
	struct PreLoad
	{
		HMODULE m = NULL;
		PreLoad(REFCLSID rclsid)
		{
			wchar_t *buf;
			StringFromCLSID(rclsid, &buf);
			auto subkey = String::concat("CLSID\\", buf, "\\InProcServer32");
			CoTaskMemFree(buf);
			HKEY key;
			if (!Advapi32<char>::RegOpenKeyEx(HKEY_CLASSES_ROOT, subkey.c_str(), 0, KEY_READ, &key))
			{
				wchar_t data[MAX_PATH];
				DWORD size = MAX_PATH;
				if (!Advapi32<wchar_t>::RegQueryValueEx(key, NULL, NULL, NULL, (BYTE*)&data, &size))
				{
					wchar_t dest[MAX_PATH];
					Kernel32<wchar_t>::ExpandEnvironmentStrings(data, dest, MAX_PATH);
					m = Kernel32<wchar_t>::GetModuleHandle(dest);
					if (m)
					{
						m = NULL;
					}
					else
					{
						log(1, "CoCreateInstance:PreLoad:", dest);
						m = Kernel32<wchar_t>::LoadLibraryEx(dest, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
					}
				}
				Advapi32<char>::RegCloseKey(key);
			}
		}
		~PreLoad()
		{
			if (m)
				FreeLibrary(m);
		}
	};

	static HRESULT WINAPI CoCreateInstance(
		REFCLSID rclsid,
		LPUNKNOWN pUnkOuter,
		DWORD dwClsContext,
		REFIID riid,
		LPVOID *ppv
	)
	{
		auto x = info<HRESULT>("CoCreateInstance", rclsid, pUnkOuter, dwClsContext, riid, ppv);
		if (config.dword("NO_CLSID_ShellLink") && rclsid == CLSID_ShellLink)
		{
			x.l(1, "CLSID_ShellLink");
			return x.result = 632L;
		}
		x.l(1, FuncInfo::guid(rclsid));
		PreLoad p(rclsid);
		x.result = orig(CoCreateInstance)(rclsid, pUnkOuter, dwClsContext, riid, ppv);
		return x.result;
	}
	static HRESULT WINAPI combaseCoCreateInstance(
		REFCLSID rclsid,
		LPUNKNOWN pUnkOuter,
		DWORD dwClsContext,
		REFIID riid,
		LPVOID *ppv
	)
	{
		auto x = info<HRESULT>("combase.CoCreateInstance", rclsid, pUnkOuter, dwClsContext, riid, ppv);
		if (config.dword("NO_CLSID_ShellLink") && rclsid == CLSID_ShellLink)
		{
			x.l(1, "CLSID_ShellLink");
			return x.result = 632L;
		}
		x.l(1, FuncInfo::guid(rclsid));
		PreLoad p(rclsid);
		x.result = orig(combaseCoCreateInstance)(rclsid, pUnkOuter, dwClsContext, riid, ppv);
		return x.result;
	}

	static HRESULT WINAPI CoCreateInstanceEx(
		REFCLSID Clsid,
		IUnknown *punkOuter,
		DWORD dwClsCtx,
		COSERVERINFO *pServerInfo,
		DWORD dwCount,
		MULTI_QI *pResults
	)
	{
		auto x = info<HRESULT>("CoCreateInstanceEx", Clsid, punkOuter, dwClsCtx, pServerInfo, dwCount, pResults);
		if (config.dword("NO_CLSID_ShellLink") && Clsid == CLSID_ShellLink)
		{
			x.l(1, "CLSID_ShellLink");
			return x.result = 632L;
		}
		x.l(1, FuncInfo::guid(Clsid));
		PreLoad p(Clsid);
		x.result = orig(CoCreateInstanceEx)(Clsid, punkOuter, dwClsCtx, pServerInfo, dwCount, pResults);
		return x.result;
	}

	static HRESULT WINAPI CoGetInterfaceAndReleaseStream(
		LPSTREAM pStm,
		REFIID iid,
		LPVOID *ppv
	)
	{
		auto x = info<HRESULT>("CoGetInterfaceAndReleaseStream", pStm, iid, ppv);
		x.l(9, "log only");
		x.result = orig(CoGetInterfaceAndReleaseStream)(pStm, iid, ppv);
		return x.result;
	}

	static HRESULT WINAPI CoMarshalInterThreadInterfaceInStream(
		REFIID riid,
		LPUNKNOWN pUnk,
		LPSTREAM *ppStm
	)
	{
		auto x = info<HRESULT>("CoMarshalInterThreadInterfaceInStream", riid, pUnk, ppStm);
		x.l(9, "log only");
		x.result = orig(CoMarshalInterThreadInterfaceInStream)(riid, pUnk, ppStm);
		return x.result;
	}

	static HRESULT WINAPI CoRegisterClassObject(
		REFCLSID rclsid,
		LPUNKNOWN pUnk,
		DWORD dwClsContext,
		DWORD flags,
		LPDWORD lpdwRegister
	)
	{
		auto x = info<HRESULT>("CoRegisterClassObject", rclsid, pUnk, dwClsContext, flags, lpdwRegister);
		x.l(9, "log only");
		x.result = orig(CoRegisterClassObject)(rclsid, pUnk, dwClsContext, flags, lpdwRegister);
		return x.result;
	}

	static HRESULT WINAPI CreateFileMoniker(
		LPCOLESTR lpszPathName,
		LPMONIKER *ppmk
	)
	{
		auto x = info<HRESULT>("CreateFileMoniker", lpszPathName, ppmk);
		x.l(9, "log only");
		return x.result = orig(CreateFileMoniker)(lpszPathName, ppmk);
	}

	static HRESULT WINAPI StgCreateDocfile(
		const WCHAR *pwcsName,
		DWORD grfMode,
		DWORD reserved,
		IStorage **ppstgOpen
	)
	{
		auto x = info<HRESULT>("StgCreateDocfile", pwcsName, grfMode, reserved, ppstgOpen);
		x.l(9, "log only");
		x.result = orig(StgCreateDocfile)(pwcsName, grfMode, reserved, ppstgOpen);
		return x.result;
	}

	static HRESULT WINAPI StgOpenStorage(
		const WCHAR *pwcsName,
		IStorage *pstgPriority,
		DWORD grfMode,
		SNB snbExclude,
		DWORD reserved,
		IStorage **ppstgOpen
	)
	{
		auto x = info<HRESULT>("StgOpenStorage", pwcsName, pstgPriority, grfMode, snbExclude, reserved, ppstgOpen);
		x.l(9, "log only");
		return x.result = orig(StgOpenStorage)(pwcsName, pstgPriority, grfMode, snbExclude, reserved, ppstgOpen);
	}

	static HRESULT WINAPI StgOpenStorageEx(
		const WCHAR *pwcsName,
		DWORD grfMode,
		DWORD stgfmt,
		DWORD grfAttrs,
		STGOPTIONS *pStgOptions,
		PSECURITY_DESCRIPTOR pSecurityDescriptor,
		REFIID riid,
		void **ppObjectOpen
	)
	{
		auto x = info<HRESULT>("StgOpenStorage", pwcsName, grfMode, stgfmt, grfAttrs, pStgOptions, pSecurityDescriptor, riid, ppObjectOpen);
		x.l(9, "log only");
		return x.result = orig(StgOpenStorageEx)(pwcsName, grfMode, stgfmt, grfAttrs, pStgOptions, pSecurityDescriptor, riid, ppObjectOpen);
	}

	static HRESULT WINAPI OleLoadPicturePath(
		LPOLESTR  szURLorPath,
		LPUNKNOWN punkCaller,
		DWORD dwReserved,
		OLE_COLOR clrReserved,
		REFIID riid,
		LPVOID *ppvRet
	)
	{
		auto x = info<HRESULT>("OleLoadPicturePath", szURLorPath, punkCaller, dwReserved, clrReserved, riid, ppvRet);
		auto f = cnv<wchar_t>::getExists(szURLorPath);
		return x.result = orig(OleLoadPicturePath)(szURLorPath, punkCaller, dwReserved, clrReserved, riid, ppvRet);
	}

	static HRESULT WINAPI LoadTypeLib(
		LPCOLESTR szFile,
		ITypeLib **pptlib
	)
	{
		auto x = info<HRESULT>("LoadTypeLib", szFile, pptlib);
		auto f = cnv<wchar_t>::getExists(szFile);
		x.l(7, szFile, " > ", f);
		x.result = orig(LoadTypeLib)(f.c_str(), pptlib);
		return x.result;
	}
	static HRESULT WINAPI LoadTypeLibEx(
		LPCOLESTR szFile,
		REGKIND   regkind,
		ITypeLib  **pptlib
	)
	{
		auto x = info<HRESULT>("LoadTypeLib", szFile, regkind, pptlib);
		auto f = cnv<wchar_t>::getExists(szFile);
		x.l(7, szFile, " > ", f);
		x.result = orig(LoadTypeLibEx)(f.c_str(), regkind, pptlib);
		return x.result;
	}

	static HRESULT WINAPI RegisterTypeLib(
		ITypeLib  *ptlib,
		LPCOLESTR szFullPath,
		LPCOLESTR szHelpDir
	)
	{
		auto x = info<HRESULT>("RegisterTypeLib", ptlib, szFullPath, szHelpDir);
		auto f = cnv<wchar_t>::getExists(szFullPath);
		x.l(7, szFullPath, " > ", f);
		x.result = orig(RegisterTypeLib)(ptlib, f.c_str(), szHelpDir);
		return x.result;
	}

	static void Attach()
	{
		HMODULE ole32 = LoadLibraryA("ole32");
		reg(ole32, "CoCreateInstance", CoCreateInstance);
		reg(ole32, "CoCreateInstanceEx", CoCreateInstanceEx);
		//CoGetObject
		/*reg(ole32, "CoGetInterfaceAndReleaseStream", CoGetInterfaceAndReleaseStream);
		reg(ole32, "CoMarshalInterThreadInterfaceInStream", CoMarshalInterThreadInterfaceInStream);
		reg(ole32, "CoRegisterClassObject", CoRegisterClassObject);
		reg(ole32, "CreateFileMoniker", CreateFileMoniker);
		reg(ole32, "StgCreateDocfile", StgCreateDocfile);
		reg(ole32, "StgOpenStorage", StgOpenStorage);
		reg(ole32, "StgOpenStorageEx", StgOpenStorageEx);*/

		HMODULE oleaut32 = LoadLibraryA("oleaut32");
		reg(oleaut32, "OleLoadPicturePath", OleLoadPicturePath);
		reg(oleaut32, "LoadTypeLib", LoadTypeLib);
		reg(oleaut32, "LoadTypeLibEx", LoadTypeLibEx);
		reg(oleaut32, "RegisterTypeLib", RegisterTypeLib);

		if (config.dword("combase"))
		{
			HMODULE combase = LoadLibraryA("combase");
			if (combase)
			{
				log(9, "combase load");
				reg(combase, "CoCreateInstance", combaseCoCreateInstance);
			}
		}
	}
};
//IShellLink
//IPersistFile
