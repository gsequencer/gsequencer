#ifndef __AGS_FFPLAYER_CALLBACKS_H__ 
#define __AGS_FFPLAYER_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_ffplayer.h>

void ags_ffplayer_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsFFPlayer *ffplayer);
gboolean ags_ffplayer_destroy_callback(GtkObject *object, AgsFFPlayer *ffplayer);

void ags_ffplayer_open_clicked_callback(GtkWidget *widget, AgsFFPlayer *ffplayer);

gboolean ags_ffplayer_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsFFPlayer *ffplayer);
gboolean ags_ffplayer_drawing_area_configure_callback(GtkWidget *widget, GdkEventConfigure *event, AgsFFPlayer *ffplayer);
gboolean ags_ffplayer_drawing_area_button_press_callback(GtkWidget *widget, AgsFFPlayer *ffplayer);

void ags_ffplayer_hscrollbar_value_changed(GtkAdjustment *adjustment, AgsFFPlayer *ffplayers);

#endif /*__AGS_FFPLAYER_CALLBACKS_H__ */
