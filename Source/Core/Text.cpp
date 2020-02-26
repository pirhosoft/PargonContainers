#include "Pargon/Containers/Buffer.h"
#include "Pargon/Containers/String.h"
#include "Pargon/Containers/Text.h"

#include "utf8.h"

using namespace Pargon;

auto Pargon::GuessEncoding(BufferView buffer) -> Encoding
{
	auto size = buffer.Size();
	auto data = buffer.begin();

	if (size >= 4 && data[0] == 0x00 && data[1] == 0x00 && data[2] == 0xFE && data[3] == 0xFF) return Encoding::Utf32Big;
	if (size >= 4 && data[0] == 0xFF && data[1] == 0xFE && data[2] == 0x00 && data[3] == 0x00) return Encoding::Utf32Little;
	if (size >= 2 && data[0] == 0xFE && data[1] == 0xFF) return Encoding::Utf16Big;
	if (size >= 2 && data[0] == 0xFF && data[1] == 0xFE) return Encoding::Utf16Little;
	if (size >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) return Encoding::Utf8;

	if (size > 1000000)
		return Encoding::Unknown;

	auto scan = size < 1000 ? size : 1000;
	for (auto i = 0; i < scan; i++)
	{
		if ((data[i] >= 127) || (data[i] <= 8) || (data[i] >= 14 && data[i] <= 31))
			return Encoding::Unknown;
	}

	return Encoding::Ascii;
}

auto TextIterator::operator*() -> char32_t
{
	return utf8::unchecked::peek_next(_location);
}

auto TextIterator::operator++() -> TextIterator&
{
	utf8::unchecked::next(_location);
	return *this;
}

auto TextIterator::operator--() -> TextIterator&
{
	utf8::unchecked::prior(_location);
	return *this;
}

auto TextIterator::operator+(int increment) -> TextIterator
{
	auto incremented = *this;
	utf8::unchecked::advance(incremented, increment);
	return incremented;
}

auto TextIterator::operator-(int decrement) -> TextIterator
{
	auto decremented = *this;

	for (auto i = 0; i < decrement; i++)
		utf8::unchecked::prior(decremented);

	return decremented;
}

auto TextIterator::operator+=(int increment) -> TextIterator&
{
	utf8::unchecked::advance(_location, increment);
	return *this;
}

auto TextIterator::operator-=(int decrement) -> TextIterator&
{
	for (auto i = 0; i < decrement; i++)
		utf8::unchecked::prior(_location);

	return *this;
}

auto TextIterator::operator-(TextIterator& other) -> int
{
	return static_cast<int>(utf8::unchecked::distance(_location, other._location));
}

Text::Text(const std::string& string)
{
	FromUtf8(string.data(), string.data() + string.size());
}

Text::Text(const std::u16string& string)
{
	FromUtf16(string.data(), string.data() + string.size());
}

Text::Text(const std::u32string& string)
{
	FromUtf32(string.data(), string.data() + string.size());
}

Text::Text(StringView string)
{
	FromUtf8(string.begin(), string.end());
}

Text::Text(TextView text) :
	_string(text.begin().Data(), text.end().Data())
{
}

