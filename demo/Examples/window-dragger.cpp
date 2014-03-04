#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/dragger.hpp>

int main()
{
    using namespace nana::gui;
    form   fm;
    
    button btn(fm, nana::rectangle(10, 10, 100, 20));
    btn.caption(STR("Drag Me"));
    
    form & adherent = form_loader<form>()();
    adherent.show();

    dragger dg;
    dg.target(fm);
    dg.target(adherent);
    dg.trigger(btn);
    dg.trigger(adherent);
    fm.show();
    exec();
}