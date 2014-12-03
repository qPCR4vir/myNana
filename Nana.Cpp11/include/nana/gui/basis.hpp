/*
 *	Basis Implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/basis.hpp
 *
 *	This file provides basis class and data structrue that required by gui
 */

#ifndef NANA_GUI_BASIS_HPP
#define NANA_GUI_BASIS_HPP

#include "../basic_types.hpp"
#include "../traits.hpp"	//metacomp::fixed_type_set
#include <array>

namespace nana
{
	namespace detail
	{
		struct native_window_handle_impl{};
		struct window_handle_impl{};
		struct event_handle_impl{};
	}

	enum class checkstate
	{
		unchecked, checked, partial
	};

	enum class window_border
	{
		none,
		left, right, top, bottom,
		top_left, top_right, bottom_left, bottom_right
	};

	enum class bground_mode
	{
		none,
		basic,
		blend
	};

	namespace category
	{
		enum class flags
		{
			super,
			widget = 0x1,
			lite_widget = 0x3,
			root = 0x5,
			frame = 0x9
		};
		//wait for constexpr
		struct widget_tag{ static const flags value = flags::widget; };
		struct lite_widget_tag : widget_tag{ static const flags value = flags::lite_widget;};
		struct root_tag : widget_tag{ static const flags value = flags::root; };
		struct frame_tag: widget_tag{ static const flags value = flags::frame; };
	}// end namespace category

	typedef detail::native_window_handle_impl * native_window_type;

	typedef detail::window_handle_impl*	window; ///< \see [What is window class ](https://sourceforge.net/p/nanapro/discussion/general/thread/bd0fabfb/) 
	typedef detail::event_handle_impl*	event_handle;


	struct keyboard
	{
		enum t{
			//Control Code for ASCII
			select_all	= 0x1,
			end_of_text		= 0x3,	//Ctrl+C
			backspace	= 0x8,	tab		= 0x9,
			enter_n		= 0xA,	enter	= 0xD,	enter_r = 0xD,
			alt			= 0x12,
			sync_idel		= 0x16,	//Ctrl+V
			cancel			= 0x18,	//Ctrl+X
			end_of_medium	= 0x19,	//Ctrl+Y
			substitute		= 0x1A,	//Ctrl+Z
			escape		= 0x1B,

			//The following names are intuitive name of ASCII control codes
			copy	= 0x3,	//end_of_text
			paste	= 0x16,	//sync_idel
			cut		= 0x18,	//cancel
			redo	= 0x19,	//end_of_medium
			undo	= 0x1A,	//substitue

			//System Code for OS
			os_pageup		= 0x21,	os_pagedown,
			os_arrow_left	= 0x25, os_arrow_up, os_arrow_right, os_arrow_down,
			os_insert		= 0x2D, os_del
		};
	};

