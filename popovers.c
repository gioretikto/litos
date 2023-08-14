#include "litos.h"

void searchWord (GtkButton *button, gpointer userData);
void replaceButtonClicked (GtkButton *button, gpointer userData);
void open_dialog (GtkWidget *widget, gpointer userData);
void openFromTemplate (GtkWidget *widget, gpointer userData);
void action_save_as_dialog(struct lit *litos);
void menu_save (GtkWidget *widget, gpointer userData);
void menu_newtab (GtkWidget *widget, gpointer userData);
void spellCheck(struct lit *litos);

GtkWidget *search_entry, *replace_entry, *button_check_case;
GtkWidget *lbl_number_occurences;

void createSearchPopover(GtkMenuButton *search_menu_button, struct lit *litos)
{
	GtkWidget *vbox, *hbox_search, *hbox_replace, *label_search, *label_replace, *search_button, *replace_button, *popover;

	label_search = gtk_label_new ("Search:");
	label_replace = gtk_label_new ("Replace:");

	lbl_number_occurences= gtk_label_new (NULL);

	search_button = gtk_button_new_with_label("Search");
	replace_button = gtk_button_new_with_label("Replace All");

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	hbox_search = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	hbox_replace = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	button_check_case = gtk_check_button_new_with_mnemonic(_("_Match case\t"));

	search_entry = gtk_entry_new ();
	replace_entry = gtk_entry_new ();

	gtk_container_add (GTK_CONTAINER (vbox), hbox_search);
	gtk_container_add (GTK_CONTAINER (vbox), hbox_replace);

	gtk_container_add (GTK_CONTAINER (hbox_search), label_search);
	gtk_container_add (GTK_CONTAINER (hbox_search), search_entry);
	gtk_container_add (GTK_CONTAINER (hbox_search), search_button);
	gtk_container_add (GTK_CONTAINER (hbox_search), button_check_case);
	gtk_container_add (GTK_CONTAINER (hbox_search), lbl_number_occurences);

	gtk_container_add (GTK_CONTAINER (hbox_replace), label_replace);
	gtk_container_add (GTK_CONTAINER (hbox_replace), replace_entry);
	gtk_container_add (GTK_CONTAINER (hbox_replace), replace_button);

	popover = gtk_popover_new (NULL);
	gtk_container_add (GTK_CONTAINER (popover), vbox);
	gtk_menu_button_set_popover (search_menu_button, popover);
	gtk_popover_set_position (GTK_POPOVER (popover), GTK_POS_RIGHT);
	
	gtk_widget_show_all (vbox);

	g_signal_connect (search_button, "clicked", G_CALLBACK (searchWord), litos);
	g_signal_connect (replace_button, "clicked", G_CALLBACK (replaceButtonClicked), litos);
}

void createPreferencePopover (GtkWidget *parent, GtkPositionType pos, struct lit *litos)
{
	GtkWidget *popover;
	popover = gtk_popover_new (NULL);
	gtk_popover_set_position (GTK_POPOVER (popover), pos);

	gtk_menu_button_set_popover (GTK_MENU_BUTTON(parent), popover);
	gtk_container_set_border_width (GTK_CONTAINER (popover), 6);
  
  	GtkWidget *preference_box, *new_tab_button, *open_button, *new_from_template_button, *save_button, *save_as_button, *spell_btn;

  	preference_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  	gtk_container_add (GTK_CONTAINER (popover), preference_box);

	new_tab_button = gtk_button_new_with_label("New Tab");
	open_button = gtk_button_new_with_label("Open");
	new_from_template_button = gtk_button_new_with_label("New From Template");
	save_button = gtk_button_new_with_label("Save");
	save_as_button = gtk_button_new_with_label("Save As");
	spell_btn = gtk_button_new_with_label("Check Spell");

	gtk_container_add(GTK_CONTAINER (preference_box), open_button);
	gtk_container_add(GTK_CONTAINER (preference_box), new_from_template_button);
	gtk_container_add(GTK_CONTAINER (preference_box), save_button);
	gtk_container_add(GTK_CONTAINER (preference_box), save_as_button);
	gtk_container_add(GTK_CONTAINER (preference_box), spell_btn);

	g_signal_connect (new_tab_button, "clicked", G_CALLBACK (menu_newtab), litos);
	g_signal_connect (open_button, "clicked", G_CALLBACK (open_dialog), litos);
	g_signal_connect (new_from_template_button, "clicked", G_CALLBACK (openFromTemplate), litos);
	g_signal_connect (save_button, "clicked", G_CALLBACK (menu_save), litos);
	g_signal_connect (spell_btn, "clicked", G_CALLBACK (spellCheck), litos);

	gtk_widget_show_all (preference_box);
}
