#include "litos.h"

gboolean get_notebook_no_pages(void);
void find_button_clicked ();
GtkTextBuffer* get_current_buffer();
void find_button_clicked ();
void my_grab_focus();

GtkWidget *popover;
GtkWidget *search_entry;

void init_find_replace_popover(GtkMenuButton *find_replace_button)
{	
	GtkWidget *vbox, *hbox_find, *hbox_replace, *label_find, *find_button;
	
	search_entry = gtk_entry_new ();
	
	label_find = gtk_label_new ("Find:");
	
	find_button = gtk_button_new_with_label("Find");

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	hbox_find = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	hbox_replace = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	gtk_container_add (GTK_CONTAINER (vbox), hbox_find);
	gtk_container_add (GTK_CONTAINER (vbox), hbox_replace);

	gtk_container_add (GTK_CONTAINER (hbox_find), label_find);
	gtk_container_add (GTK_CONTAINER (hbox_find), search_entry);
	gtk_container_add (GTK_CONTAINER (hbox_find), find_button);

	popover = gtk_popover_new (NULL);
	gtk_container_add (GTK_CONTAINER (popover), vbox);
	gtk_menu_button_set_popover (find_replace_button, popover);
	gtk_popover_set_position (GTK_POPOVER (popover), GTK_POS_RIGHT);
	
	gtk_widget_show_all (vbox);
	
	g_signal_connect (find_button, "clicked", G_CALLBACK (find_button_clicked), NULL);
}

/* Called when find button is clicked. */
void find_button_clicked ()
{
	gchar *text;
	GtkTextBuffer *buffer_selected;

    GtkTextIter start_sel, end_sel;
	GtkTextIter start_find, end_find;
	GtkTextIter start_match, end_match;
	gboolean selected = FALSE;

	buffer_selected = get_current_buffer();

	gtk_text_buffer_create_tag(buffer_selected, "gray_bg", 
      "background", "#657b83", NULL);

	selected = gtk_text_buffer_get_selection_bounds(buffer_selected, 
            &start_sel, &end_sel);
	
	if (selected)
	{
        gtk_text_buffer_get_start_iter(buffer_selected, &start_find);
        gtk_text_buffer_get_end_iter(buffer_selected, &end_find);

        gtk_text_buffer_remove_tag_by_name(buffer_selected, "gray_bg", 
            &start_find, &end_find);
        text = (gchar *) gtk_text_buffer_get_text(buffer_selected, &start_sel,
            &end_sel, FALSE);

        while (gtk_text_iter_forward_search(&start_find, text,
                GTK_TEXT_SEARCH_TEXT_ONLY |
                GTK_TEXT_SEARCH_VISIBLE_ONLY,
                &start_match, &end_match, NULL)) {

          gtk_text_buffer_apply_tag_by_name(buffer_selected, "gray_bg",
              &start_match, &end_match);
          gint offset = gtk_text_iter_get_offset(&end_match);
          gtk_text_buffer_get_iter_at_offset(buffer_selected,
              &start_find, offset);
        }
        
            g_free(text);
            gtk_text_buffer_remove_tag_by_name(buffer_selected, "gray_bg", 
            &start_find, &end_find);
      }
      
      else
      	return;
}
