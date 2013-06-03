#ifndef NANA_GUI_Numer_HPP
#define NANA_GUI_Numer_HPP

#include <../temp/EditableForm.hpp>
#include <../temp/Units.hpp>

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

    NumerUpDown (   form &fm,      const string &label, 
                    double val, double min, double max, 
                    const string   &DefFileName=STR("NumUpDown.VertCenter.lay.txt"), 
                    double step=1, unsigned decimals=2  );
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


class UnitPicker : public nana::gui::combox
{
    CUnit::unit_name _defUnit;
    const CUnit::magnitude_name magnitude;
  public: 
     //nana::gui::combox _cb;
     UnitPicker(nana::gui::widget &wd, const CUnit::unit_name& def)
          :/*_cb*/nana::gui::combox(wd), _defUnit(def), magnitude(CUnit::UnitsDic().at(def).magnitude )
      {
          /*_cb.*/editable(false);
          for(const CUnit::unit_name& un : CUnit::MagnitudesDic().at(magnitude) )
              /*_cb.*/push_back (nana::charset ( un/*CUnit::UnitsDic().at(un).to_string ()*/  ));
          /*_cb.*/caption(nana::charset (def));
          //_cb.ext_event().selected=[&](nana::gui::combox& cb)
          //{
          //    _cb.caption(_cb.caption().substr(6, _cb.caption().find_first_of(STR(" ="),6)-6 )); 
          //};

      }

};





#endif 
