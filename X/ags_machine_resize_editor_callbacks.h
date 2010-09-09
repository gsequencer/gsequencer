#ifndef __AGS_MACHINE_RESIZE_EDITOR_CALLBACKS_H__
#define __AGS_MACHINE_RESIZE_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_machine_resize_editor.h"

#include "ags_machine_editor.h"

int ags_machine_resize_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMachineResizeEditor *machine_resize_editor);
void ags_machine_resize_editor_set_machine_callback(AgsMachineEditor *machine_editor, AgsMachine *machine, AgsMachineResizeEditor *machine_resize_editor);

void ags_machine_resize_editor_add_input_callback(AgsMachineEditor *machine_editor, AgsMachineResizeEditor *machine_resize_editor);
void ags_machine_resize_editor_add_output_callback(AgsMachineEditor *machine_editor, AgsMachineResizeEditor *machine_resize_editor);
void ags_machine_resize_editor_remove_input_callback(AgsMachineEditor *machine_editor, AgsMachineResizeEditor *machine_resize_editor);
void ags_machine_resize_editor_remove_output_callback(AgsMachineEditor *machine_editor, AgsMachineResizeEditor *machine_resize_editor);

#endif /*__AGS_MACHINE_RESIZE_EDITOR_CALLBACKS_H__*/
