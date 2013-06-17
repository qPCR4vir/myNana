/*
 *	An Implementation of Place for Layout
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/place.cpp
 */

#ifndef NANA_GUI_PLACE_HPP
#define NANA_GUI_PLACE_HPP
#include <utility>
#include <nana/gui/basis.hpp>
#include <limits>
#include <nana\gui\programming_interface.hpp>

namespace nana {namespace gui
{
	class place		: nana::noncopyable
	{
	  public:

		 //place();
		 place(window parent_widget);
		~place();

		/** @brief Bind to a window
		 *	@param handle	A handle to a window which the place wants to attach.
		 *	@remark	It will throw an exception if the place has already binded to a window.
		 */
		//void              bind     (window parent_widget);
		void              div      (const char* layout);
		void              collocate();

        struct IField ;
		struct field_t
		{
			virtual ~field_t() = 0;
            virtual field_t& operator<<(IField * fld)		= 0;
			virtual field_t& operator<<(window wd)		= 0;    //
			virtual field_t& operator<<(unsigned gap)	= 0;
			virtual field_t& fasten(window wd)	= 0;
		};
		typedef field_t & field_reference;
		field_reference    field   (const char* name);    /// TODO: Add min and max

		static IField*     fixed   (window wd, unsigned size         );/// TODO: Add min and max
		static IField*     percent (window wd, double   percent_     );/// TODO: Add min and max
		static IField*     room    (window wd, unsigned w, unsigned h);/// TODO: Add min and max

		struct implement;
	  private:
		implement * impl_;
	};
}//end namespace gui
}//end namespace nana

#endif //#ifndef NANA_GUI_PLACE_HPP
