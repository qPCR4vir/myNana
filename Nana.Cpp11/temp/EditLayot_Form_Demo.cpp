#include <../temp/EditableForm.hpp>
//#include <../temp/CompoWidget.hpp>

#include <iostream>    // temp, for debugging
#include <fstream>     // temp, for debugging

class DemoForm : public nana::gui::form, public EditableForm 
{ public:
	nana::gui::button	but1 , but2, but3, but4;
    OpenSaveBox         osb  , osb2 ;

    DemoForm ():nana::gui::form (nana::rectangle( nana::point(500,410), nana::size(400,150) )),
                EditableForm    (*this, STR("Configurable Window")),
                but1         (*this), but2(*this), but3(*this), but4(*this),
                osb          (*this, STR("Project:") ), 
                osb2         (*this, STR("Project2:"))
    {
        but1.caption (STR("But1"));
	    but2.caption (STR("But2"));
	    but3.caption (STR("But3"));
	    but4.caption (STR("But4"));

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


