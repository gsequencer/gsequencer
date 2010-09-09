#include "ags_pad_editor_callbacks.h"

#include "../audio/ags_channel.h"

#include "ags_machine.h"
#include "ags_machine_editor.h"
#include "ags_line_editor.h"

int
ags_pad_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPadEditor *pad_editor)
{
  AgsLineEditor *line_editor;
  AgsMachine *machine;
  AgsChannel *channel;
  GList *list;
  guint i;

  if(old_parent != NULL)
    return(0);

  machine = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(widget, AGS_TYPE_MACHINE_EDITOR))->machine;

  if(machine->audio->audio_channels > 0){
    channel = pad_editor->pad;
    pad_editor->line_editor =
      list = g_list_alloc();
    i = 0;
    goto ags_pad_editor_parent_set_callback0;
  }else
    return(0);

  for(; i < machine->audio->audio_channels; i++){
    list->next = (GList*) g_list_alloc();
    list->next->prev = list;
    list = list->next;

  ags_pad_editor_parent_set_callback0:

    line_editor = ags_line_editor_new();
    line_editor->channel = channel;
    list->data = (gpointer) line_editor;
    gtk_box_pack_start((GtkBox *) pad_editor, (GtkWidget *) line_editor, FALSE, FALSE, 0);

    channel = channel->next;
  }

  return(0);
}

int
ags_pad_editor_destroy_callback(GtkObject *object, AgsPadEditor *pad_editor)
{
  ags_pad_editor_destroy(object);
  return(0);
}

int
ags_pad_editor_show_callback(GtkWidget *widget, AgsPadEditor *pad_editor)
{
  ags_pad_editor_show(widget);
  return(0);
}

int
ags_pad_editor_button_clicked(GtkWidget *widget, AgsPadEditor *pad_editor)
{
  GtkArrow *arrow;
  GList *list;
  gint16 arrow_type;

  arrow = pad_editor->arrow;

  if(arrow->arrow_type != GTK_ARROW_NONE){
    //    arrow = gtk_container_get_children((GtkContainer *) widget)->data;
    list = pad_editor->line_editor;

    arrow_type = arrow->arrow_type;
    arrow->arrow_type = GTK_ARROW_NONE;

    if(arrow_type == GTK_ARROW_RIGHT){
      gtk_arrow_set(arrow, GTK_ARROW_DOWN, GTK_SHADOW_NONE);

      while(list != NULL){
	gtk_widget_show((GtkWidget *) list->data);
	list = list->next;
      }
    }else{
      gtk_arrow_set(arrow, GTK_ARROW_RIGHT, GTK_SHADOW_NONE);

      while(list != NULL){
	gtk_widget_hide((GtkWidget *) list->data);
	list = list->next;
      }
    }
  }

  return(0);
}
