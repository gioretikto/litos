#include <aspell.h>
#include <string.h>
#include "litos.h"

GtkTextBuffer* get_current_buffer(struct lit *litos);
void highlightWord(struct lit *litos, const gchar *searchString);

void spellCheck (GtkWidget *button, struct lit *litos)
{
	(void)button;

	AspellConfig * spell_config = new_aspell_config();
	aspell_config_replace(spell_config, "lang", "en_US");

	AspellCanHaveError * possible_err = new_aspell_speller(spell_config);
	AspellSpeller * spell_checker = 0;

	if (aspell_error_number(possible_err) != 0)
	  puts(aspell_error_message(possible_err));
	else
	  spell_checker = to_aspell_speller(possible_err);

	int correct;

    gint idx = 0;
    GtkTextIter start, end;
	GtkTextBuffer *buffer = get_current_buffer(litos);

    gtk_text_buffer_begin_user_action (buffer); /* begin user action */

    gtk_text_buffer_get_start_iter (buffer, &start);

    while (!gtk_text_iter_starts_word (&start))
        gtk_text_iter_forward_char (&start);

    end = start;

    while (gtk_text_iter_forward_word_end (&end)) {
        const gchar *word = gtk_text_buffer_get_text (buffer,
                                            &start, &end, FALSE);
        const gchar *p = word;

        for (; *p; p++) {}

        g_print ("[%4d] : %s\n", idx++, word);

		correct = aspell_speller_check(spell_checker, word, (int)strlen(word));

		if(!correct)
		{
        	g_print ("Error on: %s\n", word);
			highlightWord(litos, word);
		}

        start = end;

        for (;;) {
            if (!gtk_text_iter_forward_char (&start))
                goto wordseldone;
            if (gtk_text_iter_starts_word (&start))
                break;
        }

        end = start;    /* not 100% needed, fwd_word_end will jumpt to next end */
    }
    wordseldone:;

    gtk_text_buffer_end_user_action (buffer);   /* end user action */
}
