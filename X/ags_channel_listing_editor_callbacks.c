#include "ags_channel_listing_editor_callbacks.h"

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
    ags_channel_listing_editor_add_children(channel_listing_editor, machine_editor->machine->audio);

  return(0);
}
