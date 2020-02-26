#pragma once

#include <cassert>
#include <functional>

namespace Pargon
{
	template<typename Signature>
	class Function
	{
	public:
		Function() = default;
		Function(const Function<Signature>& copy) = delete;
		Function(Function<Signature>&& move) = default;
		~Function() = default;

		auto operator=(const Function<Signature>& copy) -> Function<Signature>& = delete;
		auto operator=(Function<Signature>&& move) -> Function<Signature>& = default;

		auto IsCallable() const -> bool;

		template<typename... ParameterTypes> auto operator()(ParameterTypes&&... parameters) const -> decltype(auto)
		{
			return _details->Call(std::forward<ParameterTypes>(parameters)...);
		}

	private:
		template<typename _Signature> struct Details {};
		template<typename CallableType, typename _Signature> struct Caller {};

		template<typename ReturnType, typename... ParameterTypes>
		struct Details<ReturnType(ParameterTypes...)>
		{
			virtual ~Details() = default;
			virtual ReturnType Call(ParameterTypes... parameters) const = 0;
		};

		template<typename CallableType, typename ReturnType, typename... ParameterTypes>
		struct Caller<CallableType, ReturnType(ParameterTypes...)> : Details<ReturnType(ParameterTypes...)>
		{
			Caller(Caller<CallableType, ReturnType(ParameterTypes...)>&& move) = default;

			Caller(CallableType&& callable) :
				_callable(std::move(callable))
			{
			}

			auto operator=(Caller<CallableType, ReturnType(ParameterTypes...)>&& move) -> Caller<CallableType, ReturnType(ParameterTypes...)>& = default;

			ReturnType Call(ParameterTypes... parameters) const override
			{
				return _callable(std::forward<ParameterTypes>(parameters)...);
			}

			CallableType _callable;
		};

		std::unique_ptr<Details<Signature>> _details;

	public:
		template<typename CallableType> Function(CallableType&& callable) :
			_details(std::make_unique<Caller<CallableType, Signature>>(std::move(callable)))
		{
		}
	};

	template<typename Signature>
	class FunctionView
	{
	public:
		template<typename CallerType> FunctionView(CallerType&& caller);

		template<typename... ParameterTypes> auto operator()(ParameterTypes&&... parameters) const -> decltype(auto);

	private:
		template<typename> struct Details;

		template<typename ReturnType, typename... ParameterTypes>
		struct Details<ReturnType(ParameterTypes...)>
		{
		public:
			template<typename CallerType>
			Details(CallerType&& caller) :
				_caller(reinterpret_cast<void*>(std::addressof(caller))),
				_wrapper(&Caller<std::remove_reference_t<CallerType>>::Call)
			{
			}

			auto Call(ParameterTypes... parameters) const -> decltype(auto)
			{
				return _wrapper(_caller, std::forward<ParameterTypes>(parameters)...);
			}

		private:
			using DetailsSignature = ReturnType(*)(void*, ParameterTypes...);

			void* _caller;
			DetailsSignature _wrapper;

			template<typename CallerType>
			struct Caller
			{
				static auto Call(void* caller, ParameterTypes... parameters) -> decltype(auto)
				{
					return std::invoke(*reinterpret_cast<CallerType*>(caller), std::forward<ParameterTypes>(parameters)...);
				}
			};
		};

		Details<Signature> _details;
	};
}

template<typename Signature>
auto Pargon::Function<Signature>::IsCallable() const -> bool
{
	return _details != nullptr;
}

template<typename Signature>
template<typename CallerType>
Pargon::FunctionView<Signature>::FunctionView(CallerType&& caller) :
	_details(std::forward<CallerType>(caller))
{
}

template<typename Signature>
template<typename... ParameterTypes>
auto Pargon::FunctionView<Signature>::operator()(ParameterTypes&&... parameters) const -> decltype(auto)
{
	return _details.Call(std::forward<ParameterTypes>(parameters)...);
}
