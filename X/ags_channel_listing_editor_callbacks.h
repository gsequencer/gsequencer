#ifndef __AGS_CHANNEL_LISTING_EDITOR_CALLBACKS_H__
#define __AGS_CHANNEL_LISTING_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_channel_listing_editor.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>

int ags_channel_listing_editor_parent_set_callback(GtkWidget *widget,
						   GtkObject *old_parent,
						   AgsChannelListingEditor *channel_listing_editor);

void ags_channel_listing_editor_set_pads_callback(AgsAudio *audio, GType channel_type,
						  guint pads, guint pads_old,
						  AgsChannelListingEditor *channel_listing_editor);

#endif /*__AGS_CHANNEL_LISTING_EDITOR_CALLBACKS_H__*/
