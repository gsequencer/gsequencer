#ifndef __AGS_RULER_CALLBACKS_H__
#define __AGS_RULER_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/editor/ags_ruler.h>

gboolean ags_ruler_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsRuler *ruler); 
gboolean ags_ruler_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsRuler *ruler); 

#endif /*__AGS_RULER_CALLBACKS_H__*/
