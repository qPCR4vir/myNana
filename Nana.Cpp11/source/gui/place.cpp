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
#include <sstream>
#include <cfloat>
#include <cmath>
#include <set>
#include <unordered_map>
//#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <nana/gui/place.hpp>
#include <nana/gui/programming_interface.hpp>


namespace nana{	namespace gui
{
	class number_t
	{	//number_t is used to store a number type variable
	    //such as integer, real and percent. Essentially, percent is a typo of real.

	public:
		enum class kind{integer, real, percent};

		number_t() 			: kind_(kind::integer)		{			value_.integer = 0;		}

		kind kind_of() const		{			return kind_;		}

		int integer() const
		{
			if(kind::integer == kind_)
				return value_.integer;
			return static_cast<int>(value_.real);
		}

		double real() const
		{
			if(kind::integer == kind_)
				return value_.integer;
			return value_.real;
		}

		void assign(int i)
		{
			kind_ = kind::integer;
			value_.integer = i;
		}

		void assign(double d)
		{
			kind_ = kind::real;
			value_.real = d;
		}

		void assign_percent(double d)
		{
			kind_ = kind::percent;
			value_.real = d / 100;
		}
	private:
		kind kind_;
		union valueset
		{
			int integer;
			double real;
		}value_;
	};//end class number_t

	class tokenizer
	{
	public:
		enum class token
		{
			div_start, div_end,
			identifier, vertical, horizontal, grid, number, array,
			weight, gap, min, max,
			equal,
			eof, error
		};

		tokenizer(const char* p)
			: div_str(p), divstr_(div_str.c_str() ), sp_(div_str.c_str())
		{}

		const std::string& idstr() const
		{
			return idstr_;
		}

		number_t number() const
		{
			return number_;
		}

		std::vector<number_t>& array()
		{
			return array_;
		}

		token read()
		{
			sp_ = _m_eat_whitespace(sp_);

			std::size_t readbytes = 0;
			switch(*sp_)
			{
			case '\0':				return token::eof;
			case '=':				++sp_;
				return token::equal;
			case '<':				++sp_;
				return token::div_start;
			case '>':				++sp_;
				return token::div_end;
			case '[':
				array_.clear();
				sp_ = _m_eat_whitespace(sp_ + 1);
				if(*sp_ == ']')
				{
					++sp_;
					return token::array;
				}

				while(true)
				{
					sp_ = _m_eat_whitespace(sp_);
					if(token::number != read())
						_m_throw_error("invalid array element");

					array_.push_back(number_);

					sp_ = _m_eat_whitespace(sp_);
					char ch = *sp_++;

					if(ch == ']')
						break;

					if(ch != ',')
						_m_throw_error("invalid array");
				}
				return token::array;
			case '.': case '-':
				if(*sp_ == '-')
				{
					readbytes = _m_number(sp_ + 1, true);
					if(readbytes)
						++ readbytes;
				}
				else
					readbytes = _m_number(sp_, false);
						
				if(readbytes)
				{
					sp_ += readbytes;
					return token::number;
				}
				else
					_m_throw_error(*sp_);
				break;
			default:
				if('0' <= *sp_ && *sp_ <= '9')
				{
					readbytes = _m_number(sp_, false);
					if(readbytes)
					{
						sp_ += readbytes;
						return token::number;
					}
				}
				break;
			}

			if('_' == *sp_ || isalpha(*sp_))
			{
				const char * idstart = sp_++;

				while('_' == *sp_ || isalpha(*sp_) || isalnum(*sp_))
					++sp_;

				idstr_.assign(idstart, sp_);

				if(idstr_ == "weight")
				{
					_m_attr_number_value();
					return token::weight;
				}
				else if(idstr_ == "gap")
				{
					_m_attr_number_value();
					return token::gap;
				}
				else if(idstr_ == "min")
				{
					_m_attr_number_value();
					return token::min;
				}
				else if(idstr_ == "max")
				{
					_m_attr_number_value();
					return token::max;
				}
				else if(idstr_ == "vertical")
					return token::vertical;
				else if(idstr_ == "grid")
					return token::grid;

				return token::identifier;
			}

			return token::error;
		}
	private:
		void _m_throw_error(char err_char)
		{
			std::stringstream ss;
			ss<<"place: invalid character '"<<err_char<<"' at "<<static_cast<unsigned>(sp_ - divstr_);
			throw std::runtime_error(ss.str());
		}

