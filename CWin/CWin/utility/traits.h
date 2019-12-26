#pragma once

#include <functional>

namespace cwin::utility{
	template<typename in_return_type, typename... args_types>
	struct function_traits_base{
		using return_type = in_return_type;
		using function_type = std::function<return_type(args_types...)>;
		using raw_function_type = return_type(*)(args_types...);

		static const std::size_t arg_count = sizeof...(args_types);

		template <std::size_t index>
		struct args{
			using type = typename std::tuple_element<index, std::tuple<args_types...>>::type;
		};
	};

	template<typename function_type>
	struct function_traits;

	template<typename return_type, typename... args_types>
	struct function_traits<return_type(args_types...)> : function_traits_base<return_type, args_types...>{
		static const bool is_raw = true;
		static const bool is_static = true;
		static const bool is_constant = false;
	};

	template<typename return_type, typename... args_types>
	struct function_traits<return_type(*)(args_types...)> : function_traits_base<return_type, args_types...>{
		static const bool is_raw = true;
		static const bool is_static = true;
		static const bool is_constant = false;
	};

	template<typename class_type, typename return_type, typename... args_types>
	struct function_traits<return_type(class_type:: *)(args_types...)> : function_traits_base<return_type, args_types...>{
		static const bool is_raw = true;
		static const bool is_static = false;
		static const bool is_constant = false;
	};

	template<typename class_type, typename return_type, typename... args_types>
	struct function_traits<return_type(class_type:: *)(args_types...) const> : function_traits_base<return_type, args_types...>{
		static const bool is_raw = true;
		static const bool is_static = false;
		static const bool is_constant = true;
	};

	template<typename return_type, typename... args_types>
	struct function_traits<std::function<return_type(args_types...)>> : function_traits_base<return_type, args_types...>{
		static const bool is_raw = false;
		static const bool is_static = false;
		static const bool is_constant = false;
	};

	template<typename function_object_type>
	struct function_or_function_object_traits : function_traits<decltype(&function_object_type::operator ())>{
		static const bool is_functor = true;
	};

	template<typename return_type, typename... args_types>
	struct function_or_function_object_traits<return_type(args_types...)> : function_traits<return_type(args_types...)>{
		static const bool is_functor = false;
	};

	template<typename return_type, typename... args_types>
	struct function_or_function_object_traits<return_type(*)(args_types...)> : function_traits<return_type(*)(args_types...)>{
		static const bool is_functor = false;
	};

	template<typename class_type, typename return_type, typename... args_types>
	struct function_or_function_object_traits<return_type(class_type:: *)(args_types...)> : function_traits<return_type(class_type:: *)(args_types...)>{
		static const bool is_functor = false;
	};

	template<typename class_type, typename return_type, typename... args_types>
	struct function_or_function_object_traits<return_type(class_type:: *)(args_types...) const> : function_traits<return_type(class_type:: *)(args_types...) const>{
		static const bool is_functor = false;
	};

	template<typename return_type, typename... args_types>
	struct function_or_function_object_traits<std::function<return_type(args_types...)>> : function_traits<std::function<return_type(args_types...)>>{
		static const bool is_functor = true;
	};

	struct object_to_function_traits{
		template <typename object_type>
		using traits = function_or_function_object_traits<object_type>;

		template <typename object_type>
		static typename traits<object_type>::function_type get(const object_type &object){
			return object;
		}
	};
}
