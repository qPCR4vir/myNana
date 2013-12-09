#ifndef NANA_GUI_EditableForm_HPP
#define NANA_GUI_EditableForm_HPP

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <../temp/vplace.hpp>
#include <nana/gui/widgets/textbox.hpp>

//#include <../temp/CompoWidget.hpp>
//#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
//#include <nana/gui/place.hpp>
#include <nana/gui/filebox.hpp>       


#include <iostream> 
#include <fstream> 
#include <cassert>

 std::ostream& operator<<(std::ostream& o,const nana::rectangle &r);
//{ o<<" rect("<<r.x<<","<<r.y<<","<<r.width <<","<<r.height <<")\n"; return o;}


class EnablingEditing
{
 private:
    static bool _globalBlockInteratctiveEdition, _globalBlockConfig ;
           bool       _BlockInteratctiveEdition,       _BlockConfig ;
 public:
    static bool  globalBlockInteratctiveEdition(bool block=_globalBlockInteratctiveEdition)
    {
        return _globalBlockInteratctiveEdition=block;
    }
    static bool  globalBlockConfig(bool block=_globalBlockConfig)
    {
        return _globalBlockConfig=block;
    }
    bool  BlockInteratctiveEdition() const
    {
        return _BlockInteratctiveEdition;
    }
    bool  BlockConfig() const 
    {
        return _BlockConfig;
    }
    bool  BlockInteratctiveEdition(bool block)
    {
        return _BlockInteratctiveEdition=block;
    }
    bool  BlockConfig(bool block)
    {
        return _BlockConfig=block;
    }
};

class EditLayout_Form;

class EditableWidget: public EnablingEditing
{
 public:
    EditableWidget ( nana::gui::widget* EdWd_owner,                       ///< The ownwer of the form (if any) or panel 
                     nana::gui::widget& thisEdWd,    ///< the form or panel, owner of place and all other widgets of the editable widget
                     nana::string Titel, 
                     const nana::string &DefLayoutFileName=STR("")           );

    nana::gui::widget  *_EdWd_owner ;                                    ///< The ownwer of the form or panel 
    nana::gui::widget  &_thisEdWd;   ///< the form or panel, owner of place and all other widgets of the editable widget
	nana::string		_Titel;   //  ????
    std::string         _myLayout, _DefLayout;
    nana::string        _DefLayoutFileName;	
    nana::gui::vplace	_place;

    std::vector<std::function<void(void)>> _validate, _validated;
    bool changed, validated;
    bool Validate()
    {
        try
        {
            for(auto &v:_validate)
                v();                /// TODO: change to return bool: validated &= v();
        }
        catch ( ... )
        { 
            std::cerr << "\nError in Validate of ";
            std::wcerr << _Titel;
            return false; 
        }

        if (validated)
            return Validated();
        return true;
    }
    bool Validated()
    {
        try
        {
            for ( auto &v : _validated )
                v ();             /// TODO: change to return bool: validated &= v();   ????
            return true;
        }
        catch ( ... )
        { 
            std::cerr << "\nError in Validated of ";
            std::wcerr << _Titel;
            return false; 
        }
    }
    void add_validate(const std::function<void(void)>& v)
    {
        _validate.push_back (v); 
    }
virtual    void add_validated(const std::function<void(void)>& v)
    {
        _validated.push_back (v); 
    }


	nana::gui::menu	    _menuProgram;
	EditLayout_Form*    _myEdLayForm;    	//std::unique_ptr <EditLayout_Form> _myEdLayForm;

    virtual     ~EditableWidget     ();
    virtual void SetDefLayout       ()=0;
    virtual void AsignWidgetToFields()=0;
	        void InitMyLayout       ()
	{   
        SetDefLayout   ();
        readLayout( _DefLayoutFileName, _myLayout ="");
        if (_myLayout.empty() )
            _myLayout= _DefLayout;

        AsignWidgetToFields();
        //_place.div(_myLayout.c_str ());     

        ReCollocate( );
	}
            void InitMenu   (nana::gui::menu& menuProgram)
    {
       menuProgram.append(STR("&Edit this windows Layout"),[&](nana::gui::menu::item_proxy& ip)
	                                                            {EditMyLayout(); }                  );
       menuProgram.append(STR("&Reset this windows default Layout"),[&](nana::gui::menu::item_proxy& ip)
	                                                            {ResetDefLayout(); ReCollocate( );} );
    }
            void SelectClickableWidget(nana::gui::widget& wdg, nana::gui::menu& menuProgram)
            {
                wdg.make_event<nana::gui::events::click>(nana::gui::menu_popuper(menuProgram) );   
            }
            void SelectClickableWidget(nana::gui::widget& wdg)
            {
                SelectClickableWidget(wdg, _menuProgram);
            }


