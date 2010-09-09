#ifndef __AGS_TOOLBAR_CALLBACKS_H__
#define __AGS_TOOLBAR_CALLBACKS_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_toolbar.h"

gboolean ags_toolbar_destroy_callback(GtkObject *object, AgsToolbar *toolbar);
void ags_toolbar_show_callback(GtkWidget *widget, AgsToolbar *toolbar);

void ags_toolbar_edit_callback(GtkWidget *widget, AgsToolbar *toolbar);
void ags_toolbar_clear_callback(GtkWidget *widget, AgsToolbar *toolbar);

void ags_toolbar_copy_callback(GtkWidget *widget, AgsToolbar *toolbar);
void ags_toolbar_cut_callback(GtkWidget *widget, AgsToolbar *toolbar);
void ags_toolbar_paste_callback(GtkWidget *widget, AgsToolbar *toolbar);
void ags_toolbar_select_callback(GtkWidget *widget, AgsToolbar *toolbar);

void ags_toolbar_zoom_callback(GtkOptionMenu *option, AgsToolbar *toolbar);
void ags_toolbar_tic_callback(GtkOptionMenu *option, AgsToolbar *toolbar);

void ags_toolbar_mode_default_callback(GtkWidget *widget, AgsToolbar *toolbar);
void ags_toolbar_mode_group_channels_callback(GtkWidget *widget, AgsToolbar *toolbar);

#endif /*__AGS_TOOLBAR_CALLBACKS_H__*/
