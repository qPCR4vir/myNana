#ifndef NANA_CHARSET_HPP
#define NANA_CHARSET_HPP
#include <string>

namespace nana
{
	enum class unicode
	{
		utf8, utf16, utf32
	};

	namespace detail
	{
		class charset_encoding_interface;
	}
	/// An intelligent charset class for character code conversion.
	class charset
	{
	public:
		charset(const charset&);
		charset & operator=(const charset&);
		charset(charset&&);
		charset & operator=(charset&&);

		charset(const std::string&);         ///<Attempt to convert a multibytes string.
		charset(std::string&&);              ///<Attempt to convert a multibytes string.
		charset(const std::string&, unicode);///<Attempt to convert a unicode string in byte sequence.
		charset(std::string&&, unicode);     ///<Attempt to convert a unicode string in byte sequence.
		charset(const std::wstring&);        ///<Attempt to convert a UCS2/UCS4 string.
		charset(std::wstring&&);             ///<Attempt to convert a UCS2/UCS4 string.
		~charset();
		operator std::string() const;        ///<Converts the string to multibytes string.
		operator std::string&&();            ///<Converts the string to multibytes string.
		operator std::wstring() const;       ///<Converts the string to UCS2/UCS4 string.
		operator std::wstring&&();           ///<Converts the string to UCS2/UCS4 string.
		std::string to_bytes(unicode) const; ///<Converts the string to a unicode in bytes sequenece width a specified unicode transformation format.
	private:
		detail::charset_encoding_interface* impl_;
	};

}//end namespace nana
#endif
