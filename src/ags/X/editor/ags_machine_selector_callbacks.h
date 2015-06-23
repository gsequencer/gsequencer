#ifndef __AGS_MACHINE_SELECTOR_CALLBACKS_H__
#define __AGS_MACHINE_SELECTOR_CALLBACKS_H__

#include <ags/X/editor/ags_machine_selector.h>

void ags_machine_selector_popup_add_tab_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector);
void ags_machine_selector_popup_remove_tab_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector);
void ags_machine_selector_popup_add_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector);
void ags_machine_selector_popup_remove_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector);
void ags_machine_selector_popup_link_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector);
void ags_machine_selector_popup_invert_mapping_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector);
void ags_machine_selector_popup_shift_piano_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector);

void ags_machine_selector_radio_changed(GtkWidget *radio_button, AgsMachineSelector *machine_selector);

#endif /*__AGS_MACHINE_SELECTOR_CALLBACKS_H__*/
