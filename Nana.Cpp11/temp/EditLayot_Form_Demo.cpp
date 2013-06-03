#include <../temp/EditableForm.hpp>
#include <../temp/Numer.hpp>

#include <iostream>    // temp, for debugging
#include <fstream>     // temp, for debugging

class DemoForm : public nana::gui::form, public EditableForm 
{ public:
	nana::gui::button	but1 , but2, but3, but4;
    OpenSaveBox         osb  , osb2 ;
    nana::gui::NumerUpDown num1, num2;
    nana::gui::textbox _num;
    nana::gui::button  _up, _down;
    nana::gui::label   _label;
    nana::gui::UnitPicker      UPicker;
    nana::gui::NumUnitUpDown   Ta;


        //NumUnitUpDown ( widget&wd,        const string& label, 
        //            double defVal,    double min,     double max,    const CUnit::unit_name& def  , 
        //            const string& DefLayFile ,   double step=1,  unsigned decimals=2)



    DemoForm ():nana::gui::form (nana::rectangle( nana::point(600,210), nana::size(500,250) )),
                EditableForm    (*this, STR("Configurable Window"), STR("Layot_Demo.lay.txt")),
                but1         (*this), but2(*this), but3(*this), but4(*this),
                osb          (*this, STR("Project:") ), 
                osb2         (*this, STR("Project2:")),
                num1         (*this, STR("G:") , -5, -30, 30),
                num2         (*this, STR("Tm:"), 60,  30, 90),  
                _num(*this), _up(*this), _down(*this), _label(*this),
                UPicker(*this, "m"),
                Ta           ( *this, STR("Ta:"), 55,  50, 60, "grC",STR("") /*, DefLayFile , step=1, decimals=2*/)

    {
        but1.caption (STR("But1"));
	    but2.caption (STR("But2"));
	    but3.caption (STR("But3"));
	    but4.caption (STR("But4"));
        _up.caption (STR("^"));
        _down.caption (STR("v"));
        _label.caption (STR("Num out:"));
        _label.text_align(nana::align::right  ); 

         osb.add_filter (STR("Text File" ), STR("*.txt;*.doc"));
	     osb.add_filter (STR("All File"  ), STR("*.*"));
	    osb2.add_filter (STR("Text File" ), STR("*.txt"));
	    osb2.add_filter (STR("Todos File"), STR("*.*"));

        InitMyLayout();
        AddMenuProgram();
        SelectClickableWidget( _menuBar);
        SelectClickableWidget( but2);

    }
    void SetDefLayout   () override
    {
        _DefLayout= "vertical      gap=2             \n\t"
	                 "       <weight=25>                \n\t"
	                 "       <Project weight=22>       \n\t "
	                 "       <gap=2 <b1> <b2> <b3> >   \n\t "
	                 "       <<b4> <weight=2>>         \n\t "
	                 "       <  weight=22 <label weight=60 ><Num> <vertical weight=50 <UpDown> >>     \n\t   "
	                 "       <  weight=23 <Ta  weight= 180><> <num  weight=280 ><>  <Unit weight=40 >>         \n\t  "
	                 "       <Project2 weight=22>       "
                  ;
    }
    void AsignWidgetToFields() override
    {
	    _place.field("Project" )<<osb;
	    _place.field("Project2")<<osb2;
	    _place.field("b1")<<but1;
	    _place.field("b2")<<but2;
	    _place.field("b3")<<but3;
	    _place.field("b4")<<but4;
	    _place.field("num")<<num1 << num2;
	    _place.field("Num"    ) << _num  ;
	    _place.field("Unit"   ) << UPicker/*._cb*/ ;
	    _place.field("UpDown" ) << _up << _down  ;
	    _place.field("label"  ) << _label;
	    _place.field("Ta"  ) << Ta;
    }

 };

 //std::map<std::string, Magnitude  > Magnitudes;
 //std::map<std::string, std::string> Units ;

void PrintConv(double val, const CUnit::unit_name& from, const CUnit::unit_name& to)
{
    std::cout<< "\n" <<"Converting "<< from << " into " << to ;
    CUnit f_t(from,to);
    if (f_t.error) 
        return; 
    std::cout<< ": " << val << " " << from << " = " << f_t.conv(val)<< " "  << to;
}


//int main_temp ()
int main()
{
	DemoForm form;


    for (auto& mag: CUnit::MagnitudesDic())
    {
        std::cout<< "\n\nMagnitude: "<< mag.first;
        for ( auto un: mag.second)
            std::cout<< "\n \t" << CUnit::UnitsDic().at(un);

    }
            std::cout<< "\n" ;
    

    PrintConv(10, "min", "s");
    PrintConv(30, "min", "h");
    PrintConv(1.0/24/60, "day", "s");
    PrintConv(.1, "Kg", "gr");
    PrintConv(10, "mL", "s");
    PrintConv(10, "h", "s");
    PrintConv(10, "dm3", "m3");
    PrintConv(10, "dm", "m");
    PrintConv(10, "mm", "m");
    PrintConv(1000, "dm", "Km");
    PrintConv(0.001, "Km", "mm");
    PrintConv(50, "grC", "K");
    PrintConv(0, "grC", "K");
    PrintConv(0, "K", "grC");


	form.show();
    try {
	        nana::gui::exec();

        } catch (std::exception e)
        {
            std::cout<< std::endl<< e.what();
            throw ;
        } catch (...)
        {
            std::cout<< std::endl<< "exeption !!";
            throw ;
        }
	return 0;
}


