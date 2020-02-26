#include "Pargon/Containers/Buffer.h"
#include "Pargon/Containers/List.h"
#include "Pargon/Containers/Sequence.h"
#include "Pargon/Containers/String.h"

#include <cassert>
#include <cctype>
#include <sstream>

using namespace Pargon;

auto String::Character(int index) const -> char
{
	assert (index >= 0 && index < static_cast<int>(_string.length()));
	return _string[index];
}

void String::SetCharacter(int index, char c)
{
	assert (index >= 0 && index < static_cast<int>(_string.length()));
	_string[index] = c;
}

auto String::GetHash() const -> std::size_t
{
	return std::hash<std::string>()(_string);
}

auto String::GetView() const -> StringView
{
	return GetView(0, static_cast<int>(_string.size()));
}

auto String::GetView(int start) const -> StringView
{
	return GetView(start, static_cast<int>(_string.size()) - start);
}

auto String::GetView(int start, int count) const -> StringView
{
	if (start < 0 || start + count > static_cast<int>(_string.size()))
		return {};

	return { _string.data() + start, count };
}

auto String::GetReference() -> StringReference
{
	return GetReference(0, static_cast<int>(_string.size()));
}

auto String::GetReference(int start) -> StringReference
{
	return GetReference(start, static_cast<int>(_string.size()) - start);
}

auto String::GetReference(int start, int count) -> StringReference
{
	if (start < 0 || start + count > static_cast<int>(_string.size()))
		return {};

	return { _string.data() + start, count };
}

void String::ToUpperCase()
{
	GetReference().ToUpperCase();
}

void String::ToLowerCase()
{
	GetReference().ToLowerCase();
}

void String::Reverse()
{
	GetReference().Reverse();
}

void String::Escape()
{
	*this = Pargon::Escaped(*this);
}

void String::Unescape()
{
	*this = Pargon::Unescaped(*this);
}

void String::Append(StringView string)
{
	Insert(string, Length());
}

void String::Prepend(StringView string)
{
	Insert(string, 0);
}

auto String::Insert(StringView string, int index) -> bool
{
	if (index > Length())
		return false;

	_string.insert(_string.begin() + index, string.begin(), string.end());
	return true;
}

auto String::Remove(int index, int count) -> bool
{
	if (index + count > Length())
		return false;

	_string.erase(index, count);
	return true;
}

auto String::RemoveAll(StringView string, bool ignoreCase) -> int
{
	if (string.Length() == 0)
		return 0;

	auto removed = 0;
	auto count = string.Length();
	auto index = IndexOf(*this, string, ignoreCase);

	while (index != InvalidIndex)
	{
		removed++;
		Remove(index, count);
		index = IndexOf(*this, string, ignoreCase);
	}

	return removed;
}

auto String::RemoveFirst(StringView string, bool ignoreCase) -> bool
{
	auto index = IndexOf(*this, string, ignoreCase);
	if (index == InvalidIndex)
		return false;

	return Remove(index, string.Length());
}

auto String::RemoveLast(StringView string, bool ignoreCase) -> bool
{
	auto index = LastIndexOf(*this, string, ignoreCase);
	if (index == InvalidIndex)
		return false;

	return Remove(index, string.Length());
}

auto String::Replace(int index, int count, StringView to) -> bool
{
	if (index + count > Length())
		return false;

	auto start = _string.begin() + index;
	auto end = start + count;

	_string.replace(start, end, to.begin(), to.end());
	return true;
}

auto String::ReplaceAll(StringView from, StringView to, bool ignoreCase) -> int
{
	if (from.Length() == 0)
		return 0;

	auto replaced = 0;
	auto start = IndexOf(*this, from, ignoreCase);

	while (start != InvalidIndex)
	{
		replaced++;
		Replace(start, from.Length(), to);
		start = NextIndexOf(*this, start + to.Length(), from, ignoreCase);
	}

	return replaced;
}

