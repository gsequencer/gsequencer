#ifndef __AGS_TOOLBAR_H__
#define __AGS_TOOLBAR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_TOOLBAR                (ags_toolbar_get_type())
#define AGS_TOOLBAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TOOLBAR, AgsToolbar))
#define AGS_TOOLBAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TOOLBAR, AgsToolbarClass))
#define AGS_IS_TOOLBAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TOOLBAR))
#define AGS_IS_TOOLBAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TOOLBAR))
#define AGS_TOOLBAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_TOOLBAR, AgsToolbarClass))

typedef struct _AgsToolbar AgsToolbar;
typedef struct _AgsToolbarClass AgsToolbarClass;

struct _AgsToolbar
{
  GtkToolbar toolbar;

  GtkToggleButton *edit;
  GtkToggleButton *clear;
  GtkToggleButton *select;
  GtkButton *copy;
  GtkButton *cut;
  GtkButton *paste;

  guint zoom_history;
  GtkOptionMenu *zoom;

  guint tic_history;
  GtkOptionMenu *tic;

  GtkOptionMenu *mode;
};

struct _AgsToolbarClass
{
  GtkToolbarClass toolbar;
};

AgsToolbar* ags_toolbar_new();

#endif /*__AGS_TOOLBAR_H__*/
