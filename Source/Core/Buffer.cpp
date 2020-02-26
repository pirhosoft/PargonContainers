#include "Pargon/Containers/Buffer.h"
#include "Pargon/Containers/String.h"

#include <algorithm>
#include <cassert>
#include <utility>

using namespace Pargon;

Buffer::Buffer(std::unique_ptr<uint8_t[]>&& bytes, int size) :
	_data(std::move(bytes)),
	_used(size),
	_available(size)
{
}

Buffer::Buffer(BufferView view)
{
	SetSize(view.Size());
	std::copy(view.begin(), view.end(), _data.get());
}

auto Buffer::operator=(BufferView view) -> Buffer&
{
	SetSize(view.Size());
	std::copy(view.begin(), view.end(), _data.get());
	return *this;
}

auto Buffer::Byte(int index) const -> uint8_t
{
	assert(index >= 0 && index < _used);
	return _data[index];
}

void Buffer::SetByte(int index, uint8_t value)
{
	assert(index >= 0 && index < _used);
	_data[index] = value;
}

void Buffer::SetBytes(int index, BufferView bytes)
{
	assert(index >= 0 && index + bytes.Size() <= _used);
	std::copy(bytes.begin(), bytes.end(), _data.get() + index);
}

void Buffer::SetSize(int size)
{
	if (size > _available)
	{
		auto newSize = _used + _used / 2;
		if (size > newSize)
			newSize = size;

		auto buffer = std::make_unique<uint8_t[]>(newSize);
		std::copy(_data.get(), _data.get() + _used, buffer.get());
		_data = std::move(buffer);
		_available = newSize;
	}

	_used = size;
}

void Buffer::Append(uint8_t value, int count)
{
	auto start = _used;
	SetSize(_used + count);

	for (auto i = start; i < _used; i++)
		_data[i] = value;
}

void Buffer::Append(BufferView view, bool reverse)
{
	auto start = _used;
	SetSize(_used + view.Size());

	if (reverse)
		std::reverse_copy(view.begin(), view.end(), _data.get() + start);
	else
		std::copy(view.begin(), view.end(), _data.get() + start);
}

auto Buffer::Reserve(int size) -> BufferReference
{
	auto start = _used;
	SetSize(_used + size);
	return { _data.get() + start, size };
}

void Buffer::Erase(uint8_t value)
{
	std::fill(_data.get(), _data.get() + _used, value);
}

void Buffer::Clear()
{
	_used = 0;
	_available = 0;
	_data.reset();
}

auto Buffer::GetView() const -> BufferView
{
	return GetView(0, _used);
}

auto Buffer::GetView(int index) const -> BufferView
{
	return GetView(index, _used - index);
}

auto Buffer::GetView(int index, int count) const -> BufferView
{
	if (index < 0 || index + count > _used)
		return {};

	return { _data.get() + index, count };
}

auto Buffer::GetReference() -> BufferReference
{
	return GetReference(0, _used);
}

auto Buffer::GetReference(int index) -> BufferReference
{
	return GetReference(index, _used - index);
}

auto Buffer::GetReference(int index, int count) -> BufferReference
{
	if (index < 0 || index + count > _used)
		return {};

	return { _data.get() + index, count };
}

BufferView::BufferView(StringView string) :
	_data(reinterpret_cast<const uint8_t*>(string.begin())),
	_size(string.Length())
{
}

BufferView::BufferView(StringReference string) :
	_data(reinterpret_cast<uint8_t*>(string.begin())),
	_size(string.Length())
{
}

auto BufferView::Byte(int index) const -> uint8_t
{
	assert(index >= 0 && index < _size);
	return _data[index];
}

auto BufferView::GetSubview(int index) const -> BufferView
{
	return GetSubview(index, _size - index);
}

auto BufferView::GetSubview(int index, int count) const -> BufferView
{
	if (index < 0 || index + count > _size)
		return {};

	return { _data + index, count };
}

auto BufferView::GetBuffer() const -> Buffer
{
	return Buffer(*this);
}

auto Pargon::operator==(BufferView left, BufferView right) -> bool
{
	if (left.Size() != right.Size())
		return false;

	return std::equal(left.begin(), left.end(), right.begin());
}

auto Pargon::operator!=(BufferView left, BufferView right) -> bool
{
	return !operator==(left, right);
}

