/*
 *	A "virtual" Implementation of Place for Layout
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/place.cpp
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
    {
        struct adjustable ;
        struct implement;
    }
    using  vplace_impl::adjustable ;
    using  vplace_impl::implement ;

    class vplace		: noncopyable
	{
	  public:
        struct minmax
        {
            minmax(unsigned Min=MIN, unsigned Max=MAX);
            unsigned min,max /*, Min(), Max()*/;
            void     MinMax      (minmax Size_Range) 
                {
                    min=Size_Range.min; 
                    max=Size_Range.max;
                };
            void     MinMax      (unsigned min_,unsigned max_=MAX) {min=min_; max=max_;} 
            minmax   MinMax      (                 )  { return *this;};
            static const unsigned MIN=0,MAX=1000000;
        };

        vplace&  operator<< (window        wd) ;    
		vplace&  operator<< (unsigned     gap) ;
        vplace&  operator<< (std::wstring txt) ;
        vplace&  operator<< (std::string  txt) ;
        vplace&  operator<< (adjustable  &fld) ;
		vplace&  operator<< (minmax Size_range) ;

        vplace&  fasten(window wd)	 ;

		 vplace(window parent_widget);
		 vplace();

         /** @brief Bind to a window
		 *	@param handle	A handle to a window which the place wants to attach.
		 *	@remark	It will throw an exception if the place has already binded to a window.
		 */
		void               bind     (window parent_widget);
		~vplace();

		void               div      (const ::std::string & layout);
		void               collocate();
        vplace&   operator[]  (std::string name){set_target_field (std::move(name));return *this;};    /// \todo: Add min and max
        vplace&       field   (std::string name){set_target_field (std::move(name));return *this;};    /// \todo: Add min and max
        void set_target_field (std::string name); 
        
        void field_visible(std::string name, bool visible); ///<Shows/Hides an existing field.
        bool field_visible(std::string name     ) const;    ///<Determines whether the specified field is visible.

        void field_display(std::string name, bool display); ///<Displays/Discards an existing field.
        bool field_display(std::string name     ) const;    ///<Determines whether the specified field is displayed.




		static adjustable&     fixed   (window wd                    , unsigned size    );
		static adjustable&     fixed   (const std::wstring& txt      , unsigned size    );
		static adjustable&     fixed   (const std::string&  txt      , unsigned size    );
		static adjustable&     percent (window wd, double   percent_ , minmax MinMax=minmax()    );
        /// Use room (wd,w,h) in combination with a <Table grid=[W,H]>
		static adjustable&     room    (window wd, nana::size sz);/// \todo: Add min and max

	  private:
		implement * impl_;
        //void Set_def_field  (const ::std::string & name);   
	};
}//end namespace nana

#endif //#ifndef NANA_GUI_PLACE_HPP

