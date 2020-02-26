#pragma once

#include "Pargon/Containers/Sequence.h"

#include <algorithm>
#include <array>
#include <cassert>

namespace Pargon
{
	template<typename ItemType, int N>
	class Array
	{
	public:
		using Iterator = ItemType*;
		using ConstIterator = const ItemType*;

		constexpr Array();
		constexpr Array(const std::array<ItemType, N>& array);

		auto operator=(ItemType const (&array)[N]) -> Array<ItemType, N>&;

		auto begin() -> Iterator;
		auto begin() const -> ConstIterator;
		auto end() -> Iterator;
		auto end() const -> ConstIterator;

		constexpr auto IsEmpty() const -> bool;
		constexpr auto Count() const -> int;
		constexpr auto LastIndex() const -> int;

		auto First() -> ItemType&;
		auto First() const -> const ItemType&;
		auto Last() -> ItemType&;
		auto Last() const -> const ItemType&;
		auto Item(int index) -> ItemType&;
		auto Item(int index) const -> const ItemType&;
		void SetItem(int index, ItemType&& item);
		void SetItem(int index, const ItemType& item);

		auto GetView() const -> SequenceView<ItemType>;
		auto GetView(int index) const -> SequenceView<ItemType>;
		auto GetView(int index, int count) const -> SequenceView<ItemType>;
		auto GetReference() -> SequenceReference<ItemType>;
		auto GetReference(int index) -> SequenceReference<ItemType>;
		auto GetReference(int index, int count) -> SequenceReference<ItemType>;

	private:
		std::array<ItemType, N> _array;
	};
}

template<typename ItemType, int N> constexpr
Pargon::Array<ItemType, N>::Array() :
	_array()
{
}

template<typename ItemType, int N> constexpr
Pargon::Array<ItemType, N>::Array(const std::array<ItemType, N>& array) :
	_array(array)
{
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::operator=(ItemType const (&array)[N]) -> Array<ItemType, N>&
{
	std::copy(array, array + N, _array.begin());
	return *this;
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::begin() -> Iterator
{
	return _array.data();
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::begin() const -> ConstIterator
{
	return _array.data();
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::end() -> Iterator
{
	return _array.data() + N;
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::end() const -> ConstIterator
{
	return _array.data() + N;
}

template<typename ItemType, int N> constexpr
auto Pargon::Array<ItemType, N>::IsEmpty() const -> bool
{
	return N == 0;
}

template<typename ItemType, int N> constexpr
auto Pargon::Array<ItemType, N>::Count() const -> int
{
	return N;
}

template<typename ItemType, int N> constexpr
auto Pargon::Array<ItemType, N>::LastIndex() const -> int
{
	return N - 1;
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::First() -> ItemType&
{
	assert(N != 0);
	return _array[0];
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::First() const -> const ItemType&
{
	assert(N != 0);
	return _array[0];
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::Last() -> ItemType&
{
	assert(N != 0);
	return _array[N - 1];
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::Last() const -> const ItemType&
{
	assert(N != 0);
	return _array[N - 1];
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::Item(int index) -> ItemType&
{
	assert(index < N);
	return _array[index];
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::Item(int index) const -> const ItemType&
{
	assert(index < N);
	return _array[index];
}

template<typename ItemType, int N>
void Pargon::Array<ItemType, N>::SetItem(int index, ItemType&& value)
{
	assert(index < N);
	_array[index] = std::move(value);
}

template<typename ItemType, int N>
void Pargon::Array<ItemType, N>::SetItem(int index, const ItemType& value)
{
	assert(index < N);
	_array[index] = value;
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::GetView() const -> SequenceView<ItemType>
{
	return GetView(0, N);
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::GetView(int start) const -> SequenceView<ItemType>
{
	return GetView(start, N - start);
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::GetView(int start, int count) const -> SequenceView<ItemType>
{
	if (start < 0 || (start + count > N))
		return {};

	return { _array.data() + start, count };
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::GetReference() -> SequenceReference<ItemType>
{
	return GetReference(0, N);
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::GetReference(int start) -> SequenceReference<ItemType>
{
	return GetReference(start, N - start);
}

template<typename ItemType, int N>
auto Pargon::Array<ItemType, N>::GetReference(int start, int count) -> SequenceReference<ItemType>
{
	if (start < 0 || (start + count > N))
		return {};

	return { _array.data() + start, count };
}

template<typename ItemType>
template<int N>
Pargon::SequenceView<ItemType>::SequenceView(const Array<ItemType, N>& array) :
	_data(array.begin()),
	_count(N)
{
}

template<typename ItemType>
template<int N>
Pargon::SequenceReference<ItemType>::SequenceReference(Array<ItemType, N>& array) :
	_data(array.begin()),
	_count(N)
{
}
