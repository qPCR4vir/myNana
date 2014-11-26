/*
*	An Implementation of i18n
*	Nana C++ Library(http://www.nanapro.org)
*	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
*
*	Distributed under the Boost Software License, Version 1.0.
*	(See accompanying file LICENSE_1_0.txt or copy at
*	http://www.boost.org/LICENSE_1_0.txt)
*
*	@file: nana/internationalization.cpp
*/

#include <nana/internationalization.hpp>
#include <unordered_map>
#include <fstream>
#include <memory>

namespace nana
{
	namespace internationalization_parts
	{
		enum class token
		{
			msgid, msgstr, string, eof
		};

		class tokenizer
		{
		public:
			tokenizer(const std::string& file)
			{
				std::ifstream ifs(file.data(), std::ios::binary);
				if (ifs)
				{
					ifs.seekg(0, std::ios::end);
					auto len = static_cast<unsigned>(ifs.tellg());
					ifs.seekg(0, std::ios::beg);
					if (len > 0)
					{
						data_.reset(new char[len]);
						ifs.read(data_.get(), len);
						read_ptr_ = data_.get();
						end_ptr_ = read_ptr_ + len;
					}
				}
			}

			token read()
			{
				if (read_ptr_ == end_ptr_)
					return token::eof;
				str_.clear();
				_m_eat_ws();

				if (*read_ptr_ == '"')
				{
					bool reach_right_quota;
					while (true)
					{
						reach_right_quota = false;
						bool escape = false;
						for (auto i = read_ptr_ + 1; i != end_ptr_; ++i)
						{
							if (escape)
							{
								escape = false;
								continue;
							}

							if ('"' == *i)
							{
								str_.append(read_ptr_ + 1, i - read_ptr_ - 1);
								read_ptr_ = i + 1;
								reach_right_quota = true;
								break;
							}
						}
						_m_eat_ws();
						if (read_ptr_ == end_ptr_ || '"' != *read_ptr_)
							break;
					}
					
					if (reach_right_quota)
						return token::string;
				}
				else if ('a' <= *read_ptr_ && *read_ptr_ <= 'z')
				{
					for (auto i = read_ptr_; i != end_ptr_; ++i)
					{
						if (*i < 'a' || 'z' < *i)
						{
							std::string id(read_ptr_, i);
							read_ptr_ = i;
							if (id == "msgid")
								return token::msgid;
							else if (id == "msgstr")
								return token::msgstr;
							break;
						}
					}
					read_ptr_ = end_ptr_;
				}
				return token::eof;
			}

			std::string& get_str()
			{
				return str_;
			}
		private:
			void _m_eat_ws()
			{
				for (auto i = read_ptr_; i != end_ptr_; ++i)
				{
					switch (*i)
					{
					case ' ': case '\t': case '\r': case '\n':
						break;
					default:
						read_ptr_ = i;
						return;
					}
				}
				read_ptr_ = end_ptr_;
			}

		private:
			std::unique_ptr<char[]> data_;
			const char * read_ptr_{ nullptr };
			const char * end_ptr_{ nullptr };
			std::string str_;
		};

		struct data
		{
			std::unordered_map<std::string, nana::string> table;
		};

		static std::shared_ptr<data>& get_data_ptr()
		{
			static std::shared_ptr<data> data_ptr;
			return data_ptr;
		}

		void load(const std::string& file, bool utf8)
		{
			auto impl = std::make_shared<data>();

			tokenizer tknizer(file);
			while (true)
			{
				if (token::msgid != tknizer.read())
					break;
				if (token::string != tknizer.read())
					return;

				std::string msgid = std::move(tknizer.get_str());
				
				if (!utf8)
					msgid = nana::charset(std::move(msgid)).to_bytes(nana::unicode::utf8);

				if (token::msgstr != tknizer.read())
					return;
				if (token::string != tknizer.read())
					return;

				nana::string str;
				
				if (utf8)
					str = nana::charset(std::move(tknizer.get_str()), nana::unicode::utf8);
				else
					str = nana::charset(std::move(tknizer.get_str()));

				nana::string::size_type pos = 0;
				while (true)
				{
					pos = str.find('\\', pos);
					if (pos == str.npos)
						break;

					if (pos + 1 < str.size())
					{
						auto ch = str[pos + 1];
						switch (ch)
						{
						case '"': case '\\':
							str.erase(pos, 1);
							break;
						case 'n':
							str.erase(pos, 1);
							str[pos] = '\n';
							break;
						case 't':
							str.erase(pos, 1);
							str[pos] = '\t';
							break;
						}
						++pos;
					}
					else
						break;
				}
				impl->table[std::move(msgid)].swap(str);
			}

			get_data_ptr().swap(impl);
		}
	}//end namespace internationalization_parts


	void internationalization::load(const std::string& file)
	{
		internationalization_parts::load(file, false);
	}

	void internationalization::load_utf8(const std::string& file)
	{
		internationalization_parts::load(file, true);
	}

	nana::string internationalization::get(std::string msgid) const
	{
		nana::string str;
		if(_m_get(msgid, str))
			_m_replace_args(str, nullptr);
		return str;
	}

	void internationalization::set(std::string msgid, nana::string msgstr)
	{
		auto ptr = internationalization_parts::get_data_ptr();
		ptr->table[msgid].swap(msgstr);
	}

	bool internationalization::_m_get(std::string& msgid, nana::string& msgstr) const
	{
		auto impl = internationalization_parts::get_data_ptr();
		auto i = impl->table.find(msgid);
		if (i != impl->table.end())
		{
			msgstr = i->second;
			return true;
		}

		msgstr = nana::charset(std::move(msgid));
		return false;
	}

	void internationalization::_m_replace_args(nana::string& str, std::vector<nana::string> * arg_strs) const
	{
		nana::string::size_type offset = 0;
		while (true)
		{
			auto pos = str.find(L"%arg", offset);
			if (pos == str.npos)
				break;

			offset = pos;
			pos = str.find_first_not_of(L"0123456789", offset + 4);

			if ((pos == str.npos) || (pos != offset + 4))
			{
				nana::string::size_type erase_n = 0;
				nana::string::size_type arg_n = str.npos;
				if (pos != str.npos)
				{
					erase_n = pos - offset;
					arg_n = pos - offset - 4;
				}
				else
					erase_n = str.size() - offset;

				//If there is not a parameter for %argNNN, the %argNNN will be erased.
				auto arg = static_cast<std::size_t>(std::stoi(str.substr(offset + 4, arg_n)));
				if (arg_strs && arg < arg_strs->size())
					str.replace(offset, erase_n, (*arg_strs)[arg]);
				else
					str.erase(offset, erase_n);	
			}
			else
				offset += 4;
		}
	}
}
