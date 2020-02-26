#pragma once

#include "Pargon/Containers/Sequence.h"

#include <cassert>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Pargon
{
	template<typename KeyType, typename ItemType>
	class Map
	{
	public:
		struct Entry
		{
			KeyType Key;
			ItemType Item;
		};

		Map() = default;
		Map(const std::initializer_list<Entry>& initializer);

		auto IsEmpty() const -> bool;
		auto Count() const -> int;
		auto LastIndex() const -> int;

		auto First() -> ItemType&;
		auto First() const -> const ItemType&;
		auto Last() -> ItemType&;
		auto Last() const -> const ItemType&;

		auto ItemWithKey(const KeyType& key) -> ItemType&;
		auto ItemWithKey(const KeyType& key) const -> const ItemType&;
		void SetItemWithKey(const KeyType& key, ItemType&& value);
		void SetItemWithKey(const KeyType& key, const ItemType& value);

		auto ItemAtIndex(int index) -> ItemType&;
		auto ItemAtIndex(int index) const -> const ItemType&;
		void SetItemAtIndex(int index, ItemType&& value);
		void SetItemAtIndex(int index, const ItemType& value);

		auto Keys() const -> SequenceView<KeyType>;
		auto Items() const -> SequenceView<ItemType>;
		auto Items() -> SequenceReference<ItemType>;
		auto GetKey(int index) const -> const KeyType&;
		auto GetIndex(const KeyType& key) const -> int;

		auto AddOrGet(const KeyType& key, ItemType&& item) -> ItemType&;
		auto AddOrGet(const KeyType& key, const ItemType& item) -> ItemType&;
		auto AddOrSet(const KeyType& key, ItemType&& item) -> ItemType&;
		auto AddOrSet(const KeyType& key, const ItemType& item) -> ItemType&;

		void RemoveWithKey(const KeyType& key);
		void RemoveAtIndex(int index);
		void RemoveWhere(FunctionView<bool(const KeyType&, const ItemType&)> predicate);

		void Clear();

	private:
		template<typename T> using void_t = void;
		template<typename T, typename = void> struct has_get_hash_member : std::false_type {};
		template<typename T, typename = void> struct has_get_hash_function : std::false_type {};
		template<typename T> struct has_get_hash_member<T, void_t<decltype(std::declval<const T>().GetHash())>> : std::true_type {};
		template<typename T> struct has_get_hash_function<T, void_t<decltype(GetHash(std::declval<T>()))>> : std::true_type {};
		static constexpr bool IsHashable = has_get_hash_member<KeyType>::value || has_get_hash_function<KeyType>::value || std::is_arithmetic<KeyType>::value || std::is_same<KeyType, std::type_index>::value;

		static_assert(IsHashable, "KeyType is not hashable and therefore cannot be used as the key type for a Map");

		template<typename T>
		struct Hasher
		{
			auto operator()(const T& item) const -> std::size_t
			{
				if constexpr (has_get_hash_member<T>::value)
					return item.GetHash();
				else if constexpr (has_get_hash_function<T>::value)
					return GetHash(item);
				else if constexpr (std::is_arithmetic<T>::value || std::is_same<T, std::type_index>::value)
					return std::hash<T>{}(item);
			}
		};

		std::unordered_map<KeyType, int, Hasher<KeyType>> _map;
		std::vector<KeyType> _keys;
		std::vector<ItemType> _items;
	};
}

