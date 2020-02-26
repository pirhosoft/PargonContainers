#pragma once

#include "Pargon/Containers/List.h"
#include "Pargon/Containers/Map.h"
#include "Pargon/Containers/String.h"

#include <memory>
#include <type_traits>

namespace Pargon
{
	class Blueprint
	{
	public:
		using Invalid = struct {};
		using Null = struct {};
		using Boolean = bool;
		using Integer = long long;
		using FloatingPoint = double;
		using String = String;

		struct Array
		{
			List<Blueprint> Children;
		};

		struct Object
		{
			Map<String, Blueprint> Children;
		};

		Blueprint() = default;
		Blueprint(const Blueprint& copy);
		~Blueprint();

		auto operator=(const Blueprint& copy) -> Blueprint&;

		auto IsInvalid() const -> bool;
		auto IsNull() const -> bool;
		auto IsBoolean() const -> bool;
		auto IsInteger() const -> bool;
		auto IsFloatingPoint() const -> bool;
		auto IsString() const -> bool;
		auto IsArray() const -> bool;
		auto IsObject() const -> bool;

		auto AsBoolean() const -> bool;
		auto AsInteger() const -> long long;
		auto AsFloatingPoint() const -> double;
		auto AsString() const -> const String*;
		auto AsArray() const -> const Array*;
		auto AsObject() const -> const Object*;

		auto AsString() -> String*;
		auto AsArray() -> Array*;
		auto AsObject() -> Object*;

		auto AsStringView() const -> StringView;

		void SetToInvalid();
		void SetToNull();
		void SetToBoolean(Boolean value);
		void SetToInteger(Integer value);
		void SetToFloatingPoint(FloatingPoint value);
		void SetToString(StringView value);
		auto SetToString() -> String&;
		auto SetToArray() -> Array&;
		auto SetToObject() -> Object&;

	private:
		using Storage = std::aligned_union_t<0, Invalid, Null, Boolean, Integer, FloatingPoint, String, Array, Object>;

		int _type = 0;
		Storage _value;
	};
}
