#ifndef NANA_GUI_Numer_HPP
#define NANA_GUI_Numer_HPP

#include <../temp/EditableForm.hpp>

namespace nana { namespace gui {

class NumerUpDown : public  panel<false> , public EditableWidget
{
public:
    textbox _num;
    button  _up, _down;
    label   _label;
    double  _val, _min, _max, _step;
    unsigned _decimals;
    //std::function <void()> &active,&inactive; 

    NumerUpDown (form &fm, const string &label, double val, double min, double max, 
                 const string   &DefFileName=STR("NumUpDown.VertCenter.lay.txt"), double step=1, unsigned decimals=2  )
        :	panel<false>(fm),
            EditableWidget( *this, label, DefFileName),
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
void add(double step)
{
    try    {  _val=std::stod (_num.caption()  );     }
    catch (...)     {;     }
    _val += step;
    if     (_val < _min)   _val = _min;
    else if(_val > _max)   _val = _max;
    UpDate();
}
    void UpDate()
{
    string val(50,0);
    swprintf(&val[0],val.size(), STR(" %*.*f"), 15, _decimals, _val );
    _num.on_first_change               ([&](){});
       _num.caption (val.c_str());
    _num.on_first_change               ([&](){add( 0    ); });
}

    double Value(          ){                    return _val;}
    double Value(double val){_val=val; UpDate(); return _val;}

     void SetDefLayout       () override
    {
        _DefLayout= "  <       <vertical weight=60 <><label weight=15 gap=1><> >      \n"
                    "          <vertical weight=50 <><Num weight=19><> >   \n"
                    "          <vertical weight=30 <><UpDown weight=19><> > \n"
                    "  > " ;
    }
     void AsignWidgetToFields() override
    {
	    _place.field("Num"    ) << _num ;
	    _place.field("UpDown" ) << _up << _down ;
	    _place.field("label"  ) << _label;
    }


};
}} // namespace nana { namespace gui {

#endif 
