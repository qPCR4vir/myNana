#ifndef NANA_GUI_CompoWidget_HPP
#define NANA_GUI_CompoWidget_HPP

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/filebox.hpp>       

class OpenSaveBox : public  nana::gui::panel<false>    
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
	nana::string FileName()const						{  return _fileName.caption();}
	void		 FileName(const nana::string&  FileName){ _fileName.push_back(FileName).option(_fileName.the_number_of_options());}
	void		open();
	void		save(const nana::string &file_tip=STR("") );
	void		pick();
    bool        UserSelected() const {return _user_selected ;}
    bool        Canceled()     const {return _canceled;}


 protected:
	//virtual p::field_reference	put(p::field_reference f)override;
	//virtual p&	put(p&   pl) override

 private:
	nana::gui::label	_label;
    bool _user_selected, _canceled;
    std::string         _myLayout;
    nana::gui::place	_place;

};

#endif 