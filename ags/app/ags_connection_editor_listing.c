/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_connection_editor_listing.h>
#include <ags/app/ags_connection_editor_listing_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_connection_editor.h>
#include <ags/app/ags_connection_editor_pad.h>

#include <ags/i18n.h>

void ags_connection_editor_listing_class_init(AgsConnectionEditorListingClass *connection_editor_listing);
void ags_connection_editor_listing_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_connection_editor_listing_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_connection_editor_listing_init(AgsConnectionEditorListing *connection_editor_listing);
void ags_connection_editor_listing_set_property(GObject *gobject,
						guint prop_id,
						const GValue *value,
						GParamSpec *param_spec);
void ags_connection_editor_listing_get_property(GObject *gobject,
						guint prop_id,
						GValue *value,
						GParamSpec *param_spec);

void ags_connection_editor_listing_connect(AgsConnectable *connectable);
void ags_connection_editor_listing_disconnect(AgsConnectable *connectable);

void ags_connection_editor_listing_set_update(AgsApplicable *applicable, gboolean update);
void ags_connection_editor_listing_apply(AgsApplicable *applicable);
void ags_connection_editor_listing_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_connection_editor_listing
 * @short_description: Edit audio related aspects
 * @title: AgsConnectionEditorListing
 * @section_id:
 * @include: ags/app/ags_connection_editor_listing.h
 *
 * #AgsConnectionEditorListing is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor_listings.
 */

enum{
  PROP_0,
  PROP_CHANNEL_TYPE,
};

GType
ags_connection_editor_listing_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_connection_editor_listing = 0;

    static const GTypeInfo ags_connection_editor_listing_info = {
      sizeof (AgsConnectionEditorListingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_connection_editor_listing_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConnectionEditorListing),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_connection_editor_listing_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_listing_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_listing_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_connection_editor_listing = g_type_register_static(GTK_TYPE_BOX,
								"AgsConnectionEditorListing", &ags_connection_editor_listing_info,
								0);

    g_type_add_interface_static(ags_type_connection_editor_listing,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_connection_editor_listing,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_connection_editor_listing);
  }

  return g_define_type_id__volatile;
}

