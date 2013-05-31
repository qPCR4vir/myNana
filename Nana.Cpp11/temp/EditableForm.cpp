#include <../temp/EditableForm.hpp>
//#include <../temp/CompoWidget.hpp>

#include <nana/gui/widgets/textbox.hpp>

#include <iostream> 
#include <fstream> 

bool EditableWidget::_globalBlockInteratctiveEdition=false;
bool EditableWidget::_globalBlockConfig             =false;

void EditableWidget::EditMyLayout()
		{
			if (!_myEdLayForm) 
				_myEdLayForm.reset (new EditLayout_Form ( this ));
			_myEdLayForm->show ();
		}
//EditableForm::~EditableForm(){}


EditLayout_Form::EditLayout_Form  (	EditableWidget     *fm )
		:nana::gui::form ( nana::rectangle( nana::point(300,100), nana::size(500,300) )),
         EditableForm (*this,  STR("Editing Layout of: "),STR("Layout_Form.lay.txt")),
                    _fm             (fm),
				    _OSbx			(*this, STR("Layout:")),      
					_ReCollocate	(*this),_panic  (*this),_def  (*this), 	_hide	(*this),
					_textBox		(*this),
					_menuFile		(_menuBar.push_back(STR("&File")))
	{	
		if (fm) 
            caption(_Titel +=fm->_Titel) ;  
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
		_panic.caption	        (STR("Panic !"	    ));
		_def.caption	        (STR("Default"		));
		_hide.caption	        (STR("Hide"		    ));
		_textBox.tip_string		(STR("type or load a Layout to be applied to the calling window..."		));
	}
void EditLayout_Form::MakeResponsive()
	{
        _textBox.on_first_change([&](){on_edited();});
        //_textBox.make_event <nana::gui::events::click> ([&](const nana::gui::eventinfo& ei)
        //{
        //   if (ei.mouse.right_button)
        //   {
        //       std::wcout<<std::endl<<STR("Rigth Click: x,y=")<<ei.mouse.x<<STR(",")<<ei.mouse.y  <<std::endl;
        //       _menuProgram->popup (_textBox.handle(),ei.mouse.x,ei.mouse.y,false);

        //   }
        //   if (ei.mouse.left_button )         std::wcout<<std::endl<<STR("Left  Click")<<std::endl;


        //}); 
		_menuFile.append  (STR("&Open..."   ),[&](nana::gui::menu::item_proxy& ip){ /*_OSbx.open();*/OpenFile()  ;}                );
        _menuFile.append  (STR("&Save"      ),[&](nana::gui::menu::item_proxy& ip){  ForceSave(nana::string(nana::charset(_textBox.filename())) ) ;}   );
		_menuFile.append  (STR("Save &As..."),[&](nana::gui::menu::item_proxy& ip){ _OSbx.save(nana::string(nana::charset(_textBox.filename())));SaveFile() ;} );
         AddMenuProgram ();

         //InitMenu    ();
        _menuProgramInBar->append_splitter();
		_menuProgramInBar->append (STR("&Apply Layout to calling windows"),[&](nana::gui::menu::item_proxy& ip){ReLayout();});
		_menuProgramInBar->append (STR("&Restet Default Layout to calling windows"),[&](nana::gui::menu::item_proxy& ip) {ReloadDef();});
        SelectClickableWidget( _textBox, *_menuProgramInBar );
        SelectClickableWidget( _menuBar, *_menuProgramInBar);

        _OSbx.Open.make_event	<nana::gui::events::click> ([&](){ OpenFile() ;} );
		_OSbx.Save.make_event	<nana::gui::events::click> ([&](){ SaveFile() ;} );
		//_OSbx.Pick.make_event	<nana::gui::events::click> (*this , &EditLayout_Form::OpenFile		);
        _ReCollocate.make_event <nana::gui::events::click> ([&](){ReLayout();}); 
		_OSbx._fileName.ext_event().selected = [&](nana::gui::combox&cb)
		{
		    if(! _OSbx.UserSelected()) return;

            nana::string   fileN=_OSbx.FileName();  // The newly selected name
            std::wcout<<std::endl<<STR("Selected: ")<<fileN<<std::endl;
			OpenFileN(fileN );
		};
        make_event<nana::gui::events::unload>([this](const nana::gui::eventinfo& ei)
        {
            ei.unload.cancel = true;    //Stop closing and then
            hide();
        });
        _ReCollocate.make_event <nana::gui::events::click> ([&](){ReLayout()  ;}   ); 
        _panic.make_event       <nana::gui::events::click> ([&](){_fm->ResetDefLayout(); _fm->ReCollocate( );}); 
        _def.make_event         <nana::gui::events::click> ([&](){ReloadDef() ;}   ); 
        _hide.make_event        <nana::gui::events::click> ([&](){hide()      ;}   ); 
	}
void EditLayout_Form::on_edited()
{
   nana::string newTitel  = _Titel  +  STR(" <") ;
                newTitel +=  nana::string(nana::charset(_textBox.filename())) ;
                newTitel += (_textBox.saved()? STR(">"): STR(" * >")  );
    caption	( newTitel);
}
void EditLayout_Form::SetDefLayout   ()
{
    _DefLayout= "vertical   gap=2                                    \n\t"
                            "<weight=25>                             \n\t"
                            "<weight=25  OpenSave>                   \n\t"
                            "<textBox>                               \n\t"
                            "<weight=25 <weight=15>                  \n\t" 
                            "           <re weight=50 gap=2>         \n\t"
                            "           <panic weight=50>            \n\t"
                            "           < >                          \n\t"
                            "           <def weight=50>              \n\t"
                            "           < >                          \n\t"
                            "           <hide weight=50>             \n\t"
                            "           <weight=15> >                \n\t" 
                            "<weight=5>                              \n\t";
}
void EditLayout_Form::AsignWidgetToFields() 
{
	_place.field("OpenSave"	   )<< _OSbx;
	_place.field("textBox"	   )<< _textBox;
	_place.field("re"		   )<< _ReCollocate ;
	_place.field("panic"	   )<< _panic ;
	_place.field("def"		   )<< _def ;
	_place.field("hide"		   )<< _hide ;
}
void EditLayout_Form::ReLayout()
	{   nana::string lay,line;
		for (size_t linum=0; _textBox.getline(linum , line) ; ++linum )
			lay+=line;
        _fm->ReCollocate( std::string(nana::charset (lay)).c_str() );  // try std::runtime_error msgbox
	}
void EditLayout_Form::ReloadDef()
{
    if ( _textBox.edited () )
    {
        _OSbx.fb_s.title(STR("Do you want to save your edited Layout?")); 
        SaveFileN(nana::string(nana::charset(_textBox.filename()))); 
        _OSbx.fb_s.title(STR("")); 
    }
    _fm->ResetDefLayout(); 
    _fm->ReCollocate( );
    _textBox.reset(nana::string(nana::charset(_fm->_myLayout)) );
    _textBox.select(true);
    _textBox.show();
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
        {
           _OSbx.fb_s.title(STR("Do you want to save your edited Layout?")); 
            SaveFileN(nana::string(nana::charset(_textBox.filename()))); 
           _OSbx.fb_s.title(STR("")); 
        }
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
const char* EditableWidget::readLayout(const nana::string& FileName, std::string& Layout)
    {
		std::ifstream loy(FileName);
		std::string temp;

		while (std::getline(loy,temp)) Layout+=temp + "\n";
		std::cout<< Layout;
		return Layout.c_str();
    }	


