#include <../temp/EditLayot_Form.hpp>
#include <iostream> 
#include <fstream> 

void EditableForm::EditMyLayout()
		{
			if (!_myEdLayForm) 
				_myEdLayForm.reset (new EditLayout_Form ( this ));
			_myEdLayForm->show ();
		}
//_place, _myLayout.c_str (),   STR("Layout_Form.lay.txt") ,
//nana::gui::place     &pl, 
//										const std::string    &lay, 
//										const nana::string   &DefLayoutFileName,
//										
	EditLayout_Form::EditLayout_Form  (	EditableForm     *fm )
		:nana::gui::form ( nana::rectangle( nana::point(500,200), nana::size(400,170) )),
         EditableForm (*this,  STR("Editing Layout of: "),STR("Layout_Form.lay.txt")),
                    _fm             (fm),
				    _OSbx			(*this, STR("Layout:")),      
					_ReCollocate	(*this),
					_textBox		(*this),
					_menuFile		(_menu.push_back(STR("&File")))
	{	
		if (fm) _Titel +=fm->_Titel ; //caption(); 
		InitCaptions();
		InitMyLayout();

		_textBox.editable(true);
		_OSbx.add_filter(STR("Layout File"), STR("*.lay.txt"));

	    if   (_fm->_myLayout.empty())    
        {    if (! _fm->_DefLayoutFileName.empty())
             {   _OSbx.FileName	(   _fm->_DefLayoutFileName  );
                 OpenFile();
             }
        } else   		          
        {   _textBox.append(nana::string(nana::charset(_fm->_myLayout)),false );
            _textBox.select(true);
            _textBox.show();

             if (! _fm->_DefLayoutFileName.empty())
                _OSbx.FileName	(   _fm->_DefLayoutFileName  );
        }

		MakeResponsive();
	}

void EditLayout_Form::InitCaptions()
	{
		_ReCollocate.caption	(STR("Apply"		));
		_textBox.tip_string		(STR("type or load a Layout to be applied to the calling window..."		));
	}
void EditLayout_Form::MakeResponsive()
	{
        _textBox.on_first_change([&](){on_edited();});
		_menuFile.append  (STR("&Open..."   ),[&](nana::gui::menu::item_proxy& ip){ _OSbx.open();OpenFile()  ;}                );
        _menuFile.append  (STR("&Save"      ),[&](nana::gui::menu::item_proxy& ip){  ForceSave(nana::string(nana::charset(_textBox.filename())) ) ;}   );
		_menuFile.append  (STR("Save &As..."),[&](nana::gui::menu::item_proxy& ip){ _OSbx.save(_OSbx.FileName());SaveFile() ;} );
         InitMenu    ();
		_menuProgram->append (STR("&Apply Layout to calling windows"),[&](nana::gui::menu::item_proxy& ip){ReLayout();});

		_OSbx.Open.make_event	<nana::gui::events::click> (*this , &EditLayout_Form::OpenFile		);
		_OSbx.Save.make_event	<nana::gui::events::click> (*this , &EditLayout_Form::SaveFile		);
		_OSbx.Pick.make_event	<nana::gui::events::click> (*this , &EditLayout_Form::OpenFile		);
		_ReCollocate.make_event	<nana::gui::events::click> ([&](){ReLayout();});
		_OSbx._fileName.ext_event().selected = [&](nana::gui::combox&cb)
		{
		    if(! _OSbx.UserSelected()) return;

            nana::string   fileN=_OSbx.FileName();  // The newly selected name
            std::wcout<<std::endl<<STR("Selected: ")<<fileN<<std::endl;
			OpenFileN(fileN );
		};
	}
void EditLayout_Form::on_edited()
{
   nana::string newTitel  = _Titel  +  STR(" <") ;
                newTitel +=  nana::string(nana::charset(_textBox.filename())) ;
                newTitel += (_textBox.saved()? STR(">"): STR(" * >")  );
    caption	( newTitel);
}

void EditLayout_Form::InitMyLayout()
	{   
        std::string Layout;
        readLayout( _DefLayoutFileName, Layout ="");
        if (Layout.empty() )
            _myLayout= "vertical                                            \n\t"
                                   "<weight=25>                             \n\t"
                                   "<weight=25  OpenSave>                   \n\t"
                                   "<textBox>                               \n\t"
                                   "<weight=25 <weight=5> <re weight=50 gap=1>>\n\t"
                                   "<weight=5>                              \n\t";
        else _myLayout=Layout;

        _place.div( _myLayout.c_str () );
		_place.field("OpenSave"	   )<< _OSbx;
		_place.field("textBox"	   )<< _textBox;
		_place.field("re"		   )<< _ReCollocate ;
		_place.collocate ();
	}
void EditLayout_Form::ReLayout()
	{   nana::string lay,line;
		for (size_t linum=0; _textBox.getline(linum , line) ; ++linum )
			lay+=line;
        _fm->_place.div( std::string(nana::charset (lay)).c_str() );  // try std::runtime_error msgbox
		_fm->_place.collocate ();
	}
void EditLayout_Form::OpenFile()
	{	 
      if(_OSbx.Canceled () ) return;
      std::wcout<<std::endl<<STR("OpenFile: ")<<std::endl;

	  OpenFileN(_OSbx.FileName());
	}
void EditLayout_Form::OpenFileN(const nana::string   &file)
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
void EditLayout_Form::SaveFileN(const nana::string   &fileTip)
	{	
        std::wcout<<std::endl<<STR("Seaving tip: ")<<fileTip<<std::endl;
        _OSbx.save(fileTip);
        SaveFile();
	}
void EditLayout_Form::SaveFile()
	{	
      if(_OSbx.Canceled () ) 
          return;
      ForceSave(_OSbx.FileName().c_str() );
	}
void EditLayout_Form::ForceSave(const nana::string   &file)
	{	
        std::wcout<<std::endl<<STR("Direct save file: ")<<file<<std::endl;
		_textBox.store(file.c_str () );
		std::wcout<<std::endl<<STR("SavedFIle: ")<<file<<std::endl;
	}
const char* EditableForm::readLayout(const nana::string& FileName, std::string& Layout)
    {
		std::ifstream loy(FileName);
		std::string temp;

		while (std::getline(loy,temp)) Layout+=temp + "\n";
		std::cout<< Layout;
		return Layout.c_str();
    }	


