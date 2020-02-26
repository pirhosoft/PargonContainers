#pragma once

#include <memory>
#include <type_traits>

namespace Pargon
{
	class BufferReference;
	class BufferView;
	class String;
	class StringReference;
	class StringView;

	enum class Endian
	{
		Big,
		Little
	};

	extern const Endian NativeEndian;

	class Buffer
	{
	public:
		using Iterator = uint8_t*;

		Buffer() = default;
		Buffer(const Buffer& copy) = delete;
		Buffer(Buffer&& move) = default;
		Buffer(std::unique_ptr<uint8_t[]>&& bytes, int size);
		Buffer(BufferView view);

		auto operator=(const Buffer& copy) -> Buffer& = delete;
		auto operator=(Buffer&& move) -> Buffer& = default;
		auto operator=(BufferView view) -> Buffer&;

		auto begin() const -> Iterator;
		auto end() const -> Iterator;

		auto IsEmpty() const -> bool;
		auto Size() const -> int;
		auto Byte(int index) const -> uint8_t;

		void SetByte(int index, uint8_t value);
		void SetBytes(int index, BufferView bytes);
		void SetSize(int size);
		void Append(uint8_t value, int count);
		void Append(BufferView view, bool reverse);
		auto Reserve(int size) -> BufferReference;
		void Erase(uint8_t value);
		void Clear();

		auto GetView() const -> BufferView;
		auto GetView(int index) const -> BufferView;
		auto GetView(int index, int count) const -> BufferView;
		auto GetReference() -> BufferReference;
		auto GetReference(int index) -> BufferReference;
		auto GetReference(int index, int count) -> BufferReference;

	private:
		int _used = 0;
		int _available = 0;

		std::unique_ptr<uint8_t[]> _data;
	};

	class BufferView
	{
	public:
		using Iterator = const uint8_t*;

		BufferView() = default;
		BufferView(const Buffer& buffer);
		BufferView(BufferReference buffer);
		BufferView(StringView string);
		BufferView(StringReference string);
		template<typename T> explicit BufferView(const T& object);
		BufferView(const uint8_t* data, int size);

		auto begin() const -> Iterator;
		auto end() const -> Iterator;

		auto IsEmpty() const -> bool;
		auto Size() const -> int;
		auto Byte(int index) const -> uint8_t;

		auto GetSubview(int index) const -> BufferView;
		auto GetSubview(int index, int count) const -> BufferView;
		auto GetBuffer() const -> Buffer;

	private:
		const uint8_t* _data = nullptr;
		int _size = 0;
	};

	auto operator==(BufferView left, BufferView right) -> bool;
	auto operator!=(BufferView left, BufferView right) -> bool;

	class BufferReference
	{
	public:
		using Iterator = uint8_t*;

		BufferReference() = default;
		BufferReference(const Buffer& buffer);
		BufferReference(StringReference string);
		BufferReference(uint8_t* data, int size);

		auto begin() const -> Iterator;
		auto end() const -> Iterator;

		auto IsEmpty() const -> bool;
		auto Size() const -> int;
		auto Byte(int index) const -> uint8_t;

		void SetByte(int index, uint8_t value);
		void SetBytes(int index, BufferView bytes);
		void Erase(uint8_t value);

		auto GetView() const -> BufferView;
		auto GetView(int index) const -> BufferView;
		auto GetView(int index, int count) const -> BufferView;
		auto GetSubreference(int index) const -> BufferReference;
		auto GetSubreference(int index, int count) const -> BufferReference;
		auto GetBuffer() const -> Buffer;

	private:
		uint8_t* _data = nullptr;
		int _size = 0;
	};

	auto ToBase64(BufferView view) -> String;
	auto FromBase64(StringView view) -> Buffer;
}

inline
auto Pargon::Buffer::begin() const -> Iterator
{
	return _data.get();
}

inline
auto Pargon::Buffer::end() const -> Iterator
{
	return _data.get() + _used;
}

inline
auto Pargon::Buffer::IsEmpty() const -> bool
{
	return _used == 0;
}

inline
auto Pargon::Buffer::Size() const -> int
{
	return _used;
}

inline
Pargon::BufferView::BufferView(const Buffer& buffer) :
	_data(buffer.begin()),
	_size(buffer.Size())
{
}

inline
Pargon::BufferView::BufferView(BufferReference buffer) :
	_data(buffer.begin()),
	_size(buffer.Size())
{
}

inline
Pargon::BufferView::BufferView(const uint8_t* data, int size) :
	_data(data),
	_size(size)
{
}

template<typename T>
Pargon::BufferView::BufferView(const T& object) :
	_data(reinterpret_cast<const uint8_t*>(std::addressof(object))),
	_size(static_cast<int>(sizeof(T)))
{
}

inline
auto Pargon::BufferView::begin() const -> Iterator
{
	return _data;
}

inline
auto Pargon::BufferView::end() const -> Iterator
{
	return _data + _size;
}

inline
auto Pargon::BufferView::IsEmpty() const -> bool
{
	return _size == 0;
}

inline
auto Pargon::BufferView::Size() const -> int
{
	return _size;
}

inline
Pargon::BufferReference::BufferReference(const Buffer& buffer) :
	_data(buffer.begin()),
	_size(buffer.Size())
{
}

inline
Pargon::BufferReference::BufferReference(uint8_t* data, int size) :
	_data(data),
	_size(size)
{
}

inline
auto Pargon::BufferReference::begin() const -> Iterator
{
	return _data;
}

inline
auto Pargon::BufferReference::end() const -> Iterator
{
	return _data + _size;
}

inline
auto Pargon::BufferReference::IsEmpty() const -> bool
{
	return _size == 0;
}

inline
auto Pargon::BufferReference::Size() const -> int
{
	return _size;
}
