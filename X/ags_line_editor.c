#include "ags_line_editor.h"
#include "ags_line_editor_callbacks.h"

GType ags_line_editor_get_type(void);
void ags_line_editor_class_init(AgsLineEditorClass *line_editor);
void ags_line_editor_init(AgsLineEditor *line_editor);
void ags_line_editor_connect(AgsLineEditor *line_editor);
void ags_line_editor_destroy(GtkObject *object);
void ags_line_editor_show(GtkWidget *widget);

GType
ags_line_editor_get_type(void)
{
  static GType line_editor_type = 0;

  if (!line_editor_type){
    static const GtkTypeInfo line_editor_info = {
      "AgsLineEditor\0",
      sizeof(AgsLineEditor), /* base_init */
      sizeof(AgsLineEditorClass), /* base_finalize */
      (GtkClassInitFunc) ags_line_editor_class_init,
      (GtkObjectInitFunc) ags_line_editor_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    line_editor_type = gtk_type_unique (GTK_TYPE_VBOX, &line_editor_info);
  }

  return (line_editor_type);
}

void
ags_line_editor_class_init(AgsLineEditorClass *line_editor)
{
}

void
ags_line_editor_init(AgsLineEditor *line_editor)
{
  g_signal_connect_after((GObject *) line_editor, "parent_set\0",
  			 G_CALLBACK(ags_line_editor_parent_set_callback), (gpointer) line_editor);

  line_editor->link_editor = ags_link_editor_new();
}

void
ags_line_editor_connect(AgsLineEditor *line_editor)
{
  g_signal_connect((GObject *) line_editor, "destroy\0",
		   G_CALLBACK(ags_line_editor_destroy_callback), (gpointer) line_editor);

  g_signal_connect((GObject *) line_editor, "show\0",
  		   G_CALLBACK(ags_line_editor_show_callback), (gpointer) line_editor);

  ags_link_editor_connect(line_editor->link_editor);
}

void
ags_line_editor_destroy(GtkObject *object)
{
}

void
ags_line_editor_show(GtkWidget *widget)
{
  AgsLineEditor *line_editor = (AgsLineEditor *) widget;

  gtk_widget_show((GtkWidget *) line_editor->link_editor);
}

AgsLineEditor*
ags_line_editor_new()
{
  AgsLineEditor *line_editor;

  line_editor = (AgsLineEditor *) g_object_new(AGS_TYPE_LINE_EDITOR, NULL);

  return(line_editor);
}
