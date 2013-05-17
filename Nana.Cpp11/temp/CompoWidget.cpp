#include <../temp/CompoWidget.hpp>
#include <iostream>    // temp, for debugging
#include <fstream>     // temp, for debugging

//
//CompoWidgetP::CompoWidgetP (	const nana::string &caption_, 
//							const std::string& fieldName, 
//							const std::string& layot,
//							const nana::string& fileName)
//				: _caption	(caption_ ), 
//				  _lay		(layot),
//				  _fieldname (fieldName!=""? fieldName : std::string(nana::charset(caption_)))
//{
//	size_t len=0;
//	for(char c: _fieldname) 
//		if (c=='_' || iswalnum(c))
//			_fieldname[len++]=c;
//	_fieldname.resize(len); 
//}
//nana::gui::place::field_reference operator<<(nana::gui::place::field_reference f, CompoWidgetP& cw)
//{
//	return cw.put(f); 
//}
//nana::gui::place& operator<<(nana::gui::place&pl, CompoWidgetP& cw)
//{
//	return cw.put(pl); 
//}	

OpenSaveBox::OpenSaveBox     (	nana::gui::form &fm, 
								const nana::string   &label,
								const nana::string   &DefFileName )
							:	nana::gui::panel<false>    (	fm  ),
								Open(*this), Save(*this), Pick(*this),_fileName(*this),_label(*this),
								fb_o(*this,true ),fb_s(*this,false ),fb_p(*this,true ),_place(*this),
                                _user_selected(false),
                                _canceled(false)
{
    caption  (label)  ; 
	_label.caption (  caption  ()  ); 
    _label.text_align(nana::align::right  ); 
	Open.caption	(STR("Open"		));
	Save.caption	(STR("Save"		));
	Pick.caption	(STR("..."		));

    _myLayout= 
        "vertical   <weight=1>    \n"
        "           <weight=20 <weight=3><   vertical weight=49 <><label weight=15><>     ><weight=1>     \n"
        "		               <proj_buttons weight=74 gap=1>     \n"
        "					   <cbFL >       \n"
        "					   <pick weight=30>  \n"
        "					   <weight=3> 	>" 
        "            <weight=2>    \n"          ;  // an interesante idea, but allow only one instantition of a CompoWidwet of this type 

	_place.div(_myLayout.c_str ());    

	_place.field("cbFL"        ) <<  _fileName ;
	_place.field("label"       ) << _label;
	_place.field("proj_buttons") << Open << Save;
	_place.field("pick"        ) << Pick;
	_place.collocate ();



	Open.make_event	<nana::gui::events::click> (*this , &OpenSaveBox::open	);
	Pick.make_event	<nana::gui::events::click> (*this , &OpenSaveBox::pick	);
	Save.make_event	<nana::gui::events::click> (*this , &OpenSaveBox::save	);

	//_fileName.ext_event().selected = [&](nana::gui::combox&cb)
	//{
	//	SaveFile();  // save only is edited, changed ??? but how to know??	;
	//	_save =false;
	//	if(_cbProject.the_number_of_options()>0)
	//		_textBox.load(_cbProject.text(_cbProject.option()).c_str() );
	//	_save=true;
	//};

	_fileName.editable(true);
	if(DefFileName!=STR(""))
	{	_fileName.push_back	(DefFileName);
		_fileName.option(0);
	}
    _user_selected=true;
}
	
void OpenSaveBox::open()
{
	std::wcout<<std::endl<<STR("open: ")<<std::endl;

    if(fb_o())  
	{   
        _user_selected=false;
        _fileName.push_back(fb_o.file()).option(_fileName.the_number_of_options());
	    std::wcout<<std::endl<<STR("open OK: ")<<std::endl;
        _user_selected=true;
        _canceled= false;
        return;
    }
    std::wcout<<std::endl<<STR("open Canceled: ")<<std::endl;
    _canceled= true;
}
void OpenSaveBox::save()
{
	std::wcout<<std::endl<<STR("save: ")<<std::endl;
	if(fb_s())  
	{	_user_selected=false;
        _fileName.push_back(fb_s.file()).option(_fileName.the_number_of_options());
	    std::wcout<<std::endl<<STR("save OK: ")<<std::endl;
        _user_selected=true;
        _canceled= false;
        return;
    }
    std::wcout<<std::endl<<STR("save Canceled: ")<<std::endl;
    _canceled= true;
}
void OpenSaveBox::pick()
{
	std::wcout<<std::endl<<STR("pick: ")<<std::endl;
	if(fb_p())  
	{	
        _user_selected=false;
        _fileName.push_back(fb_p.file()).option(_fileName.the_number_of_options());
        _user_selected=true;
        _canceled= false;
    }
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