auto String::ReplaceFirst(StringView from, StringView to, bool ignoreCase) -> bool
{
	auto index = IndexOf(*this, from, ignoreCase);
	if (index == InvalidIndex)
		return false;

	return Replace(index, from.Length(), to);
}

auto String::ReplaceLast(StringView from, StringView to, bool ignoreCase) -> bool
{
	auto index = LastIndexOf(*this, from, ignoreCase);
	if (index == InvalidIndex)
		return false;

	return Replace(index, from.Length(), to);
}

void String::Trim(StringView string, bool ignoreCase)
{
	auto first = IndexOfOther(*this, string, ignoreCase);
	if (first == InvalidIndex)
	{
		_string = "";
		return;
	}

	auto last = LastIndexOfOther(*this, string, ignoreCase);
	if (last == InvalidIndex)
	{
		_string = "";
		return;
	}

	_string = std::string(_string.data() + first, last - first + 1);
}

void String::TrimWhitespace()
{
	TrimAny(" \t\r\n", false);
}

void String::TrimAny(StringView characters, bool ignoreCase)
{
	auto first = IndexOfAnyOther(*this, characters, ignoreCase);
	if (first == InvalidIndex)
	{
		_string = "";
		return;
	}

	auto last = LastIndexOfAnyOther(*this, characters, ignoreCase);
	if (last == InvalidIndex)
	{
		_string = "";
		return;
	}

	_string = std::string(_string.data() + first, last - first + 1);
}

void String::TrimFromStart(StringView string, bool ignoreCase)
{
	auto first = IndexOfOther(*this, string, ignoreCase);
	if (first == InvalidIndex)
	{
		_string = "";
		return;
	}

	_string = std::string(_string.data() + first, Length() - first);
}

void String::TrimWhitespaceFromStart()
{
	return TrimAnyFromStart(" \t\r\n", false);
}

void String::TrimAnyFromStart(StringView characters, bool ignoreCase)
{
	auto first = IndexOfAnyOther(*this, characters, ignoreCase);
	if (first == InvalidIndex)
	{
		_string = "";
		return;
	}

	_string = std::string(_string.data() + first, Length() - first);
}

void String::TrimFromEnd(StringView string, bool ignoreCase)
{
	auto last = LastIndexOfOther(*this, string, ignoreCase);
	if (last == InvalidIndex)
	{
		_string = "";
		return;
	}

	_string = std::string(_string.data(), last + 1);
}

void String::TrimWhitespaceFromEnd()
{
	TrimAnyFromEnd(" \t\r\n", false);
}

void String::TrimAnyFromEnd(StringView characters, bool ignoreCase)
{
	auto last = LastIndexOfAnyOther(*this, characters, ignoreCase);
	if (last == InvalidIndex)
	{
		_string = "";
		return;
	}

	_string = std::string(_string.data(), last + 1);
}

StringView::StringView(BufferView buffer) :
	_characters(reinterpret_cast<const char*>(buffer.begin())),
	_length(buffer.Size())
{
}

StringView::StringView(BufferReference buffer) :
	_characters(reinterpret_cast<const char*>(buffer.begin())),
	_length(buffer.Size())
{
}

auto StringView::Character(int index) const -> char
{
	assert(index >= 0 && index < _length);
	return _characters[index];
}

StringReference::StringReference(BufferReference buffer) :
	_characters(reinterpret_cast<char*>(buffer.begin())),
	_length(buffer.Size())
{
}

auto StringReference::Character(int index) const -> char
{
	assert (index >= 0 && index < _length);
	return _characters[index];
}

void StringReference::SetCharacter(int index, char c)
{
	assert (index >= 0 && index < _length);
	_characters[index] = c;
}

auto StringReference::GetView() const -> StringView
{
	return GetView(0, _length);
}

auto StringReference::GetView(int start) const -> StringView
{
	return GetView(start, _length - start);
}

