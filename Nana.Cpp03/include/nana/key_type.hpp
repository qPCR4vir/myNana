/*
*	A Key Implementation
*	Nana C++ Library(http://www.nanapro.org)
*	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
*
*	Distributed under the Boost Software License, Version 1.0.
*	(See accompanying file LICENSE_1_0.txt or copy at
*	http://www.boost.org/LICENSE_1_0.txt)
*
*	@file: nana/key_type.hpp
*/
#ifndef NANA_KEY_TYPE_HPP
#define NANA_KEY_TYPE_HPP

namespace nana
{
	namespace detail
	{
		class key_interface
		{
		public:
			virtual ~key_interface(){}

			virtual bool same_type(const key_interface*) const = 0;
			virtual bool compare(const key_interface*) const = 0;
		};	//end class key_interface

		//Use less compare for equal compare
		inline bool pred_equal_by_less(const key_interface * left, const key_interface* right)
		{
			return (left->compare(right) == false) && (right->compare(left) == false);
		}

		template<typename T>
		struct type_escape
		{
			typedef T type;
		};

		template<>
		struct type_escape<char*>
		{
			typedef std::string type;
		};

		template<>
		struct type_escape<const char*>
		{
			typedef std::string type;
		};

		template<int Size>
		struct type_escape<char[Size]>
		{
			typedef std::string type;
		};

		template<int Size>
		struct type_escape<const char[Size]>
		{
			typedef std::string type;
		};

		template<>
		struct type_escape<wchar_t*>
		{
			typedef std::wstring type;
		};

		template<>
		struct type_escape<const wchar_t*>
		{
			typedef std::wstring type;
		};

		template<int Size>
		struct type_escape<wchar_t[Size]>
		{
			typedef std::wstring type;
		};

		template<int Size>
		struct type_escape<const wchar_t[Size]>
		{
			typedef std::wstring type;
		};
	}

	template<typename T, typename Compare>
	class key
		: public detail::key_interface
	{
	public:
		typedef detail::key_interface key_interface;
		typedef T key_type;

		key(const key_type& k)
			: key_object_(k)
		{}
	public:
		//implement key_interface methods
		bool same_type(const key_interface * p) const
		{
			return (0 != dynamic_cast<const key*>(p));
		}

		bool compare(const key_interface* p) const
		{
			const key* rhs = dynamic_cast<const key*>(p);
			return rhs && compare_(key_object_, rhs->key_object_);
		}
	private:
		Compare		compare_;
		key_type	key_object_;
	};
}

#endif