            void ResetDefLayout()
    {
        _myLayout=_DefLayout;
        //ReCollocate( );
                //_place.div(_myLayout.c_str ());     //    ?????????????????

    }
            void ResetDefLayout( std::string  Layout)
    {
        _DefLayout.swap(Layout);
        ResetDefLayout();
        ReCollocate( );
    }

    const std::string& DefLayout() const
    {
        return _DefLayout;
    }

 	void         EditMyLayout   (/*nana::gui::widget & EdWd_own, nana::gui::widget &EdLyF_own*/);
    static const char* readLayout(const nana::string& FileName, std::string& Layout);
    void ReCollocate( std::string  Layout)
    {
        _myLayout.swap(Layout);
        ReCollocate( );
   }
    void ReCollocate( )
    {
        _place.div(_myLayout.c_str ());     
	    _place.collocate ();
    }
};


class EditableForm: public EditableWidget
{ public:
    EditableForm ( nana::gui::widget* EdWd_owner,                       ///< The ownwer of the form or panel 
                   nana::gui::widget& thisEdWd,    ///< the form or panel, owner of place and all other widgets of the editable widget
                   nana::string Titel, 
                   const nana::string &DefLayoutFileName=STR("")           );
	nana::gui::menubar	_menuBar;
	nana::gui::menu*	_menuProgramInBar;
    //virtual ~EditableForm();
    void AddMenuProgram ()
    {
        assert (!_menuProgramInBar );
        _menuProgramInBar=&_menuBar.push_back(STR("&Programm"));
        InitMenu   (*_menuProgramInBar);
    }
    void InitMyLayout       ()
	{   
        EditableWidget::InitMyLayout();
        //_place.div(_myLayout.c_str ());     

        ////ReCollocate( );
	}

};

//#include <../temp/CompoWidget.hpp>

class CompoWidget : public  nana::gui::panel<false> , public EditableWidget  
{public:
	CompoWidget ( nana::gui::widget& EdWd_owner,              ///< The ownwer of the panel 
                  nana::string Titel, 
                  const nana::string &DefLayoutFileName=STR(""));
};

class FilePickBox : public  CompoWidget
{	nana::gui::label	_label   {*this};
    bool                _user_selected{false}, 
                        _canceled{false};
	nana::gui::button	 Pick    {*this, STR("...")};
	nana::gui::filebox   fb_p    {*this, true};

    void SetDefLayout       () override ;
    void AsignWidgetToFields() override ;
	void		pick(const nana::string &file_tip=STR(""));
 protected:
	void pick_file(nana::gui::filebox&  fb, const nana::string &action, const nana::string &file_tip);

 public:
	nana::gui::combox	_fileName{*this};    //   Only temporal public   !!!!!!!!!!!!!!!!!!!!!!!!!!!
	FilePickBox     (	nana::gui::widget    &EdWd_owner, 
						const nana::string   &label,
						const nana::string   &DefLayoutFileName=STR("") );

	virtual FilePickBox& add_filter(const nana::string& description, const nana::string& filetype)
	{ 
		fb_p.add_filter(description, filetype);
        return *this;
	}
	virtual FilePickBox& add_filter(const std::vector<std::pair<nana::string, nana::string>> &filtres)
        {
            fb_p.add_filter(filtres );
            //for (auto &f : filtres)
            //    add_filter(f.first, f.second);
            return *this;
        };
    void SetDefLayout       (unsigned lab) ;
    void ResetLayout        (unsigned lab )
    {
        SetDefLayout       (lab );
        ResetDefLayout();  
        ReCollocate( );    
    }

	nana::string FileName()const						{  return _fileName.caption();}
	void		 FileName(const nana::string&  FileName){ _fileName.push_back(FileName).option(_fileName.the_number_of_options());}
    bool        UserSelected() const {return _user_selected ;}
    bool        Canceled()     const {return _canceled;}
    void        onSelect( std::function<void(const nana::string& file)> slt)
	{	 
        _fileName.ext_event().selected = ([this, slt](nana::gui::combox&cb)
                    { 
                      if( this->UserSelected() )   
                          slt ( nana::charset ( this->FileName() )) ; 
                    } );

  //      _fileName.ext_event().selected = [&]()
		//{
		//    if(! _OSbx.UserSelected()) return;
  //          nana::string   fileN= FileName();  // The newly selected name
  //          //std::wcout<<std::endl<<STR("Selected: ")<<fileN<<std::endl;   // debbug
		//	OpenFileN(fileN );
		//};
 	}

};
class OpenSaveBox : public  FilePickBox
{
    nana::gui::button	Open{*this, STR("Open") }, 
                        Save{*this, STR("Save")};

