#ifndef __AGS_NOTEBOOK_H__
#define __AGS_NOTEBOOK_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_NOTEBOOK                (ags_notebook_get_type())
#define AGS_NOTEBOOK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTEBOOK, AgsNotebook))
#define AGS_NOTEBOOK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTEBOOK, AgsNotebookClass))
#define AGS_IS_NOTEBOOK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NOTEBOOK))
#define AGS_IS_NOTEBOOK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NOTEBOOK))
#define AGS_NOTEBOOK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_NOTEBOOK, AgsNotebookClass))

typedef struct _AgsNotebook AgsNotebook;
typedef struct _AgsNotebookClass AgsNotebookClass;

struct _AgsNotebook
{
  GtkNotebook notebook;
};

struct _AgsNotebookClass
{
  GtkNotebookClass notebook;
};

AgsNotebook* ags_notebook_new();

#endif /*__AGS_NOTEBOOK_H__*/
