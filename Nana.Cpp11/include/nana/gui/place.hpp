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

namespace nana
{
namespace gui
{
	class place
		: nana::noncopyable
	{
		typedef std::pair<window, unsigned>	                        fixed_t; 
		typedef std::pair<window, int>	                            percent_t;
		typedef std::pair<window, std::pair<unsigned, unsigned> >   room_t;

        struct adj{unsigned weigth; size_t count_adj; adj():weigth(0),count_adj(0){}  };
        struct IAdjust
        {    
             virtual adj  pre_place (unsigned t_w, adj& prev=adj() )=0 ;
             virtual adj  end_place (unsigned t_w,const adj& tip, adj& prev=adj() )=0 ;
             virtual unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )=0 ;

             //virtual unsigned min   (unsigned t_w,const adj& tip)=0;    
             //virtual unsigned max   (unsigned t_w,const adj& tip)=0;    
             //virtual bool adjustable(unsigned t_w,const adj& tip)=0;
        };
        struct base_widget_field_t  :  IAdjust                      
        { 
            window   handle; 
            unsigned min_,max_; 
            base_widget_field_t(         ):handle(nullptr), min_(0),max_(0){}
            base_widget_field_t(window wd):handle(wd     ), min_(0),max_(0){}

            adj   pre_place(unsigned t_w,                        adj& prev = adj() ) override    {   ++prev.count_adj;   return  prev;        }
            adj   end_place(unsigned t_w,const adj& tip = adj(), adj& prev = adj() ) override    
            {   
                if ( tip.weigth/tip.count_adj < min_ )   {prev.weigth += min_; return prev; }
                if ( tip.weigth/tip.count_adj > max_ )   {prev.weigth += max_; return prev; }
                prev.weigth += min_; 
                ++prev.count_adj;   return  prev;        
            }
            unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )override
            {   
                if ( tip.weigth/tip.count_adj < min_ )   {return min_; }
                if ( tip.weigth/tip.count_adj > max_ )   {return max_; }

                return  prev.weigth/tip.count_adj + min_ ;        
            }
            //unsigned min   (unsigned t_w,const adj& tip)override{return min_;} 
            //unsigned max   (unsigned t_w,const adj& tip)override{return max_;} 
            // virtual bool adjustable(unsigned t_w,const adj& tip)=0;
        };

        struct fixed_widget_field_t: base_widget_field_t   
        { 
            unsigned weight_; 

            adj   pre_place(unsigned t_w,                        adj& prev = adj() ) override    {   return end_place(t_w, prev);      }
            adj   end_place(unsigned t_w,const adj& tip = adj(), adj& prev = adj() ) override    {   prev.weigth += weigth_adj() ;   return  prev;        }    
            unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )  override              {   return weigth_adj();      }        
            unsigned weigth_adj()
            {   
                if ( weight_  < min_ )    {return min_; }
                if ( weight_  > max_ )    {return max_; }
                
                return  weight_;          
            }
        };

        struct percent_widget_field_t: base_widget_field_t 
        { 
            int      percent; 
        
            adj   pre_place(unsigned t_w,                        adj& prev = adj() ) override     {   return end_place(t_w, prev);      }
            adj   end_place(unsigned t_w,const adj& tip = adj(), adj& prev = adj() ) override     {   prev.weigth +=  weigth_adj( t_w ) ;   return  prev;              }
            unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )override                 {   return   weigth_adj( t_w ) ;     
            }
            unsigned weigth_adj(unsigned t_w )
            {   
                if ( t_w * percent / 100.0 < min_ )    {return min_; }
                if ( t_w * percent / 100.0 > max_ )    {return max_; }
                return  t_w * percent / 100.0 ;     
            }
        };     // double?, unsigned?

        struct room_widget_field_t: base_widget_field_t    { unsigned rows,columns; };


		struct implement;

		class field_t
		{
		public:
			virtual ~field_t() = 0;
			virtual field_t& operator<<(window wd)		= 0;    // virtual field_t& operator<<(base_widget_field_t& wd)		= 0;
			virtual field_t& operator<<(unsigned gap)	= 0;
			virtual field_t& operator<<(const fixed_t& f)	= 0;
			virtual field_t& operator<<(const percent_t& p)	= 0;
			virtual field_t& operator<<(const room_t& r)	= 0;
			virtual field_t& fasten(window wd)	= 0;
		};
	public:
		typedef field_t & field_reference;

		place();
		place(window);
		~place();

		/** @brief Bind to a window
		 *	@param handle	A handle to a window which the place wants to attach.
		 *	@remark	It will throw an exception if the place has already binded to a window.
		 */
		void        bind    (window handle);
		void        div     (const char* s);
		field_reference field(const char* name);
		void        collocate();

		static fixed_t      fixed   (window wd, unsigned size       );
		static percent_t    percent (window wd, int per             );
		static room_t       room    (window wd, unsigned w, unsigned h);
	private:
		implement * impl_;
	};
}//end namespace gui
}//end namespace nana

#endif //#ifndef NANA_GUI_PLACE_HPP
