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


        struct Gap_field                       
        { 
            virtual ~Gap_field (){}
            unsigned collocate_(const rectangle& r){}
            window window_handle_() const { return nullptr; }
        };
        struct Widget_field                       
        { 
            window   handle; 
            Widget_field(window handle_):handle(handle_){}
            virtual ~Widget_field (){}
            window window_handle_() const { return handle; }
             void  collocate_(const rectangle& r)
            {  
                API::move_window (handle,r );
            }
        };
        struct Room_field: Widget_field    
        { 
            unsigned rows,columns;
            Room_field(window handle_,unsigned rows_,unsigned columns_):Widget_field( handle_), rows(rows_),columns(columns_){}
        };


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

        template <class Base>
        struct IAdjust : IField
        {    
             void   collocate(const rectangle& r)override   {   Base::collocate_(r); attached=true;    }
             window window_handle() const override    { return Base::window_handle_(); }
             unsigned min, max; 
             IAdjust():min(std::numeric_limits <decltype(min)>::min() ),
                       max(std::numeric_limits <decltype(max)>::max() ){}
             IAdjust(unsigned min_,unsigned max_) :min(min_), max(max_){} 
             virtual ~IAdjust(){}
        };
        template <class Base>
        struct IAdjustable  :  IAdjust<Base>                      
        { 
            IAdjustable(         ){}
            IAdjustable(unsigned min_,unsigned max_):IAdjust<Base>  (min_,max_){}

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
        template <class Base>
        struct IFixed: IAdjust <Base>    
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
        template <class Base>
        struct IPercent: IAdjust <Base>    
        { 
            double      percent; 

            IPercent(unsigned percent_):percent(percent_/100.0){}
            IPercent(double   percent_):percent(percent_){}
            IPercent(unsigned percent_, unsigned min_,unsigned max_):IAdjust<Base>    (min_,max_),percent(percent_/100.0){}
            IPercent(double   percent_, unsigned min_,unsigned max_):IAdjust<Base>    (min_,max_),percent(percent_      ){}
        
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

        struct adj_gap:  IAdjustable<Gap_field> , Gap_field 
        { 
            adj_gap( )                                                   {}
            adj_gap( unsigned min_,unsigned max_):IAdjustable (min_,max_){}
        };
        struct adj_widget:    Widget_field ,  IAdjustable<Widget_field>
        { 
            adj_widget(window handle_)                             :Widget_field(handle_)                       {}
            adj_widget(window handle_, unsigned min_,unsigned max_):Widget_field(handle_),IAdjustable(min_,max_){}
        };
        struct adj_room: Room_field, IAdjustable<Room_field>   
        { 
            adj_room(window handle_,unsigned rows_,unsigned columns_)                             :Room_field(handle_, rows_, columns_){}
            adj_room(window handle_,unsigned rows_,unsigned columns_, unsigned min_,unsigned max_):Room_field(handle_, rows_, columns_),IAdjustable<Room_field>(min_,max_){}
        };


        struct fixed_gap:  Gap_field, IFixed<Gap_field> 
        { 
            fixed_gap(unsigned weight_):IFixed(weight_){}
            fixed_gap(unsigned weight_, unsigned min_,unsigned max_):IFixed(weight_,min_,max_){}
        };
        struct fixed_widget:    Widget_field ,  IFixed<Widget_field>
        { 
            fixed_widget(window handle_,unsigned weight_):Widget_field(handle_),IFixed(weight_){}
            fixed_widget(window handle_,unsigned weight_, unsigned min_,unsigned max_):Widget_field(handle_),IFixed(weight_,min_,max_){}
        };
        struct fixed_room: Room_field, IFixed<Room_field>   
        { 
            fixed_room(window handle_,unsigned rows_,unsigned columns_):Room_field(handle_, rows_, columns_),IFixed(weight_){}
            fixed_room(window handle_,unsigned rows_,unsigned columns_, unsigned min_,unsigned max_):Room_field(handle_, rows_, columns_),IFixed(weight_,min_,max_){}
        };

        struct percent_gap: Gap_field, IPercent<Gap_field> 
        { 
            percent_gap(unsigned percent_):IPercent<Gap_field>(percent_){}
            percent_gap(unsigned percent_, unsigned min_,unsigned max_):IPercent<Gap_field>(percent_,min_,max_){}
            percent_gap(double   percent_):IPercent<Gap_field>(percent_){}
            percent_gap(double   percent_, unsigned min_,unsigned max_):IPercent<Gap_field>(percent_,min_,max_){}
        };     
        struct percent_widget: Widget_field , IPercent<Widget_field>
        { 
            percent_widget(window handle_,unsigned percent_):Widget_field(handle_),IPercent<Widget_field>(percent_){}
            percent_widget(window handle_,unsigned percent_, unsigned min_,unsigned max_):Widget_field(handle_),IPercent<Widget_field>(percent_,min_,max_){}
            percent_widget(window handle_,double   percent_):Widget_field(handle_),IPercent<Widget_field>(percent_){}
            percent_widget(window handle_,double   percent_, unsigned min_,unsigned max_):Widget_field(handle_),IPercent<Widget_field>(percent_,min_,max_){}
        };   
        //struct percent_room:  Room_field, IPercent<Room_field>   
        //{ 
        //    percent_room(window handle_,unsigned rows_,unsigned columns_):Room_field(handle_, rows_, columns_),IPercent<Room_field>  (weight_){}
        //    percent_room(window handle_,unsigned rows_,unsigned columns_, unsigned min_,unsigned max_):Room_field(handle_, rows_, columns_),IPercent<Room_field>  (weight_,min_,max_){}
        //};     



		struct implement;

		class field_t
		{
		public:
			virtual ~field_t() = 0;
            virtual field_t& operator<<(IField & fld)		= 0;
			virtual field_t& operator<<(window wd)		= 0;    //
			virtual field_t& operator<<(unsigned gap)	= 0;
			virtual field_t& operator<<(const fixed_widget& f)	= 0;
			virtual field_t& operator<<(const percent_widget& p)	= 0;
			virtual field_t& operator<<(const adj_room& r)	= 0;
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
		void        bind     (window handle);
		void        div      (const char* s);
		field_t *   field(const char* name);
		void        collocate();

		static fixed_widget      fixed   (window wd, unsigned size       );
		static percent_widget    percent (window wd, double per          );
		static adj_room          room    (window wd, unsigned w, unsigned h);
	private:
		implement * impl_;
	};
}//end namespace gui
}//end namespace nana

#endif //#ifndef NANA_GUI_PLACE_HPP
