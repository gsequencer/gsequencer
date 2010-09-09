#include "ags_line_member_editor.h"
#include "ags_line_member_editor_callbacks.h"

GType ags_line_member_editor_get_type(void);
void ags_line_member_editor_class_init(AgsLineMemberEditorClass *line_member_editor);
void ags_line_member_editor_init(AgsLineMemberEditor *line_member_editor);
void ags_line_member_editor_connect(AgsLineMemberEditor *line_member_editor);
void ags_line_member_editor_destroy(GtkObject *object);
void ags_line_member_editor_show(GtkWidget *widget);

GType
ags_line_member_editor_get_type(void)
{
  static GType line_member_editor_type = 0;

  if (!line_member_editor_type){
    static const GtkTypeInfo line_member_editor_info = {
      "AgsLineMemberEditor\0",
      sizeof(AgsLineMemberEditor), /* base_init */
      sizeof(AgsLineMemberEditorClass), /* base_finalize */
      (GtkClassInitFunc) ags_line_member_editor_class_init,
      (GtkObjectInitFunc) ags_line_member_editor_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    line_member_editor_type = gtk_type_unique (GTK_TYPE_VBOX, &line_member_editor_info);
  }

  return (line_member_editor_type);
}

void
ags_line_member_editor_class_init(AgsLineMemberEditorClass *line_member_editor)
{
}

void
ags_line_member_editor_init(AgsLineMemberEditor *line_member_editor)
{
  line_member_editor->line_member = (GtkVBox *) gtk_vbox_new(FALSE, 2);
  gtk_box_pack_start((GtkBox *) line_member_editor, (GtkWidget *) line_member_editor->line_member, FALSE, FALSE, 0);

  line_member_editor->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  line_member_editor->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
}

void
ags_line_member_editor_connect(AgsLineMemberEditor *line_member_editor)
{
  g_signal_connect((GObject *) line_member_editor, "destroy\0",
		   G_CALLBACK(ags_line_member_editor_destroy_callback), (gpointer) line_member_editor);

  g_signal_connect((GObject *) line_member_editor, "show\0",
		   G_CALLBACK(ags_line_member_editor_show_callback), (gpointer) line_member_editor);
}

void
ags_line_member_editor_destroy(GtkObject *object)
{
}

void
ags_line_member_editor_show(GtkWidget *widget)
{
}

AgsLineMemberEditor*
ags_line_member_editor_new()
{
  AgsLineMemberEditor *line_member_editor;

  line_member_editor = (AgsLineMemberEditor *) g_object_new(AGS_TYPE_LINE_MEMBER_EDITOR, NULL);

  return(line_member_editor);
}
