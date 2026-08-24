#include <gtk/gtk.h>

#include "litossymbols.h"
#include "litosapp.h"
#include "litosfile.h"
#include "litosappwin.h"

typedef struct
{
    const gchar *label;	// Text showed in the button
    const gchar *insert; // Text inserted in the buffer

} Symbol;

// Sets and constant symbols

static const Symbol symbols_sets[] =
{
	{"È", "È"},
	{"ℕ", "ℕ"},
	{"ℤ", "ℤ"},
	{"ℚ", "ℚ"},
	{"ℝ", "ℝ"},
	{"ℂ", "ℂ"},
	{"𝕂", "𝕂"},

	{"∅", "∅"},
	{"∞", "∞"},
	{"ℰ", "ℰ"},
	{"ℏ", "ℏ"},
	{"Å", "Å"},

	{NULL, NULL}
};

// Arrows

static const Symbol symbols_arrows[] =
{
	{"→", "→"},
	{"←", "←"},

	{"↑", "↑"},
	{"↓", "↓"},

	{"↔", "↔"},
	{"⇒", "⇒"},
	{"⇐", "⇐"},
	{"⇔", "⇔"},
	{"⟶", "⟶"},
	{"⟵", "⟵"},
	{"⟼", "⟼"},

	{"↛", "↛"},

	{"↦", "↦"},
	{"↪", "↪"},
	{"↩", "↩"},

	{NULL, NULL}
};

// RELATIONS

static const Symbol symbols_relations[] =
{
    {"=", "="},
    {"≠", "≠"},

    {"<", "<"},
    {">", ">"},
    {"≤", "≤"},
    {"≥", "≥"},

    {"≈", "≈"},
    {"≃", "≃"},
    {"≅", "≅"},

    {"∈", "∈"},
    {"∉", "∉"},

    {"⊂", "⊂"},
    {"⊄", "⊄"},
    {"⊆", "⊆"},
    {"⊇", "⊇"},

    {"∝", "∝"},

    {NULL, NULL}
};

// OPERATORS

static const Symbol symbols_operators[] =
{
	{"∪", "∪"},
	{"∩", "∩"},

	{"±", "±"},

	{"×", "×"},
	{"÷", "÷"},
	{"⋅", "⋅"},

	{"√", "√"},
	{"∛", "∛"},

	{"∑", "∑"},
	{"∏", "∏"},

	{"∫", "∫"},
	{"∂", "∂"},
	{"∇", "∇"},

	{"⊕", "⊕"},
	{"⊗", "⊗"},

	{"°", "°"},

	{NULL, NULL}
};

// LATIN

static const Symbol symbols_latin[] =
{
	{"ā", "ā"},
	{"ă", "ă"},
	{"ē", "ē"},
	{"ĕ", "ĕ"},
	{"ī", "ī"},
	{"ū", "ū"},
	{"ŭ", "ŭ"},
	{"ĭ", "ĭ"},
	{"ō", "ō"},
	{"ŏ", "ŏ"},
	{"ό", "ό"},
	{"ä", "ä"},
	{"ö", "ö"},
	{"ü", "ü"},

	{NULL, NULL}
};

// GREEK

static const Symbol symbols_greek[] =
{
	{"α", "α"},
	{"β", "β"},
	{"γ", "γ"},
	{"δ", "δ"},
	{"ε", "ε"},
	{"ζ", "ζ"},
	{"η", "η"},
	{"θ", "θ"},
	{"ι", "ι"},
	{"κ", "κ"},
	{"λ", "λ"},
	{"μ", "μ"},
	{"ν", "ν"},
	{"ξ", "ξ"},
	{"π", "π"},
	{"ρ", "ρ"},
	{"σ", "σ"},
	{"ς", "ς"},
	{"τ", "τ"},
	{"φ", "φ"},
	{"χ", "χ"},
	{"ψ", "ψ"},
	{"ω", "ω"},
	{"Γ", "Γ"},
	{"Δ", "Δ"},
	{"Θ", "Θ"},
	{"Λ", "Λ"},
	{"Ξ", "Ξ"},
	{"Π", "Π"},
	{"Σ", "Σ"},
	{"Φ", "Φ"},
	{"Ψ", "Ψ"},
	{"Ω", "Ω"},
	{NULL, NULL}
};

 // HTML

