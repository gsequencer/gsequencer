#ifndef __AGS_CHANNEL_LISTING_EDITOR_CALLBACKS_H__
#define __AGS_CHANNEL_LISTING_EDITOR_CALLBACKS_H__

#include "ags_channel_listing_editor.h"

#include "ags_machine.h"
#include "ags_machine_editor.h"

int ags_channel_listing_editor_parent_set_callback(GtkWidget *widget,
						   GtkObject *old_parent,
						   AgsChannelListingEditor *channel_listing_editor);

#endif /*__AGS_CHANNEL_LISTING_EDITOR_CALLBACKS_H__*/
