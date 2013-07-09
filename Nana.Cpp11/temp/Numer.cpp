#include <../temp/Numer.hpp>

namespace nana { namespace gui {

NumerUpDown::NumerUpDown (  widget &parent_,      const string &label, 
                            double val,           double min, double max, 
                            const string         &DefFileName/*=STR("NumUpDown.VertCenter.lay.txt")*/, 
                            double step/*=1*/,       unsigned width/*=6*/,    unsigned decimals/*=2*/  ) 
        :	CompoWidget( parent_, label, DefFileName),
			_num(*this), _up(*this, STR("^") ), _down(*this, STR("v") ), _label(*this,label),
            _val(val), _min(min), _max(max), _step(step), _decimals(decimals), _width(width)
    {
        _num.multi_lines(false);
        UpDate();        
        _label.text_align(align::right  ); 

        InitMyLayout();
        SelectClickableWidget( *this);
        SelectClickableWidget( _label);
        SelectClickableWidget( _num);

          _up.make_event <events::click>    ([&](){add( _step); });
          _up.make_event <events::dbl_click>([&](){add( _step); });
        _down.make_event <events::click>    ([&](){add(-_step); });
        _down.make_event <events::dbl_click>([&](){add(-_step); });
         _num.make_event <events::focus>([&](const eventinfo& ei)
                {  if (!ei.focus.getting) 
                        add( 0    );
                }); 
    }

}} // namespace nana { namespace gui {