Text::Text(BufferView buffer, Encoding encoding)
{
	if (encoding == Encoding::Unknown)
		encoding = GuessEncoding(buffer);

	switch (encoding)
	{
		case Encoding::Ascii:
		case Encoding::Utf8:
		{
			FromUtf8(reinterpret_cast<const char*>(buffer.begin()), reinterpret_cast<const char*>(buffer.end()));
			break;
		}
		case Encoding::Utf16Big:
		case Encoding::Utf16Little:
		{
			auto swap = (encoding == Encoding::Utf16Big && NativeEndian != Endian::Big) || (encoding == Encoding::Utf16Little && NativeEndian != Endian::Little);

			std::u16string string;
			for (auto i = 0; i < buffer.Size() - 1; i += 2)
			{
				auto first = char16_t{ buffer.Byte(i) };
				auto second = char16_t{ buffer.Byte(i + 1) };

				if (swap) second <<= 8;
				else first <<= 8;

				string.push_back(first | second);
			}

			FromUtf16(string.data(), string.data() + string.size());
			break;
		}
		case Encoding::Utf32Big:
		case Encoding::Utf32Little:
		{
			auto swap = (encoding == Encoding::Utf32Big && NativeEndian != Endian::Big) || (encoding == Encoding::Utf32Little && NativeEndian != Endian::Little);

			std::u32string string;
			for (auto i = 0; i < buffer.Size() - 3; i += 4)
			{
				auto first = char32_t{ buffer.Byte(i) };
				auto second = char32_t{ buffer.Byte(i + 1) };
				auto third = char32_t{ buffer.Byte(i + 2) };
				auto fourth = char32_t{ buffer.Byte(i + 3) };

				if (swap)
				{
					third <<= 8;
					second <<= 16;
					first <<= 24;
				}
				else
				{
					second <<= 8;
					third <<= 16;
					fourth <<= 24;
				}

				string.push_back(first | second | third | fourth);
			}

			FromUtf32(string.data(), string.data() + string.size());
			break;
		}
	}
}

auto Text::Length() const -> int
{
	return static_cast<int>(utf8::unchecked::distance(_string.begin(), _string.end()));
}

auto Text::Character(int index) const -> char32_t
{
	if (index >= Length())
		return 0;

	auto iterator = begin();
	utf8::unchecked::advance(iterator, index);
	return utf8::unchecked::peek_next(iterator);
}

auto Text::GetHash() const -> std::size_t
{
	return std::hash<std::string>()(_string);
}

auto Text::AsAscii() -> std::string
{
	std::string string;

	for (auto c : _string)
		string.push_back(c < 128 ? c : '\0');

	return string;
}

auto Text::AsUtf16() -> std::u16string
{
	std::u16string u16;
	utf8::unchecked::utf8to16(_string.begin(), _string.end(), std::back_inserter(u16));
	return u16;
}

auto Text::AsUtf32() -> std::u32string
{
	std::u32string u32;
	utf8::unchecked::utf8to32(_string.begin(), _string.end(), std::back_inserter(u32));
	return u32;
}

void Text::Append(char c)
{
	utf8::unchecked::append(c, std::back_inserter(_string));
}

void Text::Append(wchar_t c)
{
	utf8::unchecked::append(c, std::back_inserter(_string));
}

void Text::Append(char16_t c)
{
	utf8::unchecked::append(c, std::back_inserter(_string));
}

void Text::Append(char32_t c)
{
	utf8::unchecked::append(c, std::back_inserter(_string));
}

void Text::Append(StringView string)
{
	for (auto character : string)
		utf8::unchecked::append(character, std::back_inserter(_string));
}

void Text::Append(TextView text)
{
	for (auto character : text)
		utf8::unchecked::append(character, std::back_inserter(_string));
}

void Text::FromUtf8(const char* begin, const char* end)
{
	utf8::replace_invalid(begin, end, std::back_inserter(_string));
}

void Text::FromUtf16(const char16_t* begin, const char16_t* end)
{
	try
	{
		utf8::utf16to8(begin, end, std::back_inserter(_string));
	}
	catch (...)
	{
	}
}

void Text::FromUtf32(const char32_t* begin, const char32_t* end)
{
	try
	{
		utf8::utf32to8(begin, end, std::back_inserter(_string));
	}
	catch (...)
	{
	}
}

auto TextView::Length() const -> int
{
	return static_cast<int>(utf8::unchecked::distance(_characters, _characters + _length));
}

auto TextView::Character(int index) const -> char32_t
{
	if (index >= Length())
		return 0;

	auto iterator = begin();
	utf8::unchecked::advance(iterator, index);
	return utf8::unchecked::peek_next(iterator);
}

auto TextView::GetText() const -> Text
{
	return { *this };
}

auto TextView::GetString() const -> StringView
{
	return { _characters, _length };
}
