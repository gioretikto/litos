struct Page
{
	GtkWidget *tabbox;
	GtkWidget *close_btn_box;
	GtkWidget *scrolled;
	GtkTextBuffer *buffer;
	char *name;
	GFile *gf;
	GtkWidget *view;
	GtkWidget *lbl;
};
