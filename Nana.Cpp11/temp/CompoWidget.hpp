#ifndef NANA_GUI_CompoWidget_HPP
#define NANA_GUI_CompoWidget_HPP

#include <nana/gui/wvl.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/filebox.hpp>       
#include <nana/gui/widgets/panel.hpp>

//
//class CompoWidgetP : public nana::gui::panel<false>
//{
//  public:
//	CompoWidgetP (	const nana::string &caption_, 
//					const std::string& fieldName="", 
//					const std::string& layot="",
//					const nana::string& fileName=STR(""));
//
//	void caption(const nana::string &caption_)	{ _caption=caption_;}
//	const nana::string &caption() const			{return _caption;   }
//	const char *ly(const std::string& layot )	{return (_lay=layot).c_str ();}
//	const char *ly()const						{return _lay.c_str ();}
//	const char *fn()const						{return _fieldname.c_str();}
//	virtual ~CompoWidgetP ()						{}
//
//	friend nana::gui::place::field_reference	operator<<(nana::gui::place::field_reference f, CompoWidgetP& osb);
//	friend nana::gui::place&					operator<<(nana::gui::place& pl,				CompoWidgetP& osb);
//
//protected:
//	typedef nana::gui::place p ;
//	virtual p::field_reference	put(p::field_reference f)=0;
//	virtual p&					put(p&				  pl)
//	{	pl.field (fn ())<<*this;
//		return pl;			
//	}
//
//private:
//	nana::string _caption;
//	std::string  _lay, _fieldname;
//};

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
	void		save();
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