	namespace color
	{
		enum    // http://en.wikipedia.org/wiki/List_of_colors:_A%E2%80%93F
		{
			Black	    =	0x000000	,
            Night	    =	0x0C090A	,
            Gunmetal    =	0x2C3539	,
            Midnight	=	0x2B1B17	,
            Charcoal	=	0x34282C	,
            Dark_Slate_Grey	=	0x25383C	,
            Oil	        =	0x3B3131	,
            Black_Cat	=	0x413839	,
            Iridium	    =	0x3D3C3A	,
            Black_Eel	=	0x463E3F	,
            Black_Cow	=	0x4C4646	,
            Gray_Wolf	=	0x504A4B	,
            Vampire_Gray	=	0x565051	,
            Gray_Dolphin	=	0x5C5858	,
            Carbon_Gray	=	0x625D5D	,
            Ash_Gray	=	0x666362	,
            Cloudy_Gray	=	0x6D6968	,
            Smokey_Gray	=	0x726E6D	,
            Gray	    =	0x736F6E	,
            Granite	    =	0x837E7C	,
            Battleship_Gray	=	0x848482	,
            Gray_Cloud	=	0xB6B6B4	,
            Gray_Goose	=	0xD1D0CE	,
            Platinum	=	0xE5E4E2	,
            Metallic_Silver	=	0xBCC6CC	,
            Blue_Gray	=	0x98AFC7	,
            Light_Slate_Gray	=	0x6D7B8D	,
            Slate_Gray	=	0x657383	,
            Jet_Gray	=	0x616D7E	,
            Mist_Blue	=	0x646D7E	,
            Marble_Blue	=	0x566D7E	,
            Slate_Blue	=	0x737CA1	,
            Slate_Blue2	=	0x357EC7	,
            Steel_Blue	=	0x4863A0	,
            Blue_Jay	=	0x2B547E	,
            Dark_Slate_Blue	=	0x2B3856	,
            Midnight_Blue	=	0x151B54	,
            Navy_Blue	=	0x000080	,
            Blue_Whale	=	0x342D7E	,
            Lapis_Blue	=	0x15317E	,
            Cornflower_Blue	=	0x151B8D	,
            Earth_Blue	=	0x0000A0	,
            Cobalt_Blue	=	0x0020C2	,
            Blueberry_Blue	=	0x0041C2	,
            Sapphire_Blue	=	0x2554C7	,
            Blue_Eyes	=	0x1569C7	,
            Royal_Blue	=	0x2B60DE	,
            Blue_Orchid	=	0x1F45FC	,
            Blue_Lotus	=	0x6960EC	,
            Light_Slate_Blue	=	0x736AFF	,
            Glacial_Blue_Ice	=	0x368BC1	,
            Silk_Blue	=	0x488AC7	,
            Blue_Ivy	=	0x3090C7	,
            Blue_Koi	=	0x659EC7	,
            Columbia_Blue	=	0x87AFC7	,
            Baby_Blue	=	0x95B9C7	,
            Light_Steel_Blue	=	0x728FCE	,
            Ocean_Blue	=	0x2B65EC	,
            Blue_Ribbon	=	0x306EFF	,
            Blue_Dress	=	0x157DEC	,
            Dodger_Blue	=	0x1589FF	,
            Cornflower_Blue2	=	0x6495ED	,
            Sky_Blue	=	0x6698FF	,
            Butterfly_Blue	=	0x38ACEC	,
            Iceberg	    =	0x56A5EC	,
            Crystal_Blue	=	0x5CB3FF	,
            Deep_Sky_Blue	=	0x3BB9FF	,
            Denim_Blue	=	0x79BAEC	,
            Light_Sky_Blue	=	0x82CAFA	,
            Day_Sky_Blue	=	0x82CAFF	,
            Jeans_Blue	=	0xA0CFEC	,
            Blue_Angel	=	0xB7CEEC	,
            Pastel_Blue	=	0xB4CFEC	,
            Sea_Blue	=	0xC2DFFF	,
            Powder_Blue	=	0xC6DEFF	,
            Coral_Blue	=	0xAFDCEC	,
            Light_Blue	=	0xADDFFF	,
            Robin_Egg_Blue	=	0xBDEDFF	,
            Pale_Blue_Lily	=	0xCFECEC	,
            Light_Cyan	=	0xE0FFFF	,
            Water	    =	0xEBF4FA	,
            AliceBlue	=	0xF0F8FF	,
            Azure	    =	0xF0FFFF	,
            Light_Slate	=	0xCCFFFF	,
            Light_Aquamarine	=	0x93FFE8	,
            Electric_Blue	=	0x9AFEFF	,
            Aquamarine	=	0x7FFFD4	,
            Cyan_or_Aqua	=	0x00FFFF	,
            Tron_Blue	=	0x7DFDFE	,
            Blue_Zircon	=	0x57FEFF	,
            Blue_Lagoon	=	0x8EEBEC	,
            Celeste	    =	0x50EBEC	,
            Blue_Diamond	=	0x4EE2EC	,
            Tiffany_Blue	=	0x81D8D0	,
            Cyan_Opaque	=	0x92C7C7	,
            Blue_Hosta	=	0x77BFC7	,
            Northern_Lights_Blue	=	0x78C7C7	,
            Medium_Turquoise	=	0x48CCCD	,
            Turquoise	=	0x43C6DB	,
            Jellyfish	=	0x46C7C7	,
            Macaw_Blue_Green	=	0x43BFC7	,
            Light_Sea_Green	=	0x3EA99F	,
            Dark_Turquoise	=	0x3B9C9C	,
            Sea_Turtle_Green	=	0x438D80	,
            Medium_Aquamarine	=	0x348781	,
            Greenish_Blue	=	0x307D7E	,
            Grayish_Turquoise	=	0x5E7D7E	,
            Beetle_Green	=	0x4C787E	,
            Teal	    =	0x008080	,
            Sea_Green	=	0x4E8975	,
            Camouflage_Green	=	0x78866B	,
            Sage_Green	=	0x848b79	,
            Hazel_Green	=	0x617C58	,
            Venom_Green	=	0x728C00	,
            Fern_Green	=	0x667C26	,
            Dark_Forrest_Green	=	0x254117	,
            Medium_Sea_Green	=	0x306754	,
            Medium_Forest_Green	=	0x347235	,
            Seaweed_Green	=	0x437C17	,
            Pine_Green	=	0x387C44	,
            Jungle_Green	=	0x347C2C	,
            Shamrock_Green	=	0x347C17	,
            Medium_Spring_Green	=	0x348017	,
            Forest_Green	=	0x4E9258	,
            Green_Onion	=	0x6AA121	,
            Spring_Green	=	0x4AA02C	,
            Lime_Green	=	0x41A317	,
            Clover_Green	=	0x3EA055	,
            Green_Snake	=	0x6CBB3C	,
            Alien_Green	=	0x6CC417	,
            Green_Apple	=	0x4CC417	,
            Yellow_Green	=	0x52D017	,
            Kelly_Green	=	0x4CC552	,
            Zombie_Green	=	0x54C571	,
            Frog_Green	=	0x99C68E	,
            Green_Peas	=	0x89C35C	,
            Dollar_Bill_Green	=	0x85BB65	,
            Dark_Sea_Green	=	0x8BB381	,
            Iguana_Green	=	0x9CB071	,
            Avocado_Green	=	0xB2C248	,
            Pistachio_Green	=	0x9DC209	,
            Salad_Green	=	0xA1C935	,
            Hummingbird_Green	=	0x7FE817	,
            Nebula_Green	=	0x59E817	,
            Stoplight_Go_Green	=	0x57E964	,
            Algae_Green	=	0x64E986	,
            Jade_Green	=	0x5EFB6E	,
            Green	    =	0x00FF00	,
            Emerald_Green	=	0x5FFB17	,
            Lawn_Green	=	0x87F717	,
            Chartreuse	=	0x8AFB17	,
            Dragon_Green	=	0x6AFB92	,
            Mint_green	=	0x98FF98	,
            Green_Thumb	=	0xB5EAAA	,
            Light_Jade	=	0xC3FDB8	,
            Tea_Green	=	0xCCFB5D	,
            Green_Yellow	=	0xB1FB17	,
            Slime_Green	=	0xBCE954	,
            Goldenrod	=	0xEDDA74	,
            Harvest_Gold	=	0xEDE275	,
            Sun_Yellow	=	0xFFE87C	,
            Yellow	    =	0xFFFF00	,
            Corn_Yellow	=	0xFFF380	,
            Parchment	=	0xFFFFC2	,
            Cream	    =	0xFFFFCC	,
            Lemon_Chiffon	=	0xFFF8C6	,
            Cornsilk	=	0xFFF8DC	,
            Beige	    =	0xF5F5DC	,
            Blonde	    =	0xFBF6D9	,
            AntiqueWhite	=	0xFAEBD7	,
            Champagne	=	0xF7E7CE	,
            BlanchedAlmond	=	0xFFEBCD	,
            Vanilla	    =	0xF3E5AB	,
            Tan_Brown	=	0xECE5B6	,
            Peach	    =	0xFFE5B4	,
            Mustard	    =	0xFFDB58	,
            Rubber_Ducky_Yellow	=	0xFFD801	,
            Bright_Gold	=	0xFDD017	,
            Golden_brown	=	0xEAC117	,
            Macaroni_and_Cheese	=	0xF2BB66	,
            Saffron	    =	0xFBB917	,
            Beer	    =	0xFBB117	,
            Cantaloupe	=	0xFFA62F	,
            Bee_Yellow	=	0xE9AB17	,
            Brown_Sugar	=	0xE2A76F	,
            BurlyWood	=	0xDEB887	,
            Deep_Peach	=	0xFFCBA4	,
            Ginger_Brown	=	0xC9BE62	,
            School_Bus_Yellow	=	0xE8A317	,
            Sandy_Brown	=	0xEE9A4D	,
            Fall_Leaf_Brown	=	0xC8B560	,
            Orange_Gold	=	0xD4A017	,
            Sand	    =	0xC2B280	,
            Cookie_Brown	=	0xC7A317	,
            Caramel	    =	0xC68E17	,
            Brass	    =	0xB5A642	,
            Khaki	    =	0xADA96E	,
            Camel_brown	=	0xC19A6B	,
            Bronze	    =	0xCD7F32	,
            Tiger_Orange	=	0xC88141	,
            Cinnamon	=	0xC58917	,
            Bullet_Shell	=	0xAF9B60	,
            Dark_Goldenrod	=	0xAF7817	,
            Copper	    =	0xB87333	,
            Wood	    =	0x966F33	,
            Oak_Brown	=	0x806517	,
            Moccasin	=	0x827839	,
            Army_Brown	=	0x827B60	,
            Sandstone	=	0x786D5F	,
            Mocha	    =	0x493D26	,
            Taupe	    =	0x483C32	,
            Coffee	    =	0x6F4E37	,
            Brown_Bear	=	0x835C3B	,
            Red_Dirt	=	0x7F5217	,
            Sepia	    =	0x7F462C	,
            Orange_Salmon	=	0xC47451	,
            Rust	    =	0xC36241	,
            Red_Fox	    =	0xC35817	,
            Chocolate	=	0xC85A17	,
            Sedona	    =	0xCC6600	,
            Papaya_Orange	=	0xE56717	,
            Halloween_Orange	=	0xE66C2C	,
            Pumpkin_Orange	=	0xF87217	,
            Construction_Cone_Orange	=	0xF87431	,
            Sunrise_Orange	=	0xE67451	,
            Mango_Orange	=	0xFF8040	,
            Dark_Orange	=	0xF88017	,
            Coral	    =	0xFF7F50	,
            Basket_Ball_Orange	=	0xF88158	,
            Light_Salmon	=	0xF9966B	,
            Tangerine	=	0xE78A61	,
            Dark_Salmon	=	0xE18B6B	,
            Light_Coral	=	0xE77471	,
            Bean_Red	=	0xF75D59	,
            Valentine_Red	=	0xE55451	,
            Shocking_Orange	=	0xE55B3C	,
            Red	=	0xFF0000	,
            Scarlet	=	0xFF2400	,
            Ruby_Red	=	0xF62217	,
            Ferrari_Red	=	0xF70D1A	,
            Fire_Engine_Red	=	0xF62817	,
            Lava_Red	=	0xE42217	,
            Love_Red	=	0xE41B17	,
            Grapefruit	=	0xDC381F	,
            Chestnut_Red	=	0xC34A2C	,
            Cherry_Red	=	0xC24641	,
            Mahogany	=	0xC04000	,
            Chilli_Pepper	=	0xC11B17	,
            Cranberry	=	0x9F000F	,
            Red_Wine	=	0x990012	,
            Burgundy	=	0x8C001A	,
            Chestnut	=	0x954535	,
            Blood_Red	=	0x7E3517	,
            Sienna	    =	0x8A4117	,
            Sangria	    =	0x7E3817	,
            Firebrick	=	0x800517	,
            Maroon	    =	0x810541	,
            Plum_Pie	=	0x7D0541	,
            Velvet_Maroon	=	0x7E354D	,
            Plum_Velvet	=	0x7D0552	,
            Rosy_Finch	=	0x7F4E52	,
            Puce	    =	0x7F5A58	,
            Dull_Purple	=	0x7F525D	,
            Rosy_Brown	=	0xB38481	,
            Khaki_Rose	=	0xC5908E	,
            Pink_Bow	=	0xC48189	,
            Lipstick_Pink	=	0xC48793	,
            Rose	    =	0xE8ADAA	,
            Desert_Sand	=	0xEDC9AF	,
            Pig_Pink	=	0xFDD7E4	,
            Cotton_Candy	=	0xFCDFFF	,
            Pink_Bubblegum	=	0xFFDFDD	,
            Misty_Rose	=	0xFBBBB9	,
            Pink	    =	0xFAAFBE	,
            Light_Pink	=	0xFAAFBA	,
            Flamingo_Pink	=	0xF9A7B0	,
            Pink_Rose	=	0xE7A1B0	,
            Pink_Daisy	=	0xE799A3	,
            Cadillac_Pink	=	0xE38AAE	,
            Carnation_Pink	=	0xF778A1	,
            Blush_Red	=	0xE56E94	,
            Hot_Pink	=	0xF660AB	,
            Watermelon_Pink	=	0xFC6C85	,
            Violet_Red	=	0xF6358A	,
            Deep_Pink	=	0xF52887	,
            Pink_Cupcake	=	0xE45E9D	,
            Pink_Lemonade	=	0xE4287C	,
            Neon_Pink	=	0xF535AA	,
            Magenta	=	0xFF00FF	,
            Dimorphotheca_Magenta	=	0xE3319D	,
            Bright_Neon_Pink	=	0xF433FF	,
            Pale_Violet_Red	=	0xD16587	,
            Tulip_Pink	=	0xC25A7C	,
            Medium_Violet_Red	=	0xCA226B	,
            Rogue_Pink	=	0xC12869	,
            Burnt_Pink	=	0xC12267	,
            Bashful_Pink	=	0xC25283	,
            Carnation_Pink2	=	0xC12283	,
            Plum	    =	0xB93B8F	,
            Viola_Purple	=	0x7E587E	,
            Purple_Iris	=	0x571B7E	,
            Plum_Purple	=	0x583759	,
            Indigo	    =	0x4B0082	,
            Purple_Monster	=	0x461B7E	,
            Purple_Haze	=	0x4E387E	,
            Eggplant	=	0x614051	,
            Grape	    =	0x5E5A80	,
            Purple_Jam	=	0x6A287E	,
            Dark_Orchid	=	0x7D1B7E	,
            Purple_Flower	=	0xA74AC7	,
            Medium_Orchid	=	0xB048B5	,
            Purple_Amethyst	=	0x6C2DC7	,
            Dark_Violet	=	0x842DCE	,
            Violet	    =	0x8D38C9	,
            Purple_Sage_Bush	=	0x7A5DC7	,
            Lovely_Purple	=	0x7F38EC	,
            Purple	    =	0x8E35EF	,
            Aztech_Purple	=	0x893BFF	,
            Medium_Purple	=	0x8467D7	,
            Jasmine_Purple	=	0xA23BEC	,
            Purple_Daffodil	=	0xB041FF	,
            Tyrian_Purple	=	0xC45AEC	,
            Crocus_Purple	=	0x9172EC	,
            Purple_Mimosa	=	0x9E7BFF	,
            Heliotrope_Purple	=	0xD462FF	,
            Crimson	        =	0xE238EC	,
            Purple_Dragon	=	0xC38EC7	,
            Lilac	    =	0xC8A2C8	,
            Blush_Pink	=	0xE6A9EC	,
            Mauve	    =	0xE0B0FF	,
            Wisteria_Purple	=	0xC6AEC7	,
            Blossom_Pink	=	0xF9B7FF	,
            Thistle	    =	0xD2B9D3	,
            Periwinkle	=	0xE9CFEC	,
            Lavender_Pinocchio	=	0xEBDDE2	,
            Lavender_blue	=	0xE3E4FA	,
            Pearl	    =	0xFDEEF4	,
            SeaShell	=	0xFFF5EE	,
            Milk_White	=	0xFEFCFF	,
            White	    =	0xFFFFFF	,
            Blue        =   0x0000FF   ,
			white	= 0xFFFFFF,
			blue	= 0x0000FF,
			green	= 0x00FF00,
			red		= 0xFF0000,

