#include <../temp/EditableForm.hpp>
#include <../temp/Numer.hpp>
//#include <../temp/CompoWidget.hpp>

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


    DemoForm ():nana::gui::form (nana::rectangle( nana::point(600,210), nana::size(500,250) )),
                EditableForm    (*this, STR("Configurable Window"), STR("Layot_Demo.lay.txt")),
                but1         (*this), but2(*this), but3(*this), but4(*this),
                osb          (*this, STR("Project:") ), 
                osb2         (*this, STR("Project2:")),
                num1         (*this, STR("Numer1:")),
                num2         (*this, STR("Numer2:")),  _num(*this), _up(*this), _down(*this), _label(*this)
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
        _DefLayout= "vertical                   \n\t"
                    "<weight=25>                \n\t" 
                    "<Project weight=22>        \n\t" 
                    "<gap=2 <b1> <b2> <b3> >    \n\t"
                    "<<b4> <weight=2>>          \n\t"
                    "<  weight=20 <label weight=60 ><Num> <vertical weight=50 <UpDown> >>        \n\t"
                    "<num  weight=20 >           \n\t"
                    "<Project2 weight=22>       \n\t" ;
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
	    _place.field("Num"    ) << _num ;
	    _place.field("UpDown" ) << _up << _down ;
	    _place.field("label"  ) << _label;
    }

 };





int main()
{
	DemoForm form;

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


