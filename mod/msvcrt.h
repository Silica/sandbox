struct MSVCRT
{
	static FILE *fopen(
		const char *filename,
		const char *mode
	)
	{
		auto x = info<FILE*>("fopen", filename, mode);
		std::string m = mode;
		if (m.find("w") == std::string::npos)
		{
			auto f = cnv<char>::getExists(filename);
			x.l(6, filename, " > ", f);
			x.result = orig(fopen)(f.c_str(), mode);
		}
		else
		{
			auto sp = cnv<char>::sp(filename);
			x.l(3, filename, " > ", sp);
			x.result = orig(fopen)(sp.c_str(), mode);
		}
		return x.result;
	}

	static void Attach()
	{
		HMODULE msvcrt = LoadLibraryA("MSVCRT");
		reg(msvcrt, "fopen", fopen);
	}
};
