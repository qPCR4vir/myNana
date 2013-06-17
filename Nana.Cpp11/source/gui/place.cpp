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
#include <unordered_set>
#include <cassert>
#include <unordered_map>
//#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <nana/gui/place.hpp>
#include <nana/gui/programming_interface.hpp>
#include <iostream>    // temp, for debugging

std::ostream& operator<<(std::ostream& o,const nana::rectangle &r)
{ o<<" rect("<<r.x<<","<<r.y<<","<<r.width <<","<<r.height <<")\n"; return o;}
namespace nana{	namespace gui
{
	class number_t
	{	//number_t is used to store a number type variable
	    //such as integer, real and percent. Essentially, percent is a typo of real.
	public:
		enum class kind{/*empty, */integer, real, percent};
		number_t() 			: kind_(kind::integer)		{	value_.integer = 0;		}

		kind kind_of() const		{			return kind_;		}

		int integer() const
		{
    //        if( kind::empty  == kind_ )
				//return 0;
			
            if(kind::integer == kind_ )
				return value_.integer;
			return static_cast<int>(value_.real);
		}
		double real() const
		{
    //        if( kind::empty  == kind_ )
				//return 0;
			if(kind::integer == kind_  )
				return value_.integer;
			return value_.real;
		}
		void assign(int i)           { 	kind_ = kind::integer;  value_.integer = i; }
		void assign(double d)        { 	kind_ = kind::real; 	value_.real    = d; }
		void assign_percent(double d) { kind_ = kind::percent; value_.real = d / 100; }
        void clear(){*this = number_t(); 	}
    private:
		kind kind_;
		union valueset 	{ int integer; double real; }value_;
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

		tokenizer(const char* p)	: div_str(p), divstr_(div_str.c_str() ), sp_(div_str.c_str())	{}

