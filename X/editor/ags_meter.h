#ifndef __AGS_METER_H__
#define __AGS_METER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <cairo.h>

#define AGS_TYPE_METER                (ags_meter_get_type())
#define AGS_METER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_METER, AgsMeter))
#define AGS_METER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_METER, AgsMeterClass))
#define AGS_IS_METER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_METER))
#define AGS_IS_METER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_METER))
#define AGS_METER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_METER, AgsMeterClass))

typedef struct _AgsMeter AgsMeter;
typedef struct _AgsMeterClass AgsMeterClass;

struct _AgsMeter
{
  GtkDrawingArea drawing_area;
};

struct _AgsMeterClass
{
  GtkDrawingAreaClass drawing_area;
};

char* ags_meter_sound_string();
void ags_meter_paint(AgsMeter *meter); // , guint rows, guint nth_y, guint y0

AgsMeter* ags_meter_new();

#endif /*__AGS_METER_H__*/
