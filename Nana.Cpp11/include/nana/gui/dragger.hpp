#ifndef NANA_GUI_DRAGGER_HPP
#define NANA_GUI_DRAGGER_HPP
#include "programming_interface.hpp"

namespace nana{ namespace gui
{
	/// \brief Helper class that allows the user to drag windows. 
    ///
    /// If a user presses the mouse on the specified window and moves the mouse, the specified window is dragged.
    /// The drag target window and trigger window can be set more than once. 
    class dragger
		: nana::noncopyable
	{
		class dragger_impl_t;
	public:
		dragger();
		~dragger();
		void target(window);
		void trigger(window);
	private:
		dragger_impl_t * impl_;
	};
}//end namespace gui
}//end namespace nana
#endif
