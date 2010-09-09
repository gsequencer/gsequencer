#ifndef __AGS_MACHINE_LINK_EDITOR_CALLBACKS_H__
#define __AGS_MACHINE_LINK_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_machine_link_editor.h"

#include "ags_machine.h"
#include "ags_machine_editor.h"

int ags_machine_link_editor_parent_set_callback(GtkWidget *widget, GtkObject *parent, AgsMachineLinkEditor *machine_link_editor);
void ags_machine_link_editor_set_machine_callback(AgsMachineEditor *machine_editor, AgsMachine *machine, AgsMachineLinkEditor *machine_link_editor);

int ags_machine_link_editor_input_first_line_callback(GtkSpinButton *spin_button, AgsMachineLinkEditor *machine_link_editor);
int ags_machine_link_editor_input_link_callback(GtkOptionMenu *option, AgsMachineLinkEditor *machine_link_editor);
int ags_machine_link_editor_input_first_link_callback(GtkSpinButton *spin_button, AgsMachineLinkEditor *machine_link_editor);

int ags_machine_link_editor_output_first_line_callback(GtkSpinButton *spin_button, AgsMachineLinkEditor *machine_link_editor);
int ags_machine_link_editor_output_link_callback(GtkOptionMenu *option, AgsMachineLinkEditor *machine_link_editor);
int ags_machine_link_editor_output_first_link_callback(GtkSpinButton *spin_button, AgsMachineLinkEditor *machine_link_editor);

void ags_machine_link_editor_add_input_callback(AgsMachineEditor *machine_editor, AgsMachineLinkEditor *machine_link_editor);
void ags_machine_link_editor_add_output_callback(AgsMachineEditor *machine_editor, AgsMachineLinkEditor *machine_link_editor);
void ags_machine_link_editor_remove_input_callback(AgsMachineEditor *machine_editor, AgsMachineLinkEditor *machine_link_editor);
void ags_machine_link_editor_remove_output_callback(AgsMachineEditor *machine_editor, AgsMachineLinkEditor *machine_link_editor);

void ags_machine_link_editor_resize_audio_channels_callback(AgsMachineResizeEditor *machine_resize_editor, guint audio_channels, guint audio_channels_old, AgsMachineLinkEditor *machine_link_editor);
void ags_machine_link_editor_resize_input_pads_callback(AgsMachineResizeEditor *machine_resize_editor, guint pads, guint pads_old, AgsMachineLinkEditor *machine_link_editor);
void ags_machine_link_editor_resize_output_pads_callback(AgsMachineResizeEditor *machine_resize_editor, guint pads, guint pads_old, AgsMachineLinkEditor *machine_link_editor);

#endif /* __AGS_MACHINE_LINK_EDITOR_CALLBACKS_H__ */
