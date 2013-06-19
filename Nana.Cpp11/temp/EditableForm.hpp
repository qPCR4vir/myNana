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

class EditableWidget
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
    EditableWidget (nana::gui::widget& ThisWidget, nana::string Titel, const nana::string &DefLayoutFileName=STR(""))
            :  _place	       (ThisWidget),   
               _Titel(std::move(Titel)),        //   ???
               _DefLayoutFileName(DefLayoutFileName)
     {  
     	//nana::rectangle r;  //debugg
      //  r=nana::gui::API::window_size(ThisWidget);  //debugg
      //  //std::cerr<< "\nplace::implement::implement(window parent_widget [ "<<parent_widget<<" ]) with area: "<<r;  //debugg
      //  std::cerr<< "\nEditableWidget(parent_widget [ "<< std::string(nana::charset ( _Titel))<<" ]) with area: "<<r;  //debugg


        ThisWidget.caption(_Titel);       //   ???
        InitMenu   (_menuProgram);
    }
	nana::string		_Titel;   //  ????
    std::string         _myLayout, _DefLayout;
    nana::string        _DefLayoutFileName;	
    nana::gui::place	_place;
	nana::gui::menu	    _menuProgram;
	std::unique_ptr <EditLayout_Form> _myEdLayForm;

    virtual     ~EditableWidget     (){};
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

 	void         EditMyLayout   ();
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
    EditableForm (nana::gui::form& ThisForm,nana::string Titel, const nana::string &DefLayoutFileName=STR(""))
            :  EditableWidget( ThisForm, Titel, DefLayoutFileName),
               _menuBar	       (ThisForm), _menuProgramInBar(nullptr)
     {
         ThisForm.caption(_Titel);
     }
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
class OpenSaveBox : public  nana::gui::panel<false> , public EditableWidget   
{public:
	nana::gui::button	Open, Save, Pick;
	nana::gui::combox	_fileName;
	nana::gui::filebox  fb_o, fb_s, fb_p;
	OpenSaveBox     (	nana::gui::form &fm, 
						const nana::string   &label,
						const nana::string   &DefFileName=STR("") );

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


 protected:
	//virtual p::field_reference	put(p::field_reference f)override;
	//virtual p&	put(p&   pl) override

 private:
	nana::gui::label	_label;
    bool _user_selected, _canceled;
    //std::string         _myLayout;
    //nana::gui::place	_place;
	void pick_file(nana::gui::filebox&  fb, const nana::string &action, const nana::string &file_tip);

};

class EditLayout_Form : public nana::gui::form, public EditableForm
{public:
	EditLayout_Form (EditableWidget *fm );
 private:
    EditableWidget     *_fm ;
	OpenSaveBox			_OSbx;
	nana::gui::button	_ReCollocate, _hide, _panic, _def;
	nana::gui::textbox	_textBox;
	nana::gui::menu	   &_menuFile;

    void SetDefLayout       () override ;
    void AsignWidgetToFields() override ;
    void on_edited();
	void InitCaptions();

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

				