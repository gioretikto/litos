#include "litos.h"

void findButtonClicked (GtkButton *button, gpointer userData);
void replaceButtonClicked (GtkButton *button, gpointer userData);
void open_dialog (GtkWidget *widget, gpointer userData);
void openFromTemplate (GtkWidget *widget, gpointer userData);
void save_as_dialog(struct lit *litos);
void menu_save (GtkWidget *widget, gpointer userData);
void menu_newtab (GtkWidget *widget, gpointer userData);

GtkWidget *search_entry, *replace_entry, *button_check_case;

void createFindPopover(GtkMenuButton *find_menu_button, struct lit *litos)
{
	GtkWidget *vbox, *hbox_find, *hbox_replace, *label_find, *label_replace, *find_button, *replace_button, *popover;

	label_find = gtk_label_new ("Find:");
	label_replace = gtk_label_new ("Replace:");


	find_button = gtk_button_new_with_label("Find");
	replace_button = gtk_button_new_with_label("Replace All");

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	hbox_find = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	hbox_replace = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	button_check_case = gtk_check_button_new_with_mnemonic(_("_Match case"));

	search_entry = gtk_entry_new ();
	replace_entry = gtk_entry_new ();

	gtk_container_add (GTK_CONTAINER (vbox), hbox_find);
	gtk_container_add (GTK_CONTAINER (vbox), hbox_replace);

	gtk_container_add (GTK_CONTAINER (hbox_find), label_find);
	gtk_container_add (GTK_CONTAINER (hbox_find), search_entry);
	gtk_container_add (GTK_CONTAINER (hbox_find), find_button);
	gtk_container_add (GTK_CONTAINER (hbox_find), button_check_case);

	gtk_container_add (GTK_CONTAINER (hbox_replace), label_replace);
	gtk_container_add (GTK_CONTAINER (hbox_replace), replace_entry);
	gtk_container_add (GTK_CONTAINER (hbox_replace), replace_button);

	popover = gtk_popover_new (NULL);
	gtk_container_add (GTK_CONTAINER (popover), vbox);
	gtk_menu_button_set_popover (find_menu_button, popover);
	gtk_popover_set_position (GTK_POPOVER (popover), GTK_POS_RIGHT);
	
	gtk_widget_show_all (vbox);

	g_signal_connect (find_button, "clicked", G_CALLBACK (findButtonClicked), litos);
	g_signal_connect (replace_button, "clicked", G_CALLBACK (replaceButtonClicked), litos);
	g_signal_connect (button_check_case, "toggled", G_CALLBACK (replaceButtonClicked), litos);
}

void createFilePopover (GtkWidget *parent, GtkPositionType pos, struct lit *litos)
{
	GtkWidget *popover;

	popover = gtk_popover_new (NULL);
	gtk_popover_set_position (GTK_POPOVER (popover), pos);

	gtk_menu_button_set_popover (GTK_MENU_BUTTON(parent), popover);
	gtk_container_set_border_width (GTK_CONTAINER (popover), 6);
  
  	GtkWidget *file_box, *new_tab_button, *open_button, *new_from_template_button, *save_button, *save_as_button;

  	file_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  	gtk_container_add (GTK_CONTAINER (popover), file_box);

	new_tab_button = gtk_button_new_with_label("New Tab");
	open_button = gtk_button_new_with_label("Open");
	new_from_template_button = gtk_button_new_with_label("New From Template");
	save_button = gtk_button_new_with_label("Save");
	save_as_button = gtk_button_new_with_label("Save As");

	gtk_container_add(GTK_CONTAINER (file_box), open_button);
	gtk_container_add(GTK_CONTAINER (file_box), new_from_template_button);
	gtk_container_add(GTK_CONTAINER (file_box), save_button);
	gtk_container_add(GTK_CONTAINER (file_box), save_as_button);

	g_signal_connect (new_tab_button, "clicked", G_CALLBACK (menu_newtab), litos);
	g_signal_connect (open_button, "clicked", G_CALLBACK (open_dialog), litos);
	g_signal_connect (new_from_template_button, "clicked", G_CALLBACK (openFromTemplate), litos);
	g_signal_connect (save_as_button, "clicked", G_CALLBACK (save_as_dialog), litos);
	g_signal_connect (save_button, "clicked", G_CALLBACK (menu_save), litos);

	gtk_widget_show_all (file_box);
}
