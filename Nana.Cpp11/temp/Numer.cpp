#include <../temp/Numer.hpp>

namespace nana { namespace gui {

NumerUpDown::NumerUpDown (  widget &fm, const string &label, double val, double min, double max, 
                            const string   &DefFileName,   double step, unsigned decimals  )
        :	CompoWidget( fm, label, DefFileName),
			_num(*this), _up(*this), _down(*this), _label(*this),
            _val(val), _min(min), _max(max), _step(step), _decimals(decimals)
    {
        _num.multi_lines(false);
        string Val(50,0);
        swprintf( &Val[0],Val.size(), STR(" %*.*f"), 15, _decimals, _val );
        _up.caption (STR("^"));
        _down.caption (STR("v"));
        _label.caption (label);
        _label.text_align(align::right  ); 

        InitMyLayout();
        _num.caption (Val.c_str());
        SelectClickableWidget( _label);
        SelectClickableWidget( _num);

        //auto active=[&](){add( 0    ); };
        //auto active=[&](){add( 0    ); };

         //_num.on_first_change               ([&](){add( 0    ); });
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

