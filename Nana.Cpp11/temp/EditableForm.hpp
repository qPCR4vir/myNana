#ifndef NANA_GUI_EditableForm_HPP
#define NANA_GUI_EditableForm_HPP

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include <../temp/CompoWidget.hpp>

#include <iostream> 
#include <fstream> 

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
    bool  BlockInteratctiveEdition()
    {
        return _BlockInteratctiveEdition;
    }
    bool  BlockConfig()
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
 //   EditableWidget (nana::gui::widget& ThisWidget,nana::string Titel, const nana::string &DefLayoutFileName=STR(""))
 //           :  _place	       (ThisWidget),   
 //              _Titel(std::move(Titel)),        //   ???
 //              _DefLayoutFileName(DefLayoutFileName)
 //    {
 //         ThisWidget.caption(_Titel);       //   ???
 //       _menuBar.make_event <nana::gui::events::click> ([&](const nana::gui::eventinfo& ei)
 //       {
 //          if (ei.mouse.right_button)
 //          {
 //              std::wcout<<std::endl<<STR("Rigth Click: x,y=")<<ei.mouse.x<<STR(",")<<ei.mouse.y  <<std::endl;
 //              _menuProgram->popup (ThisWidget.handle(),ei.mouse.x,ei.mouse.y,false);

 //          }
 //          if (ei.mouse.left_button )         std::wcout<<std::endl<<STR("Left  Click")<<std::endl;
 //       }); 
 //   };
	//nana::string		_Titel;    //nana::gui::form    *_my_fm;
 //   std::string         _myLayout, _DefLayout;
 //   nana::string        _DefLayoutFileName;	
 //   nana::gui::place	_place;
	//nana::gui::menubar	_menuBar;
	//nana::gui::menu	    *_menuProgram;
	//std::unique_ptr <EditLayout_Form> _myEdLayForm;

 //   virtual ~EditableWidow();
 //   virtual void SetDefLayout   ()=0;
 //   virtual void AsignWidgetToFields()=0;
	//virtual void InitMyLayout   ()
	//{   
 //       SetDefLayout   ();
 //       readLayout( _DefLayoutFileName, _myLayout ="");
 //       if (_myLayout.empty() )
 //           _myLayout= _DefLayout;

 //       _place.div( _myLayout.c_str () );
 //       AsignWidgetToFields();
	//	_place.collocate ();
	//}
 //   virtual void AddMenuProgram (){_menuProgram=&_menuBar.push_back(STR("&Programm"));}
 //   virtual void InitMenu       ()
 //   {
 //      AddMenuProgram();
 //      _menuProgram->append(STR("&Edit this windows Layout"),[&](nana::gui::menu::item_proxy& ip)
	//                                                            {EditMyLayout(); }                  );
 //      _menuProgram->append(STR("&Reset this windows default Layout"),[&](nana::gui::menu::item_proxy& ip)
	//                                                            {ResetDefLayout(); ReCollocate( );} );

 //      
 //   }
 //   void ResetDefLayout()
 //   {
 //       _myLayout=_DefLayout;
 //   }

 //	void         EditMyLayout   ();
 //   static const char* readLayout(const nana::string& FileName, std::string& Layout);
 //   void ReCollocate( std::string  Layout)
 //   {
 //       _myLayout.swap(Layout);
 //       ReCollocate( );
 //  }
 //   void ReCollocate( )
 //   {
 //       _place.div(_myLayout.c_str ());     
	//    _place.collocate ();
 //   }


};


class EditableForm: public EditableWidget
{ public:
    EditableForm (nana::gui::form& ThisForm,nana::string Titel, const nana::string &DefLayoutFileName=STR(""))
            :  _place	       (ThisForm),                //_my_fm(&fm),
               _menuBar	       (ThisForm), 
               _Titel(std::move(Titel)),
               _DefLayoutFileName(DefLayoutFileName)
     {
         ThisForm.caption(_Titel);
         _menuBar.make_event <nana::gui::events::click> ([&](const nana::gui::eventinfo& ei)
        {
           if (ei.mouse.right_button)
           {
               std::wcout<<std::endl<<STR("Rigth Click: x,y=")<<ei.mouse.x<<STR(",")<<ei.mouse.y  <<std::endl;
               _menuProgram->popup (ThisForm.handle(),ei.mouse.x,ei.mouse.y,false);

           }
           if (ei.mouse.left_button )         std::wcout<<std::endl<<STR("Left  Click")<<std::endl;
        }); 
    };
	nana::string		_Titel;    //nana::gui::form    *_my_fm;
    std::string         _myLayout, _DefLayout;
    nana::string        _DefLayoutFileName;	
    nana::gui::place	_place;
	nana::gui::menubar	_menuBar;
	nana::gui::menu	    *_menuProgram;
	std::unique_ptr <EditLayout_Form> _myEdLayForm;

    virtual ~EditableForm();
    virtual void SetDefLayout   ()=0;
    virtual void AsignWidgetToFields()=0;
	virtual void InitMyLayout   ()
	{   
        SetDefLayout   ();
        readLayout( _DefLayoutFileName, _myLayout ="");
        if (_myLayout.empty() )
            _myLayout= _DefLayout;

        _place.div( _myLayout.c_str () );
        AsignWidgetToFields();
		_place.collocate ();
	}
    virtual void AddMenuProgram (){_menuProgram=&_menuBar.push_back(STR("&Programm"));}
    virtual void InitMenu       ()
    {
       AddMenuProgram();
       _menuProgram->append(STR("&Edit this windows Layout"),[&](nana::gui::menu::item_proxy& ip)
	                                                            {EditMyLayout(); }                  );
       _menuProgram->append(STR("&Reset this windows default Layout"),[&](nana::gui::menu::item_proxy& ip)
	                                                            {ResetDefLayout(); ReCollocate( );} );

       
    }
    void ResetDefLayout()
    {
        _myLayout=_DefLayout;
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


class EditLayout_Form : public nana::gui::form, public EditableForm
{public:
	EditLayout_Form (EditableForm *fm );
 private:
    EditableForm       *_fm ;
	OpenSaveBox			_OSbx;
	nana::gui::button	_ReCollocate;
	nana::gui::textbox	_textBox;
	nana::gui::menu	    &_menuFile;

    void SetDefLayout   () override ;
    void AsignWidgetToFields() override ;
    void on_edited();
	void InitCaptions();

    void MakeResponsive();
	void ReLayout();
	void OpenFile();
	void OpenFileN(const nana::string   &file=STR(""));
	void SaveFile();
	void SaveFileN(const nana::string   &fileTip=STR(""));
    void ForceSave(const nana::string   &file);

}	;
#endif 

				