#pragma once

#include "Pargon/Containers/Function.h"
#include "Pargon/Containers/Sequence.h"

#include <algorithm>
#include <cassert>
#include <vector>

namespace Pargon
{
	template<typename ItemType>
	class List
	{
	public:
		using Iterator = ItemType*;
		using ConstIterator = const ItemType*;

		List() = default;
		List(std::vector<ItemType>&& vector);
		List(SequenceView<ItemType> view);
		List(int count, const ItemType& copy);
		List(const std::initializer_list<ItemType>& initializer);

		auto operator=(SequenceView<ItemType> view) -> List<ItemType>&;
		auto operator=(const std::initializer_list<ItemType>& initializer) -> List<ItemType>&;

		auto begin() -> Iterator;
		auto begin() const -> ConstIterator;
		auto end() -> Iterator;
		auto end() const -> ConstIterator;

		auto IsEmpty() const -> bool;
		auto Count() const -> int;
		auto Capacity() const -> int;
		auto LastIndex() const -> int;

		auto First() -> ItemType&;
		auto First() const -> const ItemType&;
		auto Last() -> ItemType&;
		auto Last() const -> const ItemType&;
		auto Item(int index) -> ItemType&;
		auto Item(int index) const -> const ItemType&;
		void SetItem(int index, ItemType&& value);
		void SetItem(int index, const ItemType& value);

		auto GetView() const -> SequenceView<ItemType>;
		auto GetView(int index) const -> SequenceView<ItemType>;
		auto GetView(int index, int count) const -> SequenceView<ItemType>;
		auto GetReference() -> SequenceReference<ItemType>;
		auto GetReference(int index) -> SequenceReference<ItemType>;
		auto GetReference(int index, int count) -> SequenceReference<ItemType>;

		void SetCount(int count);
		void SetCount(int count, const ItemType& copy);
		void EnsureCount(int count);
		void EnsureCount(int count, const ItemType& copy);
		void EnsureCapacity(int capacity);

		auto Add(ItemType&& item) -> ItemType&;
		auto Add(const ItemType& item) -> ItemType&;
		auto Insert(ItemType&& item, int index) -> ItemType&;
		auto Insert(const ItemType& item, int index) -> ItemType&;
		template<typename... ConstructorParameterTypes> auto Increment(ConstructorParameterTypes&&... constructorParameters) -> ItemType&;

		auto Find(const ItemType& item) const -> int;
		auto FindWhere(FunctionView<bool(const ItemType&)> predicate) const -> int;

		auto Remove(const ItemType& item) -> bool;
		auto RemoveWhere(FunctionView<bool(const ItemType&)> predicate) -> bool;
		auto RemoveAt(int index) -> bool;
		auto RemoveLast() -> bool;

		void Clear();

	private:
		std::vector<ItemType> _vector;
	};
}

template<typename ItemType>
Pargon::List<ItemType>::List(std::vector<ItemType>&& vector) :
	_vector(std::move(vector))
{
}

template<typename ItemType>
Pargon::List<ItemType>::List(SequenceView<ItemType> view) :
	_vector(view.begin(), view.end())
{
}

template<typename ItemType>
Pargon::List<ItemType>::List(int count, const ItemType& copy)
{
	_vector.resize(count, copy);
}

template<typename ItemType>
Pargon::List<ItemType>::List(const std::initializer_list<ItemType>& initializer) :
	_vector(initializer)
{
}

template<typename ItemType>
auto Pargon::List<ItemType>::operator=(SequenceView<ItemType> view) -> List<ItemType>&
{
	_vector.assign(view.begin(), view.end());
	return *this;
}

template<typename ItemType>
auto Pargon::List<ItemType>::operator=(const std::initializer_list<ItemType>& initializer) -> List<ItemType>&
{
	_vector = initializer;
	return *this;
}

template<typename ItemType>
auto Pargon::List<ItemType>::begin() -> Iterator
{
	return _vector.data();
}

template<typename ItemType>
auto Pargon::List<ItemType>::begin() const -> ConstIterator
{
	return _vector.data();
}

template<typename ItemType>
auto Pargon::List<ItemType>::end() -> Iterator
{
	return _vector.data() + _vector.size();
}

template<typename ItemType>
auto Pargon::List<ItemType>::end() const -> ConstIterator
{
	return _vector.data() + _vector.size();
}

template<typename ItemType>
auto Pargon::List<ItemType>::IsEmpty() const -> bool
{
	return _vector.empty();
}

template<typename ItemType>
auto Pargon::List<ItemType>::Count() const -> int
{
	return static_cast<int>(_vector.size());
}

template<typename ItemType>
auto Pargon::List<ItemType>::Capacity() const -> int
{
	return static_cast<int>(_vector.capacity());
}

template<typename ItemType>
auto Pargon::List<ItemType>::LastIndex() const -> int
{
	return IsEmpty() ? Sequence::InvalidIndex : Count() - 1;
}

template<typename ItemType>
auto Pargon::List<ItemType>::First() -> ItemType&
{
	assert(!_vector.empty());
	return _vector.front();
}

template<typename ItemType>
auto Pargon::List<ItemType>::First() const -> const ItemType&
{
	assert(!_vector.empty());
	return _vector.front();
}

template<typename ItemType>
auto Pargon::List<ItemType>::Last() -> ItemType&
{
	assert(!_vector.empty());
	return _vector.back();
}

template<typename ItemType>
auto Pargon::List<ItemType>::Last() const -> const ItemType&
{
	assert(!_vector.empty());
	return _vector.back();
}

template<typename ItemType>
auto Pargon::List<ItemType>::Item(int index) -> ItemType&
{
	assert(index < Count());
	return _vector[index];
}

