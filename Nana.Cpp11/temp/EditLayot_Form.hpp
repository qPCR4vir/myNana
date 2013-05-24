#ifndef NANA_GUI_EditLayout_Form_HPP
#define NANA_GUI_EditLayout_Form_HPP

#include <../temp/CompoWidget.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/menubar.hpp>
class EditLayout_Form;
class EditableForm
{ public:
    EditableForm (nana::gui::form& fm,nana::string Titel, const nana::string &DefLayoutFileName=STR(""))
            :  _place	(fm), //_my_fm(&fm),
               _menu	(fm), 
               _Titel(std::move(Titel)),
               _DefLayoutFileName(DefLayoutFileName)
     {fm.caption(_Titel);};

	nana::string		_Titel;    //nana::gui::form    *_my_fm;
    std::string         _myLayout;
    nana::string        _DefLayoutFileName;	
    nana::gui::place	_place;
	nana::gui::menubar	_menu;
	nana::gui::menu	    *_menuProgram;
	std::unique_ptr <EditLayout_Form> _myEdLayForm;

	virtual void InitMyLayout   ()=0;
    virtual void AddMenuProgram (){_menuProgram=&_menu.push_back(STR("&Programm"));}
    virtual void InitMenu       ()
    {
       AddMenuProgram();
       _menuProgram->append(STR("&Edit this windows Layout"),[&](nana::gui::menu::item_proxy& ip)
	                                                            {EditMyLayout(); }                  );
    }
 	void         EditMyLayout   ();
    static const char* readLayout(const nana::string& FileName, std::string& Layout);
};

class EditLayout_Form : public nana::gui::form, public EditableForm
{public:
	EditLayout_Form (EditableForm *fm );
 private:
    EditableForm       *_fm ;
	OpenSaveBox			_OSbx;
	nana::gui::button	_ReCollocate;
	nana::gui::textbox	_textBox;
	nana::gui::menu	    &_menuFile;

    void on_edited();
	void InitCaptions();
	void InitMyLayout() override ;
    void MakeResponsive();
	void ReLayout();
	void OpenFile();
	void OpenFileN(const nana::string   &file=STR(""));
	void SaveFile();
	void SaveFileN(const nana::string   &fileTip=STR(""));
    void ForceSave(const nana::string   &file);

}	;
#endif 

//nana::gui::place     &pl, 
//						const std::string    &lay="", 
//						const nana::string   &DefLayoutFileName=STR(""),
//						