    void SetDefLayout       () override ;
    void AsignWidgetToFields() override ;

public:
	nana::gui::filebox  fb_o{*this, true },                      //   Only temporal public   !!!!!!!!!!!!!!!!!!!!!!!!!!!
                        fb_s{*this, false };
	OpenSaveBox     (	nana::gui::widget    &EdWd_owner, 
						const nana::string   &label,
						const nana::string   &DefLayoutFileName=STR("") );

	OpenSaveBox& add_filter(const nana::string& description, const nana::string& filetype) override
	{ 
		FilePickBox::add_filter(description, filetype);
		fb_o.add_filter(description, filetype);
		fb_s.add_filter(description, filetype);
        return *this;
	}
	void		open(const nana::string &file_tip=STR(""));
	void		save(const nana::string &file_tip=STR(""));
    nana::gui::event_handle onOpenFile(std::function<void(const std::string& file)> opn)
	{	 
        return Open.make_event	<nana::gui::events::click> ([this, opn]()
                    { 
                      if( ! this->Canceled () )   
                         opn ( nana::charset ( this->FileName() )) ; 
                    } );
 	}
    nana::gui::event_handle onOpen    (std::function<void(                       )> opn)
	{	 
        return Open.make_event	<nana::gui::events::click> ([this, opn]()
                    { 
                      if( ! Canceled () )   
                         opn (  ) ; 
                    } );
 	}
    nana::gui::event_handle onSaveFile(std::function<void(const std::string& file)> sve)
	{	 
        return Save.make_event	<nana::gui::events::click> ([this,sve]()
                    { 
                      if( ! Canceled () )   
                         sve ( nana::charset ( FileName() )) ; 
                    } );
 	}
    nana::gui::event_handle onSave    (std::function<void(                       )> sve)
	{	 
        return Save.make_event	<nana::gui::events::click> ([this,sve]()
                    { 
                      if( ! Canceled () )   
                         sve (  ) ; 
                    } );
 	}
};

class EditLayout_Form : public nana::gui::form, public EditableForm
{
    EditableWidget     &_owner;
	OpenSaveBox			_OSbx       {*this, STR("Layout:" )};
	nana::gui::button	_ReCollocate{*this, STR("Apply"	  )},    _hide{*this, STR("Hide"	 )}, 
                        _panic      {*this, STR("Panic !" )},     _def{*this, STR("Default"  )}, 
                        _cpp        {*this, STR("C++ code")};
    nana::gui::textbox	_textBox{ *this };
    nana::gui::menu	   &_menuFile{_menuBar.push_back(STR("&File"))};
    nana::gui::event_handle hide_{Hidable()};  // hide_(),

public:
	EditLayout_Form (EditableWidget &EdWd_owner );
    void Closable()
    { 
         //assert((     std::cerr<<"\nMaking Closeable EditLayout_Form: "   , true) );;   // debbug
         //assert((     std::wcerr<< this->caption()  , true ) ); ;  // debbug
 
        umake_event ( hide_);
    }
    ~EditLayout_Form()
    {
          //assert((    std::cerr<<"\nDestroying EditLayout_Form: " , true   ));;   // debbug
          //assert((    std::wcerr<< this->caption()  , true  )); ;  // debbug
    }

 private:
    void SetDefLayout       () override ;
    void AsignWidgetToFields() override ;
    void on_edited();
	void InitCaptions();

    nana::gui::event_handle Hidable()
    { 
        //assert((    std::cerr<<"\nMaking Hidable EditLayout_Form: " , true ) );; // debbug
        //assert((    std::wcerr<< this->caption() , true ) ); ; // debbug

        return  make_event<nana::gui::events::unload>([this](const nana::gui::eventinfo& ei)
        {
            //assert((    std::cerr<<"\n Hiding, not closing EditLayout_Form: "  , true) );;  // debbug
            //assert((    std::wcerr<< this->caption()  , true) );; // debbug

            ei.unload.cancel = true;    //Stop closing and then
            hide();
        });
    };
    void MakeResponsive();
	void ReLayout ();
    void ReloadDef();
	void OpenFile ();
	void OpenFileN(const nana::string   &file=STR(""));
	void SaveFileN(const nana::string   &fileTip=STR(""));
    void ForceSave(const nana::string   &file);
	void SaveFile ();
    void toCppCode();
}	;
#endif 

				
