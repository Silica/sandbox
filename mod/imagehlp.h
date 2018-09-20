struct Imagehlp
{
	static BOOL WINAPI MakeSureDirectoryPathExists(const char *DirPath)
	{
		auto x = info<BOOL>("MakeSureDirectoryPathExists", DirPath);
		auto sp = cnv<char>::sp(DirPath);
		x.l(3, DirPath, " > ", sp);
		return x.result = orig(MakeSureDirectoryPathExists)(sp.c_str());
	}

	static void Attach()
	{
		HMODULE imagehlp = LoadLibraryA("imagehlp");
		reg(imagehlp, "MakeSureDirectoryPathExists", MakeSureDirectoryPathExists);
	}
};
