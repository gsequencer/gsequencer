
#include "ags_notebook.h"
#include "ags_notebook_callbacks.h"

#include "../ags_editor.h"

void ags_notebook_class_init(AgsNotebookClass *notebook);
void ags_notebook_init(AgsNotebook *notebook);
void ags_notebook_connect(AgsNotebook *notebook);
void ags_notebook_destroy(GtkObject *object);
void ags_notebook_show(GtkWidget *widget);
void ags_notebook_paint(AgsNotebook *notebook);

GtkStyle *notebook_style;

GType
ags_notebook_get_type(void)
{
  static GType notebook_type = 0;

  if (!notebook_type){
    static const GtkTypeInfo notebook_info = {
      "AgsNotebook\0",
      sizeof(AgsNotebook), /* base_init */
      sizeof(AgsNotebookClass), /* base_finalize */
      (GtkClassInitFunc) ags_notebook_class_init,
      (GtkObjectInitFunc) ags_notebook_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    notebook_type = gtk_type_unique (GTK_TYPE_NOTEBOOK, &notebook_info);
  }

  return (notebook_type);
}

void
ags_notebook_class_init(AgsNotebookClass *notebook)
{
}

void
ags_notebook_init(AgsNotebook *notebook)
{
  gtk_widget_set_style((GtkWidget *) notebook, notebook_style);
}

void
ags_notebook_connect(AgsNotebook *notebook)
{
  AgsEditor *editor;

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) notebook, AGS_TYPE_EDITOR);

  g_signal_connect((GObject *) notebook, "destroy\0",
		   G_CALLBACK(ags_notebook_destroy_callback), (gpointer) notebook);

  g_signal_connect((GObject *) notebook, "show\0",
		   G_CALLBACK(ags_notebook_show_callback), (gpointer) notebook);

  g_signal_connect((GObject *) editor, "change_machine\0",
		   G_CALLBACK(ags_notebook_change_machine_callback), notebook);
}

void
ags_notebook_destroy(GtkObject *object)
{
}

void
ags_notebook_show(GtkWidget *widget)
{
}

AgsNotebook*
ags_notebook_new()
{
  AgsNotebook *notebook;

  notebook = (AgsNotebook *) g_object_new(AGS_TYPE_NOTEBOOK, NULL);

  return(notebook);
}
