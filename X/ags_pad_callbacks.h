#ifndef __AGS_PAD_CALLBACKS_H__
#define __AGS_PAD_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_pad.h"

int ags_pad_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPad *pad);
int ags_pad_destroy_callback(GtkObject *object, AgsPad *pad);
int ags_pad_show_callback(GtkWidget *widget, AgsPad *pad);

int ags_pad_option_changed_callback(GtkWidget *widget, AgsPad *pad);

int ags_pad_group_clicked_callback(GtkWidget *widget, AgsPad *pad);
int ags_pad_mute_clicked_callback(GtkWidget *widget, AgsPad *pad);
int ags_pad_solo_clicked_callback(GtkWidget *widget, AgsPad *pad);

#endif /*__AGS_PAD_CALLBACKS_H__*/
