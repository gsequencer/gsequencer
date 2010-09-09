#include "ags_pad_editor.h"
#include "ags_pad_editor_callbacks.h"

#include "ags_machine_editor.h"
#include "ags_line_editor.h"

GType ags_pad_editor_get_type(void);
void ags_pad_editor_class_init(AgsPadEditorClass *pad_editor);
void ags_pad_editor_init(AgsPadEditor *pad_editor);
void ags_pad_editor_connect(AgsPadEditor *pad_editor);
void ags_pad_editor_destroy(GtkObject *object);
void ags_pad_editor_show(GtkWidget *widget);

GType
ags_pad_editor_get_type(void)
{
  static GType pad_editor_type = 0;

  if (!pad_editor_type){
    static const GtkTypeInfo pad_editor_info = {
      "AgsPadEditor\0",
      sizeof(AgsPadEditor), /* base_init */
      sizeof(AgsPadEditorClass), /* base_finalize */
      (GtkClassInitFunc) ags_pad_editor_class_init,
      (GtkObjectInitFunc) ags_pad_editor_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    pad_editor_type = gtk_type_unique (GTK_TYPE_VBOX, &pad_editor_info);
  }

  return (pad_editor_type);
}

void
ags_pad_editor_class_init(AgsPadEditorClass *pad_editor)
{
}

void
ags_pad_editor_init(AgsPadEditor *pad_editor)
{
  g_signal_connect_after((GObject *) pad_editor, "parent_set\0",
			 G_CALLBACK(ags_pad_editor_parent_set_callback), (gpointer) pad_editor);

  pad_editor->line_editor = NULL;

  pad_editor->hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox*) pad_editor, (GtkWidget *) pad_editor->hbox, FALSE, FALSE, 0);

  pad_editor->button = (GtkButton *) gtk_button_new();
  gtk_button_set_relief (pad_editor->button, GTK_RELIEF_NONE);
  gtk_box_pack_start((GtkBox*) pad_editor->hbox, (GtkWidget *) pad_editor->button, FALSE, FALSE, 0);

  pad_editor->arrow = (GtkArrow *) gtk_arrow_new(GTK_ARROW_RIGHT, GTK_SHADOW_NONE);
  gtk_container_add((GtkContainer *) pad_editor->button, (GtkWidget *) pad_editor->arrow);

  pad_editor->label = (GtkLabel *) gtk_label_new(NULL);
  gtk_box_pack_start((GtkBox*) pad_editor->hbox, (GtkWidget *) pad_editor->label, FALSE, FALSE, 0);
}

void
ags_pad_editor_connect(AgsPadEditor *pad_editor)
{
  GList *list;

  g_signal_connect((GObject *) pad_editor, "destroy\0",
		   G_CALLBACK(ags_pad_editor_destroy_callback), (gpointer) pad_editor);

  g_signal_connect((GObject *) pad_editor, "show\0",
		   G_CALLBACK(ags_pad_editor_show_callback), (gpointer) pad_editor);

  g_signal_connect((GObject *) pad_editor->button, "clicked\0",
		   G_CALLBACK(ags_pad_editor_button_clicked), pad_editor);

  list = pad_editor->line_editor;

  while(list != NULL){
    ags_line_editor_connect(AGS_LINE_EDITOR(list->data));
    list = list->next;
  }
}

void
ags_pad_editor_destroy(GtkObject *object)
{
  AgsPadEditor *pad_editor = (AgsPadEditor *) object;

  g_list_free(pad_editor->line_editor);
}

void
ags_pad_editor_show(GtkWidget *widget)
{
  AgsPadEditor *pad_editor;

  pad_editor = (AgsPadEditor *) widget;

  gtk_widget_show((GtkWidget *) pad_editor->hbox);
  gtk_widget_show((GtkWidget *) pad_editor->button);
  gtk_widget_show((GtkWidget *) pad_editor->arrow);
  gtk_widget_show((GtkWidget *) pad_editor->label);
}

AgsPadEditor*
ags_pad_editor_new()
{
  AgsPadEditor *pad_editor;

  pad_editor = (AgsPadEditor *) g_object_new(AGS_TYPE_PAD_EDITOR, NULL);

  return(pad_editor);
}
