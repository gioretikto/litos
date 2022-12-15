#include <aspell.h>
#include <string.h>
#include <stdlib.h>
#include "litos.h"

GtkTextBuffer* get_current_buffer(struct lit *litos);
void highlightWord(struct lit *litos, const gchar *searchString);
char *highlight_ptr(struct lit *litos,const char *start,size_t len);
void spellCheck (GtkWidget *button, struct lit *litos)
{
	(void)button;
	gchar *text_buffer = NULL;
	AspellConfig * spell_config = new_aspell_config();

	aspell_config_replace(spell_config, "lang", "en_US");
	aspell_config_replace(spell_config, "mode", "html");

	AspellCanHaveError * possible_err = new_aspell_speller(spell_config);
	AspellSpeller * spell_checker = 0;

	if (aspell_error_number(possible_err) != 0)
		puts(aspell_error_message(possible_err));
	else
		spell_checker = to_aspell_speller(possible_err);

	GtkTextIter start, end;
	GtkTextBuffer *buffer = get_current_buffer(litos);
	AspellDocumentChecker *Adoc = NULL;
	gtk_text_buffer_get_start_iter (buffer, &start);
	gtk_text_buffer_get_end_iter (buffer, &end);

	text_buffer = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

	possible_err = new_aspell_document_checker(spell_checker);

	if(aspell_error_number(possible_err)){
		puts(aspell_error_message(possible_err));
		goto cleanup;
	}

	Adoc = to_aspell_document_checker(possible_err);
	aspell_document_checker_process(Adoc, text_buffer, (int)strlen(text_buffer));
	AspellToken token;

	for(;;){																	/* returns the next misspelled word in the processed string
 																				* if there are no more misspelled word than token.word
 																				* will be null and token.size will be 0 */
		token = aspell_document_checker_next_misspelling(Adoc);
		if(!token.len) break;
		if(token.len <= 3) continue; /* do not correct words with less than 4 char */ 
		char * wrong = highlight_ptr(litos,text_buffer+token.offset, token.len);
		printf("%s\n", wrong);		
	}

	g_free(text_buffer);
 cleanup:
	delete_aspell_document_checker(Adoc);
	delete_aspell_speller(spell_checker);
}
char *highlight_ptr(struct lit *litos,const char *start,size_t len){
	char *word = malloc(len+1);
	if(!word){
		abort();
	}
	strncpy(word, start, len);
	word[len] = '\0';
	highlightWord(litos, word);
	return word;
}