		void _m_attr_number_value()
		{
			if(token::equal != read())
				_m_throw_error("an equal sign is required after \'"+ idstr_ +"\'");

			const char* p = sp_;
			for(; *p == ' '; ++p);

			std::size_t len = 0;
			if(*p == '-')
			{
				len = _m_number(p + 1, true);
				if(len)	++len;
			}
			else
				len = _m_number(p, false);

			if(0 == len)
				_m_throw_error("the \'" + idstr_ + "\' requires a number(integer or real or percent)");
						
			sp_ += len + (p - sp_);
		}

		void _m_throw_error(const std::string& err)
		{
			std::stringstream ss;
			ss<<"place: "<<err<<" at "<<static_cast<unsigned>(sp_ - divstr_);
			throw std::runtime_error(ss.str());
		}

		const char* _m_eat_whitespace(const char* sp)
		{
			while (*sp && !isgraph(*sp))
				++sp;
			return sp;
		}

		std::size_t _m_number(const char* sp, bool negative)
		{
			const char* allstart = sp;
			sp = _m_eat_whitespace(sp);

			number_.assign(0);

			bool gotcha = false;
			int integer = 0;
			double real = 0;
			//read the integral part.
			const char* istart = sp;
			while('0' <= *sp && *sp <= '9')
			{
				integer = integer * 10 + (*sp - '0');
				++sp;
			}
			const char* iend = sp;

			if('.' == *sp)
			{
				double div = 1;
				const char* rstart = ++sp;
				while('0' <= *sp && *sp <= '9')
				{
					real += (*sp - '0') / (div *= 10);
					++sp;
				}

				if(rstart != sp)
				{
					real += integer;
					number_.assign(negative ? -real : real);
					gotcha = true;
				}
			}
			else if(istart != iend)
			{
				number_.assign(negative ? - integer : integer);
				gotcha = true;
			}

			if(gotcha)
			{
				for(;*sp == ' '; ++sp);
				if('%' == *sp)
				{
					if(number_t::kind::integer == number_.kind_of())
						number_.assign_percent(number_.integer());
					return sp - allstart + 1;
				}
				return sp - allstart;
			}
			return 0;
		}
	private:
        std::string div_str;
		const char* divstr_;
		const char* sp_;
		std::string idstr_;
		number_t number_;
		std::vector<number_t> array_;
	};	//end class tokenizer

    struct adj{unsigned weigth; size_t count_adj; adj():weigth(0),count_adj(0){}  };
    struct place::IField
    {
            virtual adj  pre_place (unsigned t_w,      adj& prev=adj()                 )=0;
            virtual adj  end_place (unsigned t_w,const adj& tip,       adj& prev=adj() )=0 ;
            virtual unsigned weigth(unsigned t_w,const adj& tip, const adj& prev       )=0 ;
            virtual ~IField(){}
            virtual void  collocate(const rectangle& r)   =0;
            virtual window window_handle()const =0;
            bool attached;
    };

	struct place::implement           //struct implement
	{
		class division;

		window                          parent_window_handle;
		event_handle                    event_size_handle;
		std::unique_ptr<division>       root_division;
        std::set<std::string>           names;
		std::unordered_map<std::string, std::unique_ptr<IField>> fields;
		std::unordered_map<std::string,    window              > fastened;
			
		implement()		: parent_window_handle(nullptr), event_size_handle(nullptr)	{}
		~implement() 	{	   API::umake_event(event_size_handle);	    }

        void              collocate();

		division *        scan_div       (tokenizer&);
		//static division * search_div_name(division* start, const std::string&);

        class field_impl;
		class div_h;
		class div_v;
		class div_grid;

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
	};	      //struct implement

	place::field_t::~field_t(){}

	class place::implement::field_impl 		:	public place::field_t
	{
	public:
		std::string name;
	private:
		place * place_ptr_;
	 public:
		typedef std::vector<std::unique_ptr<IField>>::const_iterator const_iterator;

		field_impl(place * p, const std::string& name_)	:	place_ptr_(p),   name(name_)		{}

