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

	while (gtk_text_iter_forward_word_end (&end))
	{
		const gchar *word = gtk_text_buffer_get_text (buffer,
                                            &start, &end, FALSE);

		g_print ("word is: %s\n", word);

		const gchar *p = word;

		if(p[0] == '<')			/* if word contains a tag skip it */
		{
			while(*p != '>')
				p++;
		}

		g_print ("[%4d] : %s\n", idx++, p);

		correct = aspell_speller_check(spell_checker, p, (int)strlen(p));

		if (!correct)
		{
			g_print ("Error on: %s\n", p);
			highlightWord(litos, p);
		}

		start = end;

		for (;;)
		{
			if (!gtk_text_iter_forward_char (&start))
				goto wordsdone;
			if (gtk_text_iter_starts_word (&start))
				break;
		}
	}

	wordsdone:;

	gtk_text_buffer_end_user_action (buffer);   /* end user action */
	delete_aspell_speller(spell_checker);
}
