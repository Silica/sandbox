#pragma once

struct RegistryValue
{
	DWORD type;
	DWORD size;
	union
	{
		DWORD dword;
		__int64 qword;
		BYTE data[8];
		char str[8];
	};
};
struct RegistryEntry
{
	std::wstring name;
	std::vector<BYTE> data;
	RegistryValue *value;
	RegistryEntry(const std::wstring &f)
	{
		name = Path::GetFileName(f);

		auto h = CreateFileW(f.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h == INVALID_HANDLE_VALUE)
			return;

		DWORD size = GetFileSize(h, NULL);
		data.resize(size);

		DWORD dw;
		ReadFile(h, &data[0], size, &dw, NULL);
		value = (RegistryValue*)&data[0];

		CloseHandle(h);
	}
};
struct RegistryKey
{
	std::wstring path;
	RegistryKey(const std::wstring &p)
	{
		path = p;
	}
};
