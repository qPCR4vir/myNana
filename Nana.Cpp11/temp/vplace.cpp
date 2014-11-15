/*
 *	An Alternative Implementation of Place for Layout
 *  (
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
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
#include <memory>
#include <numeric> 

#include <map>
#include <set>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <../temp/vplace.hpp>
#include <nana/gui/wvl.hpp>
#include <nana/gui/programming_interface.hpp>
#include <iostream>    // temp, for debugging
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/dragger.hpp>

std::ostream& operator<<(std::ostream& o, const nana::rectangle &r)
{ o<<" rect("<<r.x<<","<<r.y<<","<<r.width <<","<<r.height <<")\n"; return o; }

namespace nana{
    namespace vplace_impl
    {
        namespace place_parts
        {
            class splitter_interface
            { public:	virtual ~splitter_interface () {} };

            class splitter_dtrigger : public drawer_trigger
            {};

            template<bool IsLite>
            class splitter
                : public widget_object <typename std::conditional<IsLite, category::lite_widget_tag, category::widget_tag>::type, splitter_dtrigger>,
                public splitter_interface
            {};

            struct Number;
            using  num = std::unique_ptr<Number>;
            struct Number
            {
                enum class Kind { none, integer, real, percent };
                virtual bool is_negative () const { return false; }
                virtual bool is_none ()     const { return true; }
                bool is_not_none () const { return  !is_none ()   ; }
                virtual Kind kind_of ()     const { return Kind::none; }
                virtual int integer ()      const { return 0; }
                virtual double real ()      const { return 0; }
                virtual double get_value ( int ref_percent = 1 ) const { return real (); }
                virtual ~Number () {}

                static num reset () ;// we need this?
                static num assign ( int    i ); // we need this?
                static num assign ( double d );
                static num assign_percent ( double d );
                static num clear () { return reset (); }
            };
            struct Integer : Number
            {
                int value_{};
                Integer ( int i ) :value_{ i } {}
                bool is_negative () const override { return value_<0; }
                bool is_none ()     const override { return false; }
                Kind kind_of ()     const override { return Kind::integer; }
                int integer ()      const override { return value_; }
                double real ()      const override { return static_cast<double>(value_); }
            };
            struct Real : Number
            {
                double value_{};
                Real ( double d ) :value_{ d } {}
                bool is_negative () const override { return value_<0; }
                bool is_none ()     const override { return false; }
                Kind kind_of ()     const override { return Kind::real; }
                int integer ()      const override { return static_cast<int>(value_); }
                double real ()      const override { return value_; }
            };
            struct Percent : Real
            {
                Percent ( double d ) :Real{ d } {}
                Kind kind_of ()     const override { return Kind::percent; }
                double get_value ( int ref_percent = 1 ) const override { return  real ()*ref_percent; }
            };
            num Number::reset () { return num{ new Number }; }
            num Number::assign ( int    i ) { return num{ new Integer ( i ) }; }
            num Number::assign ( double d ) { return num{ new Real ( d ) }; }
            num Number::assign_percent ( double d ) { return num{ new Percent ( d ) }; }
        }//end namespace place_parts
        class number_t
        {	//number_t is used to store a number type variable
            //such as integer, real and percent. Essentially, percent is a typo of real.
        public:
            enum class kind {/*empty, */ none, integer, real, percent };
            number_t () : kind_ ( kind::integer ) { value_.integer = 0; }

            void reset () { kind_ = kind::none;		value_.integer = 0; }
            bool is_negative () const
            {
                switch ( kind_ )
                {
                    case kind::integer:
                        return (value_.integer < 0);
                    case kind::real:
                    case kind::percent:
                        return (value_.real < 0);
                    default:
                        break;
                }
                return false;
            }
            bool is_none () const
            {
                return (kind::none == kind_);
            }
            bool is_not_none () const { return (kind::none != kind_); }
            kind kind_of () const { return kind_; }
            double get_value ( int ref_percent ) const
            {
                switch ( kind_ )
                {
                    case kind::integer:
                        return value_.integer;
                    case kind::real:
                        return value_.real;
                    case kind::percent:
                        return value_.real * ref_percent;
                    default:
                        break;
                }
                return 0;
            }
            int integer () const
            {
                //        if( kind::empty  == kind_ )				//return 0;
                if ( kind::integer == kind_ )				return value_.integer;
                return static_cast<int>(value_.real);
            }
            double real () const
            {
                //        if( kind::empty  == kind_ )				//return 0;
                if ( kind::integer == kind_ )				return value_.integer;
                return value_.real;
            }
            void assign ( int i ) { kind_ = kind::integer;  value_.integer = i; }
            void assign ( double d ) { kind_ = kind::real; 	value_.real = d; }
            void assign_percent ( double d ) { kind_ = kind::percent; value_.real = d / 100; }
            void clear () { *this = number_t (); }
        private:
            kind kind_;
            union valueset { int integer; double real; }value_;
        };//end class number_t
        class repeated_array
        {
            bool repeated_ = false;
            std::vector<number_t> values_;
        public:
            repeated_array () {}

            repeated_array ( repeated_array&& rhs )
                : repeated_ ( rhs.repeated_ ), values_ ( std::move ( rhs.values_ ) ) {}

            repeated_array& operator=(const repeated_array& rhs)
            {
                if ( this != &rhs )
                {
                    repeated_ = rhs.repeated_;
                    values_ = rhs.values_;
                }
                return *this;
            }
            void assign ( std::vector<number_t>&& c ) { values_ = std::move ( c ); }
            bool empty () const { return values_.empty (); }
            std::size_t size ()const { return size (); }
            void reset ()
            {
                repeated_ = false;
                values_.clear ();
            }
            void repeated () { repeated_ = true; }
            void push ( const number_t& n ) { values_.emplace_back ( n ); }
            number_t at ( std::size_t pos ) const
            {
                if ( values_.empty () )
                    return{};

                if ( repeated_ )
                    pos %= values_.size ();
                else if ( pos >= values_.size () )
                    return{};

                return values_[pos];
            }
            bool pass_end ( std::size_t pos ) const
            {
                return !repeated_ || pos>=values_.size () ;
            }
        };
        class tokenizer
        {
            std::string     div_str;   // for the future. kip a copy of the layout
            const char*     divstr_;
            const char*     sp_;
            std::string     idstr_;
            number_t        number_;
            std::vector<number_t> array_;
            repeated_array		  reparray_;
            std::vector<number_t> parameters_;
        public:
            enum class token
            {
                div_start, div_end, splitter,
                identifier, vertical, horizontal, grid, number, array, reparray,
                weight, gap, min, max, margin, arrange, variable, repeated, collapse, parameters,
                equal,
                eof, error
            };

            tokenizer ( const char* p ) : div_str ( p ), divstr_ ( div_str.c_str () ), sp_ ( div_str.c_str () ) {}

            const std::string&      idstr () const { return idstr_; }
            number_t                number () const { return number_; }
            std::vector<number_t>&  array () { return array_; }
            repeated_array&         reparray () { return reparray_; }
            std::vector<number_t>&  parameters () { return parameters_; }

            token read ()
            {
                sp_ = _m_eat_whitespace ( sp_ );

                std::size_t readbytes = 0;
                switch ( *sp_ )
                {
                    case '\0':				                    return token::eof;
                    case '=':		        ++sp_;				return token::equal;
                    case '|':	            ++sp_;
                        readbytes = _m_number ( sp_, false );
                        sp_ += readbytes;               return token::splitter;

                    case '<':				++sp_;				return token::div_start;
                    case '>':				++sp_;				return token::div_end;
                    case '[':{
                        array_.clear ();
                        sp_ = _m_eat_whitespace ( sp_ + 1 );
                        if ( *sp_ == ']' ) { ++sp_;		        return token::array; }

                        bool repeated = false;    //When search the repeated.

                        while ( true )
                        {
                            sp_ = _m_eat_whitespace ( sp_ );
                            auto tk = read ();
                            if ( token::number != tk && token::variable != tk && token::repeated != tk )
                                _m_throw_error ( "invalid array element" );
                            if ( !repeated )
                            {
                                switch ( tk )
                                {
                                    case token::number:	  array_.push_back ( number_ );			break;
                                    case token::variable: array_.push_back ( {} );					break;/// token::variable is translated into number_t::none
                                    default:
                                        repeated = true;
                                        reparray_.repeated ();
                                        reparray_.assign ( std::move ( array_ ) );
                                }
                            }
                            sp_ = _m_eat_whitespace ( sp_ );
                            char ch = *sp_++;

                            if ( ch == ']' )			return (repeated ? token::reparray : token::array);
                            if ( ch != ',' )			_m_throw_error ( "invalid array" );
                        }}
                        break;
                    case '(':
                        parameters_.clear ();
                        sp_ = _m_eat_whitespace ( sp_ + 1 );
                        if ( *sp_ == ')' ) { ++sp_;			return token::parameters; }

                        while ( true )
                        {
                            if ( token::number == read () )		parameters_.push_back ( number_ );
                            else						        _m_throw_error ( "invalid parameter." );

                            sp_ = _m_eat_whitespace ( sp_ );
                            char ch = *sp_++;
                            if ( ch == ')' )						return token::parameters;
                            if ( ch != ',' )						_m_throw_error ( "invalid parameter." );
                        }
                        break;
                    case '.': case '-':
                        if ( *sp_ == '-' )
                        {
                            readbytes = _m_number ( sp_ + 1, true );
                            if ( readbytes )			++readbytes;
                        } else 					readbytes = _m_number ( sp_, false );

                        if ( readbytes ) { sp_ += readbytes; 	return token::number; } else    					        _m_throw_error ( *sp_ );
                        break;
                    default:
                        if ( isdigit ( *sp_ ) )
                        {
                            readbytes = _m_number ( sp_, false );
                            if ( readbytes ) { sp_ += readbytes; return token::number; }
                        }
                        break;
                }

                if ( '_' == *sp_ || isalpha ( *sp_ ) )
                {
                    const char * idstart = sp_++;

                    while ( '_' == *sp_ || isalpha ( *sp_ ) || isalnum ( *sp_ ) ) 	++sp_;

                    idstr_.assign ( idstart, sp_ );

                    if ( idstr_ == "weight" ) { _m_attr_number_value ();	return token::weight; } else if ( idstr_ == "gap" ) { _m_attr_number_value ();	return token::gap; } else if ( idstr_ == "min" ) { _m_attr_number_value (); return token::min; } else if ( idstr_ == "max" ) { _m_attr_number_value ();	return token::max; } else if ( idstr_ == "vertical" || idstr_  == "vert" ) 	return token::vertical;
                    else if ( idstr_ == "horizontal" ) 		   	 	 	return token::horizontal;
                    else if ( idstr_ == "variable" )                       return token::variable;
                    else if ( idstr_ == "arrange" )                        return token::arrange;
                    else if ( idstr_ == "repeated" )                       return token::repeated;
                    else if ( idstr_ == "grid" )
                    {
                        if ( token::equal != read () )
                            _m_throw_error ( "an equal sign is required after \'" + idstr_ + "\'" );
                        return token::grid;
                    } else if ( idstr_ == "margin" )
                    {
                        if ( token::equal != read () )
                            _m_throw_error ( "an equal sign is required after \'" + idstr_ + "\'" );
                        return token::margin;
                    } else if ( idstr_ =="collapse" )
                    {
                        if ( token::parameters != read () )
                            _m_throw_error ( "a parameter list is required after 'collapse'" );
                        return token::collapse;
                    }
                    return token::identifier;
                }
                _m_throw_error ( std::string ( "an invalid character '" ) + *sp_ + "'" );
                return token::error;	//Useless, just for syntax correction.
            }
        private:
            void _m_throw_error ( char err_char )
            {
                std::stringstream ss;
                ss<<"place: invalid character '"<<err_char<<"' at "<<static_cast<unsigned>(sp_ - divstr_);
                throw std::runtime_error ( ss.str () );
            }

            void _m_attr_number_value ()
            {
                if ( token::equal != read () )
                    _m_throw_error ( "an equal sign is required after \'"+ idstr_ +"\'" );

                const char* p = sp_;
                for ( ; *p == ' '; ++p );

                std::size_t len = 0;
                if ( *p == '-' )
                {
                    len = _m_number ( p + 1, true );
                    if ( len )	++len;
                } else
                    len = _m_number ( p, false );

                if ( 0 == len )
                    _m_throw_error ( "the \'" + idstr_ + "\' requires a number(integer or real or percent)" );

                sp_ += len + (p - sp_);
            }

            void _m_throw_error ( const std::string& err )
            {
                std::stringstream ss;
                ss  <<"NANA::vplace layout syntax error:\n\t"      <<  err
                    <<" at "<<static_cast<unsigned>(sp_ - divstr_)
                    <<" of:\n" << divstr_ ;
                std::cerr<< ss.str ();
                throw std::runtime_error ( ss.str () );
            }

            const char* _m_eat_whitespace ( const char* sp )
            {
                while ( *sp && !isgraph ( *sp ) )	++sp;
                return sp;
            }

            std::size_t _m_number ( const char* sp, bool negative )
            {
                const char* allstart = sp;
                sp = _m_eat_whitespace ( sp );

                number_.assign ( 0 );

                bool gotcha = false;
                int integer = 0;
                double real = 0;
                //read the integral part.
                const char* istart = sp;
                while ( isdigit ( *sp ) ) { integer = integer * 10 + (*sp - '0');	++sp; }
                const char* iend = sp;

                if ( '.' == *sp )
                {
                    double div = 1;
                    const char* rstart = ++sp;
                    while ( isdigit ( *sp ) ) { real += (*sp - '0') / (div *= 10);		++sp; }

                    if ( rstart != sp )
                    {
                        real += integer;
                        number_.assign ( negative ? -real : real );
                        gotcha = true;
                    }
                } else if ( istart != iend )
                {
                    number_.assign ( negative ? -integer : integer );
                    gotcha = true;
                }

                if ( gotcha )
                {
                    for ( ; *sp == ' '; ++sp );
                    if ( '%' == *sp )
                    {
                        number_.assign_percent ( number_.real () );
                        return sp - allstart + 1;
                    }
                    return sp - allstart;
                }
                return 0;
            }
        private:
        };	//end class tokenizer


        typedef vplace::minmax  minmax;//     minmax(unsigned Min=MIN, unsigned Max=MAX);
        struct adj { unsigned weigth, min, count_adj; adj () :weigth ( 0 ), min ( 0 ), count_adj ( 0 ) {} };

        struct adjustable :minmax
        {
            adjustable ( minmax MinMax = minmax () ) : minmax ( MinMax ) {}
            virtual ~adjustable () {}
            virtual void        collocate ( const rectangle& r ) = 0;
            virtual rectangle   cells ()   const = 0;
            virtual void        populate_children ( implement*   place_impl_ ) = 0;

            virtual adj   pre_place ( unsigned t_w, adj& fixed = adj () )
            {
                ++fixed.count_adj;
                fixed.min += min;
                return  fixed;
            }
            virtual adj   end_place ( unsigned t_w, const adj& fixed = adj (), adj& adj_min = adj () )
            {
                if ( t_w      <   fixed.weigth + fixed.min )
                { adj_min.weigth += min; return adj_min; }
                if ( t_w <    min * fixed.count_adj + fixed.weigth )
                { adj_min.weigth += min; return adj_min; }
                if ( t_w >    max * fixed.count_adj + fixed.weigth )
                { adj_min.weigth += max; return adj_min; }

                adj_min.min += min;
                ++adj_min.count_adj;   return  adj_min;
            }
            virtual unsigned weigth ( unsigned t_w, const adj& fixed, const adj& adj_min )
            {
                if ( t_w      <   fixed.weigth + fixed.min )
                { return min; }
                if ( t_w <    min * fixed.count_adj   + fixed.weigth )
                { return min; }
                if ( t_w >    max * fixed.count_adj   + fixed.weigth )
                { return max; }
                if ( t_w <    min * adj_min.count_adj + adj_min.weigth )
                { return min; }
                if ( t_w >    max * adj_min.count_adj + adj_min.weigth )
                { return max; }

                return  (t_w - adj_min.weigth) / adj_min.count_adj  ;
            }
        };
        struct fixed : adjustable
        {
            fixed ( unsigned weight, minmax MinMax = minmax () ) : weight_ ( weight ), adjustable ( MinMax ) {}

            unsigned weight_;

            void        setWeigth ( unsigned w ) { weight_ = w; }
            unsigned    getWeigth () { return weight_; }


            adj   pre_place ( unsigned t_w, adj&   fixed = adj () ) override
            { fixed.weigth += weigth_adj ( t_w ) ;   return  fixed; }

            adj   end_place ( unsigned t_w, const adj& fixed = adj (), adj& adj_min = adj () ) override
            { adj_min.weigth += weigth_adj ( t_w ) ;   return  adj_min; }

            unsigned weigth ( unsigned t_w, const adj& fixed, const adj& adj_min )  override
            { return weigth_adj ( t_w ); }

            virtual unsigned weigth_adj ( unsigned t_w )
            {
                if ( weight_  < min ) { return min; }
                if ( weight_  > max ) { return max; }

                return  weight_;
            }
        };
        struct percent : fixed
        {
            percent ( double percent_, minmax MinMax = minmax () ) :fixed ( static_cast<unsigned>(fx*percent_), MinMax ) {}

            const static unsigned fx = 10000;

            void        setPercent ( double p ) { weight_ = p*fx; }
            double      getPercent () { return double ( weight_ )/fx; }

            unsigned weigth_adj ( unsigned t_w )override
            {
                if ( (t_w * weight_) /fx  < min ) { return min; }
                if ( (t_w * weight_) /fx  > max ) { return max; }

                return  (t_w * weight_) /fx;
            }
        };


        struct Ifield
        {
            rectangle           last ;
            virtual            ~Ifield () {}
            virtual void        collocate_field ( const rectangle& r ) { last = r; }
            virtual void        populate_children ( implement*   place_impl_ ) {}
            virtual rectangle   cells_ ()   const = 0;
        };
        struct Gap : Ifield
        {
            rectangle      cells_ () const override { return rectangle ( -1, -1, 0, 0 ); }
        };
        struct Widget : Gap
        {
            Widget ( window handle_ ) :handle ( handle_ ) {}
            ~Widget ()override { API::umake_event ( destroy_evh ); }

            window          handle{ nullptr };
            event_handle    destroy_evh{ nullptr };

            window          window_handle () const { return handle; }
            rectangle       cells_ () const override { return rectangle ( -1, -1, 1, 1 ); }
            void            collocate_field ( const rectangle& r )override
            {
                Ifield::collocate_field ( r );
                API::move_window ( handle, r );
            }
        };
        struct Room : Widget    ///  
        {
            Room ( window handle_, nana::size sz ) :
                Widget ( handle_ ), sz ( sz  ) 
            {
            }
            nana::size sz;
            rectangle  cells_ () const override 
            { 
                return rectangle ( point(-1, -1), sz ); 
            }
        };
        //struct Label_field: Widget_field
        //{
        //
        //};
        //struct Cell:  Widget   
        //{ 
        //    Cell(window handle_,unsigned row_,unsigned column_)  :                           
        //            Widget(handle_), row(row_), column (column_){}
        //    unsigned   row,column;
        //    rectangle  cells_         () const override          {return rectangle(column,row,1,1);}             
        //};

        struct Splitter;
        class  division : public Ifield
        {
        protected:
            using Owned = std::vector<std::unique_ptr< adjustable>> ;
        public:
            std::vector<window>       fastened_in_div;
            std::vector <std::string> field_names;     ///< names used to find the fields in the global multimap of place fields to built the children
            repeated_array                                  gap, margin;
            std::unordered_map<std::string, repeated_array> arrange_;

            using Children = std::vector< adjustable*>;
            Children                  children;        ///< contain the fiels with are directly collocated
            Owned   owned;           ///< betwen fields

            bool set_arrange ( const repeated_array& arranges, int gap_index, adjustable* field );      ///< betwen fields
            bool insert_gap ( int gap_index );



            Splitter                  *splitter{ nullptr }; ///< this division have an splitter?
        public:
            virtual int&      weigth_c ( rectangle& r ) = 0;
            virtual unsigned& weigth_s ( rectangle& r ) = 0;
            virtual int&       fixed_c ( rectangle& r ) = 0;
            virtual unsigned&  fixed_s ( rectangle& r ) = 0;
            int&      weigth_c () { return weigth_c ( last ); }
            unsigned& weigth_s () { return weigth_s ( last ); }
            int&       fixed_c () { return fixed_c ( last ); }
            unsigned&  fixed_s () { return fixed_s ( last ); }

            rectangle cells_ () const  override { return rectangle ( -1, -1, 1, 1 ); }
            /// populate childen in the same order in with they were introduced in the div layout str,
            /// and then in the order in with they were added to the field
            void populate_children ( implement*   place_impl_ ) override;
            /// add field names in the same order in with they are introduced in the div layout str
            virtual void collocate_field ( const rectangle& r ) override
            {
                Ifield::collocate_field ( r );       //std::cerr<< "\ncollocating div in: "<<r; // debugg
                rectangle area ( r );

                adj pre_adj, end_adj; auto t_w = weigth_s ( area );
                for ( auto child: children )
                    child->pre_place ( t_w, pre_adj );
                for ( auto child: children )
                    child->end_place ( t_w, pre_adj, end_adj );

                for ( auto child : children )
                {
                    rectangle child_area ( area );
                    weigth_s ( child_area ) = child->weigth ( t_w, pre_adj, end_adj )   ;
                    weigth_c ( area ) += weigth_s ( child_area );
                    weigth_s ( area ) -= weigth_s ( child_area );
                    
                    child->collocate ( child_area ); //std::cerr<<"\ncollocating child in: "<<child_area; //debugg
                }
                for ( auto & fsn: fastened_in_div )
                {
                    API::move_window ( fsn, r );   // si alguien habia cerrado la w fsn despues del populate children, tendremos problemas? make unload fastened erase?
                    API::show_window ( fsn, API::visible ( fsn ) );
                }

                split ();
            }
            void split ();

            virtual Splitter* create_splitter () = 0;
        };
        class  div_h : public division
        {
        public:
            int&      weigth_c ( rectangle& r )override { return r.x; }
            unsigned& weigth_s ( rectangle& r )override { return r.width; }
            int&       fixed_c ( rectangle& r )override { return r.y; }
            unsigned&  fixed_s ( rectangle& r )override { return r.height; }
            Splitter* create_splitter ()override;// temp
        };
        class  div_v : public division
        {
        public:
            int&      weigth_c ( rectangle& r )override { return r.y; }
            unsigned& weigth_s ( rectangle& r )override { return r.height; }
            int&       fixed_c ( rectangle& r )override { return r.x; }
            unsigned&  fixed_s ( rectangle& r )override { return r.width; }
            Splitter* create_splitter ()override;// temp
        };
        struct comp_collapse
        {
            bool operator()( const rectangle&a, const rectangle& b )
            {
                if ( a.overlap ( b ) ) return false;
                if ( a.y < b.y )    return true;
                if ( a.y > b.y )    return false;
                if ( a.x < b.x )    return true;
                //if (a.x > b.x)    return false;
                return false;
            };
        };
        struct Cell : div_h  ///\todo:  
        {
            Cell ( const rectangle& r ) : cell ( r ) {}
            rectangle  cells_ () const override { return cell; }
            rectangle  cell;
        };
        class  div_grid : public div_h
        {
        public:
            div_grid ( const std::string& name_, size dim_ ) :name ( name_ ), rows ( dim_.height ), columns ( dim_.width ) {};
            div_grid ( const std::string& name_, unsigned rows_, unsigned columns_ ) :name ( name_ ), rows ( rows_ ), columns ( columns_ ) {};

            Splitter* create_splitter ()override { return nullptr; }// temp

            std::string name; ///< field name to be refered in the field(name)<<room instr.
            unsigned rows, columns;      ///< w=rows and h=columns   dim; 
            using Collapses = std::set<rectangle, comp_collapse> ;
            Collapses collapses_ ;
            Collapses get_collapses ();
            void populate_children ( implement*   place_impl_ ) override;

            void collocate_field ( const rectangle& r ) override
            {
                Ifield::collocate_field ( r );

                double block_w = r.width / double ( columns ); /*weigth_s( area )*/     /// \todo: adapt to vert???
                double block_h = r.height/ double ( rows    ); /*fixed_s ( area )*/  

                for (auto child : children)
                {
                    rectangle area, cr=child->cells() ;
                    area.x = r.x + static_cast<int>(cr.x * block_w);
                    area.y = r.y + static_cast<int>(cr.y * block_h);
                    area.width  =  static_cast<int>(cr.width * block_w);
                    area.height =  static_cast<int>(cr.height * block_h);

                    child->collocate ( area );
                }
                for ( auto & fsn: fastened_in_div )
                    API::move_window ( fsn, r );
            }
        };//end class div_grid

        template <class Adj = adjustable, class Fld = Widget>
        struct Field : Fld, Adj
        {
            void      Adj::collocate ( const rectangle& r ) override { Fld::collocate_field ( r ); }
            rectangle  cells () const override { return cells_ (); }
            void       populate_children ( implement*   place_impl_ )override { Fld::populate_children ( place_impl_ ); }

            // for Gap
            Field ( minmax MinMax = minmax () ) : Adj ( MinMax ) {}
            Field ( unsigned weight, minmax MinMax = minmax () ) : Adj ( weight, MinMax ) {}
            Field ( double percent_, minmax MinMax = minmax () ) : Adj ( percent_, MinMax ) {}

            // for Widget
            Field ( window handle_, minmax MinMax = minmax () ) : Fld ( handle_ ), Adj ( MinMax ) {}
            Field ( window handle_, unsigned weight, minmax MinMax = minmax () ) : Fld ( handle_ ), Adj ( weight, MinMax ) {}
            Field ( window handle_, double percent_, minmax MinMax = minmax () ) : Fld ( handle_ ), Adj ( percent_, MinMax ) {}

            // for Room
            Field ( window handle_, nana::size sz, minmax MinMax = minmax () ) : Fld ( handle_, sz ), Adj ( MinMax ) {}
            Field ( window handle_, nana::size sz, unsigned weight, minmax MinMax = minmax () ) : Fld ( handle_, sz ), Adj ( weight, MinMax ) {}
            Field ( window handle_, nana::size sz, double percent_, minmax MinMax = minmax () ) : Fld ( handle_, sz ), Adj ( percent_, MinMax ) {}

            // for Cell
            Field ( rectangle r, minmax MinMax = minmax () ) : Fld ( r ), Adj ( MinMax ) {}
            Field ( rectangle r, unsigned weight, minmax MinMax = minmax () ) : Fld ( r ), Adj ( weight, MinMax ) {}
            Field ( rectangle r, double percent_, minmax MinMax = minmax () ) : Fld ( r ), Adj ( percent_, MinMax ) {}

            // for div_grid
            Field ( const std::string& name_, size dim_, minmax MinMax = minmax () ) : Adj ( MinMax ), Fld ( name_, dim_ ) {}
            Field ( const std::string& name_, size dim_, unsigned weight, minmax MinMax = minmax () ) : Adj ( weight, MinMax ), Fld ( name_, dim_ ) {}
            Field ( const std::string& name_, size dim_, double percent_, minmax MinMax = minmax () ) : Fld ( name_, dim_ ), Adj ( static_cast<unsigned>(fx*percent_), MinMax ) {}
            Field ( const std::string& name_, unsigned rows_, unsigned columns_, minmax MinMax = minmax () ) : Adj ( MinMax ), Fld ( name_, rows_, columns_ ) {}
            Field ( const std::string& name_, unsigned rows_, unsigned columns_, unsigned weight, minmax MinMax = minmax () ) : Fld ( name_, rows_, columns_ ), Adj ( weight, MinMax ) {}
            Field ( const std::string& name_, unsigned rows_, unsigned columns_, double   percent_, minmax MinMax = minmax () ) : Fld ( name_, rows_, columns_ ), Adj ( static_cast<unsigned>(fx*percent_), MinMax ) {}
        };

        struct Splitter : public Field<fixed, Widget> //fixed_widget
        {
            nana::cursor	            splitter_cursor_{ cursor::arrow };
            place_parts::splitter<true>	splitter_;
            nana::point	                begin_point_;

            percent                     *leaf_left_a;
            adjustable                  *leaf_right_a;
            division                    *parent;
            std::unique_ptr<division>   leaf_left_, leaf_right_;
            dragger	                    dragger_;
            //bool	                    pause_move_collocate_ {false};	//A flag represents whether do move when collocating.
            bool                        splitted{ false };
            double                      init_perc{ 30 };
            nana::arrange               arrange_;

            Splitter ( window wd, double  init_perc = 0.3 ) :
                Field<fixed, Widget> ( nullptr, unsigned ( 4 ) ), init_perc ( init_perc ? init_perc : 0.3 )
            {
                splitter_.create ( wd );
                dragger_.trigger ( splitter_ );
                handle = splitter_.handle ();

                splitter_.events ().mouse_move.connect ( [this]( const arg_mouse& arg )
                {
                    if ( false == arg.left_button )	return;
                    last = rectangle ( splitter_.pos (), splitter_.size () );
                    leaf_left_->weigth_s () = parent->weigth_c ( last ) - parent->weigth_c () ;
                    leaf_left_->collocate_field ( leaf_left_->last );

                    leaf_right_->weigth_c () = parent->weigth_c ( last ) + parent->weigth_s ( last )+1 ;
                    leaf_right_->weigth_s () = parent->weigth_s () - (leaf_right_->weigth_c () -  parent->weigth_c ());
                    leaf_right_->collocate_field ( leaf_right_->last );

                    //API::lazy_refresh();      //API::refresh_window_tree(parent_window_handle);
                    leaf_left_a->setPercent ( double ( leaf_left_->weigth_s () )/parent->weigth_s () );
                } );
            }
            void populate_children ( implement*   place_impl_ )  // ???????????????????
            {
                splitted = false;
            }
            void restrict ( const rectangle &r, nana::arrange   arrange )
            {
                arrange_ = arrange;
                restrict ( r );
            }
                void restrict ( rectangle  r )
            {
                dragger_.target ( splitter_, r.pare_off ( 1 ), arrange_ );
            }

                void  collocate_field ( const rectangle& r )override
            {
                Field<fixed, Widget>::collocate ( r );
            }
        };

        struct implement           //struct implement
        {
            std::string                     curr_field;
            minmax                          curr_minmax;
            window                          parent_window_handle{ nullptr };
            event_handle                    event_size_handle{ nullptr };
            std::unique_ptr<adjustable>     root_division;
            std::unordered_set<std::string> names;          ///<  All the names defines. Garant no repited name.
            unsigned                        div_numer{ 0 }; ///<  Used to generate unique div name.
            bool                            recollocate{ false }; /// we need to rebuilt the children of each division??

            //std::multimap<std::string,std::unique_ptr< adjustable>> fields;    
            std::multimap<std::string, window    >  fastened;

            ///  labels automaticaly created "on the fly" by field(name)<<"myLabel";
            std::vector<nana::label*>   labels;

            ~implement () { API::umake_event ( event_size_handle ); }

            void                       collocate ();
            void                       div ( const char* s );
            bool                       bind ( window parent_widget );
            std::unique_ptr<adjustable>  scan_div ( tokenizer& );

            /// Generate and registre in the set a new unique div name from the current layout
            bool          add_field_name ( const std::string& n )  /// is unique? (registre it)
            {
                if ( names.insert ( n ).second )
                    return true;
                else return false;//trow name repit;
            }
            std::string   registre ( std::unique_ptr<adjustable> adj )
            {
                auto n = add_div_name_ ();
                registre ( n, std::move ( adj ) );
                return n;
            }

            void registre ( std::string name, std::unique_ptr<adjustable> adj )
            {
                //std::cout << "\nRegistering: " << name<< "(Min=" << adj->min<<", Max="<<adj->max<<"). Type: "<< std::type_index(typeid(*adj)).name();
                fields_.emplace ( name, std::move ( adj ) );
                recollocate = true;
            }
            
            void add_label ( const nana::string& txt )
            {
                nana::label*lab = std::addressof ( nana::form_loader <nana::label> ()(parent_window_handle, txt) );
                labels.push_back ( lab );
                add ( new Field< adjustable, Widget> (*lab) );
            }            
            void add ( adjustable * fld )
            {
                fld->MinMax ( curr_minmax );      //std::cout<< "\n Add "<<name<<" adjustable to : Min=" <<fld->min;
                _m_make_destroy ( fld );
                registre ( curr_field, std::unique_ptr<adjustable> ( fld ) );
                recollocate = true;                //return *this;
            }
            void fasten ( window wd )              
            {
                fastened.emplace ( curr_field, wd );
                recollocate = true;
                //Listen to destroy of a window. The deleting a fastened window
                //does not change the layout.
                auto dtr = API::events ( wd ).destroy.connect ( [this]( const arg_destroy& ei )
                {
                    for ( auto f = fastened.begin (), end = fastened.end (); f!=end; ++f )
                        if ( f->second ==  ei.window_handle )
                        {
                        fastened.erase ( f );    // delete ???
                        recollocate = true;
                        return;
                        }
                } );
                //API::make_event<events::destroy>(wd, [dtr](const eventinfo& ei)
                //{
                //    API::umake_event(dtr); 
                //});	
            }


        private:
            /// All the named fields defined by user with field(name)<<IField, plus the automatic division finded from the layot in div()
            std::multimap<std::string, std::unique_ptr< adjustable>> fields_;
            std::string   add_div_name_ ()
            {
                std::string name = std::to_string ( div_numer++ );
                assert ( names.insert ( name ).second );
                return name;
            }

        public:
            /// return all the fields associated whit this name (a par of iterators)
            auto find ( std::string name )-> decltype(fields_.equal_range ( name ))
            {
                return fields_.equal_range ( name );
            }
            /// Listen to destroy of a window. It will delete the element and recollocate when the window is destroyed.
            void _m_make_destroy ( adjustable *fld )
            {
                auto fd = dynamic_cast<Widget *>(fld);
                if ( !fd || !fd->handle ) return;
                fd->destroy_evh = API::events ( fd->handle ).destroy.connect ( [this]( const arg_destroy& ei )
                {
                    for ( auto f = fields_.begin (), end = fields_.end (); f!=end; ++f )
                        if ( Widget *fd = dynamic_cast<Widget *>(f->second.get ()) )
                            if ( fd->window_handle () ==  ei.window_handle )
                            {
                        fields_.erase ( f );    // delete ???
                        recollocate = true;
                        collocate ();
                        return;
                            }
                } );
            }
        };	      //struct implement


        void division::split ()
        {
            if ( !splitter || !create_splitter () ) return;

            auto cb = children.begin ();
            auto ce = children.end ();
            auto spl = std::find ( cb, ce, splitter );
            assert ( spl!=ce );
            auto &l = *splitter->leaf_left_;
            auto &r = *splitter->leaf_right_;

            l.children = Children ( cb, spl ) ;
            r.children = Children ( spl+1, ce ) ;
            children = Children{ splitter->leaf_left_a, splitter, splitter->leaf_right_a };
            splitter->splitted = true;
        }
        Splitter * div_h::create_splitter ()
        {
            splitter->restrict ( last, nana::arrange::horizontal );

            if ( splitter->splitted ) return nullptr;
            splitter->splitter_.cursor ( cursor::size_we );

            auto div_l = std::make_unique<Field<percent, div_h>> ( splitter->init_perc );

            splitter->leaf_left_a = div_l.get ();
            splitter->leaf_left_.reset ( div_l.release () );

            auto div_r = std::make_unique<Field<adjustable, div_h>> () ;

            splitter->leaf_right_a = div_r.get ();
            splitter->leaf_right_.reset ( div_r.release () );

            return splitter ;
        }
        Splitter * div_v::create_splitter ()
        {
            splitter->restrict ( last, nana::arrange::vertical );

            if ( splitter->splitted ) return nullptr;
            splitter->splitter_.cursor ( cursor::size_ns );

            auto div_l = std::make_unique<Field<percent, div_h>> ( splitter->init_perc );

            splitter->leaf_left_a = div_l.get ();
            splitter->leaf_left_.reset ( div_l.release () );

            auto div_r = std::make_unique<Field<adjustable, div_v>> () ;

            splitter->leaf_right_a = div_r.get ();
            splitter->leaf_right_.reset ( div_r.release () );

            return splitter ;
        }
        div_grid::Collapses div_grid::get_collapses ()
        {
            Collapses c;
            for ( rectangle r : collapses_ )
                if ( r.x >= columns || r.y >= rows )  // >=   ?????
                    continue;
                else
                {
                    if ( r.right () > columns ) r.width  = columns - r.x  ;
                    if ( r.bottom() > rows    ) r.height = rows    - r.y  ;
                    c.insert ( r );
                }
            return c;
        }
        bool division::insert_gap ( int gap_index )
        {
            if ( gap.empty () ) return false;
            auto &gp = gap.at ( gap_index );
            bool added = false;
            switch ( gp.kind_of () )
            {
                case number_t::kind::percent:
                    if ( gp.real () > 0 )
                    {
                        owned.emplace_back ( new Field<percent, Gap> ( gp.real () ) );
                        added = true;
                    } break;
                case number_t::kind::integer:
                case number_t::kind::real:
                    if ( gp.integer () > 0 )
                    {
                        owned.emplace_back ( new Field<fixed, Gap> ( unsigned ( gp.integer () ) ) );
                        added = true;
                    } break;
                case number_t::kind::none:
                    if ( !gap.pass_end ( gap_index ) )
                    {
                        owned.emplace_back ( new Field<adjustable, Gap> () );
                        added = true;
                    } break;

                default:
                    break;
            }
            if ( added ) children.push_back ( owned.back ().get () );
            return added;
        }
        bool division::set_arrange ( const repeated_array& arrang_array, int arr_index, adjustable* field )       ///< betwen fields
        {
            if ( arrang_array.empty () ) return false;    //?????
            auto &arr = arrang_array.at ( arr_index );
            div_h* div{};
            adjustable *adj;
            switch ( arr.kind_of () )
            {
                case number_t::kind::percent:
                    if ( arr.real () > 0 )
                    {
                        auto fld = new Field<percent, div_h> ( arr.real () ) ;
                        div = fld; adj = fld;
                    }
                    break;
                case number_t::kind::integer:
                case number_t::kind::real:
                    if ( arr.integer () > 0 )
                    {
                        auto fld = new Field<fixed, div_h> ( unsigned ( arr.integer () ) ) ;
                        div = fld; adj = fld;
                    }
                    break;
                case number_t::kind::none:
                    if ( !arrang_array.pass_end ( arr_index ) )
                    {
                        auto fld = new Field<adjustable, div_h> () ;
                        div = fld; adj = fld;
                    }
                    break;
                default:
                    break;
            }
            if ( div )
            {
                owned.emplace_back ( adj );
                children.push_back ( adj );
                div->children.push_back ( field );
            }
            return div;
        }
        /// \brief Completely (re)built the division, constructing the children (vector of adjustable fields) 
        /// only from the (vector) field names in division and the place global multimap name/field
        void division::populate_children ( implement*   place_impl_ )
        {
            fastened_in_div.clear ();               /// the vector of direct windows (not resized) is (re)built to.
            children.clear ();                      /// .clear(); the children or it is empty allways ????
            owned.clear ();
            int gap_index{ 0 } ;
            for ( const auto &name : field_names )    /// for all the names in this div
            {                                       /// find in the place global list of fields all the fields attached to it
                int  arrange_index{ 0 };
                auto arr_it = arrange_.find ( name );
                auto r = place_impl_->find ( name );
                for ( auto fi = r.first ; fi != r.second ; ++fi )   /// fi iterator that point to unique_ptr<IField>   
                {
                    if ( fi!=r.first )                /// add posible gaps betwen fields,
                        if ( insert_gap ( gap_index ) )
                            gap_index++;
                    auto field = fi->second.get ();      /// a ref to the unique_ptr<IField> 
                    if ( arr_it!=arrange_.end () && set_arrange ( arr_it->second, gap_index, field ) )
                        arrange_index++;
                    else
                        children.push_back ( field );       /// add the finded field to form the div children

                    field->populate_children ( place_impl_ ); /// and let the child field to populate recursively his own children
                }
                auto f = place_impl_->fastened.equal_range ( name );
                for ( auto fi = f.first ; fi != f.second ; ++fi )
                    fastened_in_div.push_back ( fi->second );
            }
            split (); /// check if this division have an split and implement it
        }
        void div_grid::populate_children ( implement*   place_impl_ )
        {
            fastened_in_div.clear ();               /// the vector of direct windows (not resized) is (re)built to.
            children.clear ();                      /// .clear(); the children or it is empty allways ????
            owned.clear ();
            int gap_index{ 0 } ;
            int  arrange_index{ 0 };
            auto arr_it = arrange_.find ( name );
            auto r = place_impl_->find ( name );

            if ( !rows || !columns )
            {
                unsigned min_n_of_cells = std::accumulate( r.first, r.second, 0, 
                                      [](unsigned mc, decltype(*r.first)& cell)
                                        { return mc + cell.second->cells().width * cell.second->cells().width; } );
                unsigned dc{},dr{};
                if (!rows) dr=1; else dc=1;
                while ( columns*rows <= min_n_of_cells )  
                {    
                    rows +=dr;
                    if ( columns*rows > min_n_of_cells )  break;
                    columns +=dc; 
                }
            }
            auto cells = get_collapses ();
            for ( int c = 0; c < columns; ++c )
                for ( int r = 0; r < rows; ++r )
                    cells.insert ( rectangle ( c, r, 1, 1 ) );

            auto cell = cells.begin () ;
            for ( auto fi = r.first ; fi != r.second && cell!=cells.end () ; ++fi, ++cell )   /// fi iterator that point to unique_ptr<IField>   
            {
                auto field = fi->second.get ();      /// a ref to the unique_ptr<IField>, the field that come in this cell

                auto  cell_field = new Field<adjustable, Cell> ( *cell ) ;
                Cell  *cell_div{ cell_field };    /// create a div where the gap will live
                adjustable *adj{ cell_field };
                owned.emplace_back ( adj );
                children.push_back ( adj );
                if ( cell->x )                /// add posible horizontal gap betwen fields. And the vertical  ??????
                    if ( cell_div->insert_gap ( gap_index ) )
                        gap_index++;
                if ( arr_it!=arrange_.end () && cell_div->set_arrange ( arr_it->second, gap_index, field ) )
                    arrange_index++;
                else
                    cell_div->children.push_back ( field );       /// add the finded field to form the div children

                field->populate_children ( place_impl_ ); /// and let the child field to populate recursively his own children
            }
            auto f = place_impl_->fastened.equal_range ( name );
            for ( auto fi = f.first ; fi != f.second ; ++fi )
                fastened_in_div.push_back ( fi->second );
            split (); /// check if this division have an split and implement it  ????????????
        }

        void implement::div ( const char* s )
            {
            names.clear ();
            while ( div_numer )
                fields_.erase ( std::to_string ( --div_numer ) );


            tokenizer tknizer ( s );
            root_division = scan_div ( tknizer ) ;//  .reset(dynamic_cast<division*>(scan_div(tknizer).get()));
            for ( auto &f: fields_ )
                //std::cout<< "\n Added ------------ "<<f.first <<" adj :------ Min="<< f.second->min;// <<ld.get()->min;
                recollocate = true;
        }
        std::unique_ptr<adjustable> implement::scan_div ( tokenizer& tknizer )
        {
            typedef tokenizer::token token;

            token       div_type = token::eof;
            number_t    weight;  // , gap;
            bool        have_gap{ false }, have_weight{ false }, margin_for_all{ true };
            repeated_array arrange, gap;

            minmax      w;
            std::string gr_name;
            Splitter    *splitter{ nullptr };

            //std::vector<rectangle> collapses;
            std::set<rectangle, comp_collapse> collapses;
            unsigned rows = 1, columns = 1;

            std::vector<number_t>    array, margin;
            std::vector<std::string> field_names_in_div;
            std::unordered_map<std::string, repeated_array> arrange_;


            for ( token tk = tknizer.read (); tk != token::eof && tk!=token::div_end ; tk = tknizer.read () )
            {
                switch ( tk )
                {
                    case token::div_start:
                        field_names_in_div.push_back ( registre ( scan_div ( tknizer ) ) );
                        break;
                    case token::splitter:
                    {       // Use only the first splitter. with some fields at the left??
                        if ( !splitter ) // && (  field_names_in_div.size())
                        {
                            double p = 0 ;
                            if ( tknizer.number ().kind_of () == number_t::kind::percent )
                                p = 1-tknizer.number ().real ();
                            std::unique_ptr<Splitter> spl = std::make_unique<Splitter>
                                ( parent_window_handle, p );
                            splitter = spl.get ();
                            //std::cout<< "\n Add Splitter:" ;
                            field_names_in_div.push_back ( registre ( std::move ( spl ) ) );
                            /// we can move spl because splitter is only used as a bool, and not dereferenced
                        }
                    }                                                           break;
                    case token::array:		    tknizer.array ().swap ( array );   		break;
                    case token::identifier:
                    {
                        std::string field_name ( tknizer.idstr () );
                        if ( add_field_name ( field_name ) )
                            field_names_in_div.push_back ( field_name );
                        else
                            throw std::runtime_error ( "place, the name '"
                            + field_name + "' is redefined." );
                        /* throw repeated name in layout !!!!!!! */        break;
                    }
                    case token::horizontal:
                    case token::vertical:    	div_type = tk;		   		        break;
                    case token::grid:			div_type = tk;
                    {
                        gr_name = field_names_in_div.back ();  /// \todo Find the last user defined name !!! if not exist wait until end of division and take the name of the div??                  
                        number_t c, r;
                        switch ( tknizer.read () )
                        {
                            case token::number:
                                c = tknizer.number ();
                                r = tknizer.number ();                  break;
                            case token::array:
                                if ( tknizer.array().size () > 0 ) c = tknizer.array()[0];
                                if ( tknizer.array().size () > 1 ) r = tknizer.array()[1];   
                                                                        break;
                            case token::reparray:
                                c = tknizer.reparray ().at ( 0 );
                                r = tknizer.reparray ().at ( 1 );       break;
                            default:                                    break;
                        }
                        if ( c.kind_of () != number_t::kind::percent )
                            columns = c.is_negative() ? 0 : c.integer ();
                        if ( r.kind_of () != number_t::kind::percent )
                            rows    = r.is_negative() ? 0 : r.integer ();
                    }                                                    break;
                    case token::collapse:
                        if ( tknizer.parameters ().size () == 4 )
                        {
                            auto get_number = []( const number_t & arg, const std::string& nth )
                            {
                                if ( arg.kind_of () == number_t::kind::integer )
                                    return arg.integer ();
                                else if ( arg.kind_of () == number_t::kind::real )
                                    return static_cast<int>(arg.real ());

                                throw std::runtime_error ( "place: the type of the "+ nth +" parameter for collapse should be integer." );
                            };

                            ::nana::rectangle col;
                            auto arg = tknizer.parameters ().at ( 0 );
                            col.x = get_number ( arg, "1st" );

                            arg = tknizer.parameters ().at ( 1 );
                            col.y = get_number ( arg, "2nd" );

                            arg = tknizer.parameters ().at ( 2 );
                            col.width = static_cast<decltype(col.width)>(get_number ( arg, "3rd" ));

                            arg = tknizer.parameters ().at ( 3 );
                            col.height = static_cast<decltype(col.height)>(get_number ( arg, "4th" ));

                            //Check the collapse area.
                            //Ignore this collapse if its area is less than 2(col.width * col.height < 2)
                            if ( col.width * col.height > 1  &&  col.x >= 0 && col.y >= 0 )
                                collapses.insert ( col );
                        } else
                            throw std::runtime_error ( "place: collapse requires 4 parameters." );
                        break;

                    case token::weight:	weight = tknizer.number (); have_weight = true; break;
                        //case token::gap:	   gap = tknizer.number();have_gap=true;    break;
                    case token::gap:	    gap = tknizer.reparray (); have_gap = true; break;   // ???????
                    case token::min:
                    {
                        if ( tknizer.number ().kind_of () != number_t::kind::percent )
                            w.min = tknizer.number ().integer ();
                        /*else throw no min percent possible ??? */             break;
                    }
                    case token::max:
                    {
                        if ( tknizer.number ().kind_of () != number_t::kind::percent )
                            w.max = tknizer.number ().integer ();
                        /*else throw no max percent possible ??? */             break;
                    }
                    case token::arrange:   /// \todo Find the last user defined name !!! if not exist wait until end of division and take the name of the div??                  
                        arrange_[field_names_in_div.back ()] = tknizer.reparray ();  break;

                    case token::margin:
                    {
                        switch ( tknizer.read () )
                        {
                            case token::number:
                                margin = std::vector<number_t> ( 4, tknizer.number () );
                                break;
                            case token::array:
                                tknizer.array ().swap ( margin );       break;
                            case token::reparray:
                                for ( std::size_t i = 0; i < 4; ++i )
                                {
                                    auto n = tknizer.reparray ().at ( i );
                                    if ( n.is_none () ) n.assign ( 0 );
                                    margin.emplace_back ( n );
                                }
                                break;

                            default:					            break;
                        }
                    }				                                             break;

                    default:	break;
                }
            }   // token::div_end

            if ( div_type == token::grid )
                if ( gr_name.empty () )     gr_name = field_names_in_div.back ();

            std::unique_ptr<adjustable> div;
            if ( weight.kind_of () == number_t::kind::percent && weight.real () > 0 )
            {
                double perc = weight.real () ;
                switch ( div_type )
                {
                    case token::eof:
                    case token::horizontal: div.reset ( new Field<percent, div_h> ( perc, w ) );  break;
                    case token::vertical:   div.reset ( new Field<percent, div_v> ( perc, w ) );	 break;
                    case token::grid:       div.reset ( new Field<percent, div_grid> ( gr_name,  rows,
                        columns, perc,w ) ); break;
                    default:
                        throw std::runtime_error ( "nana.place: invalid division type." );
                }
            } else
            {
                unsigned fix = weight.integer ();
                if ( fix )
                    switch ( div_type )
                {
                        case token::eof:
                        case token::horizontal:	 div.reset ( new Field<fixed, div_h> ( fix, w ) ); break;
                        case token::vertical:	 div.reset ( new Field<fixed, div_v> ( fix, w ) ); break;
                        case token::grid:        div.reset ( new Field<fixed, div_grid> ( gr_name, 
                            rows, columns, fix, w ) );        break;
                        default:
                            throw std::runtime_error ( "nana.place: invalid division type." );
                } else
                    switch ( div_type )
                {
                        case token::eof:
                        case token::horizontal:	 div.reset ( new Field<adjustable, div_h> ( w ) );	 break;
                        case token::vertical:	 div.reset ( new Field<adjustable, div_v> ( w ) );	 break;
                        case token::grid:        div.reset ( new Field<adjustable, div_grid> ( gr_name,
                            rows, columns, w ) );          break;
                        default:
                            throw std::runtime_error ( "nana.place: invalid division type." );
                }
            }
            auto pdiv = dynamic_cast<division*>(div.get ());
            assert ( pdiv );

            if ( have_gap ) { pdiv->gap = gap; }

            pdiv->field_names.swap ( field_names_in_div );

            if ( splitter )
            {
                pdiv->splitter = splitter;
                splitter->parent = pdiv;
            }

            pdiv->arrange_.swap ( arrange_ );

            if ( div_type == token::grid )
                static_cast<div_grid*>(pdiv)->collapses_.swap ( collapses );
            if ( margin.size () )
            {
                std::unique_ptr<division> vd, td, hd, ld, cd, rd, bd;


                // vert div
                vd.reset ( new Field<adjustable, div_v> );

                //top :  margin[0]
                if ( margin[0].kind_of () == number_t::kind::percent )
                    td.reset ( new Field<percent, div_h> ( margin[0].real () ) );
                else
                    td.reset ( new Field<fixed, div_h> ( unsigned ( margin[0].integer () ) ) );

                //std::cout<< "\n Add top margin:";// <<ld.get()->min;
                vd->field_names.push_back ( registre ( std::unique_ptr<adjustable> ( dynamic_cast<adjustable*>(td.release ()) ) ) );


                // central div
                switch ( div_type )
                {
                    case token::eof:
                    case token::horizontal:	 cd.reset ( new Field<adjustable, div_h> );	                    break;
                    case token::vertical:	 cd.reset ( new Field<adjustable, div_v> );	                    break;
                    case token::grid:        cd.reset ( new Field<adjustable, div_grid> ( gr_name, rows, columns ) ); break;
                    default:
                        throw std::runtime_error ( "nana.place: invalid division type." );
                }
                cd->field_names.swap ( pdiv->field_names );
                std::swap ( cd->splitter, pdiv->splitter );

                int lm, rm, bm;

                switch ( margin.size () )
                {
                    case 1: lm = rm = bm = 0; break;
                    case 2: lm = 1; rm = bm = 0; break;
                    case 3: rm = 1; bm = 2; lm = 0; break;
                    case 4: rm = 1; bm = 2; lm = 3; break;
                    default:
                        throw std::runtime_error ( "nana.place: invalid numer of margins." );
                }

                if ( lm || rm )
                {
                    //  horizontal central "row": left margin + central + rigth margin   
                    hd.reset ( new Field<adjustable, div_h> );

                    if ( lm )
                    {
                        //left  
                        if ( margin[lm].kind_of () == number_t::kind::percent )
                            ld.reset ( new Field<percent, div_h> ( margin[lm].real () ) );
                        else
                            ld.reset ( new Field<fixed, div_h> ( unsigned ( margin[lm].integer () ) ) );

                        //std::cout<< "\n Add left margin:"; 
                        hd->field_names.push_back ( registre ( std::unique_ptr<adjustable> ( dynamic_cast<adjustable*>(ld.release ()) ) ) );
                    }

                    // central  
                    //std::cout<< "\n Add central, real div betwen margins:"; 
                    hd->field_names.push_back ( registre ( std::unique_ptr<adjustable> ( dynamic_cast<adjustable*>(cd.release ()) ) ) );

                    if ( rm )
                    {
                        // rigth  
                        if ( margin[rm].kind_of () == number_t::kind::percent )
                            rd.reset ( new Field<percent, div_h> ( margin[rm].real () ) );
                        else
                            rd.reset ( new Field<fixed, div_h> ( unsigned ( margin[rm].integer () ) ) );

                        //std::cout<< "\n Add rigth margin:"; 
                        hd->field_names.push_back ( registre ( std::unique_ptr<adjustable> ( dynamic_cast<adjustable*>(rd.release ()) ) ) );
                    }


                    //std::cout<< "\n Add horizontal central row: left margin + central + rigth margin:"; 
                    vd->field_names.push_back ( registre ( std::unique_ptr<adjustable> ( dynamic_cast<adjustable*>(hd.release ()) ) ) );
                } else
                {
                    // only central  
                    //std::cout<< "\n Add only central row: no left margin or rigth margin:"; 
                    vd->field_names.push_back ( registre ( std::unique_ptr<adjustable> ( dynamic_cast<adjustable*>(cd.release ()) ) ) );
                }

                if ( bm )
                {
                    // botton  
                    if ( margin[bm].kind_of () == number_t::kind::percent )
                        bd.reset ( new Field<percent, div_h> ( margin[bm].real () ) );
                    else
                        bd.reset ( new Field<fixed, div_h> ( unsigned ( margin[bm].integer () ) ) );

                    //std::cout<< "\n Add botton margin:"; 
                    vd->field_names.push_back ( registre ( std::unique_ptr<adjustable> ( dynamic_cast<adjustable*>(bd.release ()) ) ) );
                }
                //std::cout<< "\n Add vertical div for margin:"; 
                pdiv->field_names.push_back ( registre ( std::unique_ptr<adjustable> ( dynamic_cast<adjustable*>(vd.release ()) ) ) );
            }

            return div;
        } // scan_div
        void implement::collocate ()
        {
            if ( root_division && parent_window_handle )
            {
                rectangle r ( API::window_size ( this->parent_window_handle ) );  //debugg
                if ( r.width && r.height )
                {
                    //auto re=recollocate;
                    if ( recollocate )
                        root_division->populate_children ( this );
                    root_division->collocate ( r );  /* r=API::window_size(this->parent_window_handle)*/
                    //if (re)       
                    //{    
                    //    API::lazy_refresh();
                    //    API::refresh_window_tree(parent_window_handle);
                    //    API::update_window(parent_window_handle);
                    //}
                    recollocate = false;

        }
            }
        }
    } // namespace place_impl

    vplace::vplace ( window wd ) : impl_ ( new implement ) { bind ( wd ); }
    vplace::vplace () : impl_ ( new implement ) {}
    vplace::~vplace () { delete impl_; }
    void        vplace::div ( const std::string& s ) { impl_->div ( s.c_str () ); }
    void        vplace::collocate () { impl_->collocate (); }

    adjustable&         vplace::fixed ( window wd, unsigned size )
    {
        return *new vplace_impl::Field<vplace_impl::fixed, vplace_impl::Widget> ( wd, size );//fixed_widget
    }
    adjustable&         vplace::percent ( window wd, double per, minmax w )
    {
        return *new vplace_impl::Field<vplace_impl::percent, vplace_impl::Widget> ( wd, per, w );//percent_widget
    }
    adjustable&         vplace::room ( window wd, nana::size sz)
    {
        return *new vplace_impl::Field<vplace_impl::adjustable, vplace_impl::Room > ( wd, sz );//adj_room
    }
    vplace::minmax::minmax ( unsigned Min, unsigned Max ) : min ( Min ), max ( Max ) {}
    void vplace::set_target_field (std::string name)
    {
        impl_->curr_field = name;
    }
    //vplace& vplace::field ( std::string name )
    //{
    //    impl_->curr_field = name;
    //    //impl_->temp_field_t.reset ( new vplace_impl::field_impl ( this->impl_, name ) );
    //    return *this; // *impl_->temp_field_t;
    //}

    void vplace::bind ( window wd )
    {
        if ( impl_->bind ( wd ) )
            throw std::runtime_error ( "place.bind: it has already binded to a window." );
    }
    bool implement::bind ( window wd )
    {
        assert ( !parent_window_handle );
        if ( parent_window_handle ) return true;
        parent_window_handle = wd;
        //  rectangle r;  //debugg
        //  r=API::window_size(this->parent_window_handle);  //debugg
        //  std::cerr<< "\nplace(parent_widget [ "<<parent_widget<<" ]) with area: "<<r;  //debugg

        event_size_handle = API::events ( parent_window_handle ).resized.connect ( [this]( const arg_resized&ei )
        {
            this->collocate ();
            //        //std::cerr<< "\nResize: collocating root div ??:[ "<<this->parent_window_handle<<" ]) with event :[ "//debug
            //        //         <<ei.window <<" ]) ";  //debug
            //        if(this->root_division)
            //        {
            //rectangle r(API::window_size(this->parent_window_handle));  //debugg
            //            //if(r.width && r.height ) 
            //               this->root_division->collocate(r/*=API::window_size(this->parent_window_handle)*/);
            //            //std::cerr<< "\ncollocating root div  [ "<<this->parent_window_handle<<" ]) with area: "<<r;  //debugg
            //        }
        } );
        return false;
    }

    vplace& vplace::operator<<(std::string txt)	 
    {
        impl_->add_label ( nana::charset ( txt ) );      
        return *this;  
    };
    vplace& vplace::operator<<(std::wstring txt)	 
    {
        impl_->add_label ( nana::charset ( txt ) );      
        return *this;  
    };
    vplace& vplace::operator<<(minmax Size_range)	 
    {
        impl_->curr_minmax = Size_range;      
        return *this;  
    };
    vplace& vplace::operator<<(adjustable& fld) 
    {
        impl_->add ( &fld );   
        return *this;  
    };
    vplace& vplace::operator<<(window   wd)  
    {
        impl_->add ( new vplace_impl::Field< adjustable, vplace_impl::Widget> (wd) );
        return *this;  
    };
    vplace& vplace::operator<<(unsigned gap) 
    {
        impl_->add ( new vplace_impl::Field<vplace_impl::fixed, vplace_impl::Gap> ( gap )) ;
        return *this;  
    };
    vplace& vplace::fasten ( window wd )    
    {
        impl_->fasten ( wd );
        return *this;  
    };

	void vplace::field_display(std::string name, bool dsp)
	{
		auto div = impl_->search_div_name(impl_->root_division.get(), name);
		if (div)
			div->set_display(dsp);
	}
	bool vplace::field_display(std::string name) const
	{
		auto div = impl_->search_div_name(impl_->root_division.get(), name);
		return (div && div->display);
	}


}//end namespace nana