template<typename ItemType>
auto Pargon::List<ItemType>::Item(int index) const -> const ItemType&
{
	assert(index < Count());
	return _vector[index];
}

template<typename ItemType>
void Pargon::List<ItemType>::SetItem(int index, ItemType&& value)
{
	assert(index < Count());
	_vector[index] = std::move(value);
}

template<typename ItemType>
void Pargon::List<ItemType>::SetItem(int index, const ItemType& value)
{
	assert(index < Count());
	_vector[index] = value;
}

template<typename ItemType>
auto Pargon::List<ItemType>::GetView() const -> SequenceView<ItemType>
{
	return GetView(0, Count());
}

template<typename ItemType>
auto Pargon::List<ItemType>::GetView(int start) const -> SequenceView<ItemType>
{
	return GetView(start, Count() - start);
}

template<typename ItemType>
auto Pargon::List<ItemType>::GetView(int start, int count) const -> SequenceView<ItemType>
{
	if (start < 0 || start + count > Count())
		return {};

	return { begin() + start, count };
}

template<typename ItemType>
auto Pargon::List<ItemType>::GetReference() -> SequenceReference<ItemType>
{
	return GetReference(0, Count());
}

template<typename ItemType>
auto Pargon::List<ItemType>::GetReference(int start) -> SequenceReference<ItemType>
{
	return GetReference(start, Count() - start);
}

template<typename ItemType>
auto Pargon::List<ItemType>::GetReference(int start, int count) -> SequenceReference<ItemType>
{
	if (start < 0 || start + count > Count())
		return {};

	return { begin() + start, count };
}

template<typename ItemType>
void Pargon::List<ItemType>::SetCount(int count)
{
	_vector.resize(count);
}

template<typename ItemType>
void Pargon::List<ItemType>::SetCount(int count, const ItemType& copy)
{
	_vector.resize(count, copy);
}

template<typename ItemType>
void Pargon::List<ItemType>::EnsureCount(int count)
{
	if (count > static_cast<int>(_vector.size()))
		_vector.resize(count);
}

template<typename ItemType>
void Pargon::List<ItemType>::EnsureCount(int count, const ItemType& copy)
{
	if (count > static_cast<int>(_vector.size()))
		_vector.resize(count, copy);
}

template<typename ItemType>
void Pargon::List<ItemType>::EnsureCapacity(int capacity)
{
	_vector.reserve(capacity);
}

template<typename ItemType>
auto Pargon::List<ItemType>::Add(ItemType&& item) -> ItemType&
{
	_vector.push_back(std::move(item));
	return _vector.back();
}

template<typename ItemType>
auto Pargon::List<ItemType>::Add(const ItemType& item) -> ItemType&
{
	_vector.push_back(item);
	return _vector.back();
}

template<typename ItemType>
auto Pargon::List<ItemType>::Insert(ItemType&& item, int index) -> ItemType&
{
	assert(index <= Count());

	auto iterator = _vector.begin() + index;
	_vector.insert(iterator, std::move(item));
	return _vector[index];
}

template<typename ItemType>
auto Pargon::List<ItemType>::Insert(const ItemType& item, int index) -> ItemType&
{
	assert(index <= Count());

	auto iterator = _vector.begin() + index;
	_vector.insert(iterator, item);
	return _vector[index];
}

template<typename ItemType>
template<typename... ConstructorParameterTypes>
auto Pargon::List<ItemType>::Increment(ConstructorParameterTypes&&... constructorParameters) -> ItemType&
{
	_vector.emplace_back(std::forward<ConstructorParameterTypes>(constructorParameters)...);
	return _vector.back();
}

template<typename ItemType>
auto Pargon::List<ItemType>::Find(const ItemType& item) const -> int
{
	for (auto i = 0u; i < _vector.size(); i++)
	{
		if (item == _vector[i])
			return i;
	}

	return Sequence::InvalidIndex;
}

template<typename ItemType>
auto Pargon::List<ItemType>::FindWhere(FunctionView<bool(const ItemType&)> predicate) const -> int
{
	for (auto i = 0u; i < _vector.size(); i++)
	{
		if (predicate(_vector[i]))
			return i;
	}

	return Sequence::InvalidIndex;
}

template<typename ItemType>
auto Pargon::List<ItemType>::Remove(const ItemType& item) -> bool
{
	auto iterator = std::find(_vector.begin(), _vector.end(), item);
	if (iterator == _vector.end())
		return false;

	_vector.erase(iterator);
	return true;
}

template<typename ItemType>
auto Pargon::List<ItemType>::RemoveWhere(FunctionView<bool(const ItemType&)> predicate) -> bool
{
	auto removed = false;

	for (auto i = 0u; i < _vector.size(); i++)
	{
		if (predicate(_vector[i]))
		{
			RemoveAt(i--);
			removed = true;
		}
	}

	return removed;
}

template<typename ItemType>
auto Pargon::List<ItemType>::RemoveAt(int index) -> bool
{
	if (index < 0 || index >= static_cast<int>(_vector.size()))
		return false;

	auto iterator = _vector.begin() + index;
	_vector.erase(iterator);
	return true;
}

template<typename ItemType>
auto Pargon::List<ItemType>::RemoveLast() -> bool
{
	if (_vector.empty())
		return false;

	auto iterator = _vector.end() - 1;
	_vector.erase(iterator);
	return true;
}

template<typename ItemType>
void Pargon::List<ItemType>::Clear()
{
	_vector.clear();
}

template<typename ItemType>
Pargon::SequenceView<ItemType>::SequenceView(const List<ItemType>& list) :
	_data(list.begin()),
	_count(list.Count())
{
}

template<typename ItemType>
Pargon::SequenceReference<ItemType>::SequenceReference(List<ItemType>& list) :
	_data(list.begin()),
	_count(list.Count())
{
}