void
ags_connection_editor_listing_class_init(AgsConnectionEditorListingClass *connection_editor_listing)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) connection_editor_listing;

  gobject->set_property = ags_connection_editor_listing_set_property;
  gobject->get_property = ags_connection_editor_listing_get_property;

  /* properties */
  /**
   * AgsConnectionEditorListing:channel-type:
   *
   * The channel type.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_gtype("channel-type",
				  i18n_pspec("channel type"),
				  i18n_pspec("The channel type"),
				  AGS_TYPE_CHANNEL,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);
}

void
ags_connection_editor_listing_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_connection_editor_listing_connect;
  connectable->disconnect = ags_connection_editor_listing_disconnect;
}

void
ags_connection_editor_listing_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_connection_editor_listing_set_update;
  applicable->apply = ags_connection_editor_listing_apply;
  applicable->reset = ags_connection_editor_listing_reset;
}

void
ags_connection_editor_listing_init(AgsConnectionEditorListing *connection_editor_listing)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(connection_editor_listing),
				 GTK_ORIENTATION_VERTICAL);
  
  connection_editor_listing->flags = 0;
  connection_editor_listing->connectable_flags = 0;

  connection_editor_listing->channel_type = G_TYPE_NONE;

  connection_editor_listing->parent_connection_editor = NULL;
  
  connection_editor_listing->enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));

  gtk_widget_set_halign(connection_editor_listing->enabled,
			GTK_ALIGN_START);
  gtk_widget_set_valign(connection_editor_listing->enabled,
			GTK_ALIGN_START);
  
  gtk_box_append((GtkBox *) connection_editor_listing,
		 (GtkWidget *) connection_editor_listing->enabled);

  connection_editor_listing->pad = NULL;

  connection_editor_listing->pad_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							      AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) connection_editor_listing,
		 (GtkWidget *) connection_editor_listing->pad_box);
}

void
ags_connection_editor_listing_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec)
{
  AgsConnectionEditorListing *connection_editor_listing;

  connection_editor_listing = AGS_CONNECTION_EDITOR_LISTING(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    {
      connection_editor_listing->channel_type = g_value_get_gtype(value);  
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_editor_listing_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec)
{
  AgsConnectionEditorListing *connection_editor_listing;

  connection_editor_listing = AGS_CONNECTION_EDITOR_LISTING(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    g_value_set_gtype(value, connection_editor_listing->channel_type);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_editor_listing_connect(AgsConnectable *connectable)
{
  AgsConnectionEditorListing *connection_editor_listing;

  GList *start_pad, *pad;

  connection_editor_listing = AGS_CONNECTION_EDITOR_LISTING(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_listing->connectable_flags)) != 0){
    return;
  }

  connection_editor_listing->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  pad =
    start_pad = ags_connection_editor_listing_get_pad(connection_editor_listing);

  while(pad != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(pad->data));

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);
}

void
ags_connection_editor_listing_disconnect(AgsConnectable *connectable)
{
  AgsConnectionEditorListing *connection_editor_listing;

  GList *start_pad, *pad;

  connection_editor_listing = AGS_CONNECTION_EDITOR_LISTING(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_listing->connectable_flags)) == 0){
    return;
  }
  
  connection_editor_listing->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  pad =
    start_pad = ags_connection_editor_listing_get_pad(connection_editor_listing);

  while(pad != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(pad->data));

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);
}

void
ags_connection_editor_listing_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsConnectionEditorListing *connection_editor_listing;

  GList *start_pad, *pad;

  connection_editor_listing = AGS_CONNECTION_EDITOR_LISTING(applicable);

  pad =
    start_pad = ags_connection_editor_listing_get_pad(connection_editor_listing);

  while(pad != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(pad->data),
			      update);

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);
}

void
ags_connection_editor_listing_apply(AgsApplicable *applicable)
{
  AgsConnectionEditorListing *connection_editor_listing;

  GList *start_pad, *pad;

  connection_editor_listing = AGS_CONNECTION_EDITOR_LISTING(applicable);

  pad =
    start_pad = ags_connection_editor_listing_get_pad(connection_editor_listing);

  while(pad != NULL){
    ags_applicable_apply(AGS_APPLICABLE(pad->data));

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);
}

void
ags_connection_editor_listing_reset(AgsApplicable *applicable)
{
  AgsMachine *machine;
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorListing *connection_editor_listing;

  AgsChannel *start_output, *start_input;

  GList *start_pad, *pad;

  guint output_pads, input_pads;
  guint i;
  
  connection_editor_listing = AGS_CONNECTION_EDITOR_LISTING(applicable);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_listing,
								      AGS_TYPE_CONNECTION_EDITOR);
  
  pad =
    start_pad = ags_connection_editor_listing_get_pad(connection_editor_listing);

  while(pad != NULL){
    ags_connection_editor_listing_remove_pad(connection_editor_listing,
					     pad->data);

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);

  if(!AGS_IS_CONNECTION_EDITOR(connection_editor) ||
     connection_editor->machine == NULL){
    return;
  }

  machine = connection_editor->machine;

  if(machine == NULL){
    return;
  }

  start_output = ags_audio_get_output(machine->audio);
  output_pads = ags_audio_get_output_pads(machine->audio);
  
  input_pads = ags_audio_get_input_pads(machine->audio);
  start_input = ags_audio_get_input(machine->audio);

  if(g_type_is_a(connection_editor_listing->channel_type, AGS_TYPE_OUTPUT)){
    for(i = 0; i < output_pads; i++){
      AgsConnectionEditorPad *pad;

      AgsChannel *current_channel;

      current_channel = ags_channel_pad_nth(start_output,
					    i);
      
      pad = ags_connection_editor_pad_new(current_channel);
      ags_connection_editor_listing_add_pad(connection_editor_listing,
					    pad);

      if(current_channel != NULL){
	g_object_unref(current_channel);
      }
    }
  }else{
    for(i = 0; i < input_pads; i++){
      AgsConnectionEditorPad *pad;

      AgsChannel *current_channel;

      current_channel = ags_channel_pad_nth(start_input,
					    i);
      
      pad = ags_connection_editor_pad_new(current_channel);
      ags_connection_editor_listing_add_pad(connection_editor_listing,
					    pad);

      if(current_channel != NULL){
	g_object_unref(current_channel);
      }
    }
  }

  /* reset */
  pad =
    start_pad = ags_connection_editor_listing_get_pad(connection_editor_listing);

  while(pad != NULL){
    ags_applicable_reset(AGS_APPLICABLE(pad->data));

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

/**
 * ags_connection_editor_listing_get_pad:
 * @connection_editor_listing: the #AgsConnectionEditorListing
 * 
 * Get pad.
 * 
 * Returns: the #GList-struct containig #AgsConnectionEditorPad
 * 
 * Since: 4.0.0
 */
GList*
ags_connection_editor_listing_get_pad(AgsConnectionEditorListing *connection_editor_listing)
{
  g_return_val_if_fail(AGS_IS_CONNECTION_EDITOR_LISTING(connection_editor_listing), NULL);

  return(g_list_reverse(g_list_copy(connection_editor_listing->pad)));
}

/**
 * ags_connection_editor_listing_add_pad:
 * @connection_editor_listing: the #AgsConnectionEditorListing
 * @pad: the #AgsConnectionEditorPad
 * 
 * Add @pad to @connection_editor_listing.
 * 
 * Since: 4.0.0
 */
void
ags_connection_editor_listing_add_pad(AgsConnectionEditorListing *connection_editor_listing,
				      AgsConnectionEditorPad *pad)
{
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_LISTING(connection_editor_listing));
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_PAD(pad));

  if(g_list_find(connection_editor_listing->pad, pad) == NULL){
    connection_editor_listing->pad = g_list_prepend(connection_editor_listing->pad,
						    pad);
    
    gtk_box_append(connection_editor_listing->pad_box,
		   pad);
  }
}

/**
 * ags_connection_editor_listing_remove_pad:
 * @connection_editor_listing: the #AgsConnectionEditorListing
 * @pad: the #AgsConnectionEditorPad
 * 
 * Remove @pad from @connection_editor_listing.
 * 
 * Since: 4.0.0
 */
void
ags_connection_editor_listing_remove_pad(AgsConnectionEditorListing *connection_editor_listing,
					 AgsConnectionEditorPad *pad)
{
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_LISTING(connection_editor_listing));
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_PAD(pad));

  if(g_list_find(connection_editor_listing->pad, pad) != NULL){
    connection_editor_listing->pad = g_list_remove(connection_editor_listing->pad,
						   pad);
    
    gtk_box_remove(connection_editor_listing->pad_box,
		   pad);
  }
}

/**
 * ags_connection_editor_listing_new:
 * @channel_type: the channel type
 *
 * Creates an #AgsConnectionEditorListing
 *
 * Returns: a new #AgsConnectionEditorListing
 *
 * Since: 4.0.0
 */
AgsConnectionEditorListing*
ags_connection_editor_listing_new(GType channel_type)
{
  AgsConnectionEditorListing *connection_editor_listing;

  connection_editor_listing = (AgsConnectionEditorListing *) g_object_new(AGS_TYPE_CONNECTION_EDITOR_LISTING,
									  "channel-type", channel_type,
									  NULL);

  return(connection_editor_listing);
}
