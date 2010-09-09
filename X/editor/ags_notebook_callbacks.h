#ifndef __AGS_NOTEBOOK_CALLBACKS_H__
#define __AGS_NOTEBOOK_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../ags_machine.h"
#include "../ags_editor.h"

#include "ags_notebook.h"


gboolean ags_notebook_destroy_callback(GtkObject *object, AgsNotebook *notebook);
void ags_notebook_show_callback(GtkWidget *widget, AgsNotebook *notebook);

void ags_notebook_change_machine_callback(AgsEditor *editor, AgsMachine *machine,
					  AgsNotebook *notebook);

#endif /*__AGS_NOTEBOOK_CALLBACKS_H__*/
