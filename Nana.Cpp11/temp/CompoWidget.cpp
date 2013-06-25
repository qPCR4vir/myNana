#include <../temp/EditableForm.hpp>

//#include <../temp/CompoWidget.hpp>
#include <iostream>    // temp, for debugging
#include <fstream>     // temp, for debugging

	//CompoWidget (nana::gui::widget& ParentWidget, nana::string Titel, const nana::string &DefLayoutFileName=STR(""))
 //       :  nana::gui::panel<false>(ParentWidget),  EditableWidget( *this, Titel, DefLayoutFileName){}



OpenSaveBox::OpenSaveBox     (	nana::gui::widget &fm, 
								const nana::string   &label,
								const nana::string   &DefFileName )
							:	CompoWidget(fm, label, DefFileName),
								Open(*this), Save(*this), Pick(*this),_fileName(*this),_label(*this),
								fb_o(*this,true ),fb_s(*this,false ),fb_p(*this,true ),
                                _user_selected(false),
                                _canceled(false)
{
    //caption  (label)  ; 
	_label.caption (  caption  ()  ); 
    _label.text_align(nana::align::right  ); 
	Open.caption	(STR("Open"		));
	Save.caption	(STR("Save"		));
	Pick.caption	(STR("..."		));

    Open.make_event	<nana::gui::events::click> ([&](){open(FileName());}	);
	Pick.make_event	<nana::gui::events::click> ([&](){pick(FileName());}	); 
	Save.make_event	<nana::gui::events::click> ([&](){save(FileName());}    );

	_fileName.editable(true);
	if(DefFileName!=STR(""))
	{	_fileName.push_back	(DefFileName);
		_fileName.option(0);
	}
    _user_selected=true;
    InitMyLayout();
    SelectClickableWidget( _label);
}
    void OpenSaveBox::SetDefLayout       () 
{
    _DefLayout= 
        "vertical   <weight=1>    \n"
        "           <weight=20 <weight=3><   vertical weight=49 <><label weight=15><>     ><weight=1>     \n"
        "		               <proj_buttons weight=74 gap=1>     \n"
        "					   <cbFL >       \n"
        "					   <pick weight=30>  \n"
        "					   <weight=3> 	>" 
        "            <weight=2>    \n"          ;  
}
    void OpenSaveBox::AsignWidgetToFields() 
{
	_place.field("cbFL"        ) <<  _fileName ;
	_place.field("label"       ) << _label;
	_place.field("proj_buttons") << Open << Save;
	_place.field("pick"        ) << Pick;
}
	
void OpenSaveBox::open(const nana::string &file_tip)
{
	pick_file( fb_o, STR("open"), file_tip);
}
void OpenSaveBox::save(const nana::string &file_tip)
{
	pick_file( fb_s, STR("save"), file_tip);
}
void OpenSaveBox::pick(const nana::string &file_tip)
{
	pick_file( fb_p, STR("pick"), file_tip);
}
void OpenSaveBox::pick_file(nana::gui::filebox&  fb, const nana::string &action, const nana::string &file_tip)
{
	std::wcout<<std::endl<<action<<STR(" (with tip: ") << file_tip<<STR(")")<<std::endl;
    fb.init_file(file_tip); 
	if(fb())  
	{	_user_selected=false;
        _fileName.push_back(fb.file()).option(_fileName.the_number_of_options());
	    std::wcout<<std::endl<<action<<STR(" OK: ")<<std::endl;
        _user_selected=true;
        _canceled= false;
        return;
    }
    std::wcout<<std::endl<<action<<STR(" Canceled: ")<<std::endl;
    _canceled= true;
}

//OpenSaveBox::p::field_reference	OpenSaveBox::put(p::field_reference f)
//{
//	return f << p::fixed(_label,49) << p::fixed(Open,40) << p::fixed(Save,35) <<_fileName << p::fixed(Pick,25); 
//}
	//virtual p&	put(p&   pl) override
	//{	
	//	pl.field (fn ())<<*this;
	//	return pl;			
	//}

