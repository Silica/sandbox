template<class C> struct Shell32
{
	typedef std::basic_string<C> string;
	template<class T>
	struct subtypes
	{
		typedef SHELLEXECUTEINFOA ShellExecuteInfo;
		typedef SHFILEINFOA ShellFileInfo;
		typedef SHFILEOPSTRUCTA ShellFileOperation;
	};
	template<>
	struct subtypes<wchar_t>
	{
		typedef SHELLEXECUTEINFOW ShellExecuteInfo;
		typedef SHFILEINFOW ShellFileInfo;
		typedef SHFILEOPSTRUCTW ShellFileOperation;
	};
	typedef typename subtypes<C>::ShellExecuteInfo ShellExecuteInfo;
	typedef typename subtypes<C>::ShellFileInfo ShellFileInfo;
	typedef typename subtypes<C>::ShellFileOperation ShellFileOperation;

	#include "shell32/folder.h"
	#include "shell32/directory.h"
	#include "shell32/exec.h"
	#include "shell32/icon.h"

	static void Attach()
	{
		HMODULE shell32 = LoadLibraryA("shell32");
		reg(shell32, add_suffix<C>("SHGetFolderPath"), SHGetFolderPath);
		reg(shell32, add_suffix<C>("SHGetFolderPathAndSubDir"), SHGetFolderPathAndSubDir);
		reg(shell32, add_suffix<C>("SHGetSpecialFolderPath"), SHGetSpecialFolderPath);

		reg(shell32, "SHGetSpecialFolderLocation", SHGetSpecialFolderLocation);
		reg(shell32, "SHGetFolderLocation", SHGetFolderLocation);
		reg(shell32, "SHGetFolderPathEx", SHGetFolderPathEx);
		reg(shell32, "SHGetKnownFolderPath", SHGetKnownFolderPath);
		reg(shell32, "SHGetKnownFolderIDList", SHGetKnownFolderIDList);

		reg(shell32, add_suffix<C>("SHGetPathFromIDList"), SHGetPathFromIDList);
		reg(shell32, "SHGetPathFromIDListEx", SHGetPathFromIDListEx);
		reg(shell32, "SHILCreateFromPath", SHILCreateFromPath);
		reg(shell32, add_suffix<C>("ILCreateFromPath"), ILCreateFromPath);
		reg(shell32, "SHSimpleIDListFromPath", SHSimpleIDListFromPath);

		reg(shell32, "SHGetDesktopFolder", SHGetDesktopFolder);

		reg(shell32, add_suffix<C>("SHCreateDirectoryEx"), SHCreateDirectoryEx);
		reg(shell32, "SHCreateDirectory", SHCreateDirectory);
		reg(shell32, add_suffix<C>("SHPathPrepareForWrite"), SHPathPrepareForWrite);
		reg(shell32, add_suffix<C>("SHGetFileInfo"), SHGetFileInfo);
		reg(shell32, add_suffix<C>("SHFileOperation"), SHFileOperation);

		reg(shell32, add_suffix<C>("ShellExecuteEx"), ShellExecuteEx);
		reg(shell32, add_suffix<C>("ShellExecute"), ShellExecute);

		reg(shell32, add_suffix<C>("ExtractIcon"), ExtractIcon);
		reg(shell32, add_suffix<C>("ExtractIconEx"), ExtractIconEx);
	}
};
//SHParseDisplayName
//SHGetSettings
