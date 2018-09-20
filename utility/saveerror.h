#pragma once

struct SaveError
{
	DWORD e;
	SaveError()
	{
		e = GetLastError();
	}
	~SaveError()
	{
		SetLastError(e);
	}
};
