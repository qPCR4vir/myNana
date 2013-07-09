#ifndef NANA_GUI_Numer_HPP
#define NANA_GUI_Numer_HPP
#pragma warning(disable : 4996)
#include <../temp/EditableForm.hpp>
#include <../temp/Units.hpp>


namespace nana { namespace gui {

class NumberLabel : public label
{
  public:
    NumberLabel (   widget &parent,    
                    double val=0, 
                    unsigned decimals=2,
                    unsigned width=6)
        :	label(parent),
			_val(val),  _decimals(decimals), _width(width)
    {
        UpDate();
    }
    double    _val;
    unsigned  _decimals, _width;
    double Value(          )const{                    return _val;}
    double Value(double val)     {_val=val; UpDate(); return _val;}
    void UpDate()
    {
        string val(50,0);
        swprintf(&val[0],val.size(), STR(" %*.*f"), _width, _decimals, _val );
           caption (val.c_str());
    }
};


class NumberBox : public textbox
{
  public:
    NumberBox (   widget &parent,    
                    double val=0, 
                    unsigned decimals=2,
                    unsigned width=6)
        :	textbox(parent),
			_val(val),  _decimals(decimals), _width(width)
    {
        multi_lines(false);
        UpDate();
        make_event <events::focus>([&](const eventinfo& ei)
                {  if (!ei.focus.getting) 
                        add( 0    );
                }); 
    }
    double    _val;
    unsigned  _decimals, _width;
    double Value(          )const{                    return _val;}
    double Value(double val)     {_val=val; UpDate(); return _val;}
    void add(double step)
    {
        try    {  _val=std::stod (caption()  );     }
        catch (...)     {;     }
        _val += step;
        UpDate();
    }
    void UpDate()
    {
        string val(50,0);
        swprintf(&val[0],val.size(), STR(" %*.*f"), _width, _decimals, _val );
        ext_event().first_change= ([&](){});
           caption (val.c_str());
        ext_event().first_change=     ([&](){add( 0    ); });
    }
};

class NumerUpDown : public  CompoWidget
{
  public:
    NumerUpDown (   widget &parent_,      const string &label, 
                    double val,           double min, double max, 
                    const string         &DefFileName=STR("NumUpDown.VertCenter.lay.txt"), 
                    double step=1,       unsigned width=6,    unsigned decimals=2  );
    double Value    (          )const{                        return _val;}
    double Min      (          )const{                        return _min;}
    double Max      (          )const{                        return _max;}
    double Step     (          )const{                        return _step;}
    double Width    (          )const{                        return _width;}
    double Decimals (          )const{                        return _decimals;}
    double Value    (double val)     {_val=val;   UpDate() ;  return _val;}
    double Min      (double val)     {_min=val; /*UpDate();*/ return _min;}
    double Max      (double val)     {_max=val; /*UpDate(); */return _max;}
    double Step     (double val)     {_step=val; /*UpDate();*/return _step;}
    double Width    (double val)     {_width=val;/*UpDate();*/return _width;}
    double Decimals (double val)     {_decimals=val;/*UpDate();*/return _decimals;}
  private:
    textbox _num;
    button  _up, _down;
    label   _label;
    double  _val, _min, _max, _step;
    unsigned _decimals, _width;

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
    swprintf(&val[0],val.size(), STR(" %*.*f"), _width, _decimals, _val );
    _num.ext_event().first_change= ([&](){});
       _num.caption (val.c_str());
    _num.ext_event().first_change=     ([&](){add( 0    ); });
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

class NumUnitUpDown : public CompoWidget
{
    //double      _val;
    CUnit::unit_name _curr_un;
public:
    NumerUpDown _num; /// TODO: make private and provide a funtion to change the def lay, especialy the length of the label
    UnitPicker  _unit; /// TODO: make private and provide a funtion to change the def lay, especialy the length of the label
    NumUnitUpDown ( widget &wd,        
                    const string& label, 
                    double defVal,    double min,     double max,    
                    const CUnit::unit_name& def  , 
                    const string& DefLayFile =STR("NumUnitUpDonw.Lay.txt"),   
                    double step=1,  unsigned decimals=2)
        : CompoWidget (wd,label,STR("NumUnitUpDonw.Lay.txt")),
          _num(*this,label, defVal, min,max,STR("Vert-Invert.NumUpDonw.Lay.txt"),step,decimals),
          _unit(*this, def), _curr_un(def) //_val(defVal)
    {
        _unit.ext_event().selected=[&](combox& cb)
                                    {
                                        CUnit u(_curr_un , charset( _unit.caption() )/*CUnit::unit_name ( nana::charset(cb.option ()) ) */);
                                        if(u.error )
                                        {    _unit.caption (_unit.caption ()+STR("?"));
                                             return;
                                        }
                                        _num.Max   ( u.conv(_num.Max  () )  );
                                        _num.Min   ( u.conv(_num.Min  () )  );
                                        _num.Value ( u.conv(_num.Value() )  );
                                        if (u.conv.lineal )
                                          _num.Step( u.conv.c*_num.Step()   );
                                       
                                        _curr_un=charset(_unit.caption ());
                                    };
        InitMyLayout();
        SelectClickableWidget( _num);
        SelectClickableWidget( _unit);

    }
     void SetDefLayout       () override
    {
        SetDefLayout       (60);
        //_DefLayout= "  <                        \n"
        //                        "  <vertical     min=125               Num                 >           \n"
        //                        "  <vertical     min=50    <><vertical Unit weight=21><>  >             \n"
        //                 " > ";   

        //_num._DefLayout="  <                        \n"
        //                "                           \n"
        //                        "  <vertical weight=60       <><label weight=15 gap=1>        <>          >           \n"
        //                        "  <vertical weight=15       <><vertical UpDown weight=21><>          >               \n"
        //                        "  <vertical                 <><Num weight=21>                    <>          >        \n"
        //                 " > ";
    }
     void SetDefLayout       (unsigned lab, unsigned n=50, unsigned unit=50)
    {
       
        char d[]= "  <                        \n"
                                "  <vertical     min=%u               Num                 >           \n"
                                "  <vertical     min=%u    <><vertical Unit weight=21><>  >             \n"
                         " > ";   

        char nd[]="  <                        \n"
                        "                           \n"
                                "  <vertical weight=%u       <><label weight=15 gap=1>        <>          >           \n"
                                "  <vertical weight=%u       <><vertical UpDown weight=21><>          >               \n"
                                "  <vertical                 <><Num weight=21>                    <>          >        \n"
                         " > ";
   
             _DefLayout.resize ( std::strlen ( d)+20);
       _num. _DefLayout.resize ( std::strlen (nd)+20);
       sprintf(&      _DefLayout[0], d,lab+15+n,unit);
       sprintf(&_num. _DefLayout[0],nd,lab,15);
             _DefLayout.resize ( std::strlen (&      _DefLayout[0]));
       _num. _DefLayout.resize ( std::strlen (&_num. _DefLayout[0]));
     }
    void ResetLayout       (unsigned lab, unsigned n=50, unsigned unit=50)
    {
        SetDefLayout       (lab, n, unit);
        ResetDefLayout();  _num. ResetDefLayout();
        ReCollocate( );    _num.ReCollocate( );
    }

     void AsignWidgetToFields() override
    {
	    _place.field("Num"    ) << _num ;
	    _place.field("Unit"   ) << _unit ;
    }
     /// expresed in default Units
     double Value()
     {
         return _unit.to_def(_num.Value()); 
     }
     /// expresed in the especified "un" Units
     double Value(const CUnit::unit_name& un)
     {
         return _unit.convert_to(_num.Value(),un); 
     }
     /// expresed in default Units
     void   Value(double val_in_default_Units)
     {
         _num.Value(_unit.from_def(val_in_default_Units)); 
     }
     /// expresed in the especified "un" Units
     void   Value(double val, const CUnit::unit_name& un)
     {
         _num.Value(_unit.convert_from(val,un)); 
     }

};


}} // namespace nana { namespace gui {

        //cb.caption(_cb.caption().substr(6, _cb.caption().find_first_of(STR(" ="),6)-6 )); 

#endif 
