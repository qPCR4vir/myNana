#include <../temp/EditableForm.hpp>

#include <iostream>    // temp, for debugging
#include <fstream>     // temp, for debugging

FilePickBox::FilePickBox (nana::gui::widget &fm, 
						  const nana::string   &label,
						  const nana::string   &DefLayoutFileName )
				:	CompoWidget(fm, label, DefLayoutFileName),
					_label(*this, caption  () ),  
                    _fileName(*this), 
                    Pick (*this, STR("...")),
                    fb_p (*this, true ),
                    _user_selected(false),
                    _canceled(false)
{
    _label.text_align(nana::align::right  ); 

	Pick.make_event	<nana::gui::events::click> ([&](){pick(FileName());}	); 

	_fileName.editable(true);

    _user_selected=true;
    InitMyLayout();
    SelectClickableWidget( _label);
}
    void FilePickBox::SetDefLayout       () 
{
    _DefLayout= 
        "vertical   <weight=1>    \n"
        "           <weight=20 <weight=3><   vertical weight=49 <><label weight=15><>     ><weight=1>     \n"
        "					   <cbFL >       \n"
        "					   <pick weight=30>  \n"
        "					   <weight=3> 	>" 
        "            <weight=2>    \n"          ;  
}
    void FilePickBox::AsignWidgetToFields() 
{
	_place.field("cbFL"        ) <<  _fileName ;
	_place.field("label"       ) << _label;
	_place.field("pick"        ) << Pick;
}
void FilePickBox::pick(const nana::string &file_tip)
{
	pick_file( fb_p, STR("pick"), file_tip);
}
void FilePickBox::pick_file(nana::gui::filebox&  fb, const nana::string &action, const nana::string &file_tip)
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


OpenSaveBoxP::OpenSaveBoxP (nana::gui::widget &fm, 
						  const nana::string   &label,
						  const nana::string   &DefLayoutFileName )
				:	FilePickBox(fm, label, DefLayoutFileName),
                    Open (*this, STR("Open") ),  
                    Save (*this, STR("Save")), 
					fb_o (*this, true ),
                    fb_s (*this, false )
{
    Open.make_event	<nana::gui::events::click> ([&](){open(FileName());}	);
	Save.make_event	<nana::gui::events::click> ([&](){save(FileName());}    );

	_fileName.editable(true);

    InitMyLayout();
}
    void OpenSaveBoxP::SetDefLayout       () 
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
    void OpenSaveBoxP::AsignWidgetToFields() 
{
	_place.field("proj_buttons") << Open << Save;
    //FilePickBox::AsignWidgetToFields() ;
}
	
void OpenSaveBoxP::open(const nana::string &file_tip)
{
	pick_file( fb_o, STR("open"), file_tip);
}
void OpenSaveBoxP::save(const nana::string &file_tip)
{
	pick_file( fb_s, STR("save"), file_tip);
}



OpenSaveBox::OpenSaveBox (nana::gui::widget &fm, 
						  const nana::string   &label,
						  const nana::string   &DefLayoutFileName )
				:	CompoWidget(fm, label, DefLayoutFileName),
					_label(*this, caption  () ),  
                    _fileName(*this), 
                    Open (*this, STR("Open") ),  
                    Save (*this, STR("Save")), 
                    Pick (*this, STR("...")),
					fb_o (*this, true ),
                    fb_s (*this, false ),
                    fb_p (*this, true ),
                    _user_selected(false),
                    _canceled(false)
{
    _label.text_align(nana::align::right  ); 

    Open.make_event	<nana::gui::events::click> ([&](){open(FileName());}	);
	Pick.make_event	<nana::gui::events::click> ([&](){pick(FileName());}	); 
	Save.make_event	<nana::gui::events::click> ([&](){save(FileName());}    );

	_fileName.editable(true);

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