auto StringReference::GetView(int start, int count) const -> StringView
{
	if (start < 0 || start + count > _length)
		return {};

	return { _characters + start, count };
}

auto StringReference::GetSubreference(int start) const -> StringReference
{
	return GetSubreference(start, _length - start);
}

auto StringReference::GetSubreference(int start, int count) const -> StringReference
{
	if (start < 0 || start + count > _length)
		return {};

	return { _characters + start, count };
}

void StringReference::ToUpperCase()
{
	for (auto i = 0; i < _length; i++)
		_characters[i] = std::toupper(_characters[i]);
}

void StringReference::ToLowerCase()
{
	for (auto i = 0; i < _length; i++)
		_characters[i] = std::tolower(_characters[i]);
}

void StringReference::Reverse()
{
	std::reverse(_characters, _characters + _length);
}

auto Pargon::Compare(StringView left, StringView right, bool ignoreCase) -> int
{
	auto minCount = left.Length() < right.Length() ? left.Length() : right.Length();

	for (auto i = 0; i < minCount; i++)
	{
		auto lc = ignoreCase ? std::tolower(left.Character(i)) : left.Character(i);
		auto rc = ignoreCase ? std::tolower(right.Character(i)) : right.Character(i);

		if (lc < rc)
			return -1;
		else if (lc > rc)
			return 1;
	}

	return left.Length() == right.Length() ? 0 : (left.Length() == minCount ? -1 : 1);
}

auto Pargon::Equals(StringView left, StringView right, bool ignoreCase) -> bool
{
	if (left.Length() != right.Length())
		return false;

	for (auto i = 0; i < left.Length(); i++)
	{
		auto lc = ignoreCase ? std::tolower(left.Character(i)) : left.Character(i);
		auto rc = ignoreCase ? std::tolower(right.Character(i)) : right.Character(i);

		if (lc != rc)
			return false;
	}

	return true;
}

auto Pargon::IndexOf(StringView view, StringView string, bool ignoreCase) -> int
{
	if (view.Length() < string.Length())
		return String::InvalidIndex;

	auto end = view.Length() - string.Length();

	for (auto i = 0; i <= end; i++)
	{
		if (Equals(view.GetSubview(i, string.Length()), string, ignoreCase))
			return i;
	}

	return String::InvalidIndex;
}

auto Pargon::IndexOfAny(StringView view, StringView characters, bool ignoreCase) -> int
{
	for (auto i = 0; i < view.Length(); i++)
	{
		if (IndexOf(characters, view.GetSubview(i, 1), ignoreCase) != String::InvalidIndex)
			return i;
	}

	return String::InvalidIndex;
}

auto Pargon::IndexOfOther(StringView view, StringView string, bool ignoreCase) -> int
{
	if (view.Length() == 0)
		return String::InvalidIndex;

	if (string.Length() == 0 || view.Length() < string.Length())
		return 0;

	auto end = view.Length() - string.Length();

	auto i = 0;
	for (; i <= end; i += string.Length())
	{
		if (!Equals(view.GetSubview(i, string.Length()), string, ignoreCase))
			return i;
	}

	return i == view.Length() ? String::InvalidIndex : i;
}

auto Pargon::IndexOfAnyOther(StringView view, StringView characters, bool ignoreCase) -> int
{
	for (auto i = 0; i < view.Length(); i++)
	{
		if (IndexOf(characters, view.GetSubview(i, 1), ignoreCase) == String::InvalidIndex)
			return i;
	}

	return String::InvalidIndex;
}

auto Pargon::NextIndexOf(StringView view, int start, StringView string, bool ignoreCase) -> int
{
	if (start > view.Length())
		return String::InvalidIndex;

	auto index = IndexOf(view.GetSubview(start), string, ignoreCase);
	return index == String::InvalidIndex ? index : start + index;
}

