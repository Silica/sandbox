template<class C> struct Msi
{
	typedef std::basic_string<C> string;

	static UINT WINAPI MsiInstallProduct(
		const C *szPackagePath,
		const C *szCommandLine
	)
	{
		auto x = info<UINT>(add_suffix<C>("MsiInstallProduct"), szPackagePath, szCommandLine);
		x.l(-1, "unimplemented");
		x.l(-1, szPackagePath, " szCommandLine=", szCommandLine);
		return x.result = orig(MsiInstallProduct)(szPackagePath, szCommandLine);
	}

	static void Attach()
	{
		HMODULE msi = LoadLibraryA("msi");
		reg(msi, add_suffix<C>("MsiInstallProduct"), MsiInstallProduct);
	}
};
