/*
 *	Basis Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/basis.cpp
 *
 *	This file provides basis class and data structrue that required by gui
 */

#include <nana/gui/basis.hpp>

namespace nana
{
	//struct appearance
	//@brief: Window appearance structure
		appearance::appearance()
			:taskbar(true), floating(false), no_activate(false),
			 minimize(true), maximize(true), sizable(true),
			 decoration(true)
		{}

		appearance::appearance(bool has_decorate, bool taskbar, bool is_float, bool no_activate, bool min, bool max, bool sizable)
			:	taskbar(taskbar), floating(is_float), no_activate(no_activate),
				minimize(min), maximize(max), sizable(sizable),
				decoration(has_decorate)
		{}
        namespace color
        {
		    color_t mix(color_t a, color_t b, double fade_rate)  
		    {
			    pixel_rgb_t pa, pb, ret;
			    ret.u.color = 0;
			    pa.u.color = a;
			    pb.u.color = b;

			    ret.u.element.red   = static_cast<unsigned char>(pa.u.element.red   * fade_rate + pb.u.element.red   * (1 - fade_rate));
			    ret.u.element.green = static_cast<unsigned char>(pa.u.element.green * fade_rate + pb.u.element.green * (1 - fade_rate));
			    ret.u.element.blue  = static_cast<unsigned char>(pa.u.element.blue  * fade_rate + pb.u.element.blue  * (1 - fade_rate));

			    return ret.u.color;
		    }
            schema::schema():sys(def_schema)    { }
            color_schema current_schema= def_schema;

        }

	//end struct appearance
}//end namespace nana

