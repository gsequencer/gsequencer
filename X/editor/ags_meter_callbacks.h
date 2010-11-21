#ifndef __AGS_METER_CALLBACKS_H__
#define __AGS_METER_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/editor/ags_meter.h>

gboolean ags_meter_destroy_callback(GtkObject *object, AgsMeter *meter);
void ags_meter_show_callback(GtkWidget *widget, AgsMeter *meter);

gboolean ags_meter_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsMeter *meter); 
gboolean ags_meter_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsMeter *meter); 

#endif /*__AGS_METER_CALLBACKS_H__*/
