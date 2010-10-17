#ifndef __AGS_CHANNEL_LISTING_EDITOR_H__
#define __AGS_CHANNEL_LISTING_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_property_listing_editor.h"

#include "../audio/ags_audio.h"

#define AGS_TYPE_CHANNEL_LISTING_EDITOR                (ags_channel_listing_editor_get_type())
#define AGS_CHANNEL_LISTING_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANNEL_LISTING_EDITOR, AgsChannelListingEditor))
#define AGS_CHANNEL_LISTING_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CHANNEL_LISTING_EDITOR, AgsChannelListingEditorClass))
#define AGS_IS_CHANNEL_LISTING_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CHANNEL_LISTING_EDITOR))
#define AGS_IS_CHANNEL_LISTING_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CHANNEL_LISTING_EDITOR))
#define AGS_CHANNEL_LISTING_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CHANNEL_LISTING_EDITOR, AgsChannelListingEditorClass))

typedef struct _AgsChannelListingEditor AgsChannelListingEditor;
typedef struct _AgsChannelListingEditorClass AgsChannelListingEditorClass;

struct _AgsChannelListingEditor
{
  AgsPropertyListingEditor property_listing_editor;

  GType channel_type;

  GtkVBox *child;
};

struct _AgsChannelListingEditorClass
{
  AgsPropertyListingEditorClass property_listing_editor;
};

GType ags_channel_listing_editor_get_type();

void ags_channel_listing_editor_add_children(AgsChannelListingEditor *channel_listing_editor,
					     AgsAudio *audio, guint nth_channel,
					     gboolean connect);
AgsChannelListingEditor* ags_channel_listing_editor_new(GType channel_type);

#endif /*__AGS_CHANNEL_LISTING_EDITOR_H__*/
