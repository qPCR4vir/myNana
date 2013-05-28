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
    NumerUpDown (form &fm, const string &label, const string   &DefFileName=STR("NumUpDown.VertCenter.lay.txt")  )
        :	panel<false>(fm),
            EditableWidget( *this, label, DefFileName),
			_num(*this), _up(*this), _down(*this), _label(*this)
    {
        _up.caption (STR("^"));
        _down.caption (STR("v"));
        _label.caption (label);
        _label.text_align(align::right  ); 

        InitMyLayout();
        SelectClickableWidget( _label);
        SelectClickableWidget( _num);

    }

     void SetDefLayout       () override
    {
        _DefLayout= "  <       <vertical weight=60 <><label weight=15><> >"
                     "          <vertical                     <><Num weight=19><> >  "
                       "        <vertical weight=35 <><UpDown weight=19><> > "
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
