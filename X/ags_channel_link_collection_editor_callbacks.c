#include <ags/X/ags_channel_link_collection_editor_callbacks.h>

#include <ags/X/ags_machine_editor.h>

int
ags_channel_link_collection_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsChannelLinkCollectionEditor *channel_link_collection_editor)
{
  AgsMachineEditor *machine_editor;

  if(old_parent != NULL)
    return(0);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(widget, 
								AGS_TYPE_MACHINE_EDITOR);

  if(machine_editor != NULL &&
     machine_editor->machine != NULL){
    gtk_combo_box_set_model(channel_link_collection_editor->link,
			    GTK_TREE_MODEL(ags_machine_get_possible_links(machine_editor->machine)));

    ags_channel_link_collection_editor_check(channel_link_collection_editor);
  }
}

void ags_channel_link_collection_editor_link_callback(GtkComboBox *combo,
						      AgsChannelLinkCollectionEditor *channel_link_collection_editor)
{
  ags_channel_link_collection_editor_check(channel_link_collection_editor);
}

void ags_channel_link_collection_editor_first_line_callback(GtkSpinButton *spin_button,
							    AgsChannelLinkCollectionEditor *channel_link_collection_editor)
{
  ags_channel_link_collection_editor_check(channel_link_collection_editor);
}

void ags_channel_link_collection_editor_first_link_callback(GtkSpinButton *spin_button,
							    AgsChannelLinkCollectionEditor *channel_link_collection_editor)
{
  ags_channel_link_collection_editor_check(channel_link_collection_editor);
}
