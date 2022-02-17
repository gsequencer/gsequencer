/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/GSequencer/ags_listing_editor.h>
#include <ags/GSequencer/ags_listing_editor_callbacks.h>

#include <ags/GSequencer/ags_machine_editor.h>
#include <ags/GSequencer/ags_connection_editor.h>
#include <ags/GSequencer/ags_pad_editor.h>
#include <ags/GSequencer/ags_line_editor.h>
#include <ags/GSequencer/ags_link_editor.h>
#include <ags/GSequencer/ags_line_member_editor.h>

void ags_listing_editor_class_init(AgsListingEditorClass *listing_editor);
void ags_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_listing_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_listing_editor_init(AgsListingEditor *listing_editor);

void ags_listing_editor_connect(AgsConnectable *connectable);
void ags_listing_editor_disconnect(AgsConnectable *connectable);

void ags_listing_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_listing_editor_apply(AgsApplicable *applicable);
void ags_listing_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_listing_editor
 * @short_description: pack pad editors.
 * @title: AgsListingEditor
 * @section_id:
 * @include: ags/X/ags_listing_editor.h
 *
 * #AgsListingEditor is a composite widget to pack #AgsPadEditor.
 */

static gpointer ags_listing_editor_parent_class = NULL;
static AgsConnectableInterface* ags_listing_editor_parent_connectable_interface;

GType
ags_listing_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_listing_editor = 0;

    static const GTypeInfo ags_listing_editor_info = {
      sizeof (AgsListingEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_listing_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsListingEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_listing_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_listing_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_listing_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_listing_editor = g_type_register_static(AGS_TYPE_PROPERTY_LISTING_EDITOR,
						     "AgsListingEditor",
						     &ags_listing_editor_info,
						     0);
    
    g_type_add_interface_static(ags_type_listing_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_listing_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_listing_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_listing_editor_class_init(AgsListingEditorClass *listing_editor)
{
  ags_listing_editor_parent_class = g_type_class_peek_parent(listing_editor);
}

void
ags_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_listing_editor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_listing_editor_connect;
  connectable->disconnect = ags_listing_editor_disconnect;
}

void
ags_listing_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_listing_editor_set_update;
  applicable->apply = ags_listing_editor_apply;
  applicable->reset = ags_listing_editor_reset;
}

void
ags_listing_editor_init(AgsListingEditor *listing_editor)
{
  listing_editor->channel_type = G_TYPE_NONE;

  listing_editor->child = NULL;
}

void
ags_listing_editor_connect(AgsConnectable *connectable)
{
  AgsMachineEditor *machine_editor;
  AgsListingEditor *listing_editor;
  
  GList *pad_editor, *pad_editor_start;

  listing_editor = AGS_LISTING_EDITOR(connectable);

  if((AGS_PROPERTY_EDITOR_CONNECTED & (AGS_PROPERTY_EDITOR(listing_editor)->flags)) != 0){
    return;
  }

  ags_listing_editor_parent_connectable_interface->connect(connectable);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(GTK_WIDGET(listing_editor),
								AGS_TYPE_MACHINE_EDITOR);

  if(machine_editor != NULL &&
     machine_editor->machine != NULL){
    g_signal_connect_after(G_OBJECT(machine_editor->machine), "resize-pads",
			   G_CALLBACK(ags_listing_editor_resize_pads_callback), listing_editor);
  }

  /* AgsPadEditor */
  pad_editor_start = 
    pad_editor = gtk_container_get_children(GTK_CONTAINER(listing_editor->child));

  while(pad_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(pad_editor->data));

    pad_editor = pad_editor->next;
  }
  
  g_list_free(pad_editor_start);
}

void
ags_listing_editor_disconnect(AgsConnectable *connectable)
{
  AgsMachineEditor *machine_editor;
  AgsListingEditor *listing_editor;

  GList *pad_editor, *pad_editor_start;

  listing_editor = AGS_LISTING_EDITOR(connectable);

  if((AGS_PROPERTY_EDITOR_CONNECTED & (AGS_PROPERTY_EDITOR(listing_editor)->flags)) == 0){
    return;
  }

  ags_listing_editor_parent_connectable_interface->disconnect(connectable);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(GTK_WIDGET(listing_editor),
								AGS_TYPE_MACHINE_EDITOR);

  if(machine_editor != NULL &&
     machine_editor->machine != NULL){
    g_object_disconnect(G_OBJECT(machine_editor->machine),
			"any_signal::resize-pads",
			G_CALLBACK(ags_listing_editor_resize_pads_callback),
			listing_editor,
			NULL);
  }
  
  /* AgsPadEditor */
  pad_editor_start = 
    pad_editor = gtk_container_get_children(GTK_CONTAINER(listing_editor->child));

  while(pad_editor != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(pad_editor->data));

    pad_editor = pad_editor->next;
  }
  
  g_list_free(pad_editor_start);
}

void
ags_listing_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsListingEditor *listing_editor;

  GList *pad_editor, *pad_editor_start;

  listing_editor = AGS_LISTING_EDITOR(applicable);

  pad_editor_start = 
    pad_editor = gtk_container_get_children(GTK_CONTAINER(listing_editor->child));

  while(pad_editor != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(pad_editor->data), update);

    pad_editor = pad_editor->next;
  }

  g_list_free(pad_editor_start);
}