			button_face_shadow_start = 0xF5F4F2,
			button_face_shadow_end = 0xD5D2CA,
			button_face = 0xD4D0C8,
			dark_border	= 0x404040,
			gray_border	= 0x808080,
			highlight = 0x1CC4F7,
			header_border = 0xDEDFE1,
			list_header_border = header_border,
            header_bg = 0xF1F2F4,
            list_header_bg = header_bg,
            header_highlighted_bg = White,
            list_header_highlighted_bg = header_highlighted_bg,
            header_grabed_bg = 0x8BD6F6,
            list_header_grabed_bg = header_grabed_bg,
            header_pressed_bg = header_grabed_bg,
            list_header_pressed_bg = header_pressed_bg,
            header_floated_bg = 0xBABBBC,
            list_header_floated_bg = header_floated_bg,
            header_floated_fg = White,
            list_header_floated_fg = header_floated_fg,

		};
        struct schema
        {
		    enum // after any change here, actualize def_schema
		    {
			    button_face_shadow_start,
			    button_face_shadow_end,
			    button_face,
			    dark_border,
			    gray_border,
			    highlight,
                header_border,
                list_header_border,
                header_bg  ,
                list_header_bg  ,
                header_highlighted_bg  ,
                list_header_highlighted_bg  ,
                header_grabed_bg  ,
                list_header_grabed_bg  ,
                header_pressed_bg  ,
                list_header_pressed_bg  ,
                header_floated_bg  ,
                list_header_floated_bg  ,
                header_floated_fg ,
                list_header_floated_fg ,
                end
		    };
            using color_schema=  std::array<color_t,schema::end> ;
            color_schema sys ; // ??
            schema();
        };
        using color_schema=  schema::color_schema;
        const static color_schema def_schema = color_schema{// please, copy and paste from the schema::enum (no end !)
		    {
			    button_face_shadow_start,
			    button_face_shadow_end,
			    button_face,
			    dark_border,
			    gray_border,
			    highlight,
                header_border,
                list_header_border,
                header_bg  ,
                list_header_bg  ,
                header_highlighted_bg  ,
                list_header_highlighted_bg  ,
                header_grabed_bg  ,
                list_header_grabed_bg  ,
                header_pressed_bg  ,
                list_header_pressed_bg  ,
                header_floated_bg  ,
                list_header_floated_bg  ,
                header_floated_fg ,
                list_header_floated_fg ,
            }
		    };
        extern color_schema current_schema; /// \todo need thread protection
		color_t mix(color_t a, color_t b, double fade_rate)  ;

