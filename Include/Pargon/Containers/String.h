#pragma once

#include <string>

namespace Pargon
{
	class BufferReference;
	class BufferView;
	template<typename T> class List;
	template<typename T> class SequenceView;
	class StringReference;
	class StringView;

	class String
	{
	public:
		using Iterator = const char*;
		static constexpr int InvalidIndex = -1;

		String() = default;
		String(std::string&& string);
		String(StringView view);
		String(const char* characters, int length);

		auto operator=(std::string&& string) -> String&;
		auto operator=(StringView view) -> String&;

		auto begin() const -> Iterator;
		auto end() const -> Iterator;

		auto IsEmpty() const -> bool;
		auto Length() const -> int;
		auto Character(int index) const -> char;
		void SetCharacter(int index, char c);

		auto GetHash() const -> std::size_t;

		auto GetView() const -> StringView;
		auto GetView(int start) const -> StringView;
		auto GetView(int start, int count) const -> StringView;
		auto GetReference() -> StringReference;
		auto GetReference(int start) -> StringReference;
		auto GetReference(int start, int count) -> StringReference;

		void ToUpperCase();
		void ToLowerCase();
		void Reverse();
		void Escape();
		void Unescape();

		void Append(StringView string);
		void Prepend(StringView string);
		auto Insert(StringView string, int index) -> bool;
		
		auto Remove(int index, int count) -> bool;
		auto RemoveAll(StringView string, bool ignoreCase = false) -> int;
		auto RemoveFirst(StringView string, bool ignoreCase = false) -> bool;
		auto RemoveLast(StringView string, bool ignoreCase = false) -> bool;
		
		auto Replace(int index, int count, StringView to) -> bool;
		auto ReplaceAll(StringView from, StringView to, bool ignoreCase = false) -> int;
		auto ReplaceFirst(StringView from, StringView to, bool ignoreCase = false) -> bool;
		auto ReplaceLast(StringView from, StringView to, bool ignoreCase = false) -> bool;
		
		void Trim(StringView string, bool ignoreCase = false);
		void TrimWhitespace();
		void TrimAny(StringView characters, bool ignoreCase = false);
		void TrimFromStart(StringView string, bool ignoreCase = false);
		void TrimWhitespaceFromStart();
		void TrimAnyFromStart(StringView characters, bool ignoreCase = false);
		void TrimFromEnd(StringView string, bool ignoreCase = false);
		void TrimWhitespaceFromEnd();
		void TrimAnyFromEnd(StringView characters, bool ignoreCase = false);

	private:
		std::string _string;
	};

	class StringView
	{
	public:
		using Iterator = const char*;

		StringView() = default;
		StringView(char character);
		StringView(const String& string);
		StringView(StringReference string);
		StringView(BufferView buffer);
		StringView(BufferReference buffer);
		template<int N> constexpr StringView(const char (&characters)[N]);
		constexpr StringView(const char* characters, int length);

		auto begin() const -> Iterator;
		auto end() const -> Iterator;

		auto IsEmpty() const -> bool;
		auto Length() const -> int;
		auto Character(int index) const -> char;

		auto GetSubview(int index) const -> StringView;
		auto GetSubview(int index, int count) const -> StringView;
		auto GetString() const -> String;

	private:
		const char* _characters = nullptr;
		int _length = 0;
		char _character = '\0';
	};

	class StringReference
	{
	public:
		using Iterator = char*;

		StringReference() = default;
		StringReference(String& string);
		StringReference(BufferReference buffer);
		StringReference(char* characters, int length);

		auto begin() const -> Iterator;
		auto end() const -> Iterator;

		auto IsEmpty() const -> bool;
		auto Length() const -> int;
		auto Character(int index) const -> char;
		void SetCharacter(int index, char c);

		auto GetView() const -> StringView;
		auto GetView(int start) const -> StringView;
		auto GetView(int start, int count) const -> StringView;
		auto GetSubreference(int start) const -> StringReference;
		auto GetSubreference(int start, int count) const -> StringReference;

		void ToUpperCase();
		void ToLowerCase();
		void Reverse();

	private:
		char* _characters = nullptr;
		int _length = 0;
	};

	auto Compare(StringView left, StringView right, bool ignoreCase = false) -> int;
	auto Equals(StringView left, StringView right, bool ignoreCase = false) -> bool;

