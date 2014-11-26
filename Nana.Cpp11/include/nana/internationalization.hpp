/*
 *	An Implementation of i18n
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/internationalization.hpp
 */

#ifndef NANA_INTERNATIONALIZATION_HPP
#define NANA_INTERNATIONALIZATION_HPP
#include "basic_types.hpp"
#include <vector>
#include <sstream>

namespace nana
{
	class internationalization
	{
	public:
		void load(const std::string& file);
		void load_utf8(const std::string& file);

		template<typename ...Args>
		nana::string get(std::string msgid_utf8, Args&&... args) const
		{
			std::vector<nana::string> arg_strs;
			_m_fetch_args(arg_strs, std::forward<Args>(args)...);
			
			nana::string msgstr;
			if (_m_get(msgid_utf8, msgstr))
				_m_replace_args(msgstr, &arg_strs);
			return msgstr;
		}

		nana::string get(std::string msgid_utf8) const;
		void set(std::string msgid_utf8, nana::string msgstr);

		template<typename ...Args>
		nana::string operator()(std::string msgid_utf8, Args&&... args) const
		{
			return get(msgid_utf8, std::forward<Args>(args)...);
		}
	private:
		bool _m_get(std::string& msgid, nana::string& msgstr) const;
		void _m_replace_args(nana::string& str, std::vector<nana::string> * arg_strs) const;

		void _m_fetch_args(std::vector<nana::string>&) const //Termination of _m_fetch_args
		{}

		template<typename Arg>
		void _m_fetch_args(std::vector<nana::string>& v, Arg&& arg) const
		{
			std::wstringstream ss;
			ss << arg;
			v.emplace_back(ss.str());
		}

		template<typename ...Args>
		void _m_fetch_args(std::vector<nana::string>& v, const char* arg, Args&&... args) const
		{
			std::wstringstream ss;
			ss << nana::string(nana::charset(arg));
			v.emplace_back(ss.str());

			_m_fetch_args(v, std::forward<Args>(args)...);
		}

		template<typename ...Args>
		void _m_fetch_args(std::vector<nana::string>& v, const std::string& arg, Args&&... args) const
		{
			std::wstringstream ss;
			ss << nana::string(nana::charset(arg));
			v.emplace_back(ss.str());

			_m_fetch_args(v, std::forward<Args>(args)...);
		}

		template<typename ...Args>
		void _m_fetch_args(std::vector<nana::string>& v, std::string& arg, Args&&... args) const
		{
			std::wstringstream ss;
			ss << nana::string(nana::charset(arg));
			v.emplace_back(ss.str());

			_m_fetch_args(v, std::forward<Args>(args)...);
		}

		template<typename ...Args>
		void _m_fetch_args(std::vector<nana::string>& v, std::string&& arg, Args&&... args) const
		{
			std::wstringstream ss;
			ss << nana::string(nana::charset(arg));
			v.emplace_back(ss.str());

			_m_fetch_args(v, std::forward<Args>(args)...);
		}

		template<typename Arg, typename ...Args>
		void _m_fetch_args(std::vector<nana::string>& v, Arg&& arg, Args&&... args) const
		{
			std::wstringstream ss;
			ss << arg;
			v.emplace_back(ss.str());

			_m_fetch_args(v, std::forward<Args>(args)...);
		}
	};
}

#endif//NANA_I18N_HPP