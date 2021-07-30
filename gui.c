#include <string.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gtksourceview/gtksource.h>
#include <glib.h>

GtkWidget *window;
GtkSourceBuffer *buffer;

static void
quit_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app)
{
  g_application_quit (G_APPLICATION (app));
}

void open_file(char *filename)
{
    gboolean read_file_status;
   	GError *error;
   	char* contents;
    read_file_status = g_file_get_contents(filename, &contents, NULL, &error);

    if (read_file_status == FALSE) {
    	g_error("error opening file: %s\n",error && error->message ? error->message : "No Detail");
        return;
    }

    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer), contents, -1);
}

void open_dialog()
{    
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Open File",
                                      GTK_WINDOW(window),
                                      action,
                                      _("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      _("_Open"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

	res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT)
	  {
		gchar *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);
		g_print("%s\n",filename);
		open_file (filename);
		g_free (filename);
	  }
	  
	 gtk_widget_destroy (dialog);		  
}

void save_file(GtkFileChooser *chooser)
{
		GFile *loc = gtk_file_chooser_get_file(chooser);
		GtkTextIter begin, end;
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &begin);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
		char* contents = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &begin, &end, 0);
		g_file_replace_contents(loc, contents, strlen(contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, NULL);
		g_object_unref(loc);
}

void save_dialog()
{
	GtkWidget *dialog;
	GtkFileChooser *chooser;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Save File",
		                                  GTK_WINDOW(window),
		                                  action,
		                                  _("_Cancel"),
		                                  GTK_RESPONSE_CANCEL,
		                                  _("_Save"),
		                                  GTK_RESPONSE_ACCEPT,
		                                  NULL);
	chooser = GTK_FILE_CHOOSER (dialog);

	gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

	/*if (user_edited_a_new_document)
	  gtk_file_chooser_set_current_name (chooser,
		                                 _("Untitled document"));
	else
	  gtk_file_chooser_set_filename (chooser,
		                             existing_filename);*/

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	  {
		char *filename;

		filename = gtk_file_chooser_get_filename (chooser);
		save_file (chooser);
		g_free (filename);
	  }

	gtk_widget_destroy (dialog);
}

void action_quit()
{
	gtk_main_quit();
}

const GActionEntry app_entries[] = {
  //{"new", new_activated, NULL, NULL, NULL, {0,0,0}},
	{"open", open_dialog, NULL, NULL, NULL, {0,0,0}},
	{"save", save_dialog, NULL, NULL, NULL, {0,0,0}},
    {"quit", quit_activated, NULL, NULL, NULL, {0,0,0}}
};

struct {
  const gchar *action;
  const gchar *accels[2];
} action_accels[] = {
  { "app.new", { "<Control>n", NULL} },
  { "app.open", { "<Control>o", NULL} },
  { "app.quit", { "<Control>q", NULL} },
  { "app.save", { "<Control>s", NULL} },
  { "app.saveas", { "<Shift><Control>s", NULL} },
  { "win.close", { "<Control>w", NULL} },
  { "win.cut", { "<Control>x", NULL} },
  { "win.copy", { "<Control>c", NULL} },
  { "win.paste", { "<Control>v", NULL} },
  { "win.selectall", { "<Control>a", NULL} }
};

void new_file()
{
	typedef struct App 
	{
		GtkWidget *source_view;
		GtkWidget *search_entry;
	}App;

	GtkWidget *swindow;
	
	App AppSource;

	swindow = gtk_scrolled_window_new (NULL, NULL);
  
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swindow),
	GTK_POLICY_AUTOMATIC,
	GTK_POLICY_AUTOMATIC);

	gtk_widget_set_vexpand (swindow, TRUE);
  
	gtk_container_add(GTK_CONTAINER (window), swindow);

	//GtkSourceBuffer *buffer;

    buffer = gtk_source_buffer_new (NULL);
	gtk_source_buffer_set_style_scheme (buffer,
	gtk_source_style_scheme_manager_get_scheme (
	gtk_source_style_scheme_manager_get_default (), "solarized-dark"));

	AppSource.source_view = gtk_source_view_new_with_buffer (buffer);	

	GFile *location = g_file_new_for_path ("/home/gidna/prova");

	GtkSourceFile *file = gtk_source_file_new ();

	gtk_source_file_set_location (file, location);

    gtk_container_add (GTK_CONTAINER (swindow), AppSource.source_view);

  	gtk_source_file_saver_new (buffer, file);

    gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW((AppSource.source_view)), TRUE);
}

void create_menu (GtkApplication *app)
{
		GMenu *m, *fm;
		long unsigned int i;
		
		m = g_menu_new ();
		fm = g_menu_new();
		g_menu_append(fm, "Open	", "app.open");
		g_menu_append(fm, "Save	", "app.save");
		g_menu_append(fm, "Close Window	", "app.quit");
		g_menu_append_submenu (m, "File", G_MENU_MODEL(fm));
		gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(m));

		  g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);
  			for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
				gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
				
		g_object_unref (m);
}

void
activate (GtkApplication* app,
          gpointer        user_data)
{
	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "Litos");
	gtk_window_maximize (GTK_WINDOW(window));
  
	user_data = "well";
  
    g_print ("Hello again - %s done\n", (char *) user_data);    
    
    create_menu(app);
    
    new_file();
    
    gtk_widget_show_all (window);
}