	auto IndexOf(StringView view, StringView string, bool ignoreCase = false) -> int;
	auto IndexOfAny(StringView view, StringView characters, bool ignoreCase = false) -> int;
	auto IndexOfOther(StringView view, StringView string, bool ignoreCase = false) -> int;
	auto IndexOfAnyOther(StringView view, StringView characters, bool ignoreCase = false) -> int;

	auto NextIndexOf(StringView view, int start, StringView string, bool ignoreCase = false) -> int;
	auto NextIndexOfAny(StringView view, int start, StringView characters, bool ignoreCase = false) -> int;
	auto NextIndexOfOther(StringView view, int start, StringView string, bool ignoreCase = false) -> int;
	auto NextIndexOfAnyOther(StringView view, int start, StringView characters, bool ignoreCase = false) -> int;

	auto LastIndexOf(StringView view, StringView string, bool ignoreCase = false) -> int;
	auto LastIndexOfAny(StringView view, StringView characters, bool ignoreCase = false) -> int;
	auto LastIndexOfOther(StringView view, StringView string, bool ignoreCase = false) -> int;
	auto LastIndexOfAnyOther(StringView view, StringView characters, bool ignoreCase = false) -> int;

	auto PreviousIndexOf(StringView view, int start, StringView string, bool ignoreCase = false) -> int;
	auto PreviousIndexOfAny(StringView view, int start, StringView characters, bool ignoreCase = false) -> int;
	auto PreviousIndexOfOther(StringView view, int start, StringView string, bool ignoreCase = false) -> int;
	auto PreviousIndexOfAnyOther(StringView view, int start, StringView characters, bool ignoreCase = false) -> int;

	auto StartsWith(StringView view, StringView string, bool ignoreCase = false) -> bool;
	auto EndsWith(StringView view, StringView string, bool ignoreCase = false) -> bool;
	auto Contains(StringView view, StringView string, bool ignoreCase = false) -> bool;
	auto ContainsAny(StringView view, StringView characters, bool ignoreCase = false) -> bool;
	auto ContainsAnyOther(StringView view, StringView characters, bool ignoreCase = false) -> bool;

	auto Split(StringView view, StringView string, bool ignoreCase = false) -> List<StringView>;
	auto SplitOnWhitespace(StringView view) -> List<StringView>;
	auto SplitOnAny(StringView view, StringView characters, bool ignoreCase = false) -> List<StringView>;
	auto Join(SequenceView<String> view, StringView separator) -> String;
	auto Join(SequenceView<StringView> view, StringView separator) -> String;

	auto AsUpperCase(StringView view) -> String;
	auto AsLowerCase(StringView view) -> String;
	auto Reversed(StringView view) -> String;
	auto Escaped(StringView view) -> String;
	auto Unescaped(StringView view) -> String;

	auto Append(StringView view, StringView string) -> String;
	auto Prepend(StringView view, StringView string) -> String;
	auto Insert(StringView view, StringView string, int index) -> String;

	auto Remove(StringView view, int index, int count) -> String;
	auto RemoveAll(StringView view, StringView string, bool ignoreCase = false) -> String;
	auto RemoveFirst(StringView view, StringView string, bool ignoreCase = false) -> String;
	auto RemoveLast(StringView view, StringView string, bool ignoreCase = false) -> String;

	auto Replace(StringView view, int index, int count, StringView to) -> String;
	auto ReplaceAll(StringView view, StringView from, StringView to, bool ignoreCase = false) -> String;
	auto ReplaceFirst(StringView view, StringView from, StringView to, bool ignoreCase = false) -> String;
	auto ReplaceLast(StringView view, StringView from, StringView to, bool ignoreCase = false) -> String;

	auto Trim(StringView view, StringView string, bool ignoreCase = false) -> String;
	auto TrimWhitespace(StringView view) -> String;
	auto TrimAny(StringView view, StringView characters, bool ignoreCase = false) -> String;
	auto TrimFromStart(StringView view, StringView string, bool ignoreCase = false) -> String;
	auto TrimWhitespaceFromStart(StringView view) -> String;
	auto TrimAnyFromStart(StringView view, StringView characters, bool ignoreCase = false) -> String;
	auto TrimFromEnd(StringView view, StringView string, bool ignoreCase = false) -> String;
	auto TrimWhitespaceFromEnd(StringView view) -> String;
	auto TrimAnyFromEnd(StringView view, StringView characters, bool ignoreCase = false) -> String;

