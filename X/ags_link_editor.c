#include "ags_link_editor.h"
#include "ags_link_editor_callbacks.h"

#include "../audio/ags_audio.h"
#include "../audio/ags_input.h"

#include "ags_machine_editor.h"
#include "ags_line_editor.h"

GType ags_link_editor_get_type(void);
void ags_link_editor_class_init(AgsLinkEditorClass *link_editor);
void ags_link_editor_init(AgsLinkEditor *link_editor);
void ags_link_editor_connect(AgsLinkEditor *link_editor);
void ags_link_editor_destroy(GtkObject *object);
void ags_link_editor_show(GtkWidget *widget);

GType
ags_link_editor_get_type(void)
{
  static GType link_editor_type = 0;

  if (!link_editor_type){
    static const GtkTypeInfo link_editor_info = {
      "AgsLinkEditor\0",
      sizeof(AgsLinkEditor), /* base_init */
      sizeof(AgsLinkEditorClass), /* base_finalize */
      (GtkClassInitFunc) ags_link_editor_class_init,
      (GtkObjectInitFunc) ags_link_editor_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    link_editor_type = gtk_type_unique (GTK_TYPE_HBOX, &link_editor_info);
  }

  return (link_editor_type);
}

void
ags_link_editor_class_init(AgsLinkEditorClass *link_editor)
{
}

void
ags_link_editor_init(AgsLinkEditor *link_editor)
{
  GtkHBox *hbox;
  GtkMenu *menu;
  GtkMenuItem *item;

  g_signal_connect_after((GObject *) link_editor, "parent_set\0",
			 G_CALLBACK(ags_link_editor_parent_set_callback), (gpointer) link_editor);

  link_editor->flags = 0;

  gtk_box_pack_start((GtkBox *) link_editor,
		     (GtkWidget *) gtk_label_new(NULL),
		     FALSE, FALSE, 0);

  link_editor->option = (GtkOptionMenu *) gtk_option_menu_new();
  menu = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("NULL \0"));
  g_object_set_data((GObject *) item, g_type_name(AGS_TYPE_MACHINE), NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  gtk_option_menu_set_menu(link_editor->option, (GtkWidget *) menu);
  gtk_box_pack_start((GtkBox *) link_editor,
		     (GtkWidget *) link_editor->option,
		     FALSE, FALSE, 0);

  link_editor->spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_box_pack_start((GtkBox *) link_editor,
		     (GtkWidget *) link_editor->spin_button,
		     FALSE, FALSE, 0);

  link_editor->audio_file = NULL;

  link_editor->file_chooser = NULL;
}

void
ags_link_editor_connect(AgsLinkEditor *link_editor)
{
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  GList *list;
  gboolean can_file_link;

  line_editor = (AgsLineEditor *) gtk_widget_get_ancestor((GtkWidget *) link_editor, AGS_TYPE_LINE_EDITOR);
  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_editor, AGS_TYPE_MACHINE_EDITOR);

  g_signal_connect((GObject *) link_editor, "destroy\0",
		   G_CALLBACK(ags_link_editor_destroy_callback), (gpointer) link_editor);

  g_signal_connect((GObject *) link_editor, "show\0",
		   G_CALLBACK(ags_link_editor_show_callback), (gpointer) link_editor);

  //  g_signal_connect((GObject *) link_editor->option, "changed\0",
  //		   G_CALLBACK(ags_link_editor_option_changed_callback), (gpointer) link_editor);

  list = GTK_MENU_SHELL(gtk_option_menu_get_menu(link_editor->option))->children;
  can_file_link = (AGS_IS_INPUT(line_editor->channel) && (AGS_AUDIO_INPUT_TAKES_FILE & machine_editor->machine->audio->flags) != 0) ? TRUE: FALSE;

  while((can_file_link && list->next != NULL) || (!can_file_link && list != NULL)){
    g_signal_connect(G_OBJECT(list->data), "activate\0",
		     G_CALLBACK(ags_link_editor_menu_item_callback), (gpointer) link_editor);
    list = list->next;
  }

  if(can_file_link){
    g_signal_connect((GObject *) list->data, "activate\0",
  		     G_CALLBACK(ags_link_editor_menu_item_file_callback), link_editor);
  }
}

void
ags_link_editor_destroy(GtkObject *object)
{
}

void
ags_link_editor_show(GtkWidget *widget)
{
  AgsLinkEditor *link_editor;

  link_editor = (AgsLinkEditor *) widget;

  gtk_widget_show_all((GtkWidget *) link_editor->option);
  gtk_widget_show((GtkWidget *) link_editor->spin_button);
}

AgsLinkEditor*
ags_link_editor_new()
{
  AgsLinkEditor *link_editor;

  link_editor = (AgsLinkEditor *) g_object_new(AGS_TYPE_LINK_EDITOR, NULL);

  return(link_editor);
}
