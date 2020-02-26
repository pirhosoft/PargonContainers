#pragma once

#include "Pargon/Containers/Function.h"

#include <algorithm>
#include <cassert>
#include <initializer_list>

namespace Pargon
{
	struct Sequence
	{
		static constexpr int InvalidIndex = -1;
	};

	template<typename ItemType, int N> class Array;
	template<typename ItemType> class List;
	template<typename ItemType> class SequenceReference;

	template<typename ItemType>
	class SequenceView
	{
	public:
		using Iterator = const ItemType*;

		SequenceView() = default;
		SequenceView(SequenceReference<ItemType> sequence);
		SequenceView(const List<ItemType>& list);
		template<int N> SequenceView(const Array<ItemType, N>& array);
		template<int N> SequenceView(ItemType const (&array)[N]);
		SequenceView(const std::initializer_list<ItemType>& initializer);
		SequenceView(const ItemType* pointer, int count);

		auto begin() const -> Iterator;
		auto end() const -> Iterator;

		auto IsEmpty() const -> bool;
		auto Count() const -> int;
		auto LastIndex() const -> int;
		auto First() const -> const ItemType&;
		auto Last() const -> const ItemType&;
		auto Item(int index) const -> const ItemType&;
		
		auto GetSubview(int index) const -> SequenceView<ItemType>;
		auto GetSubview(int index, int count) const -> SequenceView<ItemType>;

		auto Contains(const ItemType& item) -> bool;
		auto Contains(FunctionView<bool(const ItemType&)> comparer) -> bool;
		auto IndexOf(const ItemType& item) -> int;
		auto IndexOf(FunctionView<bool(const ItemType&)> comparer) -> int;

	private:
		const ItemType* _data = nullptr;
		int _count = 0;
	};

	template<typename ItemType>
	class SequenceReference
	{
	public:
		using Iterator = ItemType*;

		SequenceReference() = default;
		SequenceReference(List<ItemType>& list);
		template<int N> SequenceReference(Array<ItemType, N>& array);
		template<int N> SequenceReference(ItemType (&array)[N]);
		SequenceReference(ItemType* pointer, int count);

		auto begin() const -> Iterator;
		auto end() const -> Iterator;

		auto IsEmpty() const -> bool;
		auto Count() const -> int;
		auto LastIndex() const -> int;
		auto First() const -> ItemType&;
		auto Last() const -> ItemType&;
		auto Item(int index) const -> ItemType&;
		void SetItem(int index, ItemType&& value) const;
		void SetItem(int index, const ItemType& value) const;

		auto GetView() const -> SequenceView<ItemType>;
		auto GetView(int index) const -> SequenceView<ItemType>;
		auto GetView(int index, int count) const -> SequenceView<ItemType>;
		auto GetSubreference(int index) const -> SequenceReference<ItemType>;
		auto GetSubreference(int index, int count) const -> SequenceReference<ItemType>;

		void Reverse();
		void Sort();
		void Sort(FunctionView<bool(const ItemType&, const ItemType&)> comparer);

	private:
		ItemType* _data = nullptr;
		int _count = 0;
	};
}

template<typename ItemType>
Pargon::SequenceView<ItemType>::SequenceView(SequenceReference<ItemType> sequence) :
	_data(sequence.begin()),
	_count(sequence.Count())
{
}

template<typename ItemType>
template<int N>
Pargon::SequenceView<ItemType>::SequenceView(ItemType const (&array)[N]) :
	_data(N == 0 ? nullptr : array),
	_count(N)
{
}

template<typename ItemType>
Pargon::SequenceView<ItemType>::SequenceView(const std::initializer_list<ItemType>& initializer) :
	_data(initializer.size() == 0 ? nullptr : initializer.begin()),
	_count(static_cast<int>(initializer.size()))
{
}

