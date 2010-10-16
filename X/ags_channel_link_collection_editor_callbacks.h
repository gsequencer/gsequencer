#ifndef __AGS_CHANNEL_LINK_COLLECTION_EDITOR_CALLBACKS_H__
#define __AGS_CHANNEL_LINK_COLLECTION_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_channel_link_collection_editor.h"

int ags_channel_link_collection_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsChannelLinkCollectionEditor *channel_link_collection_editor);

/* control widgets */
void ags_channel_link_collection_editor_link_callback(GtkComboBox *combo,
						      AgsChannelLinkCollectionEditor *channel_link_collection_editor);

void ags_channel_link_collection_editor_first_line_callback(GtkSpinButton *spin_button,
							    AgsChannelLinkCollectionEditor *channel_link_collection_editor);

void ags_channel_link_collection_editor_first_link_callback(GtkSpinButton *spin_button,
							    AgsChannelLinkCollectionEditor *channel_link_collection_editor);

void ags_channel_link_collection_editor_count_callback(GtkSpinButton *spin_button,
						      AgsChannelLinkCollectionEditor *channel_link_collection_editor);

#endif /*__AGS_CHANNEL_LINK_COLLECTION_EDITOR_CALLBACKS_H__*/
