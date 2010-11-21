#include <ags/X/ags_pad_editor_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>

int
ags_pad_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPadEditor *pad_editor)
{
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

void
ags_pad_editor_set_audio_channels_callback(AgsAudio *audio,
					   guint audio_channels, guint audio_channels_old,
					   AgsPadEditor *pad_editor)
{
  if(audio_channels > audio_channels_old){
    AgsLineEditor *line_editor;
    AgsChannel *channel, *next_pad;

    channel = ags_channel_nth(pad_editor->pad, audio_channels_old);
    next_pad = pad_editor->pad->next_pad;

    while(channel != next_pad){
      line_editor = ags_line_editor_new(channel);
      gtk_box_pack_start(GTK_BOX(pad_editor->line_editor),
			 GTK_WIDGET(line_editor),
			 FALSE, FALSE,
			 0);
      ags_connectable_connect(AGS_CONNECTABLE(line_editor));
      gtk_widget_show_all(GTK_WIDGET(line_editor));

      channel = channel->next;
    }
  }else{
    GList *list, *list_next;

    list = gtk_container_get_children(GTK_CONTAINER(pad_editor->line_editor));
    list = g_list_nth(list, audio_channels);

    while(list != NULL){
      list_next = list->next;

      gtk_widget_destroy(GTK_WIDGET(list->data));

      list = list_next;
    }
  }
}
