#ifndef __AGS_LIST_H__
#define __AGS_LIST_H__

#include <glib.h>
#include <glib-object.h>

void ags_list_free_and_free_link(GList *list);
void ags_list_free_and_unref_link(GList *list);

#endif /*__AGS_LIST_H__*/