	  public:
        IField * create_field(window    wd                                      ){IField *p= new adj_widget     (wd)                          ;}
        IField * create_field(unsigned gap                                      ){IField *p= new fixed_gap      (gap)                         ;}
        IField * create_field(window handle_,unsigned weight_                   ){IField *p= new fixed_widget   ( handle_, weight_)           ;}
        IField * create_field(window handle_,double   percent_                  ){IField *p= new percent_widget ( handle_, percent_)          ;}
        IField * create_field(window handle_,unsigned rows_,unsigned columns_   ){IField *p= new adj_room       ( handle_, rows_, columns_ )  ;}
	 private:
        field_t& operator<<(IField * fld) override
		{
			place_ptr_->impl_->fields.emplace(name,fld);
			_m_make_destroy(fld->window_handle());
			return *this;
		}

        field_t& operator<<(window wd) override
		{
			place_ptr_->impl_->fields.emplace(name,create_field(wd));
			_m_make_destroy(wd);
			return *this;
		}
		field_t& operator<<(unsigned gap) override
		{
			place_ptr_->impl_->fields.emplace(name,create_field(gap));
			return *this;
		}
		field_t& operator<<(const fixed_widget& fx) 
		{
			place_ptr_->impl_->fields.emplace(name,new fixed_widget(fx));
			_m_make_destroy(fx.window_handle());
			return *this;
		}
    	field_t& operator<<(const percent_widget& pcnt) 
		{
			place_ptr_->impl_->fields.emplace(name,new percent_widget(pcnt));
			_m_make_destroy(pcnt.window_handle());
			return *this;
		}
		field_t& operator<<(const adj_room& r) 
		{
			adj_room x = r;
			if(x.rows == 0)
				x.rows = 1;
			if(x.columns == 0)
				x.columns = 1;
			place_ptr_->impl_->fields.emplace(name,new adj_room(x));
			_m_make_destroy(r.window_handle());
			return *this;
		}
		field_t& fasten(window wd) override
		{
			place_ptr_->impl_->fastened.emplace (name , wd); 

			//Listen to destroy of a window. The deleting a fastened window
			//does not change the layout.
			API::make_event<events::destroy>(wd, [this](const eventinfo& ei)
			{
				for (auto f=place_ptr_->impl_->fastened.begin(); f!=place_ptr_->impl_->fastened.end(); ++f)
                    if (f->second ==  ei.window )
                    {
                        place_ptr_->impl_->fastened.erase(f);    // delete ???
                        break;
                    }
			});	
			return *this;
		}
		//std::vector<std::unique_ptr<IField>> elements;
		//std::vector<window>	                 fastened;
		//Listen to destroy of a window
		//It will delete the element and recollocate when the window destroyed.
		void _m_make_destroy(window wd)
		{
			API::make_event<events::destroy>(wd, [this](const eventinfo& ei)
			{
				for (auto f=place_ptr_->impl_->fields.begin(); f!=place_ptr_->impl_->fields.end(); ++f)
                    if (f->second->window_handle() ==  ei.window )
                    {
                        place_ptr_->impl_->fields.erase(f);    // delete ???
				        place_ptr_->collocate();
                        break;
                    }
			});
		}

	};//end class field_impl

	class place::implement::division
	{
	public:
		division(place*plc, const std::string& name_)	 :place_ptr_(p)		{field_names.push_back(name_);}
        virtual int&      weigth_c(rectangle& r )=0;
        virtual unsigned& weigth_s(rectangle& r )=0;
        virtual int&       fixed_c(rectangle& r )=0;
        virtual unsigned&  fixed_s(rectangle& r )=0;

        virtual ~division()
		{
			//detach the field
			if(field)
				field->attached = false;

			for(auto p : children)
			{
				delete p;
			}
		}
        void populate_children()
        {
            children.clear();
            for (const auto &name : field_names)
            {
                auto r= place_ptr_->impl_->fields.equal_range(name);
                for (auto fi=r.first ; fi != r.second ; ++fi)
                    children.push_back (fi->second.get () );
            }
        }