static const Symbol symbols_html[] =
{
	{"Bold", "<b></b>"},
	{"Italic", "<i></i>"},
	{"Paragraph", "<p></p>"},
	{"Heading H2", "<h2></h2>"},
	{"Heading H3", "<h3></h3>"},
	{"Heading H4", "<h4></h4>"},
	{"List", "<li><p></p></li>"},
	{"Apex", "<sup></sup>"},
	{"Pedix", "<sub></sub>"},
	{"Equation", "<div class=\"eq\">\n<p></p>\n</div>"},
	{"Link","<a href=\"\"></a>"},
	{"Break","<br>"},
	{NULL, NULL}
};

// LATEX

static const Symbol symbols_latex[] =
{
    {"\\frac{}{}", "\\frac{}{}"},
    {"\\sqrt{}", "\\sqrt{}"},
    {"Bold", "\\mathbf{}"},
    {"Testo", "\\text{}"},
    {"Somma", "\\sum"},
    {"Product", "\\prod"},
    {"Integral", "\\int \\text{d}\\,x"},
    {"Limit",  "\\lim_{n \\to \\infty}"},
    {"Partial", "\\partial"},
    {"Pedix", "_{}"},
    {"Apex", "^{}"},

    {NULL, NULL}
};

void
insert_text_with_cursor(GtkTextBuffer *buffer,
                        const gchar *text)
{
    GtkTextIter iter;

    gtk_text_buffer_get_iter_at_mark(
        buffer,
        &iter,
        gtk_text_buffer_get_insert(buffer));

    gtk_text_buffer_insert(
        buffer,
        &iter,
        text,
        -1);
}

// Buttons Callback

static void
symbol_clicked(GtkButton *button,
               gpointer user_data)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(user_data);

    const gchar *text = g_object_get_data(G_OBJECT(button),"symbol");

    if (!text || !buffer) return;

    insert_text_with_cursor(buffer, text);
}

// Creation of Symbol button

static GtkWidget *
create_symbol_button(const Symbol *symbol)
{
	GtkWidget *button;

	button = gtk_button_new_with_label(symbol->label);

	// Uniform dimension

	gtk_widget_set_size_request(
		button,
		70,
		42);

	g_object_set_data(
		G_OBJECT(button),
		"symbol",
		(gpointer)symbol->insert);

	return button;
}

/* Creation of a Category page
   Every category is a FlowBox
   in a ScrolledWindow */

static GtkWidget *
create_symbol_page(const Symbol *symbols,
                   GtkTextBuffer *buffer)
{
    GtkWidget *flow;
    GtkWidget *scroll;

    flow = gtk_flow_box_new();

    gtk_flow_box_set_selection_mode(
        GTK_FLOW_BOX(flow),
        GTK_SELECTION_NONE);

    gtk_flow_box_set_max_children_per_line(
        GTK_FLOW_BOX(flow),
        8);

    gtk_flow_box_set_row_spacing(
        GTK_FLOW_BOX(flow),
        6);

    gtk_flow_box_set_column_spacing(
        GTK_FLOW_BOX(flow),
        6);

    for (int i = 0; symbols[i].label != NULL; i++)
    {
        GtkWidget *button;

        button = create_symbol_button(&symbols[i]);

        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(symbol_clicked),
            buffer);

        gtk_flow_box_insert(
            GTK_FLOW_BOX(flow),
            button,
            -1);
    }
	
	// Vertical Scroll

	scroll = gtk_scrolled_window_new();

	gtk_widget_set_hexpand(scroll, TRUE);
	gtk_widget_set_vexpand(scroll, TRUE);

	gtk_scrolled_window_set_policy(
		GTK_SCROLLED_WINDOW(scroll),
		GTK_POLICY_AUTOMATIC,
		GTK_POLICY_AUTOMATIC);

	gtk_scrolled_window_set_child(
		GTK_SCROLLED_WINDOW(scroll),
		flow);

	return scroll;
}

// DIALOG SYMBOLS

