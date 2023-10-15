#ifndef _LITOS_FILE_H
#define _LITOS_FILE_H

G_BEGIN_DECLS

#define LITOS_TYPE_FILE (litos_file_get_type ())

#include "litosappwin.h"

G_DECLARE_FINAL_TYPE(LitosFile, litos_file, LITOS, FILE, GObject)

LitosFile *          litos_file_new           ();

G_END_DECLS

#endif
