#ifndef __AGS_RULER_H__
#define __AGS_RULER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_RULER                (ags_ruler_get_type())
#define AGS_RULER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_RULER, AgsRuler))
#define AGS_RULER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_RULER, AgsRulerClass))
#define AGS_IS_RULER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RULER))
#define AGS_IS_RULER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RULER))
#define AGS_RULER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_RULER, AgsRulerClass))

typedef struct _AgsRuler AgsRuler;
typedef struct _AgsRulerClass AgsRulerClass;

struct _AgsRuler
{
  GtkDrawingArea drawing_area;

  /*
  PangoLayout *layout;

  char *font_name;
  PangoFontDescription *font_desc;

  PangoContext *context;
  */
};

struct _AgsRulerClass
{
  GtkDrawingAreaClass drawing_area;
};

AgsRuler* ags_ruler_new();

#endif /*__AGS_RULER_H__*/