inline
Pargon::BufferReference::BufferReference(StringReference string) :
	_data(reinterpret_cast<uint8_t*>(string.begin())),
	_size(string.Length())
{
}

auto BufferReference::Byte(int index) const -> uint8_t
{
	assert(index >= 0 && index < _size);
	return _data[index];
}

void BufferReference::SetByte(int index, uint8_t value)
{
	assert(index >= 0 && index < _size);
	_data[index] = value;
}

void BufferReference::SetBytes(int index, BufferView bytes)
{
	assert(index >= 0 && index + bytes.Size() <= _size);
	std::copy(bytes.begin(), bytes.end(), _data + index);
}

void BufferReference::Erase(uint8_t value)
{
	std::fill(_data, _data + _size, value);
}

auto BufferReference::GetView() const -> BufferView
{
	return GetView(0, _size);
}

auto BufferReference::GetView(int index) const -> BufferView
{
	return GetView(index, _size - index);
}

auto BufferReference::GetView(int index, int count) const -> BufferView
{
	if (index < 0 || index + count > _size)
		return {};

	return { _data + index, count };
}

auto BufferReference::GetSubreference(int index) const -> BufferReference
{
	return GetSubreference(index, _size - index);
}

auto BufferReference::GetSubreference(int index, int count) const -> BufferReference
{
	if (index < 0 || index + count > _size)
		return {};

	return { _data + index, count };
}

auto BufferReference::GetBuffer() const -> Buffer
{
	return Buffer(*this);
}

namespace
{
	const char _base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	auto Base64Index(char c) -> uint8_t
	{
		const char* base64 = _base64;
		for (; (*base64 && (*base64 != c)); base64++);
		return static_cast<uint8_t>(base64 - _base64);
	}
}

auto Pargon::ToBase64(BufferView buffer) -> String
{
	String string;
	auto size = buffer.Size();

	uint8_t input[3];
	auto p = (size % 3) == 0 ? 0 : 3 - (size % 3);
	for (auto i = 0; i < size + p; i++)
	{
		auto j = i % 3;
		input[j] = i < size ? buffer.Byte(i) : 0;
		if (j == 2)
		{
			char output[5] =
			{
				_base64[(input[0] & 0xFC) >> 2],
				_base64[((input[0] & 0x03) << 4) + ((input[1] & 0xF0) >> 4)],
				_base64[((input[1] & 0x0F) << 2) + ((input[2] & 0xC0) >> 6)],
				_base64[input[2] & 0x3F],
				'\0'
			};

			string.Append(output);
		}
	}

	for (auto i = 1; i <= p; i++)
		string.SetCharacter(string.Length() - i, '=');

	return string;
}

auto Pargon::FromBase64(StringView base64) -> Buffer
{
	auto size = base64.Length();
	auto p = base64.Character(size - 2) == '=' ? 2 : (base64.Character(size - 1) == '=' ? 1 : 0);
	auto outputSize = ((size * 3) / 4) - p;

	Buffer buffer;
	buffer.SetSize(outputSize);

	uint8_t input[4];
	for (auto i = 0, o = 0; i < size; i++)
	{
		input[i % 4] = Base64Index(base64.Character(i));
		if ((i % 4) == 3)
		{
			if (o < outputSize) buffer.SetByte(o++, static_cast<uint8_t>((input[0] << 2) + ((input[1] & 0x30) >> 4)));
			if (o < outputSize) buffer.SetByte(o++, static_cast<uint8_t>(((input[1] & 0x0F) << 4) + ((input[2] & 0x3C) >> 2)));
			if (o < outputSize) buffer.SetByte(o++, static_cast<uint8_t>(((input[2] & 0x03) << 6) + input[3]));
		}
	}

	return buffer;
}

namespace
{
	class EndianDetector
	{
	private:
		static constexpr uint32_t _test = 0x01020304;
		static constexpr uint8_t _byte = (const uint8_t&)_test;

	public:
		static constexpr bool Little = _byte == 0x04;
		static constexpr bool Big = _byte == 0x01;

		static_assert(Little || Big, "cannot detect endianness");
	};
}

const Endian Pargon::NativeEndian = EndianDetector::Little ? Endian::Little : Endian::Big;