        void add_field_name(const std::string& n){field_names.push_back(n);}
		virtual void collocate(const rectangle& r) 
		{   
			rectangle area (r);	
            adj pre_adj, end_adj;
			for(auto child: children)                            
                child->pre_place(  weigth_s(area) , pre_adj );  
            for(auto &f: field->elements)                            
                    f->pre_place(  weigth_s(area) , pre_adj );

			for(auto child: children)                            
                child->end_place(  weigth_s(area) , pre_adj, end_adj );
            for(auto &f: field->elements)                            
                    f->end_place(  weigth_s(area) , pre_adj, end_adj );
       
			rectangle left = area;
			for(auto child : children)                          /// First collocate child div's !!!
			{
			    rectangle child_area (left);
                weigth_s(child_area) = child->weigth(  weigth_s(area) , pre_adj, end_adj )   ;
                weigth_c(left)      += weigth_s(child_area);
                weigth_s(left)      -= weigth_s(child_area);
                child->collocate(child_area);
			}
            for(auto &f: field->elements)                            
			{
			    rectangle child_area (left);
                weigth_s(child_area) = f->weigth(  weigth_s(area) , pre_adj, end_adj )   ;
                weigth_c(left)      += weigth_s(child_area);
                weigth_s(left)      -= weigth_s(child_area);
                f->collocate(child_area);
			}

				for(auto & fsn: field->fastened)
				{
					API::move_window(fsn, area.x, area.y, area.width, area.height);
				}
		}

	public:
		place* place_ptr_;
        std::vector <std::string> field_names;
		std::vector<IField*>      children;   //  std::vector<div*> 
		IField*                 gap;        //  
		//nana::rectangle         area;
		//number_t                weight;
		//field_impl *            field;
	};
    
	class place::implement::div_h 	: public division
	{
	  public:
         int&      weigth_c(rectangle& r )override{return r.x;}
         unsigned& weigth_s(rectangle& r )override{return r.width;}
         int&       fixed_c(rectangle& r )override{return r.y;}
         unsigned&  fixed_s(rectangle& r )override{return r.height;}
        
        div_h(std::string&& name)	: division(std::move(name))		{}
	};
	class place::implement::div_v 	: public division
	{
	  public:
         int&      weigth_c(rectangle& r )override{return r.y;}
         unsigned& weigth_s(rectangle& r )override{return r.height;}
         int&       fixed_c(rectangle& r )override{return r.x;}
         unsigned&  fixed_s(rectangle& r )override{return r.width;}
        
		div_v (std::string&& name) 	: division( std::move(name))		{}
	};
	class place::implement::div_grid	: public division
	{
	  public:
		div_grid(std::string&& name)
			: division(kind::grid, std::move(name))
		{
			dimension.first = dimension.second = 0;
		}