void
ags_listing_editor_apply(AgsApplicable *applicable)
{

  AgsListingEditor *listing_editor;

  GList *pad_editor, *pad_editor_start;

  listing_editor = AGS_LISTING_EDITOR(applicable);

  if((AGS_PROPERTY_EDITOR_ENABLED & (AGS_PROPERTY_EDITOR(listing_editor)->flags)) == 0){
    return;
  }

  pad_editor_start = 
    pad_editor = gtk_container_get_children(GTK_CONTAINER(listing_editor->child));

  while(pad_editor != NULL){
    ags_applicable_apply(AGS_APPLICABLE(pad_editor->data));

    pad_editor = pad_editor->next;
  }
  
  g_list_free(pad_editor_start);
}

void
ags_listing_editor_reset(AgsApplicable *applicable)
{
  AgsListingEditor *listing_editor;

  GList *pad_editor, *pad_editor_start;

  listing_editor = AGS_LISTING_EDITOR(applicable);

  pad_editor_start = 
    pad_editor = gtk_container_get_children(GTK_CONTAINER(listing_editor->child));

  while(pad_editor != NULL){
    ags_applicable_reset(AGS_APPLICABLE(pad_editor->data));

    pad_editor = pad_editor->next;
  }

  g_list_free(pad_editor_start);
}

/**
 * ags_listing_editor_add_children:
 * @listing_editor: the #AgsListingEditor
 * @audio: the #AgsAudio to use
 * @nth: nth channel to start creation until end
 * @connect: if %TRUE widget is connected and shown
 *
 * Creates new pad editors or destroys them.
 *
 * Since: 3.0.0
 */
void
ags_listing_editor_add_children(AgsListingEditor *listing_editor,
				AgsAudio *audio, guint nth,
				gboolean connect)
{
  GtkWidget *toplevel;
  AgsPadEditor *pad_editor;
  GtkBox *vbox;

  AgsChannel *start_channel;
  AgsChannel *channel, *next_pad, *nth_channel;

  if(nth == 0 &&
     listing_editor->child != NULL){
    vbox = listing_editor->child;
    listing_editor->child = NULL;
    gtk_widget_destroy(GTK_WIDGET(vbox));
  }

  if(audio == NULL){
    return;
  }

  toplevel = gtk_widget_get_toplevel(listing_editor);
  
  /* instantiate pad editor vbox */
  if(nth == 0){
    listing_editor->child = (GtkBox *) gtk_vbox_new(GTK_ORIENTATION_VERTICAL,
						    0);
    gtk_box_set_homogeneous(listing_editor->child,
			    FALSE);
    gtk_box_pack_start(GTK_BOX(listing_editor),
		       GTK_WIDGET(listing_editor->child),
		       FALSE, FALSE,
		       0);
  }

  /* get current channel */
  start_channel = NULL;
  
  if(listing_editor->channel_type == AGS_TYPE_OUTPUT){
    g_object_get(audio,
		 "output", &start_channel,
		 NULL);
  }else{
    g_object_get(audio,
		 "input", &start_channel,
		 NULL);
  }

  nth_channel = ags_channel_nth(start_channel,
				nth);
  
  channel = nth_channel;

  next_pad = NULL;

  while(channel != NULL){
    /* instantiate pad editor */
    pad_editor = ags_pad_editor_new(NULL);

    if(AGS_IS_MACHINE_EDITOR(toplevel)){
      pad_editor->editor_type_count = 2;
      pad_editor->editor_type = (GType *) malloc(pad_editor->editor_type_count * sizeof(GType));
      pad_editor->editor_type[0] = AGS_TYPE_LINK_EDITOR;
      pad_editor->editor_type[1] = AGS_TYPE_LINE_MEMBER_EDITOR;
    }else if(AGS_IS_CONNECTION_EDITOR(toplevel)){
      if(listing_editor->channel_type == AGS_TYPE_OUTPUT){
	pad_editor->editor_type_count = 1;
	pad_editor->editor_type = (GType *) malloc(pad_editor->editor_type_count * sizeof(GType));
	pad_editor->editor_type[0] = AGS_TYPE_OUTPUT_EDITOR;
      }else{
	pad_editor->editor_type_count = 1;
	pad_editor->editor_type = (GType *) malloc(pad_editor->editor_type_count * sizeof(GType));
	pad_editor->editor_type[0] = AGS_TYPE_INPUT_EDITOR;
      }
    }
    
    g_object_set(pad_editor,
		 "channel", channel,
		 NULL);
    
    gtk_box_pack_start(GTK_BOX(listing_editor->child),
		       GTK_WIDGET(pad_editor),
		       FALSE, FALSE,
		       0);

    if(connect){
      ags_connectable_connect(AGS_CONNECTABLE(pad_editor));
      gtk_widget_show_all(GTK_WIDGET(pad_editor));
    }

    /* iterate */
    next_pad = ags_channel_next_pad(channel);

    g_object_unref(channel);

    channel = next_pad;
  }

  if(next_pad != NULL){
    g_object_unref(next_pad);
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
}

/**
 * ags_listing_editor_new:
 * @channel_type: the channel type to represent
 *
 * Create a new instance of #AgsListingEditor
 *
 * Returns: the new #AgsListingEditor
 *
 * Since: 3.0.0
 */
AgsListingEditor*
ags_listing_editor_new(GType channel_type)
{
  AgsListingEditor *listing_editor;

  listing_editor = (AgsListingEditor *) g_object_new(AGS_TYPE_LISTING_EDITOR,
						     NULL);
  
  listing_editor->channel_type = channel_type;

  return(listing_editor);
}
