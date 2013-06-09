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
             virtual adj  pre_place (unsigned t_w, adj& prev=adj() )=0;
             virtual adj  end_place (unsigned t_w,const adj& tip, adj& prev=adj() )=0 ;
             virtual unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )=0 ;
             virtual void  collocate(const rectangle& r)=0 ;
             unsigned min, max; 
             IAdjust():min(std::numeric_limits <decltype  (min)>::min ()),
                       max(std::numeric_limits <decltype  (max)>::max ()){}
             IAdjust(unsigned min_,unsigned max_):min(min_), max(max_){} 
             virtual ~IAdjust(){}
        };
        struct IAdjustable  :  IAdjust                      
        { 
            IAdjustable(         ){}
            IAdjustable(unsigned min_,unsigned max_):IAdjust(min_,max_){}

            adj   pre_place(unsigned t_w,                        adj& prev = adj() ) override    {   ++prev.count_adj;   return  prev;        }
            adj   end_place(unsigned t_w,const adj& tip = adj(), adj& prev = adj() ) override    
            {   
                if ( tip.weigth/tip.count_adj < min )   {prev.weigth += min; return prev; }
                if ( tip.weigth/tip.count_adj > max )   {prev.weigth += max; return prev; }
                prev.weigth += min; 
                ++prev.count_adj;   return  prev;        
            }
            unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )override
            {   
                if ( tip.weigth/tip.count_adj < min )   {return min; }
                if ( tip.weigth/tip.count_adj > max )   {return max; }

                return  prev.weigth/tip.count_adj + min ;        
            }
        };
        struct IFixed: IAdjust   
        { 
            unsigned weight_; 

            IFixed(unsigned weight):weight_(weight){}
            IFixed(unsigned weight, unsigned min_,unsigned max_):IAdjust(min_,max_),weight_(weight){}


            adj   pre_place(unsigned t_w,                        adj& prev = adj() ) override    {   return end_place(t_w, prev);      }
            adj   end_place(unsigned t_w,const adj& tip = adj(), adj& prev = adj() ) override    {   prev.weigth += weigth_adj() ;   return  prev;        }    
            unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )  override              {   return weigth_adj();      }        
            unsigned weigth_adj()
            {   
                if ( weight_  < min )    {return min; }
                if ( weight_  > max )    {return max; }
                
                return  weight_;          
            }
        };
        struct IPercent: IAdjust 
        { 
            double      percent; 

            IPercent(unsigned percent_):percent(percent_/100.0){}
            IPercent(double   percent_):percent(percent_){}
            IPercent(unsigned percent_, unsigned min_,unsigned max_):IAdjust(min_,max_),percent(percent_/100.0){}
            IPercent(double   percent_, unsigned min_,unsigned max_):IAdjust(min_,max_),percent(percent_      ){}
        
            adj   pre_place(unsigned t_w,                        adj& prev = adj() ) override     {   return end_place(t_w, prev);      }
            adj   end_place(unsigned t_w,const adj& tip = adj(), adj& prev = adj() ) override     {   prev.weigth +=  weigth_adj( t_w ) ;   return  prev;              }
            unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )override                 {   return   weigth_adj( t_w ) ;     
            }
            unsigned weigth_adj(unsigned t_w )
            {   
                if ( t_w * percent / 100.0 < min )    {return min; }
                if ( t_w * percent / 100.0 > max )    {return max; }
                return  t_w * percent / 100.0 ;     
            }
        };     // double?, unsigned?

        struct Gap_field                       
        { 
            virtual ~Gap_field (){}
            unsigned collocate(const rectangle& r){}
        };
        struct Widget_field                       
        { 
            window   handle; 
            Widget_field(window handle_):handle(handle_){}
            virtual ~Widget_field (){}
            void collocate(const rectangle& r)
            {  
                API::move_window (handle,r );
            }
        };
        struct Room_field: Widget_field    
        { 
            unsigned rows,columns;
            Room_field(window handle_,unsigned rows_,unsigned columns_):Widget_field( handle_), rows(rows_),columns(columns_){}
        };

        struct fixed_gap: Gap_field, IFixed   
        { 
            fixed_gap(unsigned weight_):IFixed(weight_){}
            fixed_gap(unsigned weight_, unsigned min_,unsigned max_):IFixed(weight_,min_,max_){}
        };
        struct fixed_widget: Widget_field, IFixed   
        { 
            fixed_widget(window handle_,unsigned weight_):Widget_field(handle_),IFixed(weight_){}
            fixed_widget(window handle_,unsigned weight_, unsigned min_,unsigned max_):Widget_field(handle_),IFixed(weight_,min_,max_){}
        };
        struct fixed_room: Room_widget_field, IFixed   
        { 
            fixed_widget_field(window handle_,unsigned weight_):Widget_field(handle_),IFixed(weight_){}
            fixed_widget_field(window handle_,unsigned weight_, unsigned min_,unsigned max_):Widget_field(handle_),IFixed(weight_,min_,max_){}
        };

        struct percent_gap: Gap_field, IPercent 
        { 
            percent_gap(unsigned percent_):IPercent(percent_){}
            percent_gap(unsigned percent_, unsigned min_,unsigned max_):IPercent(percent_,min_,max_){}
            percent_gap(double   percent_):IPercent(percent_){}
            percent_gap(double   percent_, unsigned min_,unsigned max_):IPercent(percent_,min_,max_){}
        };     
        struct percent_widget: Widget_field , IPercent
        { 
            percent_widget(window handle_,unsigned percent_):Widget_field(handle_),IPercent(percent_){}
            percent_widget(window handle_,unsigned percent_, unsigned min_,unsigned max_):Widget_field(handle_),IPercent(percent_,min_,max_){}
            percent_widget(window handle_,double   percent_):Widget_field(handle_),IPercent(percent_){}
            percent_widget(window handle_,double   percent_, unsigned min_,unsigned max_):Widget_field(handle_),IPercent(percent_,min_,max_){}
        };   
        struct percent_room: base_widget_field_t 
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