template<typename KeyType, typename ItemType>
Pargon::Map<KeyType, ItemType>::Map(const std::initializer_list<Entry>& initializer)
{
	for (auto& entry : initializer)
		AddOrSet(entry.Key, entry.Item);
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::IsEmpty() const -> bool
{
	return _keys.empty();
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::Count() const -> int
{
	return static_cast<int>(_keys.size());
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::LastIndex() const -> int
{
	return _keys.empty() ? Sequence::InvalidIndex : Count() - 1;
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::First() -> ItemType&
{
	assert(!_keys.empty());
	return _items.front();
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::First() const -> const ItemType&
{
	assert(!_keys.empty());
	return _items.front();
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::Last() -> ItemType&
{
	assert(!_keys.empty());
	return _items.back();
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::Last() const -> const ItemType&
{
	assert(!_keys.empty());
	return _items.back();
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::ItemWithKey(const KeyType& key) -> ItemType&
{
	auto index = GetIndex(key);
	return ItemAtIndex(index);
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::ItemWithKey(const KeyType& key) const -> const ItemType&
{
	auto index = GetIndex(key);
	return ItemAtIndex(index);
}

template<typename KeyType, typename ItemType>
void Pargon::Map<KeyType, ItemType>::SetItemWithKey(const KeyType& key, ItemType&& value)
{
	auto index = GetIndex(key);
	SetItemAtIndex(index, std::move(value));
}

template<typename KeyType, typename ItemType>
void Pargon::Map<KeyType, ItemType>::SetItemWithKey(const KeyType& key, const ItemType& value)
{
	auto index = GetIndex(key);
	SetItemAtIndex(index, value);
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::ItemAtIndex(int index) -> ItemType&
{
	assert(index >= 0 && index < Count());
	return _items[index];
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::ItemAtIndex(int index) const -> const ItemType&
{
	assert(index >= 0 && index < Count());
	return _items[index];
}

template<typename KeyType, typename ItemType>
void Pargon::Map<KeyType, ItemType>::SetItemAtIndex(int index, ItemType&& value)
{
	assert(index >= 0 && index < Count());
	_items[index] = std::move(value);
}

template<typename KeyType, typename ItemType>
void Pargon::Map<KeyType, ItemType>::SetItemAtIndex(int index, const ItemType& value)
{
	assert(index >= 0 && index < Count());
	_items[index] = value;
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::Keys() const -> SequenceView<KeyType>
{
	return { _keys.data(), static_cast<int>(_keys.size()) };
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::Items() const -> SequenceView<ItemType>
{
	return { _items.data(), static_cast<int>(_items.size()) };
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::Items() -> SequenceReference<ItemType>
{
	return { _items.data(), static_cast<int>(_items.size()) };
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::GetKey(int index) const -> const KeyType&
{
	assert(index >= 0 && index < Count());
	return _keys[index];
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::GetIndex(const KeyType& key) const -> int
{
	auto index = _map.find(key);
	return index == _map.end() ? Sequence::InvalidIndex : index->second;
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::AddOrGet(const KeyType& key, ItemType&& item) -> ItemType&
{
	auto entry = _map.insert(std::make_pair(key, Count()));

	if (!entry.second)
		return _items[entry.first->second];

	_keys.push_back(key);
	_items.push_back(std::move(item));

	return _items.back();
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::AddOrGet(const KeyType& key, const ItemType& item) -> ItemType&
{
	auto entry = _map.insert(std::make_pair(key, Count()));

	if (!entry.second)
		return _items[entry.first->second];

	_keys.push_back(key);
	_items.push_back(item);

	return _items.back();
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::AddOrSet(const KeyType& key, ItemType&& item) -> ItemType&
{
	auto entry = _map.insert(std::make_pair(key, Count()));

	if (!entry.second)
	{
		_items[entry.first->second] = std::move(item);
		return _items[entry.first->second];
	}

	_keys.push_back(key);
	_items.push_back(std::move(item));

	return _items.back();
}

template<typename KeyType, typename ItemType>
auto Pargon::Map<KeyType, ItemType>::AddOrSet(const KeyType& key, const ItemType& item) -> ItemType&
{
	auto entry = _map.insert(std::make_pair(key, Count()));

	if (!entry.second)
	{
		_items[entry.first->second] = item;
		return _items[entry.first->second];
	}

	_keys.push_back(key);
	_items.push_back(item);

	return _items.back();
}

template<typename KeyType, typename ItemType>
void Pargon::Map<KeyType, ItemType>::RemoveWithKey(const KeyType& key)
{
	auto index = GetIndex(key);
	RemoveAtIndex(index);
}

template<typename KeyType, typename ItemType>
void Pargon::Map<KeyType, ItemType>::RemoveAtIndex(int index)
{
	assert(index >= 0 && index < Count());

	_map.erase(_keys[index]);

	for (auto& mapping : _map)
	{
		if (mapping.second > index)
			mapping.second--;
	}

	_keys.erase(_keys.begin() + index);
	_items.erase(_items.begin() + index);
}

template<typename KeyType, typename ItemType>
void Pargon::Map<KeyType, ItemType>::RemoveWhere(FunctionView<bool(const KeyType&, const ItemType&)> predicate)
{
	for (auto i = 0u; i < _keys.size(); i++)
	{
		if (predicate.Call(_keys[i], _items[i]))
			RemoveAtIndex(i--);
	}
}

template<typename KeyType, typename ItemType>
void Pargon::Map<KeyType, ItemType>::Clear()
{
	_map.clear();
	_keys.clear();
	_items.clear();
}
