#include "Pargon/Containers/Text.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace Pargon;

Text::Text(const std::wstring& string)
{
	if (!string.empty())
	{
		auto length = WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, string.data(), static_cast<int>(string.length()), nullptr, 0, NULL, NULL);

		if (length > 0)
		{
			_string.resize(length);
			WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, string.data(), static_cast<int>(string.length()), _string.data(), length, NULL, NULL);
		}
	}
}

auto Text::AsWide() -> std::wstring
{
	std::wstring string;

	if (!_string.empty())
	{
		auto length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, _string.data(), static_cast<int>(_string.length()), nullptr, 0);

		if (length > 0)
		{
			string.resize(length);
			MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, _string.data(), static_cast<int>(_string.length()), string.data(), length);
		}
	}

	return string;
}
