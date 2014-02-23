/** \class nana::gui::listbox
\brief A rectangle containing a list of strings from which the user can select. This widget contain a list of \a categories, with in turn contain \a items. 
A category is a text with can be \a selected, \a checked and \a expanded to show the items.
An item is formed by \a column-fields, each corresponding to one of the \a headers. 
An item can be \a selected and \a checked.
The user can \a drag the header to \a reisize it or to \a reorganize it. 
By \a clicking on a header the list get \a reordered, first up, and then down alternatively,

1. The resolver is used to resolute an object of the specified type for a listbox item.
2. The any_objective of listbox have a 2-Dimension indexing. The first dimension is for the category, and the second one is for the item of the specified category.

		int main()
		{
		   using namespace nana::gui;
		   form fm;
		   listbox lb(fm, nana::rectangle(10, 10, 280, 120));
		   lb.append_header(STR("Header"), 200);
		   lb.append_item(STR("int"));
		   lb.append_item(STR("double"));

		   lb.anyobj(0, 0, 10);
		   lb.anyobj(0, 1, 0.1);

		   int * pi = lb.anyobj<int>(0, 0); 	  // it returns a nullptr if there is not an int object specified.
		   double * pd = lb.anyobj<double>(0, 1); // it returns a nullptr if there is not an double object specified.

		   fm.show();
		   exec();
		 }

3. nana::gui::listbox creates the category 0 by default. The member functions without the categ parameter operate the items that belong to category 0.



\see nana::gui::drawerbase::listbox::cat_proxy
\see nana::gui::drawerbase::listbox::item_proxy

\example listbox_Resolver.cpp

*/