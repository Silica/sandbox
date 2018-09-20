template<class C> struct Winmm
{
	typedef std::basic_string<C> string;

	static HMMIO WINAPI mmioOpen(
		C *szFilename,
		LPMMIOINFO lpmmioinfo,
		DWORD dwOpenFlags
	)
	{
		auto x = info<HMMIO>(add_suffix<C>("mmioOpen"), szFilename, lpmmioinfo, dwOpenFlags);
		x.l(9, "log only");
		return x.result = orig(mmioOpen)(szFilename, lpmmioinfo, dwOpenFlags);
	}

	static BOOL WINAPI sndPlaySound(
		const C *lpszSound,
		UINT fuSound
	)
	{
		auto x = info<BOOL>(add_suffix<C>("sndPlaySound"), lpszSound, fuSound);
		string f;
		if (lpszSound)
		{
			f = cnv<C>::getExists(lpszSound);
			lpszSound = f.c_str();
		}
		return x.result = orig(sndPlaySound)(lpszSound, fuSound);
	}

	static void Attach()
	{
		HMODULE winmm = LoadLibraryA("winmm");
		reg(winmm, add_suffix<C>("mmioOpen"), mmioOpen);
		reg(winmm, add_suffix<C>("sndPlaySound"), sndPlaySound);
	}

	static void Detach()
	{
		IAT(mmioOpen, orig(mmioOpen));
		IAT(sndPlaySound, orig(sndPlaySound));
	}
};
