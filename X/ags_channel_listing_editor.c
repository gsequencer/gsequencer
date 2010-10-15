#include "ags_channel_listing_editor.h"
#include "ags_channel_listing_editor_callbacks.h"

#include "../object/ags_connectable.h"

#include "../audio/ags_channel.h"
#include "../audio/ags_output.h"

#include "ags_machine_editor.h"
#include "ags_pad_editor.h"

void ags_channel_listing_editor_class_init(AgsChannelListingEditorClass *channel_listing_editor);
void ags_channel_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_listing_editor_init(AgsChannelListingEditor *channel_listing_editor);
void ags_channel_listing_editor_connect(AgsConnectable *connectable);
void ags_channel_listing_editor_disconnect(AgsConnectable *connectable);
void ags_channel_listing_editor_destroy(GtkObject *object);
void ags_channel_listing_editor_show(GtkWidget *widget);

GType
ags_channel_listing_editor_get_type(void)
{
  static GType ags_type_channel_listing_editor = 0;

  if(!ags_type_channel_listing_editor){
    static const GTypeInfo ags_channel_listing_editor_info = {
      sizeof (AgsChannelListingEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_listing_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannelListingEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_listing_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_listing_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel_listing_editor = g_type_register_static(AGS_TYPE_PROPERTY_LISTING_EDITOR,
							     "AgsChannelListingEditor\0",
							     &ags_channel_listing_editor_info,
							     0);
    
    g_type_add_interface_static(ags_type_channel_listing_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_channel_listing_editor);
}

void
ags_channel_listing_editor_class_init(AgsChannelListingEditorClass *channel_listing_editor)
{
}

void
ags_channel_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_channel_listing_editor_connect;
  connectable->disconnect = ags_channel_listing_editor_disconnect;
}

void
ags_channel_listing_editor_init(AgsChannelListingEditor *channel_listing_editor)
{
  g_signal_connect_after(G_OBJECT(channel_listing_editor), "parent_set\0",
			 G_CALLBACK(ags_channel_listing_editor_parent_set_callback), channel_listing_editor);

  channel_listing_editor->child = NULL;
}

void
ags_channel_listing_editor_connect(AgsConnectable *connectable)
{
  AgsMachineEditor *machine_editor;
  AgsChannelListingEditor *channel_listing_editor;
  GList *pad_list;

  /* AgsChannelListingEditor */
  channel_listing_editor = AGS_CHANNEL_LISTING_EDITOR(connectable);

  /* empty */
}

void
ags_channel_listing_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_channel_listing_editor_destroy(GtkObject *object)
{
  /* empty */
}

void
ags_channel_listing_editor_show(GtkWidget *widget)
{
  /* empty */
}

void
ags_channel_listing_editor_add_children(AgsChannelListingEditor *channel_listing_editor,
					AgsAudio *audio)
{
  AgsPadEditor *pad_editor;
  GtkVBox *vbox;
  AgsChannel *channel;

  if(channel_listing_editor->child != NULL){
    vbox = channel_listing_editor->child;
    channel_listing_editor->child = NULL;
    gtk_widget_destroy(GTK_WIDGET(vbox));
  }

  if(audio == NULL)
    return;
  
  channel_listing_editor->child = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(channel_listing_editor),
		     GTK_WIDGET(channel_listing_editor->child),
		     FALSE, FALSE,
		     0);

  if(channel_listing_editor->channel_type == AGS_TYPE_OUTPUT)
    channel = audio->output;
  else
    channel = audio->input;

  while(channel != NULL){
    pad_editor = ags_pad_editor_new(channel);
    gtk_box_pack_start(GTK_BOX(channel_listing_editor->child),
		       GTK_WIDGET(pad_editor),
		       FALSE, FALSE,
		       0);

    channel = channel->next_pad;
  }
}

AgsChannelListingEditor*
ags_channel_listing_editor_new(GType channel_type)
{
  AgsChannelListingEditor *channel_listing_editor;

  channel_listing_editor = (AgsChannelListingEditor *) g_object_new(AGS_TYPE_CHANNEL_LISTING_EDITOR,
								    NULL);
  
  channel_listing_editor->channel_type = channel_type;

  return(channel_listing_editor);
}
