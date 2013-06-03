#ifndef NANA_GUI_Numer_HPP
#define NANA_GUI_Numer_HPP

#include <../temp/EditableForm.hpp>
#include <../temp/Units.hpp>

namespace nana { namespace gui {

class NumerUpDown : public  panel<false> , public EditableWidget
{
  public:
    NumerUpDown (   widget &fm,      const string &label, 
                    double val, double min, double max, 
                    const string   &DefFileName=STR("NumUpDown.VertCenter.lay.txt"), 
                    double step=1, unsigned decimals=2  );
    double Value(          )const{                    return _val;}
    double Value(double val)     {_val=val; UpDate(); return _val;}
    double Min(          )const{                    return _min;}
    double Min(double val)     {_min=val; /*UpDate();*/ return _min;}
    double Max(          )const{                    return _max;}
    double Max(double val)     {_max=val; /*UpDate(); */return _max;}
    //double Step(          )const{                    return _step;}
    //double Step(double val)     {_step=val; /*UpDate(); */return _step;}
  private:
    textbox _num;
    button  _up, _down;
    label   _label;
    double  _val, _min, _max, _step;
    unsigned _decimals;
    //std::function <void()> &active,&inactive; 

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

class UnitPicker : public combox
{
          CUnit::unit_name      _defUnit;
    const CUnit::magnitude_name  magnitude;
  public: 
    UnitPicker(widget &wd, const CUnit::unit_name& def)
          :combox(wd), _defUnit(def), magnitude(CUnit::UnitsDic().at(def).magnitude )
    {
        editable(false);
        for(const CUnit::unit_name& un : CUnit::MagnitudesDic().at(magnitude) )
            push_back (charset ( un /*CUnit::UnitsDic().at(un).to_string ()*/  ));
        caption(charset (def));
        //ext_event().selected=[&](combox& cb)
        //{
        //    _cb.caption(_cb.caption().substr(6, _cb.caption().find_first_of(STR(" ="),6)-6 )); 
        //};
    }

    double to_def   (double val) ///< Convert a value in user selected Unit into Default Unit 
    {
        return CUnit::CUnit(nana::charset (caption()) ,  _defUnit  ).conv (val);
    }
    double to_def   (double val, const CUnit::unit_name& un ) ///< Convert a value in Unit un into Default Unit 
    {
        return CUnit::CUnit(un ,  _defUnit  ).conv (val);
    }
    double from_def (double val) ///< Convert a value in Default Unit into user selected Unit 
    {
        return CUnit::CUnit( _defUnit, nana::charset (caption())  ).conv (val);
    }
    double from_def (double val, const CUnit::unit_name& un ) ///< Convert a value in Default Unit into Unit un 
    {
        return CUnit::CUnit( _defUnit, nana::charset (caption())  ).conv (val);
    }
    double convert_to(double val, const CUnit::unit_name& un) ///< Convert a value in user selected Unit into Unit un 
    {
        return CUnit::CUnit( nana::charset (caption()), un ).conv (val);
    }
    double convert_from(double val, const CUnit::unit_name& un) ///< Convert a value in Unit un into the user selected Unit
    {
        return CUnit::CUnit(un ,  nana::charset (caption()) ).conv (val);
    }

};

class NumUnitUpDown : public panel<false> , EditableWidget 
{
    NumerUpDown _num;
    UnitPicker  _unit;
    //double      _val;
    CUnit::unit_name _curr_un;
public:
    NumUnitUpDown ( widget &wd,        const string& label, 
                    double defVal,    double min,     double max,    const CUnit::unit_name& def  , 
                    const string& DefLayFile ,   double step=1,  unsigned decimals=2)
        : nana::gui::panel<false>(wd),
          EditableWidget (*this,label,STR("NumUnitUpDonw.Lay.txt")),
          _num(*this,label, defVal, min,max,STR("Vert-Invert.NumUpDonw.Lay.txt"),step,decimals),
          _unit(*this, def), _curr_un(def) //_val(defVal)
    {
        _unit.ext_event().selected=[&](combox& cb)
                                    {
                                        _num.Max (_unit.convert_from(  _num.Max(), _curr_un ) );
                                        _num.Min (_unit.convert_from(  _num.Min(), _curr_un ) );
                                        //_num.Step (_unit.convert_from(  _num.Step(), _curr_un ) );
                                       
                                        _num.Value (_unit.convert_from(  _num.Value(), _curr_un ) );
                                        _curr_un=charset(_unit.caption ());
                                    };
        InitMyLayout();
        SelectClickableWidget( _num);

    }
     void SetDefLayout       () override
    {
        _DefLayout= "  <                        \n"
                                "  <vertical   weight=120            Num                 >           \n"
                                "  <vertical      <><vertical Unit weight=21><>          >             \n"
                         " > ";   

        _num._DefLayout="  <                        \n"
                        "          <>               \n"
                                "  <vertical weight=60       <><label weight=15 gap=1>        <>          >           \n"
                                "  <vertical weight=15       <><vertical UpDown weight=21><>          >               \n"
                                "  <vertical weight=50       <><Num weight=21>                    <>          >        \n"
                         " > ";
    }
     void AsignWidgetToFields() override
    {
	    _place.field("Num"    ) << _num ;
	    _place.field("Unit"   ) << _unit ;
    }
     double Value()
     {
         return _unit.to_def(_num.Value()); 
     }
     double Value(const CUnit::unit_name& un)
     {
         return _unit.convert_to(_num.Value(),un); 
     }
     void   Value(double val)
     {
         _num.Value(_unit.from_def(val)); 
     }
     void   Value(double val, const CUnit::unit_name& un)
     {
         _num.Value(_unit.convert_from(val,un)); 
     }

};


}} // namespace nana { namespace gui {

        //cb.caption(_cb.caption().substr(6, _cb.caption().find_first_of(STR(" ="),6)-6 )); 

#endif 
