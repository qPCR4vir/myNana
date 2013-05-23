#include <../temp/EditLayot_Form.hpp>
#include <iostream> 
#include <fstream> 

	EditLayot_Form::EditLayot_Form  (	nana::gui::place     &pl, 
										const std::string    &lay, 
										const nana::string   &DefLayotFileName,
										nana::gui::form      *fm )
		:nana::gui::form ( nana::rectangle( nana::point(500,200), nana::size(400,170) )),
					_OriPlace		(pl),
					//_DefLayFile		(DefLayotFileName),
				    _OSbx			(*this, STR("Layot:")),      
					_ReCollocate	(*this),
					_textBox		(*this),
				    _place			(*this),
					_menu			(*this),
					_menuFile		(_menu.push_back(STR("&File"))),
					_menuProgram	(_menu.push_back(STR("&Programm"))),
					_Titel          (STR("Editing layot of: "))
	{	
		if (fm) _Titel +=fm->caption(); 
		InitCaptions();
		InitMyLayot();

		_textBox.editable(true);
		_OSbx.add_filter(STR("Layot File"), STR("*.lay.txt"));

	    if   (lay.empty())    
        {    if (! DefLayotFileName.empty())
             {   _OSbx.FileName	(   DefLayotFileName  );
                 OpenFile();
             }
        } else   		          
        {   _textBox.append(nana::string(nana::charset(lay)),false );
            _textBox.select(true);
            _textBox.show();

             if (! DefLayotFileName.empty())
                _OSbx.FileName	(   DefLayotFileName  );
        }

		MakeResponsive();
	}

void EditLayot_Form::InitCaptions()
	{
		 caption				(_Titel);
		_ReCollocate.caption	(STR("Apply"		));
		_textBox.tip_string		(STR("type or load a layot to be applied to the calling window..."		));
	}
void EditLayot_Form::MakeResponsive()
	{
        _textBox.on_first_change([&](){on_edited();});
		_menuFile.append  (STR("&Open..."),[&](nana::gui::menu::item_proxy& ip){_OSbx.open();OpenFile();});
		_menuFile.append  (STR("&Save..."),[&](nana::gui::menu::item_proxy& ip){_OSbx.save(_OSbx.FileName());SaveFile();});
		_menuProgram.append (STR("&Apply Layot to calling windows"),[&](nana::gui::menu::item_proxy& ip){ReLayot();});
		_menuProgram.append (STR("&Edit this windows Layot"       ),[&](nana::gui::menu::item_proxy& ip){EditMyLayot();});

		_OSbx.Open.make_event	<nana::gui::events::click> (*this , &EditLayot_Form::OpenFile		);
		_OSbx.Save.make_event	<nana::gui::events::click> (*this , &EditLayot_Form::SaveFile		);
		_OSbx.Pick.make_event	<nana::gui::events::click> (*this , &EditLayot_Form::OpenFile		);
		_ReCollocate.make_event	<nana::gui::events::click> ([&](){ReLayot();});
		_OSbx._fileName.ext_event().selected = [&](nana::gui::combox&cb)
		{
		    if(! _OSbx.UserSelected()) return;

            nana::string   fileN=_OSbx.FileName();  // The newly selected name
            std::wcout<<std::endl<<STR("Selected: ")<<fileN<<std::endl;
			OpenFileN(fileN );
		};
	}
void EditLayot_Form::on_edited()
{
   nana::string newTitel  = _Titel  +  STR(" <") ;
                newTitel +=  nana::string(nana::charset(_textBox.filename())) ;
                newTitel += (_textBox.saved()? STR(">"): STR(" * >")  );
    caption	( newTitel);
}

void EditLayot_Form::InitMyLayot()
	{
		std::string layot;
        readLayot( STR("Layot_Form.lay.txt"), layot ="");
        if (layot.empty() )
            _myLayout= "vertical                                            \n\t"
                                   "<weight=25>                             \n\t"
                                   "<weight=25  OpenSave>                   \n\t"
                                   "<textBox>                               \n\t"
                                   "<weight=25 <weight=5> <re weight=50 gap=1>>\n\t"
                                   "<weight=5>                              \n\t";
        else _myLayout=layot;

        _place.div( _myLayout.c_str () );
		_place.field("OpenSave"	   )<< _OSbx;
		_place.field("textBox"	   )<< _textBox;
		_place.field("re"		   )<< _ReCollocate ;
		_place.collocate ();
	}
void EditLayot_Form::ReLayot()
	{   nana::string lay,line;
		for (size_t linum=0; _textBox.getline(linum , line) ; ++linum )
			lay+=line;
        _OriPlace.div( std::string(nana::charset (lay)).c_str() );  // try std::runtime_error msgbox
		_OriPlace.collocate ();
	}
void EditLayot_Form::OpenFile()
	{	 
      if(_OSbx.Canceled () ) return;
      std::wcout<<std::endl<<STR("OpenFile: ")<<std::endl;

	  OpenFileN(_OSbx.FileName());
	}
void EditLayot_Form::OpenFileN(const nana::string   &file)
	{	  
		if( file.empty() ) 
            return;
        std::wcout<<std::endl<<STR("OpenFileN: ")<<file<<std::endl;
        if ( _textBox.edited () )
           SaveFileN(nana::string(nana::charset(_textBox.filename()))); 
		_textBox.load(file.c_str() );
        _textBox.select(true);
        _textBox.show();
        std::wcout<<std::endl<<STR("OpenedFileN: ")<<file<<std::endl;
	}
void EditLayot_Form::SaveFileN(const nana::string   &fileTip)
	{	
        std::wcout<<std::endl<<STR("Seaving tip: ")<<fileTip<<std::endl;
        _OSbx.save(fileTip);
       if(_OSbx.Canceled () ) 
           return;
       SaveFile();
	}
void EditLayot_Form::SaveFile()
	{	
      if(_OSbx.Canceled () ) 
          return;
        std::wcout<<std::endl<<STR("Seaving file: ")<<_OSbx.FileName()<<std::endl;
		_textBox.store(_OSbx.FileName().c_str() );
		std::wcout<<std::endl<<STR("SavedFIle: ")<<_OSbx.FileName()<<std::endl;
	}
void EditLayot_Form::EditMyLayot()
		{
			if (!_myEdLayForm) 
				_myEdLayForm.reset (new EditLayot_Form (_place, _myLayout.c_str (),   STR("Layot_Form.lay.txt") , this ));
			_myEdLayForm->show ();
		}
const char* EditLayot_Form::readLayot(const nana::string& FileName, std::string& layot)
    {
		std::ifstream loy(FileName);
		std::string temp;

		while (std::getline(loy,temp)) layot+=temp + "\n";
		std::cout<< layot;
		return layot.c_str();
    }	


