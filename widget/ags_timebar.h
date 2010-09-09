#ifndef __AGS_TIMEBAR_H__
#define __AGS_TIMEBAR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_TIMEBAR                (ags_timebar_get_type())
#define AGS_TIMEBAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TIMEBAR, AgsTimebar))
#define AGS_TIMEBAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TIMEBAR, AgsTimebarClass))
#define AGS_IS_TIMEBAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_TIMEBAR))
#define AGS_IS_TIMEBAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_TIMEBAR))
#define AGS_TIMEBAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_TIMEBAR, AgsTimebarClass))

typedef struct _AgsTimebar AgsTimebar;
typedef struct _AgsTimebarClass AgsTimebarClass;

struct _AgsTimebar
{
  GtkRange range;
};

struct _AgsTimebarClass
{
  GtkRangeClass range;
};

AgsTimebar* ags_timebar_new();

#endif /*__AGS_TIMEBAR_H__*/
