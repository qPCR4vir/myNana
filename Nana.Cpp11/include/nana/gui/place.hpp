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

		 place();
		 place(window parent_widget);
		~place();

		/** @brief Bind to a window
		 *	@param handle	A handle to a window which the place wants to attach.
		 *	@remark	It will throw an exception if the place has already binded to a window.
		 */
		void              bind     (window parent_widget);
		void              div      (const char* layout);
		void              collocate();

        struct adj{unsigned weigth; size_t count_adj; adj():weigth(0),count_adj(0){}  };
        struct IField
        {
             virtual adj  pre_place (unsigned t_w,      adj& prev=adj()                 )=0;
             virtual adj  end_place (unsigned t_w,const adj& tip,       adj& prev=adj() )=0 ;
             virtual unsigned weigth(unsigned t_w,const adj& tip, const adj& prev       )=0 ;
             virtual ~IField(){}
             virtual void  collocate(const rectangle& r)   =0;
             virtual window window_handle()const =0;
             bool attached;
        };
		struct field_t
		{
			virtual ~field_t() = 0;
            virtual field_t& operator<<(IField * fld)		= 0;
			virtual field_t& operator<<(window wd)		= 0;    //
			virtual field_t& operator<<(unsigned gap)	= 0;
			virtual field_t& fasten(window wd)	= 0;
		};
		typedef field_t & field_reference;
		field_reference    field   (const char* name);
		static IField*     fixed   (window wd, unsigned size         );
		static IField*     percent (window wd, double   percent_     );
		static IField*     room    (window wd, unsigned w, unsigned h);

		struct implement;
	  private:
		implement * impl_;
	};
}//end namespace gui
}//end namespace nana

#endif //#ifndef NANA_GUI_PLACE_HPP
			//virtual field_t& operator<<(const fixed_widget& f)	= 0;
			//virtual field_t& operator<<(const percent_widget& p)	= 0;
			//virtual field_t& operator<<(const adj_room& r)	= 0;
