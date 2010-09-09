#ifndef __AGS_LINK_EDITOR_CALLBACKS_H__
#define __AGS_LINK_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "ags_link_editor.h"

int ags_link_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLinkEditor *link_editor);
int ags_link_editor_destroy_callback(GtkObject *object, AgsLinkEditor *link_editor);
int ags_link_editor_show_callback(GtkWidget *widget, AgsLinkEditor *link_editor);

int ags_link_editor_option_changed_callback(GtkWidget *widget, AgsLinkEditor *link_editor);
int ags_link_editor_menu_item_callback(GtkWidget *widget, AgsLinkEditor *link_editor);
int ags_link_editor_menu_item_file_callback(GtkWidget *widget, AgsLinkEditor *link_editor);

#endif /*__AGS_LINK_EDITOR_CALLBACKS_H__*/
