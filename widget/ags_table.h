#ifndef __AGS_TABLE_H__
#define __AGS_TABLE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_TABLE                (ags_table_get_type())
#define AGS_TABLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TABLE, AgsTable))
#define AGS_TABLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TABLE, AgsTableClass))
#define AGS_IS_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_TABLE))
#define AGS_IS_TABLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_TABLE))
#define AGS_TABLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_TABLE, AgsTableClass))

typedef struct _AgsTable AgsTable;
typedef struct _AgsTableClass AgsTableClass;

struct _AgsTable
{
  GtkTable table;
};

struct _AgsTableClass
{
  GtkTableClass table;
};

GType ags_table_get_type(void);

AgsTable* ags_table_new(guint row, guint columns, gboolean homogeneous);

#endif /*__AGS_TABLE_H__*/