auto Pargon::NextIndexOfAny(StringView view, int start, StringView characters, bool ignoreCase) -> int
{
	if (start > view.Length())
		return String::InvalidIndex;

	auto index = Pargon::IndexOfAny(view.GetSubview(start), characters, ignoreCase);
	return index == String::InvalidIndex ? index : start + index;
}

auto Pargon::NextIndexOfOther(StringView view, int start, StringView string, bool ignoreCase) -> int
{
	if (start > view.Length())
		return String::InvalidIndex;

	auto index = Pargon::IndexOfOther(view.GetSubview(start), string, ignoreCase);
	return index == String::InvalidIndex ? index : start + index;
}

auto Pargon::NextIndexOfAnyOther(StringView view, int start, StringView characters, bool ignoreCase) -> int
{
	if (start > view.Length())
		return String::InvalidIndex;

	auto index = Pargon::IndexOfAnyOther(view.GetSubview(start), characters, ignoreCase);
	return index == String::InvalidIndex ? index : start + index;
}

auto Pargon::LastIndexOf(StringView view, StringView string, bool ignoreCase) -> int
{
	if (view.Length() < string.Length())
		return String::InvalidIndex;

	auto end = view.Length() - string.Length() + 1;

	for (auto i = end; i > 0; i--)
	{
		if (Equals(view.GetSubview(i - 1, string.Length()), string, ignoreCase))
			return i - 1;
	}

	return String::InvalidIndex;
}

auto Pargon::LastIndexOfAny(StringView view, StringView characters, bool ignoreCase) -> int
{
	if (view.Length() == 0)
		return String::InvalidIndex;

	auto end = view.Length();

	for (auto i = end; i > 0; i--)
	{
		if (IndexOf(characters, view.GetSubview(i - 1, 1), ignoreCase) != String::InvalidIndex)
			return i - 1;
	}

	return String::InvalidIndex;
}

auto Pargon::LastIndexOfOther(StringView view, StringView string, bool ignoreCase) -> int
{
	if (view.Length() == 0)
		return String::InvalidIndex;

	if (string.Length() == 0 || view.Length() < string.Length())
		return view.Length() - 1;

	auto end = view.Length() - string.Length() + 1;

	auto i = end;
	for (; i > 0; i -= string.Length())
	{
		if (!Equals(view.GetSubview(i - 1, string.Length()), string, ignoreCase))
			return i + string.Length() - 2;
	}

	return i + string.Length() - 2;
}

auto Pargon::LastIndexOfAnyOther(StringView view, StringView characters, bool ignoreCase) -> int
{
	if (view.Length() == 0)
		return String::InvalidIndex;

	auto end = view.Length() + 1;

	for (auto i = end; i > 0; i--)
	{
		if (IndexOf(characters, view.GetSubview(i - 1, 1), ignoreCase) == String::InvalidIndex)
			return i - 1;
	}

	return String::InvalidIndex;
}

auto Pargon::PreviousIndexOf(StringView view, int start, StringView string, bool ignoreCase) -> int
{
	if (start > view.Length())
		return String::InvalidIndex;

	return Pargon::LastIndexOf(view.GetSubview(0, start), string, ignoreCase);
}

auto Pargon::PreviousIndexOfAny(StringView view, int start, StringView characters, bool ignoreCase) -> int
{
	if (start > view.Length())
		return String::InvalidIndex;

	return Pargon::LastIndexOfAny(view.GetSubview(0, start), characters, ignoreCase);
}

auto Pargon::PreviousIndexOfOther(StringView view, int start, StringView string, bool ignoreCase) -> int
{
	if (start > view.Length())
		return String::InvalidIndex;

	return Pargon::LastIndexOfOther(view.GetSubview(0, start), string, ignoreCase);
}

auto Pargon::PreviousIndexOfAnyOther(StringView view, int start, StringView characters, bool ignoreCase) -> int
{
	if (start > view.Length())
		return String::InvalidIndex;

	return Pargon::LastIndexOfAnyOther(view.GetSubview(0, start), characters, ignoreCase);
}

