#include <ags/X/ags_channel_listing_editor_callbacks.h>

int
ags_channel_listing_editor_parent_set_callback(GtkWidget *widget,
					       GtkObject *old_parent,
					       AgsChannelListingEditor *channel_listing_editor)
{
  AgsMachineEditor *machine_editor;

  if(old_parent != NULL)
    return(0);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(widget,
								AGS_TYPE_MACHINE_EDITOR);

  if(machine_editor->machine != NULL)
    ags_channel_listing_editor_add_children(channel_listing_editor,
					    machine_editor->machine->audio, 0,
					    FALSE);

  return(0);
}

void
ags_channel_listing_editor_set_pads_callback(AgsAudio *audio, GType channel_type,
					     guint pads, guint pads_old,
					     AgsChannelListingEditor *channel_listing_editor)
{
  if(channel_type != channel_listing_editor->channel_type)
    return;

  if(pads_old < pads){
    guint nth_channel;
  
    nth_channel = pads_old * audio->audio_channels;
    
    ags_channel_listing_editor_add_children(channel_listing_editor,
					    audio, nth_channel,
					    TRUE);
  }else{
    GList *list, *list_next;

    list = gtk_container_get_children(GTK_CONTAINER(channel_listing_editor->child));
    list = g_list_nth(list, pads);
    
    while(list != NULL){
      list_next = list->next;

      gtk_widget_destroy(GTK_WIDGET(list->data));

      list = list_next;
    }
  }
}
