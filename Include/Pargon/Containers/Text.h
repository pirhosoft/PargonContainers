#pragma once

#include <string>

namespace Pargon
{
	class BufferView;
	class String;
	class StringView;

	enum class Encoding
	{
		Unknown,
		Ascii,
		Utf8,
		Utf16Little,
		Utf16Big,
		Utf32Little,
		Utf32Big
	};

	auto GuessEncoding(BufferView buffer) -> Encoding;

	class TextIterator
	{
	public:
		using difference_type = intptr_t;
		using value_type = char32_t;
		using pointer = char32_t*;
		using reference = char32_t&;
		using iterator_category = std::bidirectional_iterator_tag;

		TextIterator() = default;
		TextIterator(const TextIterator& copy) = default;
		~TextIterator() = default;

		auto operator=(const TextIterator& copy) -> TextIterator& = default;

		auto operator*() -> char32_t;

		auto operator==(const TextIterator& other) -> bool;
		auto operator!=(const TextIterator& other) -> bool;
		auto operator<=(const TextIterator& other) -> bool;
		auto operator<(const TextIterator& other) -> bool;
		auto operator>=(const TextIterator& other) -> bool;
		auto operator>(const TextIterator& other) -> bool;

		auto operator++() -> TextIterator&;
		auto operator++(int) -> TextIterator;
		auto operator--() -> TextIterator&;
		auto operator--(int) -> TextIterator;
		auto operator+(int increment) -> TextIterator;
		auto operator-(int decrement) -> TextIterator;
		auto operator-(TextIterator& other) -> int;

		auto operator+=(int increment) -> TextIterator&;
		auto operator-=(int decrement) -> TextIterator&;

		auto Data() -> const char*;

	private:
		friend class Text;
		friend class TextView;

		TextIterator(const char* location);

		const char* _location = nullptr;
	};

	class Text
	{
	public:
		using Iterator = TextIterator;
		static constexpr int InvalidIndex = -1;

		Text() = default;
		Text(const std::string& string);
		Text(const std::u16string& string);
		Text(const std::u32string& string);
		Text(const std::wstring& string);
		Text(TextView view);
		explicit Text(StringView view);
		Text(BufferView view, Encoding encoding);

		auto begin() const -> Iterator;
		auto end() const -> Iterator;

		auto IsEmpty() const -> bool;
		auto Length() const -> int;
		auto Character(int index) const -> char32_t;

		auto GetHash() const -> std::size_t;

		auto AsAscii() -> std::string;
		auto AsUtf16() -> std::u16string;
		auto AsUtf32() -> std::u32string;
		auto AsWide() -> std::wstring;

		void Append(char c);
		void Append(wchar_t c);
		void Append(char16_t c);
		void Append(char32_t c);
		void Append(StringView string);
		void Append(TextView text);

	private:
		std::string _string;

		void FromUtf8(const char* begin, const char* end);
		void FromUtf16(const char16_t* begin, const char16_t* end);
		void FromUtf32(const char32_t* begin, const char32_t* end);
	};

	class TextView
	{
	public:
		using Iterator = TextIterator;

		TextView();
		TextView(const Text& text);
		TextView(TextIterator begin, TextIterator end);

		auto begin() const -> Iterator;
		auto end() const -> Iterator;

		auto IsEmpty() const -> bool;
		auto Length() const -> int;
		auto Character(int index) const -> char32_t;

		auto GetText() const -> Text;
		auto GetString() const -> StringView;

	private:
		const char* _characters;
		int _length;
	};
}

inline
Pargon::TextIterator::TextIterator(const char* location) :
	_location(location)
{
}

inline
auto Pargon::TextIterator::operator==(const TextIterator& other) -> bool
{
	return _location == other._location;
}

inline
auto Pargon::TextIterator::operator!=(const TextIterator& other) -> bool
{
	return _location != other._location;
}

inline
auto Pargon::TextIterator::operator<=(const TextIterator& other) -> bool
{
	return _location <= other._location;
}

inline
auto Pargon::TextIterator::operator<(const TextIterator& other) -> bool
{
	return _location < other._location;
}

inline
auto Pargon::TextIterator::operator>=(const TextIterator& other) -> bool
{
	return _location >= other._location;
}

inline
auto Pargon::TextIterator::operator>(const TextIterator& other) -> bool
{
	return _location > other._location;
}

inline
auto Pargon::TextIterator::operator++(int) -> TextIterator
{
	auto temp = *this;
	operator++();
	return temp;
}

inline
auto Pargon::TextIterator::operator--(int) -> TextIterator
{
	auto temp = *this;
	operator--();
	return temp;
}

inline
auto Pargon::TextIterator::Data() -> const char*
{
	return _location;
}

inline
auto Pargon::Text::begin() const -> Iterator
{
	return _string.data();
}

inline
auto Pargon::Text::end() const -> Iterator
{
	return _string.data() + _string.size();
}

inline
auto Pargon::Text::IsEmpty() const -> bool
{
	return _string.size() == 0;
}

inline
Pargon::TextView::TextView() :
	_characters(nullptr),
	_length(0)
{
}

inline
Pargon::TextView::TextView(const Text& text) :
	_characters(text.begin().Data()),
	_length(text.Length())
{
}

inline
Pargon::TextView::TextView(TextIterator begin, TextIterator end) :
	_characters(begin.Data()),
	_length(static_cast<int>(end.Data() - begin.Data()))
{
}

inline
auto Pargon::TextView::begin() const -> Iterator
{
	return _characters;
}

inline
auto Pargon::TextView::end() const -> Iterator
{
	return _characters + _length;
}

inline
auto Pargon::TextView::IsEmpty() const -> bool
{
	return _length == 0;
}