auto Pargon::StartsWith(StringView view, StringView string, bool ignoreCase) -> bool
{
	return view.Length() >= string.Length() && Equals(view.GetSubview(0, string.Length()), string, ignoreCase);
}

auto Pargon::EndsWith(StringView view, StringView string, bool ignoreCase) -> bool
{
	return view.Length() >= string.Length() && Equals(view.GetSubview(view.Length() - string.Length()), string, ignoreCase);
}

auto Pargon::Contains(StringView view, StringView string, bool ignoreCase) -> bool
{
	return IndexOf(view, string, ignoreCase) != String::InvalidIndex;
}

auto Pargon::ContainsAny(StringView view, StringView characters, bool ignoreCase) -> bool
{
	return IndexOfAny(view, characters, ignoreCase) != String::InvalidIndex;
}

auto Pargon::ContainsAnyOther(StringView view, StringView characters, bool ignoreCase) -> bool
{
	return Pargon::IndexOfAnyOther(view, characters, ignoreCase) != String::InvalidIndex;
}

auto Pargon::Split(StringView view, StringView string, bool ignoreCase) -> List<StringView>
{
	auto split = List<StringView>();
	auto index = 0;
	auto nextIndex = IndexOf(view, string, ignoreCase);

	while (nextIndex != String::InvalidIndex)
	{
		split.Increment(view.GetSubview(index, nextIndex - index));
		index = nextIndex + string.Length();
		nextIndex = NextIndexOf(view, index, string, ignoreCase);
	}

	split.Increment(view.GetSubview(index));
	return split;
}

auto Pargon::SplitOnWhitespace(StringView view) -> List<StringView>
{
	return SplitOnAny(view, " \t\r\n", false);
}

auto Pargon::SplitOnAny(StringView view, StringView characters, bool ignoreCase) -> List<StringView>
{
	auto split = List<StringView>();
	auto index = 0;
	auto nextIndex = IndexOfAny(view, characters, ignoreCase);

	while (nextIndex != String::InvalidIndex)
	{
		split.Increment(view.GetSubview(index, nextIndex - index));
		index = nextIndex + 1;
		nextIndex = NextIndexOfAny(view, index, characters, ignoreCase);
	}

	split.Increment(view.GetSubview(index));
	return split;
}

auto Pargon::Join(SequenceView<String> view, StringView separator) -> String
{
	String string;

	for (auto i = 0; i < view.Count(); i++)
	{
		if (i > 0)
			string.Append(separator);

		string.Append(view.Item(i));
	}

	return string;
}

auto Pargon::Join(SequenceView<StringView> view, StringView separator) -> String
{
	String string;

	for (auto i = 0; i < view.Count(); i++)
	{
		if (i > 0)
			string.Append(separator);

		string.Append(view.Item(i));
	}

	return string;
}

auto Pargon::AsUpperCase(StringView view) -> String
{
	auto copy = view.GetString();
	copy.ToUpperCase();
	return copy;
}

auto Pargon::AsLowerCase(StringView view) -> String
{
	auto copy = view.GetString();
	copy.ToLowerCase();
	return copy;
}

auto Pargon::Reversed(StringView view) -> String
{
	auto copy = view.GetString();
	copy.Reverse();
	return copy;
}

auto Pargon::Escaped(StringView view) -> String
{
	std::ostringstream stream;

	for (auto c : view)
	{
		switch (c)
		{
			case '\"': stream << "\\\""; break;
			case '\\': stream << "\\\\"; break;
			case '\b': stream << "\\b"; break;
			case '\f': stream << "\\f"; break;
			case '\n': stream << "\\n"; break;
			case '\r': stream << "\\r"; break;
			case '\t': stream << "\\t"; break;
			default: stream << c; break;
		}
	}

	auto string = stream.str();
	return { string.data(), static_cast<int>(string.length()) };
}