		virtual void collocate()
		{
			if(nullptr == field)
				return;

			unsigned gap_size = (gap.kind_of() == number_t::kind::percent ?
				static_cast<unsigned>(area.width * gap.real()) : static_cast<unsigned>(gap.integer()));

			if(dimension.first <= 1 && dimension.second <= 1)
			{
				std::size_t n_of_wd = _m_number_of_window();

				std::size_t edge;
				switch(n_of_wd)
				{
				case 0:
				case 1:
					edge = 1;	break;
				case 2: case 3: case 4:
					edge = 2;	break;
				default:
					edge = static_cast<std::size_t>(std::sqrt(n_of_wd));
					if((edge * edge) < n_of_wd) ++edge;
				}

				bool exit_for = false;
				double y = area.y;
				double block_w = area.width / double(edge);
				double block_h = area.height / double((n_of_wd / edge) + (n_of_wd % edge ? 1 : 0));
				unsigned uns_block_w = static_cast<unsigned>(block_w);
				unsigned uns_block_h = static_cast<unsigned>(block_h);
				unsigned height = (uns_block_h > gap_size ? uns_block_h - gap_size : uns_block_h);

				auto i = field->elements.cbegin(), end = field->elements.cend();

				for(std::size_t u = 0; u < edge; ++u)
				{
					double x = area.x;
					for(std::size_t v = 0; v < edge; ++v)
					{
						i = _m_search(i, end);
						if(i == end)
						{
							exit_for = true;
							break;
						}

						window wd = nullptr;
						unsigned value = 0;
						typedef field_impl::element_t::kind ekind;
						switch(i->kind_of_element)
						{
						case ekind::fixed:
							wd = i->u.fixed_ptr->first;
							value = i->u.fixed_ptr->second;
							break;
						case ekind::percent:
							wd = i->u.percent_ptr->first;
							value = i->u.percent_ptr->second * area.width / 100;
							break;
						case ekind::window:
							wd = i->u.handle;
							value = static_cast<unsigned>(block_w);
							break;
						default:	break;
						}
						++i;

						unsigned width = (value > uns_block_w ? uns_block_w : value);
						if(width > gap_size)	width -= gap_size;
						API::move_window(wd, static_cast<int>(x), static_cast<int>(y), width, height);
						x += block_w;
					}
					if(exit_for) break;
					y += block_h;
				}
			}
			else
			{
				double block_w = area.width / double(dimension.first);
				double block_h = area.height / double(dimension.second);

				std::unique_ptr<char[]> table_ptr(new char[dimension.first * dimension.second]);

				char *table = table_ptr.get();
				std::memset(table, 0, dimension.first * dimension.second);

				std::size_t lbp = 0;
				bool exit_for = false;

				auto i = field->elements.cbegin(), end = field->elements.cend();

				for(std::size_t c = 0; c < dimension.second; ++c)
				{
					for(std::size_t l = 0; l < dimension.first; ++l)
					{
						if(table[l + lbp])
							continue;
							
						i = _m_search(i, end);
						if(i == end)
						{
							exit_for = true;
							break;
						}

						typedef field_impl::element_t::kind ekind;
						std::pair<unsigned, unsigned> room(1, 1);

						if(i->kind_of_element == ekind::room)
						{
							room = i->u.room_ptr->second;
							if(room.first > dimension.first - l)
								room.first = dimension.first - l;
							if(room.second > dimension.second - c)
								room.second = dimension.second - c;
						}

						window wd = nullptr;
						switch(i->kind_of_element)
						{
						case ekind::fixed:
							wd = i->u.fixed_ptr->first;
							break;
						case ekind::percent:
							wd = i->u.percent_ptr->first;
							break;
						case ekind::window:
							wd = i->u.handle;
							break;
						default:	break;
						}

						int pos_x = area.x + static_cast<int>(l * block_w);
						int pos_y = area.y + static_cast<int>(c * block_h);
						if(room.first <= 1 && room.second <= 1)
						{
							unsigned width = static_cast<unsigned>(block_w), height = static_cast<unsigned>(block_h);
							if(width > gap_size)	width -= gap_size;
							if(height > gap_size)	height -= gap_size;

							API::move_window(wd, pos_x, pos_y, width, height);
							table[l + lbp] = 1;
						}
						else
						{
							unsigned width = static_cast<unsigned>(block_w * room.first), height = static_cast<unsigned>(block_h * room.second);
							if(width > gap_size)	width -= gap_size;
							if(height > gap_size)	height -= gap_size;

							API::move_window(i->u.room_ptr->first, pos_x, pos_y, width, height);

							for(std::size_t y = 0; y < room.second; ++y)
								for(std::size_t x = 0; x < room.first; ++x)
								{
									table[l + x + lbp + y * dimension.first] = 1;
								}
						}
						++i;
					}

					if(exit_for)
						break;

					lbp += dimension.first;
				}
			}

			for(auto & fsn: field->fastened)
			{
				API::move_window(fsn, area.x, area.y, area.width, area.height);
			}
		}
	  private:
		static field_impl::const_iterator _m_search(field_impl::const_iterator i, field_impl::const_iterator end) /// Skyp gaps
		{
			if(i == end) return end;
				
			while(i->kind_of_element == field_impl::element_t::kind::gap)
			{
				if(++i == end) return end;
			}
			return i;
		}

		std::size_t _m_number_of_window() const
		{
			if(nullptr == field) return 0;

			std::size_t n = 0;
			for(auto & el : field->elements)
			{
				if(field_impl::element_t::kind::gap != el.kind_of_element)
					++n;
			}
			return n;
		}
	public:
		std::pair<unsigned, unsigned> dimension;
	};//end class div_grid

	//search_div_name
	//search a division with the specified name.
	//place::implement::division * place::implement::search_div_name(division* start, const std::string& name)
	//{
	//	if(nullptr == start) return nullptr;
//
	//	if(start->name == name) return start;
//
	//	for(auto child : start->children)
	//	{
	//		division * div = search_div_name(child, name);
	//		if(div)
	//			return div;
	//	}
	//	return nullptr;	
	//}

