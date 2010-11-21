#ifndef __AGS_MACHINE_EDITOR_CALLBACKS_H__
#define __AGS_MACHINE_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine_editor.h>

int ags_machine_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMachineEditor *machine_editor);
int ags_machine_editor_destroy_callback(GtkObject *object, AgsMachineEditor *machine_editor);
int ags_machine_editor_show_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);

int ags_machine_editor_switch_page_callback(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, AgsMachineEditor *machine_editor);

int ags_machine_editor_add_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);
int ags_machine_editor_remove_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);
int ags_machine_editor_back_callback(GtkWidget *button, AgsMachineEditor *machine_editor);

int ags_machine_editor_apply_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);
int ags_machine_editor_ok_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);
int ags_machine_editor_cancel_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);

#endif /*__AGS_MACHINE_EDITOR_CALLBACKS_H__*/