		const std::string& idstr() const		{			return idstr_;		}
		number_t           number() const		{			return number_;		}
		std::vector<number_t>& array()		{			return array_;		}
		token read()
		{
			sp_ = _m_eat_whitespace(sp_);

			std::size_t readbytes = 0;
			switch(*sp_)
			{
			case '\0':				                    return token::eof;
			case '=':		        ++sp_;				return token::equal;
			case '<':				++sp_;				return token::div_start;
			case '>':				++sp_;				return token::div_end;
			case '[':
				array_.clear();
				sp_ = _m_eat_whitespace(sp_ + 1);
				if(*sp_ == ']')		{	++sp_;		return token::array;	}

				while(true)
				{
					sp_ = _m_eat_whitespace(sp_);
					if(token::number != read())
						_m_throw_error("invalid array element");

					array_.push_back(number_);

					sp_ = _m_eat_whitespace(sp_);
					char ch = *sp_++;

					if(ch == ']')						break;

					if(ch != ',')						_m_throw_error("invalid array");
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
				while(isdigit (*sp))
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

    struct adj{unsigned weigth; unsigned count_adj; adj():weigth(0),count_adj(0){}  };
    struct place::IField
    {
            bool            attached;
            virtual adj         pre_place   (unsigned t_w,      adj& prev=adj()                 )=0;
            virtual adj         end_place   (unsigned t_w,const adj& tip,       adj& prev=adj() )=0 ;
            virtual unsigned    weigth      (unsigned t_w,const adj& tip, const adj& prev       )=0 ;
            virtual            ~IField      (){}
            virtual size        dimension   ()   const                              =0;
            virtual void        collocate   (const rectangle& r)   =0;
            virtual window      window_handle()const   =0;
            virtual void        populate_children(	implement*   place_impl_)  =0;
    };

	struct place::implement           //struct implement
	{
		class division;
        std::unique_ptr<field_t>        temp_field_t;
		const window                    parent_window_handle;
		event_handle                    event_size_handle;
		std::unique_ptr<division>       root_division;
        std::unordered_set<std::string> names;          ///<  All the names defines. Garant no repited name.
        unsigned                        div_numer;      ///<  Used to generate unique div name.

            /// All the fields defined by user with field(name)<<IField, 
            /// plus the div. find from the layot in div()
		std::unordered_map<std::string, std::unique_ptr<IField>> fields;    
		std::unordered_map<std::string,    window              > fastened;
			
		implement(window parent_widget)	;	
       ~implement() 	{	   API::umake_event(event_size_handle);	    }

        void              collocate();
		void              div(const char* s);
        //void              bind     (window parent_widget);


		division *        scan_div       (tokenizer&);
        std::string   add_div_name  ()  /// Generate and registre in the set a new unique div name
        {
            std::string name= std::to_string (div_numer++);
            names.insert (name);
            return name; 
        }
        bool          add_field_name(const std::string& n)  /// is unique? (registre it)
        {
            if (names.insert (n).second)
                return true; 
            else return false;//trow name repit;
        }

        class field_impl;

        struct Gap_field   : IField                     
        { 
            virtual  ~Gap_field (){}

            virtual void      collocate_         ( const rectangle& r)      {}
            virtual void      populate_children_ ( place::implement*   place_impl_){}
            virtual window    window_handle_     () const                   { return nullptr; }
            virtual size      dimension_         () const                   {return size();} 

             void     collocate    (const rectangle& r)override  { collocate_(r); attached=true;  }
             window   window_handle() const override             { return window_handle_(); }
             size     dimension    ()   const override           { return dimension_(); }              
             void    populate_children(	place::implement*   place_impl_) override
                    { return populate_children_(place_impl_);   }
        };
        struct Widget_field :  Gap_field                     
        { 
            //Widget_field    (window handle_):handle(handle_){}
            window          handle; 
            virtual        ~Widget_field    (){}
            void            init (window handle_){handle=handle_;}

            window          window_handle_  () const override{ return handle; }
            size            dimension_      () const   override        {return size(1,1);} 
            void            collocate_      (const rectangle& r)override
            {  
                API::move_window (handle,r );
            }
        };
        struct Room_field: Widget_field    
        { 
            unsigned        rows,columns;
            //Room_field      (window handle_,unsigned rows_,unsigned columns_)
            //{  Widget_field::init( handle_);   rows=rows_;   columns=columns_;   }
            //Widget_field( handle_), rows(rows_),columns(columns_)  {}            

            void init(window handle_,unsigned rows_,unsigned columns_)
            {  Widget_field::init( handle_);   rows=rows_;   columns=columns_;   }

            size            dimension_      () const    override       {return size(rows,columns);} 
        };
		class div_h;
		class div_v;
		class div_grid;

        template <class Base>
        struct IAdjust : Base
        {    
             //void   collocate(const rectangle& r)override          {        static_cast<Base>(this)->Base::collocate_(r); attached=true;    }
             //window window_handle() const override                 { return Base::window_handle_(); }
             //void  populate_children(implement*place_impl_)override{ return Base::populate_children_(place_impl_); }
             //size        dimension   ()   const override           { return Base::dimension_(); }              
             unsigned min, max; 
             static const unsigned MAX= 1000000 ;  // std::numeric_limits <decltype(max)>::max()/1000;
             IAdjust():min(std::numeric_limits <decltype(min)>::min() ),
                       max(MAX /*REVISE!!!!!temporal!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ ){}
             IAdjust(unsigned min_,unsigned max_) :min(min_), max(max_){} 
             virtual ~IAdjust(){}
        };
        template <class Base>
        struct IAdjustable  :  IAdjust<Base>                      
        { 
            IAdjustable(         ){}
            IAdjustable(unsigned min_,unsigned max_):IAdjust<Base>  (min_,max_){}

            adj   pre_place(unsigned t_w,                        adj& acc_fixed_min = adj() ) override    
                      {   
                          ++acc_fixed_min.count_adj;  
                          acc_fixed_min.weigth += min;
                          return  acc_fixed_min;        
                      }

            adj   end_place(unsigned t_w,const adj& tip = adj(), adj& prev = adj() ) override    
            {   
                if ( t_w <       min * tip.count_adj + tip.weigth )   {prev.weigth += min; return prev; }
                if ( t_w > (max-min) * tip.count_adj + tip.weigth )   {prev.weigth += max; return prev; }
                prev.weigth += min; 
                ++prev.count_adj;   return  prev;        
            }
            unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )override
            {   
                if ( t_w <        min * tip.count_adj + prev.weigth )   {return min; }
                if ( t_w > (max-min)  * tip.count_adj + prev.weigth )   {return max; }

                assert (tip.count_adj);

                return  (t_w - prev.weigth ) / prev.count_adj + min ;        
            }
        };
        template <class Base>
        struct IFixed:  IAdjust<Base>     
        { 
            unsigned weight_; 

            IFixed(unsigned weight):weight_(weight){}
            IFixed(unsigned weight, unsigned min_,unsigned max_): IAdjust<Base>     (min_,max_),weight_(weight){}


            adj   pre_place(unsigned t_w,                        adj& acc_fixed_min = adj() ) override    
                      {   acc_fixed_min.weigth += weigth_adj() ;   return  acc_fixed_min;        }

            adj   end_place(unsigned t_w,const adj& tip = adj(), adj& prev = adj() ) override    
                      {   prev.weigth += weigth_adj() ;   return  prev;        }   

            unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )  override              
                      {   return weigth_adj();      }    

            unsigned weigth_adj()
            {   
                if ( weight_  < min )    {return min; }
                if ( weight_  > max )    {return max; }
                
                return  weight_;          
            }
        };

        template <class Base>
        struct IPercent: IAdjust<Base> 
        { 
            double      percent; 

            IPercent(double percent_):percent(percent_){}
            IPercent(double percent_,unsigned min_,unsigned max_):IAdjust<Base>(min_,max_),percent(percent_){}
        
            adj   pre_place(unsigned t_w,                        adj& prev = adj() ) override
                      {   prev.weigth +=  weigth_adj( t_w ) ;   return  prev;      }

            adj   end_place(unsigned t_w,const adj& tip = adj(), adj& prev = adj() ) override    
                      {   prev.weigth +=  weigth_adj( t_w ) ;   return  prev;      }

            unsigned weigth(unsigned t_w,const adj& tip,const adj& prev )override  
                      {   return   weigth_adj( t_w ) ;                             }
            unsigned weigth_adj(unsigned t_w )
            {   
                if ( t_w * percent  < min )    {return min; }
                if ( t_w * percent  > max )    {return max; }
                return  t_w * percent  ;     
            }
        };     

        struct adj_gap:   IAdjustable<Gap_field> 
        { 
            adj_gap( )                                                   {}
            adj_gap( unsigned min_,unsigned max_):IAdjustable<Gap_field> (min_,max_){}
        };
        struct adj_widget:    IAdjustable<Widget_field>
        { 
            adj_widget(window handle_)                      
                  {Widget_field::init (handle_);}
            adj_widget(window handle_, unsigned min_,unsigned max_)
                :IAdjustable<Widget_field>(min_,max_) {Widget_field::init (handle_);}
        };
        struct adj_room: IAdjustable<Room_field>   
        { 
            adj_room(window handle_,unsigned rows_,unsigned columns_)                             
                    {Room_field::init(handle_, rows_, columns_);}
            adj_room(window handle_,unsigned rows_,unsigned columns_, unsigned min_,unsigned max_)
                    :IAdjustable<Room_field>(min_,max_){Room_field::init(handle_, rows_, columns_);}
        };
		struct adj_div_h;
		struct adj_div_v;
		struct adj_div_grid;


        struct fixed_gap: IFixed<Gap_field> 
        { 
            fixed_gap(unsigned weight_                             ):IFixed<Gap_field> (weight_)          {}
            fixed_gap(unsigned weight_, unsigned min_,unsigned max_):IFixed<Gap_field> (weight_,min_,max_){}
        };
        struct fixed_widget:   IFixed<Widget_field>
        { 
            fixed_widget(window handle_,unsigned weight_                             )
                :IFixed<Widget_field>(weight_          ){Widget_field::init (handle_);}
            fixed_widget(window handle_,unsigned weight_, unsigned min_,unsigned max_)
                :IFixed<Widget_field>(weight_,min_,max_){Widget_field::init (handle_);}
        };
        struct fixed_room:  IFixed<Room_field>   
        { 
            fixed_room(window handle_,unsigned weight_  , unsigned rows_,unsigned columns_)
                :IFixed<Room_field>   (weight_){Room_field::init(handle_, rows_, columns_);}
            fixed_room(window handle_,unsigned rows_,unsigned columns_, unsigned min_,unsigned max_)
                :IFixed<Room_field>   (weight_,min_,max_){Room_field::init(handle_, rows_, columns_);}
        };
		struct fixed_div_h;
		struct fixed_div_v;
		struct fixed_div_grid;

        struct percent_gap: IPercent<Gap_field> 
        { 
            percent_gap(double   percent_):IPercent<Gap_field>(percent_){}
            percent_gap(double   percent_, unsigned min_,unsigned max_):IPercent<Gap_field>(percent_,min_,max_){}
        };     
        struct percent_widget: IPercent<Widget_field>
        { 
            percent_widget(window handle_,double   percent_)
                :IPercent<Widget_field>(percent_){Widget_field::init (handle_);}
            percent_widget(window handle_,double   percent_, unsigned min_,unsigned max_)
                :IPercent<Widget_field>(percent_,min_,max_){Widget_field::init (handle_);}
        };   
        struct percent_room:  IPercent<Room_field>   
        { 
            percent_room(window handle_,double   percent_,unsigned rows_,unsigned columns_)
                   :IPercent<Room_field>  (percent_          ){Room_field::init(handle_, rows_, columns_);}

            percent_room(window handle_,double   percent_,unsigned rows_,unsigned columns_, unsigned min_,unsigned max_)
                   :IPercent<Room_field>  (percent_,min_,max_){Room_field::init(handle_, rows_, columns_);}
        };     
		struct percent_div_h;
		struct percent_div_v;
		struct percent_div_grid;
	};	      //struct implement

	place::field_t::~field_t(){}

	class place::implement::field_impl 		:	public place::field_t
	{
	public:
		std::string name;
	private:
		implement * place_impl_;
	 public:
		//typedef std::vector<std::unique_ptr<IField>>::const_iterator const_iterator;

		field_impl(implement * p, const std::string& name_)	:	place_impl_(p),   name(name_)		{place_impl_->temp_field_t.reset (this) ;}

	  public:
        IField * create_field(window    wd                                      )
            {IField *p= new adj_widget     ( wd )                        ; return p;}
        IField * create_field(unsigned gap                                      )
            {IField *p= new fixed_gap      ( gap )                       ; return p;}
        IField * create_field(window handle_,unsigned weight_                   )
            {IField *p= new fixed_widget   ( handle_, weight_)           ; return p;}
        IField * create_field(window handle_,double   percent_                  )
            {IField *p= new percent_widget ( handle_, percent_)          ; return p;}
        IField * create_field(window handle_,unsigned rows_,unsigned columns_   )
            {IField *p= new adj_room       ( handle_, rows_, columns_ )  ; return p;}
	 private:
        field_t& operator<<(IField * fld)     override
		{
			place_impl_->fields.emplace(name,std::unique_ptr<IField>( fld));
			_m_make_destroy(fld->window_handle());
			return *this;
		}
        field_t& operator<<(window   wd)        override
		{
			place_impl_->fields.emplace(name,std::unique_ptr<IField>( create_field(wd)));
			_m_make_destroy(wd);
			return *this;
		}
		field_t& operator<<(unsigned gap)      override
		{
			place_impl_->fields.emplace(name,std::unique_ptr<IField>(create_field(gap)));
			return *this;
		}
		field_t& operator<<(const fixed_widget& fx) 
		{
			place_impl_->fields.emplace(name,std::unique_ptr<IField>(new fixed_widget(fx)));
			_m_make_destroy(fx.window_handle());
			return *this;
		}
    	field_t& operator<<(const percent_widget& pcnt) 
		{
			place_impl_->fields.emplace(name,std::unique_ptr<IField>(new percent_widget(pcnt)));
			_m_make_destroy(pcnt.window_handle());
			return *this;
		}
		field_t& operator<<(const adj_room& r) 
		{
			adj_room x = r;
			if(x.rows == 0)   //  sure ????
				x.rows = 1;
			if(x.columns == 0)
				x.columns = 1;
			place_impl_->fields.emplace(name,std::unique_ptr<IField>(new adj_room(x)));
			_m_make_destroy(r.window_handle());
			return *this;
		}
		field_t& fasten(window wd) override
		{
			place_impl_->fastened.emplace (name , wd); 

			//Listen to destroy of a window. The deleting a fastened window
			//does not change the layout.
			API::make_event<events::destroy>(wd, [this](const eventinfo& ei)
			{
				for (auto f=place_impl_->fastened.begin(); f!=place_impl_->fastened.end(); ++f)
                    if (f->second ==  ei.window )
                    {
                        place_impl_->fastened.erase(f);    // delete ???
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
				for (auto f=place_impl_->fields.begin(); f!=place_impl_->fields.end(); ++f)
                    if (f->second->window_handle() ==  ei.window )
                    {
                        place_impl_->fields.erase(f);    // delete ???
				        place_impl_->collocate();
                        break;
                    }
			});
		}

	};//end class field_impl

	class place::implement::division : public place::IField
	{
	  public:
        std::vector <std::string> field_names;
		std::vector<IField*>      children;   //  std::vector<div*> 
		std::vector<window>       fastened_in_div;   //  
		std::unique_ptr<place::IField>   gap;        //  
	  public:
		//division()	 	{}
        virtual int&      weigth_c(rectangle& r )=0;
        virtual unsigned& weigth_s(rectangle& r )=0;
        virtual int&       fixed_c(rectangle& r )=0;
        virtual unsigned&  fixed_s(rectangle& r )=0;

        window    window_handle    () const    override               { return nullptr; }
        size      dimension        () const    override               { return size();  } 

        virtual ~division() 	{ }
        /// populate childen in the same order in with they were introduced in the div layout str, and then in the order in with they were added to the field
        void populate_children(	implement*   place_impl_)
        {
            //assert(children.empty ());             /// .clear(); the children or it is empty allways ????
            children.clear ();             /// .clear(); the children or it is empty allways ????
            for (const auto &name : field_names)   /// for all the names in this div
            {
                auto r= place_impl_->fields.equal_range(name);     /// find in the place global list of fields all the fields attached to it
                for (auto fi=r.first ; fi != r.second ; ++fi)      
                {
                    children.push_back (fi->second.get () );       /// to form the div children
                    fi->second->populate_children (place_impl_);
                    if (gap) 
                        children.push_back (gap.get() );
                }
                auto f= place_impl_->fastened.equal_range(name);
                for (auto fi=f.first ; fi != f.second ; ++fi)
                    fastened_in_div.push_back (fi->second );
            }
        }
        /// add field names in the same order in with they are introduced in the div layout str
		virtual void collocate(const rectangle& r) 
		{   
                //std::cerr<< "\ncollocating div in: "<<r; // debugg

			rectangle area (r);	
            adj pre_adj, end_adj; auto t_w=weigth_s(area);
			for(auto child: children)                            
                child->pre_place(  t_w , pre_adj );  
			for(auto child: children)                            
                child->end_place(  t_w , pre_adj, end_adj );
       
			rectangle left = area;
			for(auto child : children)                          
			{
			    rectangle child_area (left);
                weigth_s(child_area) = child->weigth(  t_w , pre_adj, end_adj )   ;
                weigth_c(left)      += weigth_s(child_area);
                weigth_s(left)      -= weigth_s(child_area);
                //std::cerr<< "\ncollocating child in: "<<child_area; // debugg
                child->collocate(child_area);
			}
			for(auto & fsn: fastened_in_div)
				API::move_window(fsn, area);
		}
	};
    
	class place::implement::div_h 	: public division
	{
	  public:
         int&      weigth_c(rectangle& r )override{return r.x;}
         unsigned& weigth_s(rectangle& r )override{return r.width;}
         int&       fixed_c(rectangle& r )override{return r.y;}
         unsigned&  fixed_s(rectangle& r )override{return r.height;}
	};
	class place::implement::div_v 	: public division
	{
	  public:
         int&      weigth_c(rectangle& r )override{return r.y;}
         unsigned& weigth_s(rectangle& r )override{return r.height;}
         int&       fixed_c(rectangle& r )override{return r.x;}
         unsigned&  fixed_s(rectangle& r )override{return r.width;}
   	};
	class place::implement::div_grid	: public div_h
	{
	  public:
        std::string name; ///< field name to be refered in the field(name)<<room instr.
        unsigned rows, columns;      ///< w=rows and h=columns   dim; 
		void Init(const std::string& name_, size dim_)	
        {  name=name_; rows=dim_.width ; columns =dim_.height ;}

		void Init(const std::string& name_, unsigned rows_, unsigned columns_)	
        {  name=name_; rows=rows_ ; columns =columns_;}

        virtual void collocate(const rectangle& r) override
	    {
            for(auto & fsn: fastened_in_div)
			    API::move_window(fsn, r);

            assert ( gap ? field_names.size() ==2 : field_names.size() ==1  ); /// by now asept only one field name in grid div
            rectangle area(r);
		    if((rows<=1 && columns<=1) || !rows || !columns)
		    {
			    unsigned min_n_of_cells = _m_number_of_cells();

			    unsigned edge=1;
                while (edge*edge <= min_n_of_cells )
                    ++edge;
                rows=columns=edge;
		    }
		    double block_w = weigth_s( area )/ double(rows);    // TODO: adapt to vert
		    double block_h = fixed_s( area ) / double(columns);
            std::vector<std::vector<size>> table(columns,std::vector<size>(rows,size(1,1)));  

		    unsigned field=0;
            unsigned clm=0, rw;
            size d_c, d_f;
            while (clm < columns)
		    {
                rw=0;
			    while (rw < rows)	
			    {
				    d_c=table[clm][rw];
                    if( ! d_c.height || ! d_c.width  )       /// pass collapsed cells
						    continue;
                    
                    d_f = children[field]->dimension();  
                    while ( ! d_f.height || ! d_f.width)      /// pass non dimension fields, the gaps
                    {
                        if (++field >= children.size () )
                            return;
                        d_f = children[++field]->dimension(); 
				    }
				    if(clm + d_f.width  >= columns )
						                                d_f.width = columns - clm;    /// adjust to borders
				    if(rw  + d_f.height >= rows    )
						                                d_f.height = rows - rw;

				    for(unsigned cc = 0; cc < d_f.width ; ++cc)
					    for(unsigned cr = 0; cr < d_f.height ; ++cr)
					    {
						    size&ccell ( table[clm+cc][rw+cr] );
                            if ( ccell.height &&  ccell.width)
                                ccell=size(0,0);
                            else 
                                d_f.width = cc;
					    }
                        table[clm][rw]=d_f;
                        rectangle cell(area);
                        weigth_c (cell) = weigth_c ( area ) + static_cast<int>(clm * block_w);
                        fixed_c (cell) = weigth_c ( area ) + static_cast<int>(rw  * block_h);
                        weigth_s (cell) =  static_cast<unsigned>(block_w * d_f.width);
                        fixed_s (cell) =  static_cast<unsigned>(block_h * d_f.height);

                        adj pre_adj, end_adj;
                        children[field]->pre_place(  weigth_s(cell) , pre_adj ); 
                                    gap->pre_place(  weigth_s(cell) , pre_adj ); 
                        children[field]->end_place(  weigth_s(cell) , pre_adj ); 
                                    gap->end_place(  weigth_s(cell) , pre_adj ); 
			            rectangle child_area (cell);
                        weigth_s(child_area) = children[field]->weigth(  weigth_s(area) , pre_adj, end_adj )   ;

                    pre_adj.count_adj = 1 ;           
                    pre_adj.weigth    = child_area.height ;
                    end_adj=pre_adj ;
                    gap->pre_place (child_area.height, pre_adj );
                    gap->end_place (child_area.height, pre_adj, end_adj );
                    child_area.height = end_adj.weigth/pre_adj.count_adj;  
                    children[field]->collocate(child_area);

                        if (++field >= children.size () )
                            return;
				    }
            }
	    }
	  private:
		unsigned _m_number_of_cells() const
		{
			unsigned n = 0;
			for(auto & child : children)
                n += child->dimension().width * child->dimension().height ;      
			return n;
		}
	public:
	};//end class div_grid

	struct place::implement::adj_div_h : public IAdjustable<div_h> 
    {
        adj_div_h()                     {}
        adj_div_h(unsigned min_,unsigned max_):IAdjustable<div_h> (min_,max_){}
    };
	struct place::implement::adj_div_v : public IAdjustable<div_v>  
    {
        adj_div_v()                     {}
        adj_div_v(unsigned min_,unsigned max_):IAdjustable<div_v>(min_,max_){}
    };
	struct place::implement::adj_div_grid : public IAdjustable<div_grid> 
    {
        adj_div_grid(const std::string& name_, unsigned rows_, unsigned columns_)   
                { Init(name_,rows_,  columns_);}

        adj_div_grid(const std::string& name_, size dim_ , unsigned min_ ,  unsigned max_     )    
            :    IAdjustable<div_grid>(dim_.width  ,dim_.height)  {Init(name_,dim_); }

        adj_div_grid(const std::string& name_, size dim_      )    
            :    IAdjustable(dim_.width  ,dim_.height)           {Init(name_,dim_); }

        adj_div_grid(const std::string& name_, unsigned rows_, unsigned columns_,unsigned min_,unsigned max_)   
            :    IAdjustable<div_grid>(min_        ,max_       ) { Init(name_,rows_,  columns_);}
    };

	struct place::implement::fixed_div_h : public IFixed<div_h>
    {
        fixed_div_h(unsigned weight_                             )  :IFixed<div_h> (weight_ )              {}
        fixed_div_h(unsigned weight_ , unsigned min_,unsigned max_):IFixed<div_h> (weight_ ,min_,max_){}
    };
	struct place::implement::fixed_div_v : public IFixed<div_v> 
    {
        fixed_div_v(unsigned weight_                             ):IFixed<div_v>(weight_   )       {}
        fixed_div_v(unsigned weight_, unsigned min_,unsigned max_):IFixed<div_v>(weight_,min_,max_){}
    };
	struct place::implement::fixed_div_grid : public IFixed<div_grid> 
    {
        fixed_div_grid(const std::string& name_,unsigned weight_ ,  unsigned rows_, unsigned columns_)  
            :IFixed<div_grid>(weight_)   { Init(name_,rows_,  columns_);}
        fixed_div_grid(const std::string& name_,unsigned weight_  , size dim_)
            :IFixed<div_grid>(weight_ )  {Init(name_,dim_); }

        fixed_div_grid(const std::string& name_,unsigned weight_ ,  unsigned rows_, unsigned columns_, unsigned min_,unsigned max_)  
            :IFixed<div_grid>(weight_,min_,max_) { Init(name_,rows_,  columns_);}
        fixed_div_grid(const std::string& name_,unsigned weight_  , size dim_, unsigned min_,unsigned max_)
            :IFixed<div_grid>(weight_,min_,max_) {Init(name_,dim_); }
    };
	struct place::implement::percent_div_h : public IPercent<div_h> 
    {
        percent_div_h(double   percent_)  :IPercent<div_h> ( percent_   )                 {}
        percent_div_h(double   percent_,unsigned min_,unsigned max_):IPercent<div_h> ( percent_ ,min_,max_){}
    };
	struct place::implement::percent_div_v : public IPercent<div_v> 
    {
        percent_div_v(double   percent_)   :IPercent<div_v> ( percent_   )                    {}
        percent_div_v(double   percent_,unsigned min_,unsigned max_):IPercent<div_v>( percent_,min_,max_){}
    };
	struct place::implement::percent_div_grid : public IPercent<div_grid>  
    {
        percent_div_grid(const std::string& name_,double   percent_, unsigned rows_, unsigned columns_)   
            :IPercent<div_grid> ( percent_    )        { Init(name_,rows_,  columns_);}
        percent_div_grid(const std::string& name_,double   percent_, size dim_)
            :IPercent<div_grid> ( percent_)            {Init(name_,dim_); }

        percent_div_grid(const std::string& name_,double   percent_, unsigned rows_, unsigned columns_, unsigned min_,unsigned max_)   
            :IPercent<div_grid> ( percent_ ,min_,max_   )   { Init(name_,rows_,  columns_);}
        percent_div_grid(const std::string& name_,double   percent_, size dim_, unsigned min_,unsigned max_)
            :IPercent<div_grid> ( percent_,min_,max_)       {Init(name_,dim_); }
    };

	place::implement::division* place::implement::scan_div(tokenizer& tknizer)
	{
		typedef tokenizer::token token;

		token       div_type = token::eof;
		number_t    weight , gap;
        bool        have_gap=false, have_weight=false;
        unsigned    min(std::numeric_limits<unsigned>::min());
        unsigned    max(place::implement::IAdjust<place::implement::Gap_field>::MAX/*std::numeric_limits<unsigned>::max()*/);
		std::vector<number_t>    array;
		std::vector<std::string> field_names_in_div;
        std::string gr_name;
 
        for(token tk = tknizer.read(); tk != token::eof && tk!=token::div_end ; tk = tknizer.read())
		{
			switch(tk)
			{
			    case token::div_start:	    
                    {
                       std::string div_name(add_div_name ());
                       fields.emplace(div_name,std::unique_ptr<IField>(scan_div(tknizer)));
                       field_names_in_div.push_back(div_name);				
                                                                                break;
                    }
			    case token::array:		    tknizer.array().swap(array);   		break;
			    case token::identifier:		
                {
                    std::string field_name(tknizer.idstr());
                    if (add_field_name (field_name))
                       field_names_in_div.push_back(field_name );				
                                                                                break;
                }
			    case token::horizontal:
			    case token::vertical:    	div_type = tk;		   		        break;
			    case token::grid:			
                    div_type = tk;
                    gr_name=field_names_in_div.back();                          break;

			    case token::weight:	weight = tknizer.number();have_weight=true; break;
			    case token::gap:	   gap = tknizer.number();have_gap=true;    break;
			    case token::min:		    
                    {
                        number_t m= tknizer.number();
                        if(m.kind_of() == number_t::kind::percent   )
                            ; // trow no min percent possible
                        else if(m.kind_of() == number_t::kind::integer   )
					        min = m.integer();
                        else 
					        min=static_cast<unsigned>(m.real());
                                                                                break;
                    }
			    case token::max:		    
                    {
                        number_t m= tknizer.number();
                        if(m.kind_of() == number_t::kind::percent   )
                            ; // trow no max percent possible
                        else if(m.kind_of() == number_t::kind::integer   )
					        max = m.integer();
                        else 
					        max=static_cast<unsigned>(m.real());
                                                                                break;
                    }
			    default:	break;
			}
		}
        division *div;
		unsigned rows=1, columns=1;		

        if (div_type == token::grid)
		{
			if(array.size())
			{
				if(array[0].kind_of() != number_t::kind::percent)
					rows = array[0].integer();
                if(array.size() > 1)
			    {
				    if(array[1].kind_of() != number_t::kind::percent)
					    columns = array[1].integer();
			    }
			}
			if( ! rows )
				rows = 1;
			if( ! columns )
				columns = 1;
            if(gr_name.empty())
                gr_name=field_names_in_div.back();  
		}
        if (weight.kind_of () == number_t::kind::percent && weight.real() > 0 )
        {
            double perc=weight.real ();
            switch(div_type)
            {
                case token::eof:
		        case token::horizontal:		div = new percent_div_h(perc,min,max);			     break;
		        case token::vertical:		div = new percent_div_v(perc,min,max);			     break;
                case token::grid:   		div = new percent_div_grid(gr_name,perc,rows,columns,min,max);break;
                default:
                    throw std::runtime_error("nana.place: invalid division type.");
		    }
        } else 
            {   unsigned fixed=0;
                if      ( weight.kind_of () == number_t::kind::real    && weight.real()    > 0 )
                    fixed = static_cast<unsigned>(weight.real());
                else if ( weight.kind_of () == number_t::kind::integer && weight.integer() > 0 )
                    fixed = static_cast<unsigned>(weight.integer());
                if (fixed)
                    switch(div_type)
                    {
                        case token::eof:
		                case token::horizontal:			div = new fixed_div_h(fixed,min,max);			break;
		                case token::vertical:			div = new fixed_div_v(fixed,min,max);			break;
                        default:
                            throw std::runtime_error("nana.place: invalid division type.");
		            }
                else
                    switch(div_type)
                    {
                        case token::eof:
		                case token::horizontal:			div = new adj_div_h(min,max);			break;
		                case token::vertical:			div = new adj_div_v(min,max);			break;
                        default:
                            throw std::runtime_error("nana.place: invalid division type.");
		            }
        }
        if (have_gap)
        {
            unsigned fixed=0;
		    if      (gap.kind_of() == number_t::kind::real    && gap.real()    > 0 )
			    fixed=static_cast<unsigned>(gap.real()); 
            else if (gap.kind_of() == number_t::kind::integer && gap.integer() > 0 )
			    fixed=static_cast<unsigned>(gap.integer()); 
            if (fixed)
                div->gap.reset (new fixed_gap(fixed) );

		    else if (gap.kind_of() == number_t::kind::percent && gap.real () > 0 )
                div->gap.reset (new percent_gap(gap.real ()) );
            else 
                div->gap.reset (new adj_gap() );
        }
		div->field_names.swap(field_names_in_div);
		return div;
	}
	//class place

		//place::place()
		//	: impl_(new implement)
		//{}
		place::place(window wd)
			: impl_(new implement(wd))
		{
			//bind(wd);
		}
		place::~place()
		{
			delete impl_;
		}
		//void place::bind(window wd)
		//{
		//	if(impl_->parent_window_handle)
		//		throw std::runtime_error("place.bind: it has already binded to a window.");

		//	impl_->parent_window_handle = wd;
		//	impl_->event_size_handle = API::make_event<events::size>(wd, [this](const eventinfo&ei)
		//		{
		//			if(impl_->root_division)
		//				impl_->root_division->collocate(API::window_size(ei.window));
		//		});
		//}
	place::implement::implement(window parent_widget)		
            : parent_window_handle(parent_widget), event_size_handle(nullptr)	
    {   
		//rectangle r;  //debugg
  //      r=API::window_size(this->parent_window_handle);  //debugg
  //      std::cerr<< "\nplace(parent_widget [ "<<parent_widget<<" ]) with area: "<<r;  //debugg

        event_size_handle = API::make_event<events::size>(parent_window_handle, [this](const eventinfo&ei)
		{
            //std::cerr<< "\nResize: collocating root div ??:[ "<<this->parent_window_handle<<" ]) with event :[ "<<ei.window <<" ]) ";  //debugg
			
            if(this->root_division)
            {
				//rectangle r;  //debugg
                this->root_division->collocate(/*r=*/API::window_size(this->parent_window_handle));
                //std::cerr<< "\ncollocating root div  [ "<<this->parent_window_handle<<" ]) with area: "<<r;  //debugg
            }
		});
    }

        void place::implement::div(const char* s)
        {
            div_numer=0;
            for (auto field=fields.begin(); field != fields.end(); ++field   )
            {
                if (isdigit( field->first[0] ) )  /// delete div, with have a name-numer
                {
                    field->second.release();  
                    fields.erase(field);
                }
            }
            for (auto name=names.begin(); name != names.end(); ++name )
                if (isdigit( (*name)[0] ) )  /// delete div names, with have a name-numer
                    names.erase(name);

			tokenizer tknizer(s);
			root_division.reset( scan_div(tknizer));
        }

        void place::div(const char* s)
		{
            impl_->div(s); 
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
            impl_->temp_field_t.reset (new implement::field_impl(this->impl_,name)); 
            return  *impl_->temp_field_t.get();
		}

		void place::collocate()
		{
			impl_->collocate();
		}
		void place::implement::collocate()
		{
			if(root_division && parent_window_handle)
			{
               root_division->populate_children (this);
               for(auto & field : fields)
                    field.second->attached = false;

				//rectangle r; // debugg
                root_division->collocate(/*r=*/API::window_size(parent_window_handle));
                //std::cerr<< "\ncollocating root div  [ "<<this->parent_window_handle<<" ]) with area: "<<r; // debugg

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
