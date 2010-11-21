#ifndef __AGS_PANEL_CALLBACKS_H__
#define __AGS_PANEL_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_panel.h>

void ags_panel_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPanel *panel);

gboolean ags_panel_destroy_callback();

#endif /*__AGS_PANEL_CALLBACKS_H__*/
