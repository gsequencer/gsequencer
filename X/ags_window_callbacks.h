#ifndef __AGS_WINDOW_CALLBACKS_H__
#define __AGS_WINDOW_CALLBACKS_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_window.h>

gboolean ags_window_delete_event_callback(GtkWidget *widget, AgsWindow *window);
gboolean ags_window_destroy_callback(GtkObject *object, AgsWindow *window);
void ags_window_show_callback(GtkWidget *widget, AgsWindow *window);

#endif /*__AGS_WINDOW_CALLBACKS_H__*/
