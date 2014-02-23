/**
\class nana::gui::menu
\brief  a list of items that specify options or group of options for an application.

1. The widget sets the shortkey that is a character behind the first of &-character in text for the item. e.g. "File(&F)" or "&File".
2. The type item_proxy is used for callbacking. A programmer should not take care about the object item_proxy. It is created and destroyed by menu. The definition is

		class item_proxy: nana::noncopyable
		{
		  public:
			implementation-specified constructor
			void enabled(bool); 			//Sets the enable state of the item.
			bool enabled() const; 		//Gets the enable state of the item.
			std::size_t index() const; 	//Gets the index of the item.
		  
		  private:			//Private data members...
		};

3. There is a helper for automatically popping a menu.

		class menu_popuper
		{
		  public:
					;//C++03
			menu_popuper(menu&, mouse::t = mouse::right_button);
			menu_popuper(menu&, window owner, const point& pos, 
			mouse::t =  mouse::right_button	);

					;//C++11
			menu_popuper(menu&, mouse = mouse::right_button);
			menu_popuper(menu&, window owner, const point& pos, mouse = mouse::right_button);

			void operator()(const eventinfo&);
		
		private: 			//Implemented-Specified private members
		
		};

	Now let's use it. There is a button, it popups the menu when it is clicked.

		#include <nana/gui/wvl.hpp>
		#include <nana/gui/widgets/button.hpp>
		#include <nana/gui/widgets/menu.hpp>

		void on_menu_item(nana::gui::menu::item_proxy& ip)
		{
			 std::size_t index = ip.index(); //Get the index of the clicking item.
		}

		int main()
		{
			using namespace nana::gui;
			form fm;

			;//We need a menu object
			menu mobj;
			mobj.append ( STR("Item 0"), &on_menu_item);
			mobj.append_splitter();
			mobj.append ( STR("Item 1"), &on_menu_item);

			;//Now we need a button.
			button btn(fm, nana::rectangle(nana::point(10, 10), nana::size(100, 25)));
			btn.caption(STR("Popup Menu"));

			;//Popup the menu when right clicking the button.
			btn.make_event<events::click>(menu_popuper(mobj));

			;//Or popuping the menu with a specified coordinate when any mouse button is clicked.
			;//btn.make_event<events::click> ( menu_popuper( mobj, btn, nana::point(0, 26),
			;//                                mouse::any_button );

			fm.show();
			exec();
		}



*/