auto Pargon::Unescaped(StringView view) -> String
{
	std::ostringstream stream;
	auto escaped = false;

	for (auto c : view)
	{
		if (escaped)
		{
			switch (c)
			{
				case '\"': stream << "\""; break;
				case '\\': stream << "\\"; break;
				case 'b': stream << "\b"; break;
				case 'f': stream << "\f"; break;
				case 'n': stream << "\n"; break;
				case 'r': stream << "\r"; break;
				case 't': stream << "\t"; break;
				default: break;
			}

			escaped = false;
		}
		else
		{
			if (c == '\\')
				escaped = true;
			else
				stream << c;
		}
	}

	auto string = stream.str();
	return { string.data(), static_cast<int>(string.length()) };
}

auto Pargon::Append(StringView view, StringView string) -> String
{
	auto copy = view.GetString();
	copy.Append(string);
	return copy;
}

auto Pargon::Prepend(StringView view, StringView string) -> String
{
	auto copy = view.GetString();
	copy.Prepend(string);
	return copy;
}

auto Pargon::Insert(StringView view, StringView string, int index) -> String
{
	auto copy = view.GetString();
	copy.Insert(string, index);
	return copy;
}

auto Pargon::Remove(StringView view, int index, int count) -> String
{
	auto copy = view.GetString();
	copy.Remove(index, count);
	return copy;
}

auto Pargon::RemoveAll(StringView view, StringView string, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.RemoveAll(string, ignoreCase);
	return copy;
}

auto Pargon::RemoveFirst(StringView view, StringView string, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.RemoveFirst(string, ignoreCase);
	return copy;
}

auto Pargon::RemoveLast(StringView view, StringView string, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.RemoveLast(string, ignoreCase);
	return copy;
}

auto Pargon::Replace(StringView view, int index, int count, StringView to) -> String
{
	auto copy = view.GetString();
	copy.Replace(index, count, to);
	return copy;
}

auto Pargon::ReplaceAll(StringView view, StringView from, StringView to, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.ReplaceAll(from, to, ignoreCase);
	return copy;
}

auto Pargon::ReplaceFirst(StringView view, StringView from, StringView to, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.ReplaceFirst(from, to, ignoreCase);
	return copy;
}

auto Pargon::ReplaceLast(StringView view, StringView from, StringView to, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.ReplaceLast(from, to, ignoreCase);
	return copy;
}

auto Pargon::Trim(StringView view, StringView string, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.Trim(string, ignoreCase);
	return copy;
}

auto Pargon::TrimWhitespace(StringView view) -> String
{
	auto copy = view.GetString();
	copy.TrimWhitespace();
	return copy;
}

auto Pargon::TrimAny(StringView view, StringView characters, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.TrimAny(characters, ignoreCase);
	return copy;
}

auto Pargon::TrimFromStart(StringView view, StringView string, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.TrimFromStart(string, ignoreCase);
	return copy;
}

auto Pargon::TrimWhitespaceFromStart(StringView view) -> String
{
	auto copy = view.GetString();
	copy.TrimWhitespaceFromStart();
	return copy;
}

auto Pargon::TrimAnyFromStart(StringView view, StringView characters, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.TrimAnyFromStart(characters, ignoreCase);
	return copy;
}

auto Pargon::TrimFromEnd(StringView view, StringView string, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.TrimFromEnd(string, ignoreCase);
	return copy;
}

auto Pargon::TrimWhitespaceFromEnd(StringView view) -> String
{
	auto copy = view.GetString();
	copy.TrimWhitespaceFromEnd();
	return copy;
}

auto Pargon::TrimAnyFromEnd(StringView view, StringView characters, bool ignoreCase) -> String
{
	auto copy = view.GetString();
	copy.TrimAnyFromEnd(characters, ignoreCase);
	return copy;
}
