#ifndef __AGS_EDITOR_CALLBACKS_H__
#define __AGS_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_editor.h"

void ags_editor_parent_set_callback(GtkWidget  *widget, GtkObject *old_parent, AgsEditor *editor);
gboolean ags_editor_destroy_callback(GtkObject *object, AgsEditor *editor);
void ags_editor_show_callback(GtkWidget *widget, AgsEditor *editor);

gboolean ags_editor_button_press_callback(GtkWidget *hpaned, GdkEventButton *event, AgsEditor *editor);
void ags_editor_index_callback(GtkRadioButton *radio_button, AgsEditor *editor);

gboolean ags_editor_drawing_area_expose_event (GtkWidget *widget, GdkEventExpose *event, AgsEditor *editor);
gboolean ags_editor_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsEditor *editor);
gboolean ags_editor_drawing_area_button_press_event (GtkWidget *widget, GdkEventButton *event, AgsEditor *editor);
gboolean ags_editor_drawing_area_button_release_event (GtkWidget *widget, GdkEventButton *event, AgsEditor *editor);
gboolean ags_editor_drawing_area_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, AgsEditor *editor);

void ags_editor_popup_add_tab_callback(GtkWidget *widget, GtkMenu *popup);
void ags_editor_popup_remove_tab_callback(GtkWidget *widget, GtkMenu *popup);
void ags_editor_popup_add_index_callback(GtkWidget *widget, GtkMenu *popup);
void ags_editor_popup_remove_index_callback(GtkWidget *widget, GtkMenu *popup);
void ags_editor_popup_link_index_callback(GtkWidget *widget, GtkMenu *popup);

gboolean ags_editor_vscrollbar_change_value(GtkRange *range, GtkScrollType scroll, gdouble value, AgsEditor *editor);
gboolean ags_editor_hscrollbar_change_value(GtkRange *range, GtkScrollType scroll, gdouble value, AgsEditor *editor);

#endif /*__AGS_EDITOR_CALLBACKS_H__*/
