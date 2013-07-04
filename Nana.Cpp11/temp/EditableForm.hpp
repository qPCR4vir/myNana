#ifndef NANA_GUI_EditableForm_HPP
#define NANA_GUI_EditableForm_HPP

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/place.hpp>
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

class EditLayout_Form;

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
    nana::gui::place	_place;

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
{public:
	nana::gui::button	Pick;
	nana::gui::combox	_fileName;
	nana::gui::filebox  fb_p;
	FilePickBox     (	nana::gui::widget    &EdWd_owner, 
						const nana::string   &label,
						const nana::string   &DefLayoutFileName=STR("") );

	void virtual add_filter(const nana::string& description, const nana::string& filetype)
	{ 
		fb_p.add_filter(description, filetype);
	}
    void SetDefLayout       () override ;
    void AsignWidgetToFields() override ;

	nana::string FileName()const						{  return _fileName.caption();}
	void		 FileName(const nana::string&  FileName){ _fileName.push_back(FileName).option(_fileName.the_number_of_options());}
	void		pick(const nana::string &file_tip=STR(""));
    bool        UserSelected() const {return _user_selected ;}
    bool        Canceled()     const {return _canceled;}

 protected:
	void pick_file(nana::gui::filebox&  fb, const nana::string &action, const nana::string &file_tip);
 private:
	nana::gui::label	_label;
    bool _user_selected, _canceled;
};
class OpenSaveBoxP : public  FilePickBox
{public:
	nana::gui::button	Open, Save;
	nana::gui::filebox  fb_o, fb_s;
	OpenSaveBoxP     (	nana::gui::widget    &EdWd_owner, 
						const nana::string   &label,
						const nana::string   &DefLayoutFileName=STR("") );

	void add_filter(const nana::string& description, const nana::string& filetype) override
	{ 
		fb_o.add_filter(description, filetype);
		fb_s.add_filter(description, filetype);
		fb_p.add_filter(description, filetype);
	}
    void SetDefLayout       () override ;
    void AsignWidgetToFields() override ;

	void		open(const nana::string &file_tip=STR(""));
	void		save(const nana::string &file_tip=STR(""));
};


class OpenSaveBox : public  CompoWidget
{public:
	nana::gui::button	Open, Save, Pick;
	nana::gui::combox	_fileName;
	nana::gui::filebox  fb_o, fb_s, fb_p;
	OpenSaveBox     (	nana::gui::widget    &EdWd_owner, 
						const nana::string   &label,
						const nana::string   &DefLayoutFileName=STR("") );

	void add_filter(const nana::string& description, const nana::string& filetype)
	{ 
		fb_o.add_filter(description, filetype);
		fb_s.add_filter(description, filetype);
		fb_p.add_filter(description, filetype);
	}
    void SetDefLayout       () override ;
    void AsignWidgetToFields() override ;

	nana::string FileName()const						{  return _fileName.caption();}
	void		 FileName(const nana::string&  FileName){ _fileName.push_back(FileName).option(_fileName.the_number_of_options());}
	void		open(const nana::string &file_tip=STR(""));
	void		save(const nana::string &file_tip=STR(""));
	void		pick(const nana::string &file_tip=STR(""));
    bool        UserSelected() const {return _user_selected ;}
    bool        Canceled()     const {return _canceled;}

 private:
	nana::gui::label	_label;
    bool _user_selected, _canceled;
	void pick_file(nana::gui::filebox&  fb, const nana::string &action, const nana::string &file_tip);
};

class EditLayout_Form : public nana::gui::form, public EditableForm
{public:
	EditLayout_Form (EditableWidget &EdWd_owner );
    void Closable()
    { 
        std::cerr<<"\nMaking Closeable EditLayout_Form: ";   // debbug
        std::wcerr<< this->caption() ;  // debbug

        umake_event ( hide_);
    }
    ~EditLayout_Form()
    {
        std::cerr<<"\nDestroying EditLayout_Form: ";   // debbug
        std::wcerr<< this->caption() ;  // debbug
    }

 private:
    EditableWidget     &_owner  ;
	OpenSaveBox			_OSbx;
	nana::gui::button	_ReCollocate, _hide, _panic, _def;
	nana::gui::textbox	_textBox;
	nana::gui::menu	   &_menuFile;
    nana::gui::event_handle hide_;

    void SetDefLayout       () override ;
    void AsignWidgetToFields() override ;
    void on_edited();
	void InitCaptions();

    nana::gui::event_handle Hidable()
    { 
        std::cerr<<"\nMaking Hidable EditLayout_Form: "; // debbug
        std::wcerr<< this->caption() ; // debbug

        return  make_event<nana::gui::events::unload>([this](const nana::gui::eventinfo& ei)
        {
            std::cerr<<"\n Hiding, not closing EditLayout_Form: ";  // debbug
            std::wcerr<< this->caption() ; // debbug

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
}	;
#endif 

				