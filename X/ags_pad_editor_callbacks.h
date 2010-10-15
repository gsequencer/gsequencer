#ifndef __AGS_PAD_EDITOR_CALLBACKS_H__
#define __AGS_PAD_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_pad_editor.h"

int ags_pad_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPadEditor *pad_editor);

int ags_pad_editor_destroy_callback(GtkObject *object, AgsPadEditor *pad_editor);
int ags_pad_editor_show_callback(GtkWidget *widget, AgsPadEditor *pad_editor);


#endif /*__AGS_PAD_EDITOR_CALLBACKS_H__*/
