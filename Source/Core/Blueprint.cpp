#include "Pargon/Containers/Blueprint.h"

using namespace Pargon;

namespace
{
	enum Type
	{
		InvalidType,
		NullType,
		BooleanType,
		IntegerType,
		FloatingPointType,
		StringType,
		ArrayType,
		ObjectType
	};

	template<typename T>
	auto Construct(void* storage) -> T*
	{
		return new (storage) T();
	}

	auto Destruct(void* storage, Type type)
	{
		if (type == StringType)
			reinterpret_cast<Blueprint::String*>(storage)->~String();
		else if (type == ArrayType)
			reinterpret_cast<Blueprint::Array*>(storage)->~Array();
		else if (type == ObjectType)
			reinterpret_cast<Blueprint::Object*>(storage)->~Object();
	}

	void Change(void* storage, int& from, Type to)
	{
		Destruct(storage, static_cast<Type>(from));
		from = to;
	}
}

Blueprint::Blueprint(const Blueprint& copy)
{
	if (copy._type == StringType)
		SetToString(copy.AsStringView());
	else if (copy._type == ArrayType)
		SetToArray() = *copy.AsArray();
	else if (copy._type == ObjectType)
		SetToObject() = *copy.AsObject();
	else
	{
		_type = copy._type;
		_value = copy._value;
	}
}

Blueprint::~Blueprint()
{
	Change(&_value, _type, Type::InvalidType);
}

auto Blueprint::operator=(const Blueprint& copy) -> Blueprint&
{
	if (copy._type == StringType)
		SetToString(copy.AsStringView());
	else if (copy._type == ArrayType)
		SetToArray() = *copy.AsArray();
	else if (copy._type == ObjectType)
		SetToObject() = *copy.AsObject();
	else
	{
		_type = copy._type;
		_value = copy._value;
	}

	return *this;
}

auto Blueprint::IsInvalid() const -> bool
{
	return _type == InvalidType;
}

auto Blueprint::IsNull() const -> bool
{
	return _type == NullType;
}

auto Blueprint::IsBoolean() const -> bool
{
	return _type == BooleanType;
}

auto Blueprint::IsInteger() const -> bool
{
	return _type == IntegerType;
}

auto Blueprint::IsFloatingPoint() const -> bool
{
	return _type == FloatingPointType;
}

auto Blueprint::IsString() const -> bool
{
	return _type == StringType;
}

auto Blueprint::IsArray() const -> bool
{
	return _type == ArrayType;
}

auto Blueprint::IsObject() const -> bool
{
	return _type == ObjectType;
}

auto Blueprint::AsBoolean() const -> Boolean
{
	return IsBoolean() ? *reinterpret_cast<const Boolean*>(&_value) : false;
}

auto Blueprint::AsInteger() const -> Integer
{
	return IsInteger() ? *reinterpret_cast<const Integer*>(&_value) : 0;
}

auto Blueprint::AsFloatingPoint() const -> FloatingPoint
{
	return IsFloatingPoint() ? *reinterpret_cast<const FloatingPoint*>(&_value) : 0.0;
}

auto Blueprint::AsString() const -> const String*
{
	return IsString() ? reinterpret_cast<const String*>(&_value) : nullptr;
}

auto Blueprint::AsArray() const -> const Array*
{
	return IsArray() ? reinterpret_cast<const Array*>(&_value) : nullptr;
}

auto Blueprint::AsObject() const -> const Object*
{
	return IsObject() ? reinterpret_cast<const Object*>(&_value) : nullptr;
}

auto Blueprint::AsString() -> String*
{
	return IsString() ? reinterpret_cast<String*>(&_value) : nullptr;
}

auto Blueprint::AsArray() -> Array*
{
	return IsArray() ? reinterpret_cast<Array*>(&_value) : nullptr;
}

auto Blueprint::AsObject() -> Object*
{
	return IsObject() ? reinterpret_cast<Object*>(&_value) : nullptr;
}

auto Blueprint::AsStringView() const -> StringView
{
	return IsString() ? reinterpret_cast<const String*>(&_value)->GetView() : ""_sv;
}

void Blueprint::SetToInvalid()
{
	Change(&_value, _type, InvalidType);
}

void Blueprint::SetToNull()
{
	Change(&_value, _type, NullType);
}

void Blueprint::SetToBoolean(Boolean value)
{
	Change(&_value, _type, BooleanType);
	*reinterpret_cast<Boolean*>(&_value) = value;
}

void Blueprint::SetToInteger(Integer value)
{
	Change(&_value, _type, IntegerType);
	*reinterpret_cast<Integer*>(&_value) = value;
}

void Blueprint::SetToFloatingPoint(FloatingPoint value)
{
	Change(&_value, _type, FloatingPointType);
	*reinterpret_cast<FloatingPoint*>(&_value) = value;
}

void Blueprint::SetToString(StringView value)
{
	Change(&_value, _type, StringType);
	*Construct<String>(&_value) = value;
}

auto Blueprint::SetToString() -> String&
{
	Change(&_value, _type, ArrayType);
	return *Construct<String>(&_value);
}

auto Blueprint::SetToArray() -> Array&
{
	Change(&_value, _type, ArrayType);
	return *Construct<Array>(&_value);
}

auto Blueprint::SetToObject() -> Object&
{
	Change(&_value, _type, ObjectType);
	return *Construct<Object>(&_value);
}