	place::implement::division* place::implement::scan_div(tokenizer& tknizer)
	{
		typedef tokenizer::token token;

		division * div = nullptr;
		token div_type = token::eof;
		std::string name;

		number_t weight;
		number_t gap;
		std::vector<number_t> array;

		std::vector<division*> children;
        for(token tk = tknizer.read(); tk != token::eof && tk!=token::div_end ; tk = tknizer.read())
		{
			bool exit_for = false;
			switch(tk)
			{
			case token::div_start:	    children.push_back(scan_div(tknizer));				break;
			case token::array:		    tknizer.array().swap(array);   				        break;
			case token::identifier:		name = tknizer.idstr();				break;

			case token::horizontal:
			case token::vertical:
			case token::grid:			div_type = tk;		   		        break;

			case token::weight:		weight = tknizer.number();
				//If the weight is type of real, convert it to integer.
				//the integer and percent are allowed for weight.
				if(weight.kind_of() == number_t::kind::real)
					weight.assign(static_cast<int>(weight.real()));
				break;
			case token::gap:		gap = tknizer.number();
				//If the gap is type of real, convert it to integer.
				//the integer and percent are allowed for gap.
				if(gap.kind_of() == number_t::kind::real)
					gap.assign(static_cast<int>(gap.real()));
				break;

			default:	break;
			}
			if(exit_for)
				break;
		}

		field_impl * field = nullptr;
		if(name.size())
		{
			//find the field with specified name.
			//the field may not be created.
			auto i = fields.find(name);
			if(fields.end() != i)
			{
				field = i->second;

				//the field is attached to a division, it means there is another division with same name.
				if(field->attached)
					throw std::runtime_error("place, the name \'"+ name +"\' is redefined.");

				//this field will be attached to the division that will be created later.
				field->attached = true;
			}
		}

		switch(div_type)
		{
		    case token::eof:
		    case token::horizontal:			div = new div_h(std::move(name));			break;
		    case token::vertical:			div = new div_v(std::move(name));			break;
		    case token::grid:
			    {
				    div_grid * p = new div_grid(std::move(name));
					
				    if(array.size())
				    {
					    if(array[0].kind_of() != number_t::kind::percent)
						    p->dimension.first = array[0].integer();
				    }
					
				    if(array.size() > 1)
				    {
					    if(array[1].kind_of() != number_t::kind::percent)
						    p->dimension.second = array[1].integer();
				    }

				    if(0 == p->dimension.first)
					    p->dimension.first = 1;

				    if(0 == p->dimension.second)
					    p->dimension.second = 1;

				    div = p;
			    }
			    break;
            default:
                throw std::runtime_error("nana.place: invalid division type.");
		}
			
		div->weight = weight;
		div->gap = gap;
		div->field = field;		//attach the field to the division
		div->children.swap(children);
		return div;
	}

	//class place


		place::place()
			: impl_(new implement)
		{}

		place::place(window wd)
			: impl_(new implement)
		{
			bind(wd);
		}

		place::~place()
		{
			delete impl_;
		}

		void place::bind(window wd)
		{
			if(impl_->parent_window_handle)
				throw std::runtime_error("place.bind: it has already binded to a window.");

			impl_->parent_window_handle = wd;
			impl_->event_size_handle = API::make_event<events::size>(wd, [this](const eventinfo&ei)
				{
					if(impl_->root_division)
						impl_->root_division->collocate(API::window_size(ei.window));
				});
		}

		void place::div(const char* s)
		{
			delete impl_->root_division;
			impl_->root_division = nullptr;

			tokenizer tknizer(s);
			impl_->root_division = impl_->scan_div(tknizer);
		}

		place::IField*  place::fixed(window wd, unsigned size)
		{
			return new implement::fixed_widget(wd, size);
		}
		place::IField*  place::percent(window wd, double per)
		{
			return new implement::percent_widget(wd, per);
		}
		place::IField*  place::room(window wd, unsigned r, unsigned c)
		{
			return new implement::adj_room(wd, r, c);
		}

		place::field_reference place::field(const char* name)
		{
			name = name ? name : "";
            return  implement::field_impl(this,name);
		}

		void place::collocate()
		{
			impl_->collocate();
		}
		void place::implement::collocate()
		{
			if(root_division && parent_window_handle)
			{
				for(auto & field : fields)
				    field.second->attached = false;
 
                root_division->collocate(API::window_size(parent_window_handle));

				for(auto & field : fields)
					API::show_window(field.second->window_handle(), field.second->attached);
			}
		}


	//end class place

}//end namespace gui
}//end namespace nana

					//for(auto & el : field.second->elements)
						//is_show = (nullptr != implement::search_div_name(impl_->root_division, field.first));
					//bool is_show = field.second->attached;