template<typename ItemType>
Pargon::SequenceView<ItemType>::SequenceView(const ItemType* pointer, int count) :
	_data(pointer),
	_count(count)
{
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::begin() const -> Iterator
{
	return _data;
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::end() const -> Iterator
{
	return _data + _count;
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::IsEmpty() const -> bool
{
	return _count == 0;
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::Count() const -> int
{
	return _count;
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::LastIndex() const -> int
{
	return _count == 0 ? Sequence::InvalidIndex : _count - 1;
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::First() const -> const ItemType&
{
	assert(_count != 0);
	return _data[0];
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::Last() const -> const ItemType&
{
	assert(_count != 0);
	return _data[_count - 1];
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::Item(int index) const -> const ItemType&
{
	assert(index < _count);
	return _data[index];
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::GetSubview(int index) const -> SequenceView<ItemType>
{
	return GetSubview(index, _count - index);
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::GetSubview(int index, int count) const -> SequenceView<ItemType>
{
	if (index + count > _count)
		return {};

	return { _data + index, count };
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::Contains(const ItemType& item) -> bool
{
	return std::find(begin(), end(), item) != end();
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::Contains(FunctionView<bool(const ItemType&)> comparer) -> bool
{
	return std::find_if(begin(), end(), comparer) != end();
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::IndexOf(const ItemType& item) -> int
{
	auto iterator = std::find(begin(), end(), item);
	return iterator == end() ? Sequence::InvalidIndex : static_cast<int>(iterator - begin());
}

template<typename ItemType>
auto Pargon::SequenceView<ItemType>::IndexOf(FunctionView<bool(const ItemType&)> comparer) -> int
{
	auto iterator = std::find_if(begin(), end(), comparer);
	return iterator == end() ? Sequence::InvalidIndex : static_cast<int>(iterator - begin());
}

template<typename ItemType>
template<int N>
Pargon::SequenceReference<ItemType>::SequenceReference(ItemType (&array)[N]) :
	_data(N == 0 ? nullptr : array),
	_count(N)
{
}

template<typename ItemType>
Pargon::SequenceReference<ItemType>::SequenceReference(ItemType* pointer, int count) :
	_data(pointer),
	_count(count)
{
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::begin() const -> Iterator
{
	return _data;
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::end() const -> Iterator
{
	return _data + _count;
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::IsEmpty() const -> bool
{
	return _count == 0;
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::Count() const -> int
{
	return _count;
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::LastIndex() const -> int
{
	return _count == 0 ? Sequence::InvalidIndex : _count - 1;
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::First() const -> ItemType&
{
	assert(_count != 0);
	return _data[0];
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::Last() const -> ItemType&
{
	assert(_count != 0);
	return _data[_count - 1];
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::Item(int index) const -> ItemType&
{
	assert(index < _count);
	return _data[index];
}

template<typename ItemType>
void Pargon::SequenceReference<ItemType>::SetItem(int index, ItemType&& value) const
{
	assert(index < _count);
	_data[index] = std::move(value);
}

template<typename ItemType>
void Pargon::SequenceReference<ItemType>::SetItem(int index, const ItemType& value) const
{
	assert(index < _count);
	_data[index] = value;
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::GetView() const -> SequenceView<ItemType>
{
	return GetView(0, _count);
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::GetView(int index) const -> SequenceView<ItemType>
{
	return GetView(index, _count - index);
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::GetView(int index, int count) const -> SequenceView<ItemType>
{
	if (index < 0 || index + count > _count)
		return {};

	return { _data + index, count };
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::GetSubreference(int index) const -> SequenceReference<ItemType>
{
	return GetSubreference(index, _count - index);
}

template<typename ItemType>
auto Pargon::SequenceReference<ItemType>::GetSubreference(int index, int count) const -> SequenceReference<ItemType>
{
	if (index < 0 || index + count > _count)
		return {};

	return { _data + index, count };
}

template<typename ItemType>
void Pargon::SequenceReference<ItemType>::Reverse()
{
	std::reverse(begin(), end());
}

template<typename ItemType>
void Pargon::SequenceReference<ItemType>::Sort()
{
	std::sort(begin(), end());
}

template<typename ItemType>
void Pargon::SequenceReference<ItemType>::Sort(FunctionView<bool(const ItemType&, const ItemType&)> comparer)
{
	std::sort(begin(), end(), comparer);
}
