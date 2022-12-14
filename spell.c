#include <aspell.h>
#include <string.h>
#include <ctype.h>
#include "litos.h"

GtkTextBuffer* get_current_buffer(struct lit *litos);
void highlightWord(struct lit *litos, const gchar *searchString);

void spellCheck (GtkWidget *button, struct lit *litos)
{
	(void)button;

	const gchar *word = NULL;
	char line[256];

	int correct;

	AspellConfig * spell_config = new_aspell_config();

	aspell_config_replace(spell_config, "lang", "en_US");
	aspell_config_replace(spell_config, "mode", "html");

	AspellCanHaveError * possible_err = new_aspell_speller(spell_config);
	AspellSpeller * spell_checker = 0;

	if (aspell_error_number(possible_err) != 0)
		puts(aspell_error_message(possible_err));
	else
		spell_checker = to_aspell_speller(possible_err);

	gint idx = 0;
	GtkTextIter start, end;
	GtkTextBuffer *buffer = get_current_buffer(litos);

	gtk_text_buffer_get_start_iter (buffer, &start);
	gtk_text_buffer_get_end_iter (buffer, &end);

	word = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

	printf("%s\n", word);

	//aspell_document_checker_process(word, line, -1); 

	//correct = aspell_speller_check(spell_checker, word, (int)strlen(word));

	/*if (!correct)
	{
		g_print ("Error on: %s\n", word);
		highlightWord(litos, word);
	}*/

	/*while (gtk_text_iter_forward_word_end (&end))
	{
		word = gtk_text_buffer_get_text (buffer,
                                            &start, &end, FALSE);

		for (;;)
		{
			if (!gtk_text_iter_forward_char (&start))
				goto wordsdone;
			if (gtk_text_iter_starts_word (&start))
				break;
		}

		if (!isalpha(word[0]) || strlen(word) <= 2)
			continue;

		else
		{
			g_print ("[%4d] : %s\n", idx++, word);

			if(strcmp(word, "DOCTYPE") != 0 && strcmp(word, "html") != 0 && strcmp(word, "utf") != 0 && strcmp(word, "charset") != 0 && strcmp(word, "lang") != 0 && strcmp(word, "href") != 0 && strcmp(word, "forall") != 0 && strcmp(word, "subseteq") != 0)
			{
				correct = aspell_speller_check(spell_checker, word, (int)strlen(word));

				if (!correct)
				{
					g_print ("Error on: %s\n", word);
					highlightWord(litos, word);
				}
			}
		}
	}

	wordsdone:;

	gtk_text_buffer_end_user_action (buffer);   /* end user action */

	delete_aspell_speller(spell_checker);
}
