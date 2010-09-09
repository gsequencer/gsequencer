#ifndef __AGS_LINE_EDITOR_CALLBACKS_H__
#define __AGS_LINE_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_line_editor.h"

int ags_line_editor_destroy_callback(GtkObject *object, AgsLineEditor *line_editor);
int ags_line_editor_show_callback(GtkWidget *widget, AgsLineEditor *line_editor);
int ags_line_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLineEditor *line_editor);

#endif /*__AGS_LINE_EDITOR_CALLBACKS_H__*/