        // use:                                             color_t myButtonColor      = color::def_schema[color::schema::button_face];
        // or abreviate for default color scheme only:      color_t myOtherBottonColor = color::button_face_shadow_end;
        // normal use of color:                             color_t myPreferedColor    = color::blue;


        //nana::color::color_schema mySchema;                           // now it is like default
        //mySchema[nana::color::schema::button_face]=nana::color::blue; // now it is different
        //nana::color_t myColor    = mySchema[nana::color::schema::button_face];

        //nana::color::schema customSchema;                            // now it is like default
        //coustomSchema.sys[coustomSchema.button_face]=nana::color::blue; // now it is different
        //nana::color_t yourColor    = coustomSchema.sys[coustomSchema.button_face];

        //bool defaultcolors= false;
        //nana::color::schema current_Schema = defaultcolors ? nana::color::schema{} : customSchema ; // now it is like default
        //nana::color_t useColor    = current_Schema.sys[current_Schema.button_face];


	};



	enum class cursor
	{
		hand	= 60,     ///< displays a hand to indicate a text or an element is clickable
		arrow	= 68,     ///< the default shape
		wait	= 150,    ///< indicates the system is currently busy
		iterm	= 152,    ///< A text caret. Displays a caret to indicate the UI is input able
		size_we	= 108,
		size_ns	= 116,
		size_top_left = 134,
		size_top_right = 136,
		size_bottom_left = 12,
		size_bottom_right = 14
	};

