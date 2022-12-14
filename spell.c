#include <aspell.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "litos.h"

GtkTextBuffer* get_current_buffer(struct lit *litos);
void highlightWord(struct lit *litos, const gchar *searchString);
char *highlight_ptr(struct lit *litos,const char *start,size_t len);
void spellCheck (GtkWidget *button, struct lit *litos)
{
	(void)button;
	gchar *word = NULL;
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

	word = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

	printf("%s\n", word);

	possible_err = new_aspell_document_checker(spell_checker);
	if(aspell_error_number(possible_err)){
		puts(aspell_error_message(possible_err));
		goto cleanup;
	}
	Adoc = to_aspell_document_checker(possible_err);
	aspell_document_checker_process(Adoc, word, (int)strlen(word));
	AspellToken token;
	for(;;){
		token = aspell_document_checker_next_misspelling(Adoc);
		if(!token.len) break;
		if(token.len <= 3) continue;
		char * wrong = highlight_ptr(litos,word+token.offset, token.len);
		printf("%s\n", wrong);
		
	}
	g_free(word);
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
