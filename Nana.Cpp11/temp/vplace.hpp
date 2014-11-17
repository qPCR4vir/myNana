/*
 *	A "virtual" Implementation of Place for Layout
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/place.cpp
 *
 *	@contributions:
 *	 by qPCR4vir.
 */


#ifndef NANA_GUI_vPLACE_HPP
#define NANA_GUI_vPLACE_HPP
#include <utility>
#include <nana/gui/basis.hpp>
#include <limits>
#include <nana\gui\programming_interface.hpp>
#include <string>


namespace nana 
{
	namespace vplace_impl
    {      ///  a field object created by place 
        struct adjustable ;
        struct implement;
    }
    using  vplace_impl::adjustable ;
    using  vplace_impl::implement ;

    ///  \briefLayout managment - an object of class place is attached to a widget, and it automatically positions and resizes the children widgets.
    /// The orignal place have only one (optional) name per field, making it almost the same as a division.
    /// But vplace allow multiple names in one field (except in grid by now). This can be not a good idea !?
    class vplace		: noncopyable
	{
	  public:
		vplace();  ///< First create the vplace.
             /// Optionally simultaneously attach it to a widget whose children you need to layout.
		vplace(window parent_widget); 
        window     window_handle  () const; ///< You can get the handle to that widget anytime.
             /// Divide (format) the attached widget into fields, some of then with a name.
		void         div      (const ::std::string & layout);     
             /// Select one of the named field to direct to it the next actions 
             /// You can think about the place as a kind of map of named fields you can modify
             /// You can use any name, but only the ones set in div() will actually shown.  
        void set_target_field (std::string name); 
        vplace&       field   (std::string name){set_target_field (std::move(name));return *this;};    /// \todo: Add min and max
        vplace&   operator[]  (std::string name){set_target_field (std::move(name));return *this;};    /// \todo: Add min and max
        
        struct minmax
        {
            static const unsigned MIN=0,    MAX=1000000;
            unsigned          min{MIN}, max{MAX} ;
            minmax(unsigned Min=MIN, unsigned Max=MAX);
            minmax   MinMax      (                 )  { return *this;};
            void     MinMax      (minmax Size_Range)  { *this = Size_Range;   };
            void     MinMax      (unsigned min_,unsigned max_=MAX) {min=min_; max=max_;} 
        };

        vplace&  operator<< (window        wd) ; ///< Add a widget to a current target field   
		vplace&  operator<< (unsigned     gap) ; ///< Add (once) a fixed gap betwen elements
        vplace&  operator<< (std::wstring txt) ; ///< Create on the fly a very simple label
        vplace&  operator<< (std::string  txt) ; ///< Create on the fly a very simple label
		vplace&  operator<< (minmax Size_range); ///< Set the default minimun and maximun for each of the next elements

        void field_visible(std::string name, bool visible); ///< Shows/Hides a named field.
        bool field_visible(std::string name     ) const;    ///< Return whether the specified field is visible.

        void field_display(std::string name, bool display); ///< Displays/Discards an existing field.
        bool field_display(std::string name     ) const;    ///< Return whether the specified field is displayed.

        vplace&  operator<< (adjustable  &fld) ; ///< Use the helper functions to add modified elements
		static adjustable&     fixed   (window wd                    , unsigned weight    );
		static adjustable&     fixed   (const std::wstring& txt      , unsigned weight    );
		static adjustable&     fixed   (const std::string&  txt      , unsigned weight    );
		static adjustable&     percent (window wd, double percent_weight , minmax MinMax=minmax()    );
        /// Use room (wd,nana::size(w,h)) in combination with a <Table grid=[W,H]>
		static adjustable&     room    (window wd, nana::size sz);/// \todo: Add min and max

        vplace&  fasten(window wd)	  ;
        void     erase (window handle);                ///< Erases a window from field.

         /// Attach this vplace to a widget
		 /// \remark It will throw an exception if the place has already binded to a window.
		void       bind           (window parent_widget);  
		void         collocate();                           ///< Layouts the widgets.

		~vplace();
	  private:
		implement * impl_;
	};
}//end namespace nana

#endif //#ifndef NANA_GUI_PLACE_HPP

