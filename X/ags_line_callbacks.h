#ifndef __AGS_LINE_CALLBACKS_H__
#define __AGS_LINE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_line.h"

int ags_line_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLine *line);
int ags_line_destroy_callback(GtkObject *object, AgsLine *line);
int ags_line_show_callback(GtkWidget *widget, AgsLine *line);

#endif /*__AGS_LINE_CALLBACKS_H__*/