static gboolean
symbols_key_pressed(GtkEventControllerKey *controller G_GNUC_UNUSED,
                    guint keyval,
                    guint keycode G_GNUC_UNUSED,
                    GdkModifierType state G_GNUC_UNUSED,
                    gpointer user_data)
{
	if (keyval == GDK_KEY_Escape)
	{
		gtk_window_destroy(GTK_WINDOW(user_data));
		return TRUE;
	}

	return FALSE;
}

void
litos_symbols_dialog(LitosAppWindow *win)
{
	if (!win) return;

	// Recupera buffer corrente

	LitosFile *file = litos_app_window_current_file(win);

	if (!file) return;

	GtkTextBuffer *buffer = litos_file_get_buffer(file);

	if (!buffer) return;

	// GTK4 Window

	GtkWidget *window;

	window = gtk_window_new();

	GtkEventController *controller;

	controller = gtk_event_controller_key_new();

	g_signal_connect(controller,
		         "key-pressed",
		         G_CALLBACK(symbols_key_pressed),
		         window);

	gtk_widget_add_controller(window, controller);

	gtk_window_set_title(
		GTK_WINDOW(window),
		"Inserisci simbolo");

	gtk_window_set_default_size(
		GTK_WINDOW(window),
		720,
		450);

	gtk_window_set_modal(
		GTK_WINDOW(window),
		TRUE);

	gtk_window_set_transient_for(
		GTK_WINDOW(window),
		GTK_WINDOW(win));

	// Main Box

	GtkWidget *main_box;

	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

	gtk_widget_set_margin_top(main_box,8);

	gtk_widget_set_margin_bottom(main_box,8);

	gtk_widget_set_margin_start(main_box,8);

	gtk_widget_set_margin_end(main_box,8);

	gtk_window_set_child(GTK_WINDOW(window),main_box);

	// Categories Stack

	GtkWidget *stack;

	stack = gtk_stack_new();

	gtk_widget_set_hexpand(stack, TRUE);
	gtk_widget_set_vexpand(stack, TRUE);

	gtk_stack_set_transition_type(
		GTK_STACK(stack),
		GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

	// Barra schede

	GtkWidget *switcher;

	switcher = gtk_stack_switcher_new();

	gtk_stack_switcher_set_stack(
		GTK_STACK_SWITCHER(switcher),
		GTK_STACK(stack));

	gtk_box_append(GTK_BOX(main_box), switcher);
	gtk_box_append(GTK_BOX(main_box), stack);

	// Add categories

	gtk_stack_add_titled(
		GTK_STACK(stack),
		create_symbol_page(
			symbols_sets,
			buffer),
		"sets",
		"Sets");

	gtk_stack_add_titled(
		GTK_STACK(stack),
		create_symbol_page(
			symbols_arrows,
			buffer),
		"arrows",
		"Frecce");

	gtk_stack_add_titled(
		GTK_STACK(stack),
		create_symbol_page(
			symbols_relations,
			buffer),
		"relations",
		"Relations");

	gtk_stack_add_titled(
		GTK_STACK(stack),
		create_symbol_page(
			symbols_operators,
			buffer),
		"operators",
		"Operators");

	gtk_stack_add_titled(
		GTK_STACK(stack),
		create_symbol_page(
			symbols_latin,
			buffer),
		"latin",
		"Latin");

	gtk_stack_add_titled(
		GTK_STACK(stack),
		create_symbol_page(
			symbols_greek,
			buffer),
		"greek",
		"Greek");

	gtk_stack_add_titled(
		GTK_STACK(stack),
		create_symbol_page(
		symbols_html,
		buffer),
		"html",
		"HTML");

	gtk_stack_add_titled(
		GTK_STACK(stack),
		create_symbol_page(
			symbols_latex,
			buffer),
		"latex",
		"LaTeX");

	// Close Button

	GtkWidget *close_button;

	close_button = gtk_button_new_with_label("Close");

	g_signal_connect_swapped(
		close_button,
		"clicked",
		G_CALLBACK(gtk_window_destroy),
		window);

	gtk_box_append(GTK_BOX(main_box), close_button);

	gtk_window_present(GTK_WINDOW(window));
}