	enum class mouse
	{
		any_button, 
		left_button, 
		middle_button, 
		right_button
	};

	enum class z_order_action
	{
		none, 
		bottom,       ///< brings a window at the bottom of z-order.
		top,          ///< brings a widget at the top of the z-order.
		topmost,      ///< brings a window at the top of the z-order and stays here.
		foreground    ///< brings a window to the foreground.
	};

	/// Window appearance structure defined to specify the appearance of a form
	struct appearance
	{
		bool taskbar;
		bool floating;
		bool no_activate;

		bool minimize;
		bool maximize;
		bool sizable;

		bool decoration;

		appearance();
		appearance(bool has_decoration, bool taskbar, bool floating, bool no_activate, bool min, bool max, bool sizable);
	};
    /// Provided to generate an appearance object with better readability and understandability   
	struct appear
	{
		struct minimize{};
		struct maximize{};
		struct sizable{};
		struct taskbar{};
		struct floating{};
		struct no_activate{};
        /// Create an appearance of a window with "decoration"
		template<   typename Minimize = null_type,
					typename Maximize = null_type,
					typename Sizable = null_type,
					typename Floating = null_type,
					typename NoActive = null_type>
		struct decorate
		{
			typedef meta::fixed_type_set<Minimize, Maximize, Sizable, Floating, NoActive> set_type;

