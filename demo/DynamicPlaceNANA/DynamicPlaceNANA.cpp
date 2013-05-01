#include <fstream> 
#include <iostream> 

#include <nana/charset.hpp>
#include <nana/gui/wvl.hpp>
#include <nana/gui/filebox.hpp>        // include in project: src/nana/gui/filebox.cpp
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>

const char* readLayot(const nana::string& FileName, std::string& layot)
{
		std::ifstream loy(FileName);
		std::string temp;

		while (std::getline(loy,temp)) layot+=temp;
		std::cout<< layot;
		return layot.c_str();
}	

class Dy_Form : public nana::gui::form
{public:
	Dy_Form ():	nana::gui::form  ( nana::rectangle(   nana::point(100,100),  nana::size(600,400) )),
				    _butOpenProject	(*this),      
					_butSaveProject	(*this),
					_pickProjectName(*this),
					_ReCollcate		(*this),
					_textBox		(*this),
					_ReLayot		(*this),
					_cbProject		(*this),
					_lProject		(*this),
				    _place			(*this),
					_Titel          (STR("A Dynamic Place for NANA"))
	{	
		 caption				(_Titel);
		_lProject.caption		(STR("Project:"	));
		_butOpenProject.caption	(STR("Open"		));
		_butSaveProject.caption	(STR("Save"		));
		_pickProjectName.caption(STR("..."		));
		_ReCollcate.caption		(STR("@"		));
		_ReLayot.caption		(STR("lay"		));
		_textBox.tip_string		(STR("type or load a layot..."		));
		_cbProject.push_back	(STR("layot.txt"));

		std::string layot;
        _place.div( readLayot(STR("layot.txt"), layot ="") );
		_place.field("proj_buttons")<< _butOpenProject<< _butSaveProject;
		_place.field("label"       )<< _lProject ;
		_place.field("proj_txt"    )<< _cbProject;
		_place.field("pick"		   )<< _pickProjectName;
		_place.field("textBox"	   )<< _textBox;
		_place.field("re"		   )<<_ReLayot<< _ReCollcate ;
		_place.collocate ();

		_cbProject.editable(false);
		_cbProject.option(0);
		_textBox.editable(true);

		_butOpenProject.make_event	<nana::gui::events::click> (*this , &Dy_Form::OpenFile				);
		_butSaveProject.make_event	<nana::gui::events::click> (*this , &Dy_Form::SaveFile				);
		_ReCollcate.make_event		<nana::gui::events::click> (_place, &nana::gui::place::collocate	);
		_ReLayot.make_event			<nana::gui::events::click> (*this , &Dy_Form::ReLayot				);
	}
 private:
	nana::string		_Titel;
	nana::gui::place	_place;
	nana::gui::button	_butOpenProject, _butSaveProject, _pickProjectName, _ReCollcate, _ReLayot;
	nana::gui::combox	_cbProject;
	nana::gui::label	_lProject;
	nana::gui::textbox	_textBox;

	void ReLayot()
	{
		std::string layot;
		nana::string FileName (  _cbProject.text(_cbProject.option())  );
        _place.div( readLayot(FileName, layot ="") );
		_place.collocate ();
	}
	void OpenFile()
	{	  
		 nana::gui::filebox fb(0, true);
		 fb.add_filter(STR("Text File"), STR("*.txt;*.doc"));
		 fb.add_filter(STR("All Files"), STR("*.*"));
		 nana::string file;
		 if(fb())  
		 { 
			 file = fb.file();
			_cbProject.push_back(file).option(_cbProject.the_number_of_options());
			caption	(_Titel+STR(" <")+ file+STR(">"));
			_textBox.load(file.c_str() );

			std::wcout<<std::endl<<STR("OpenFIle: ")<<file<<std::endl;
		 }
	}
		void SaveFile()
	{	  
		 nana::gui::filebox fb(0, false);
		 fb.add_filter(STR("Text File"), STR("*.txt;*.doc"));
		 fb.add_filter(STR("All Files"), STR("*.*"));
		 nana::string file;
		 if(fb())  
		 { 
			 file = fb.file();
			_cbProject.push_back(file).option(_cbProject.the_number_of_options());
			caption	(_Titel+STR(" <")+ file+STR(">"));
			_textBox.store(file.c_str() );

			std::wcout<<std::endl<<STR("OpenFIle: ")<<file<<std::endl;
		 }
	}
}	;


int main()
{
	Dy_Form tdf;
	tdf.show();
	nana::gui::exec();
	return 0;
}

