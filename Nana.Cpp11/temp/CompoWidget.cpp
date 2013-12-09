#include <../temp/EditableForm.hpp>

#include <iostream>    // temp, for debugging
#include <fstream>     // temp, for debugging

FilePickBox::FilePickBox (nana::gui::widget &fm, 
						  const nana::string   &label,
						  const nana::string   &DefLayoutFileName )
				:	CompoWidget(fm, label, DefLayoutFileName) 
{
    _label.caption(caption());
    _label.text_align(nana::align::right  ); 

    fb_p.title(STR("Pick ") + caption());

	Pick.make_event	<nana::gui::events::click> ([&](){pick(FileName());}	); 

	_fileName.editable(true);

    _user_selected=true;
    InitMyLayout();
    SelectClickableWidget( _label);
}
    void FilePickBox::SetDefLayout       () 
    {
       SetDefLayout       (49); 
    }

    void FilePickBox::SetDefLayout       (unsigned lab) 
{
    std::stringstream lay;
    lay<<
        "vertical   <weight=1>    \n"
        "           <weight=20 <weight=3><   vertical weight="<<lab<<" <><label weight=15><>     ><weight=1>     \n"
        "					   <cbFL >       \n"
        "					   <pick weight=30>  \n"
        "					   <weight=3> 	>" 
        "            <weight=2>    \n"          ;  
    _DefLayout=lay.str();    //ResetDefLayout();
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
	//assert((      std::wcout<<std::endl<<action<<STR(" (with tip: ") << file_tip<<STR(")")<<std::endl , true  ));;
    if (!action.empty())
        fb.title(action);
    fb.init_file(file_tip); 
	if(fb())  
	{	_user_selected=false;
        _fileName.push_back(fb.file()).option(_fileName.the_number_of_options());
	    //assert((      std::wcout<<std::endl<<action<<STR(" OK: ")<<std::endl , true  ));;
        _user_selected=true;
        _canceled= false;
    }
    else 
        _canceled= true;
    if (!action.empty()) // revisar !!
        fb.title(action);


    //assert((     std::wcout<<std::endl<<action<<STR(" Canceled: ")<<std::endl , true  ));;
}


OpenSaveBox::OpenSaveBox (nana::gui::widget &fm, 
						  const nana::string   &label,
						  const nana::string   &DefLayoutFileName )
				:	FilePickBox(fm, label, DefLayoutFileName) 
{
    fb_o.title(STR("Open ") + caption());
    fb_s.title(STR("Save ") + caption());

    Open.make_event	<nana::gui::events::click> ([&](){open(FileName());}	);
	Save.make_event	<nana::gui::events::click> ([&](){save(FileName());}    );

    InitMyLayout();
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
	_place.field("proj_buttons") << Open << Save;
    //FilePickBox::AsignWidgetToFields() ;
}
	
void OpenSaveBox::open(const nana::string &file_tip)
{
	pick_file( fb_o, STR("open"), file_tip);
}
void OpenSaveBox::save(const nana::string &file_tip)
{
	pick_file( fb_s, STR("save"), file_tip);
}