			operator appearance() const
			{
				return appearance(	true, true,
									set_type::template count<floating>::value,
									set_type::template count<no_activate>::value,
									set_type::template count<minimize>::value,
									set_type::template count<maximize>::value,
									set_type::template count<sizable>::value
									);
			}
		};
        /// Create an appearance of a window without "decoration"
		template < typename Taskbar  = null_type, 
                   typename Floating = null_type, 
                   typename NoActive = null_type, 
                   typename Minimize = null_type, 
                   typename Maximize = null_type, 
                   typename Sizable  = null_type>
		struct bald
		{
			typedef meta::fixed_type_set<Taskbar, Floating, NoActive, Minimize, Maximize, Sizable> set_type;

			operator appearance() const
			{
				return appearance(	false,
									set_type::template count<taskbar>::value,
									set_type::template count<floating>::value,
									set_type::template count<no_activate>::value,
									set_type::template count<minimize>::value,
									set_type::template count<maximize>::value,
									set_type::template count<sizable>::value
									);
			}
		};
        /// Create a window with decoration depending on the first non-type template parameter
		template < bool HasDecoration = true, 
                   typename Sizable = null_type, 
                   typename Taskbar = null_type, 
                   typename Floating = null_type, 
                   typename NoActive = null_type>
		struct optional
		{
			typedef meta::fixed_type_set<Taskbar, Floating, NoActive> set_type;

			operator appearance() const
			{
				return appearance(HasDecoration,
									set_type::template count<taskbar>::value,
									set_type::template count<floating>::value,
									set_type::template count<no_activate>::value,
									true, true,
									set_type::template count<sizable>::value);
			}
		};
	};//end namespace apper
}//end namespace nana
#endif
