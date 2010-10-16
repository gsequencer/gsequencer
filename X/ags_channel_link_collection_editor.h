#ifndef __AGS_CHANNEL_LINK_COLLECTION_EDITOR_H__
#define __AGS_CHANNEL_LINK_COLLECTION_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_CHANNEL_LINK_COLLECTION_EDITOR                (ags_channel_link_collection_editor_get_type())
#define AGS_CHANNEL_LINK_COLLECTION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANNEL_LINK_COLLECTION_EDITOR, AgsChannelLinkCollectionEditor))
#define AGS_CHANNEL_LINK_COLLECTION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CHANNEL_LINK_COLLECTION_EDITOR, AgsChannelLinkCollectionEditorClass))
#define AGS_IS_CHANNEL_LINK_COLLECTION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CHANNEL_LINK_COLLECTION_EDITOR))
#define AGS_IS_CHANNEL_LINK_COLLECTION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CHANNEL_LINK_COLLECTION_EDITOR))
#define AGS_CHANNEL_LINK_COLLECTION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CHANNEL_LINK_COLLECTION_EDITOR, AgsChannelLinkCollectionEditorClass))

typedef struct _AgsChannelLinkCollectionEditor AgsChannelLinkCollectionEditor;
typedef struct _AgsChannelLinkCollectionEditorClass AgsChannelLinkCollectionEditorClass;

struct _AgsChannelLinkCollectionEditor
{
  GtkTable table;

  GType channel_type;

  GtkComboBox *link;
  GtkSpinButton *first_line;
  GtkSpinButton *first_link;
  GtkSpinButton *count;
};

struct _AgsChannelLinkCollectionEditorClass
{
  GtkTableClass table;
};

GType ags_channel_link_collection_editor_get_type();

void ags_channel_link_collection_editor_check_first_line(AgsChannelLinkCollectionEditor *channel_link_collection_editor);

void ags_channel_link_collection_editor_check_first_link(AgsChannelLinkCollectionEditor *channel_link_collection_editor);

void ags_channel_link_collection_editor_check_count(AgsChannelLinkCollectionEditor *channel_link_collection_editor);

AgsChannelLinkCollectionEditor* ags_channel_link_collection_editor_new();

#endif /*__AGS_CHANNEL_LINK_COLLECTION_EDITOR_H__*/
