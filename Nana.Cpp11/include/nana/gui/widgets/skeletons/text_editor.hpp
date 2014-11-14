/*
 *	A text editor implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/skeletons/text_editor.hpp
 *	@description: 
 */

#ifndef NANA_GUI_SKELETONS_TEXT_EDITOR_HPP
#define NANA_GUI_SKELETONS_TEXT_EDITOR_HPP
#include "textbase.hpp"
#include <nana/gui/widgets/scroll.hpp>
#include <nana/unicode_bidi.hpp>
#include <memory>

namespace nana{	namespace widgets
{
	namespace skeletons
	{
		class text_editor
		{
			struct attributes;
			class editor_behavior_interface;
			class behavior_normal;
			class behavior_linewrapped;
		public:
			typedef nana::char_t	char_type;
			typedef textbase<char_type>::size_type size_type;
			typedef textbase<char_type>::string_type string_type;

			typedef nana::paint::graphics & graph_reference;

			struct ext_renderer_tag
			{
				std::function<void(graph_reference, const nana::rectangle& text_area, nana::color_t)> background;
			};

			text_editor(window, graph_reference);
			~text_editor();

			void typeface_changed();

			/// Determine whether the text_editor is line wrapped.
			bool line_wrapped() const;
			/// Set the text_editor whether it is line wrapped, it returns false if the state is not changed.
			bool line_wrapped(bool);

			void border_renderer(std::function<void(graph_reference, nana::color_t bgcolor)>);

			bool load(const nana::char_t*);
			void store(const nana::char_t* tfs) ;
			void store(const nana::char_t* tfs, nana::unicode encoding);

            nana::string filename() const
            {
                return textbase_.filename() ;
            }

            bool edited() const
            {
                return textbase_.edited();
            }

            bool saved() const
            {
                return textbase_.saved() ;
            }

            void set_unchanged();

			//text_area
			//@return: Returns true if the area of text is changed.
			bool text_area(const nana::rectangle&);
			bool tip_string(const nana::string&);

			const attributes & attr() const;
			bool multi_lines(bool);
			void editable(bool);
			void enable_background(bool);
			void enable_background_counterpart(bool);

			ext_renderer_tag& ext_renderer() const;

			unsigned line_height() const;
			unsigned screen_lines() const;

			bool getline(std::size_t pos, nana::string&) const;
			void text(nana::string);
			nana::string text() const;

			//move_caret
			//@brief: Set caret position through text coordinate
			void move_caret(const upoint&);
			void move_caret_end();
			void reset_caret_height() const;
			void reset_caret();
			void show_caret(bool isshow);

			bool selected() const;
			bool select(bool);
			//Set the end position of a selected string
			void set_end_caret();
			bool hit_text_area(const point&) const;
			bool hit_select_area(nana::upoint pos) const;
			bool move_select();
			bool mask(char_t);

			/// Returns width of text area excluding the vscroll size.
			unsigned width_pixels() const;
		public:
			void draw_scroll_rectangle();
			void render(bool focused);
		public:
			void put(nana::string);
			void put(nana::char_t);
			void copy() const;
			void paste();
			void enter();
			void del();
			void backspace();
			bool move(nana::char_t);
			void move_ns(bool to_north);	//Moves up and down
			void move_left();
			void move_right();
			nana::upoint mouse_caret(const point& screen_pos);
			nana::upoint caret() const;
			bool scroll(bool upwards, bool vertical);
			bool mouse_enter(bool);
			bool mouse_down(bool left_button, const point& screen_pos);
			bool mouse_move(bool left_button, const point& screen_pos);
			bool mouse_up(bool left_button, const point& screen_pos);

			skeletons::textbase<nana::char_t>& textbase();
			const skeletons::textbase<nana::char_t>& textbase() const;
		private:
			nana::color_t _m_bgcolor() const;
			bool _m_scroll_text(bool vertical);
			void _m_on_scroll(const arg_mouse&);
			void _m_scrollbar();
			nana::size _m_text_area() const;
			void _m_get_scrollbar_size();
			void _m_reset();
			nana::upoint _m_put(nana::string);
			nana::upoint _m_erase_select();

			bool _m_make_select_string(nana::string&) const;

			//_m_make_simple_nl
			//@brief: Transfers a string if it contains "0xD\0xA" or "0xA\0xD"
			static std::size_t _m_make_simple_nl(nana::string&);

			bool _m_cancel_select(int align);
			unsigned _m_tabs_pixels(size_type tabs) const;
			nana::size _m_text_extent_size(const char_type*) const;
			nana::size _m_text_extent_size(const char_type*, size_type n) const;

			//_m_move_offset_x_while_over_border
			//@brief: Moves the view window
			bool _m_move_offset_x_while_over_border(int many);

			int _m_text_top_base() const;
			//_m_endx
			//@brief: Gets the right point of text area
			int _m_endx() const;
			//_m_endy
			//@brief: Get the bottom point of text area
			int _m_endy() const;

			void _m_draw_tip_string() const;

			//_m_draw_string
			//@brief: Draw a line of string
			void _m_draw_string(int top, nana::color_t color, const nana::upoint& str_pos, const nana::string&, bool if_mask) const;
			//_m_draw
			//@brief: Draw a character at a position specified by caret pos. 
			//@return: true if beyond the border
			bool _m_draw(nana::char_t, std::size_t secondary_before);
			void _m_get_sort_select_points(nana::upoint& a, nana::upoint& b) const;

			void _m_offset_y(int y);

			unsigned _m_char_by_pixels(const nana::char_t*, std::size_t len, unsigned* pxbuf, int str_px, int pixels, bool is_rtl);
			unsigned _m_pixels_by_char(const nana::string&, std::size_t pos) const;
			static bool _m_is_right_text(const unicode_bidi::entity&);
		private:
			std::unique_ptr<editor_behavior_interface> behavior_;
			nana::window window_;
			graph_reference graph_;
			skeletons::textbase<nana::char_t> textbase_;
			nana::char_t mask_char_;

			mutable ext_renderer_tag ext_renderer_;

			struct attributes
			{
				nana::string tip_string;

				bool line_wrapped{false};
				bool multi_lines{true};
				bool editable{true};
				bool enable_background{true};
				bool enable_counterpart{false};
				nana::paint::graphics counterpart; //this is used to keep the background that painted by external part.

				std::unique_ptr<nana::scroll<true>>		vscroll;
				std::unique_ptr<nana::scroll<false>>	hscroll;
			}attributes_;

			struct text_area_type
			{
				nana::rectangle	area;

				bool captured;
				unsigned long tab_space;
				unsigned long scroll_pixels;
				unsigned long vscroll;
				unsigned long hscroll;
				std::function<void(nana::paint::graphics&, nana::color_t)> border_renderer;
			}text_area_;

			struct selection
			{
				enum mode_selection_t{mode_no_selected, mode_mouse_selected, mode_method_selected};

				mode_selection_t mode_selection;
				bool dragged;
				nana::upoint a, b;
			}select_;

			struct coordinate
			{
				nana::point offset;	//x stands for pixels, y for lines
				nana::upoint caret;	//position of caret by text, it specifies the position of a new character
				unsigned long xpos{0};	//This data is used for move up/down
			}points_;
		};
	}//end namespace skeletons
}//end namespace widgets
}//end namespace nana

#endif