	auto operator==(StringView left, StringView right) -> bool;
	auto operator!=(StringView left, StringView right) -> bool;
	auto operator<(StringView left, StringView right) -> bool;
	auto operator>(StringView left, StringView right) -> bool;
	auto operator<=(StringView left, StringView right) -> bool;
	auto operator>=(StringView left, StringView right) -> bool;

	auto operator""_s(const char* characters, size_t length) -> String;
	constexpr auto operator""_sv(const char* characters, size_t length) -> StringView;
}

inline
Pargon::String::String(std::string&& string) :
	_string(std::move(string))
{
}

inline
Pargon::String::String(StringView view) :
	_string(view.begin(), view.end())
{
}

inline
Pargon::String::String(const char* characters, int length) :
	_string(characters, characters + length)
{
}

inline
auto Pargon::String::operator=(std::string&& string) -> String&
{
	_string = std::move(string);
	return *this;
}

inline
auto Pargon::String::operator=(StringView view) -> String&
{
	_string.assign(view.begin(), view.end());
	return *this;
}

inline
auto Pargon::String::begin() const -> Iterator
{
	return _string.data();
}

inline
auto Pargon::String::end() const -> Iterator
{
	return _string.data() + _string.length();
}

inline
auto Pargon::String::IsEmpty() const -> bool
{
	return _string.size() == 0;
}

inline
auto Pargon::String::Length() const -> int
{
	return static_cast<int>(_string.size());
}

inline
Pargon::StringView::StringView(char character) :
	_characters(&_character),
	_length(1),
	_character(character)
{
}

inline
Pargon::StringView::StringView(const String& string) :
	_characters(string.begin()),
	_length(string.Length())
{
}

inline
Pargon::StringView::StringView(StringReference string) :
	_characters(string.begin()),
	_length(string.Length())
{
}

template<int N> constexpr
Pargon::StringView::StringView(const char (&characters)[N]) :
	_characters(characters),
	_length(N - 1)
{
}

constexpr
Pargon::StringView::StringView(const char* characters, int length) :
	_characters(characters),
	_length(length)
{
}

inline
auto Pargon::StringView::begin() const -> Iterator
{
	return _characters;
}

inline
auto Pargon::StringView::end() const -> Iterator
{
	return _characters + _length;
}

inline
auto Pargon::StringView::IsEmpty() const -> bool
{
	return _length == 0;
}

inline
auto Pargon::StringView::Length() const -> int
{
	return _length;
}

inline
auto Pargon::StringView::GetSubview(int start) const -> StringView
{
	if (start < 0 || start > _length)
		return {};

	return { _characters + start, _length - start };
}

inline
auto Pargon::StringView::GetSubview(int start, int count) const -> StringView
{
	if (start < 0 || start + count > _length)
		return {};

	return { _characters + start, count };
}

inline
auto Pargon::StringView::GetString() const -> String
{
	return String(*this);
}

inline
Pargon::StringReference::StringReference(String& string)
{
	*this = string.GetReference();
}

inline
Pargon::StringReference::StringReference(char* characters, int length) :
	_characters(characters),
	_length(length)
{
}

inline
auto Pargon::StringReference::begin() const -> Iterator
{
	return _characters;
}

inline
auto Pargon::StringReference::end() const -> Iterator
{
	return _characters + _length;
}

inline
auto Pargon::StringReference::IsEmpty() const -> bool
{
	return _length == 0;
}

inline
auto Pargon::StringReference::Length() const -> int
{
	return _length;
}

inline
auto Pargon::operator==(StringView left, StringView right) -> bool
{
	return Equals(left, right);
}

inline
auto Pargon::operator!=(StringView left, StringView right) -> bool
{
	return !Equals(left, right);
}

inline
auto Pargon::operator<(StringView left, StringView right) -> bool
{
	return Compare(left, right) < 0;
}

inline
auto Pargon::operator>(StringView left, StringView right) -> bool
{
	return Compare(left, right) > 0;
}

inline
auto Pargon::operator<=(StringView left, StringView right) -> bool
{
	return Compare(left, right) <= 0;
}

inline
auto Pargon::operator>=(StringView left, StringView right) -> bool
{
	return Compare(left, right) >= 0;
}

inline
auto Pargon::operator""_s(const char* characters, size_t length) -> String
{
	return { characters, static_cast<int>(length) };
}

constexpr
auto Pargon::operator""_sv(const char* characters, size_t length) -> StringView
{
	return { characters, static_cast<int>(length) };
}
