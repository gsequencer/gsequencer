#ifndef __AGS_MACHINE_CALLBACKS_H__
#define __AGS_MACHINE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

int ags_machine_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMachine *machine);
int ags_machine_destroy_callback(GtkObject *object, AgsMachine *machine);

int ags_machine_button_press_callback(GtkWidget *handle_box, GdkEventButton *event, AgsMachine *machine);

int ags_machine_popup_move_up_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_move_down_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_hide_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_show_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_destroy_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_rename_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_properties_activate_callback(GtkWidget *widget, AgsMachine *machine);

#endif /*__AGS_MACHINE_CALLBACKS_H__*/
