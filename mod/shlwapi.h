template<class C> struct Shlwapi
{
	typedef std::basic_string<C> string;

	#include "shlwapi/path.h"
	#include "shlwapi/reg.h"

	static void Attach()
	{
		HMODULE shlwapi = LoadLibraryA("shlwapi");
		reg(shlwapi, add_suffix<C>("PathFileExists"), PathFileExists);
		reg(shlwapi, add_suffix<C>("PathIsDirectory"), PathIsDirectory);

		reg(shlwapi, add_suffix<C>("SHSetValue"), SHSetValue);
		reg(shlwapi, add_suffix<C>("SHDeleteKey"), SHDeleteKey);
		reg(shlwapi, add_suffix<C>("SHDeleteValue"), SHDeleteValue);
	}
};
//SHCreateStreamOnFile
