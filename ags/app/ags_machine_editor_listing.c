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

#include <ags/app/ags_machine_editor_listing.h>
#include <ags/app/ags_machine_editor_listing_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_machine_editor.h>
#include <ags/app/ags_machine_editor_pad.h>

#include <ags/i18n.h>

void ags_machine_editor_listing_class_init(AgsMachineEditorListingClass *machine_editor_listing);
void ags_machine_editor_listing_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_editor_listing_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_editor_listing_init(AgsMachineEditorListing *machine_editor_listing);
void ags_machine_editor_listing_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_machine_editor_listing_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);

void ags_machine_editor_listing_connect(AgsConnectable *connectable);
void ags_machine_editor_listing_disconnect(AgsConnectable *connectable);

void ags_machine_editor_listing_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_editor_listing_apply(AgsApplicable *applicable);
void ags_machine_editor_listing_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_machine_editor_listing
 * @short_description: Edit audio related aspects
 * @title: AgsMachineEditorListing
 * @section_id:
 * @include: ags/app/ags_machine_editor_listing.h
 *
 * #AgsMachineEditorListing is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor_listings.
 */

enum{
  PROP_0,
  PROP_CHANNEL_TYPE,
};

GType
ags_machine_editor_listing_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_editor_listing = 0;

    static const GTypeInfo ags_machine_editor_listing_info = {
      sizeof (AgsMachineEditorListingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_editor_listing_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineEditorListing),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_editor_listing_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_listing_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_listing_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_editor_listing = g_type_register_static(GTK_TYPE_BOX,
							     "AgsMachineEditorListing", &ags_machine_editor_listing_info,
							     0);

    g_type_add_interface_static(ags_type_machine_editor_listing,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_editor_listing,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_editor_listing);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_editor_listing_class_init(AgsMachineEditorListingClass *machine_editor_listing)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) machine_editor_listing;

  gobject->set_property = ags_machine_editor_listing_set_property;
  gobject->get_property = ags_machine_editor_listing_get_property;

  /* properties */
  /**
   * AgsMachineEditorListing:channel-type:
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
ags_machine_editor_listing_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_editor_listing_connect;
  connectable->disconnect = ags_machine_editor_listing_disconnect;
}

void
ags_machine_editor_listing_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_machine_editor_listing_set_update;
  applicable->apply = ags_machine_editor_listing_apply;
  applicable->reset = ags_machine_editor_listing_reset;
}

void
ags_machine_editor_listing_init(AgsMachineEditorListing *machine_editor_listing)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(machine_editor_listing),
				 GTK_ORIENTATION_VERTICAL);
  
  machine_editor_listing->flags = 0;
  machine_editor_listing->connectable_flags = 0;

  machine_editor_listing->channel_type = G_TYPE_NONE;

  machine_editor_listing->enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));

  gtk_widget_set_halign(machine_editor_listing->enabled,
			GTK_ALIGN_START);
  gtk_widget_set_valign(machine_editor_listing->enabled,
			GTK_ALIGN_START);
  
  gtk_box_append((GtkBox *) machine_editor_listing,
		 (GtkWidget *) machine_editor_listing->enabled);

  machine_editor_listing->pad = NULL;

  machine_editor_listing->pad_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							   AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) machine_editor_listing,
		 (GtkWidget *) machine_editor_listing->pad_box);
}

void
ags_machine_editor_listing_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsMachineEditorListing *machine_editor_listing;

  machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
  {
    machine_editor_listing->channel_type = g_value_get_gtype(value);  
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_editor_listing_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsMachineEditorListing *machine_editor_listing;

  machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    g_value_set_gtype(value, machine_editor_listing->channel_type);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_editor_listing_connect(AgsConnectable *connectable)
{
  AgsMachineEditorListing *machine_editor_listing;

  GList *start_pad, *pad;

  machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_listing->connectable_flags)) != 0){
    return;
  }

  machine_editor_listing->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  pad =
    start_pad = ags_machine_editor_listing_get_pad(machine_editor_listing);

  while(pad != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(pad->data));

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);
}

void
ags_machine_editor_listing_disconnect(AgsConnectable *connectable)
{
  AgsMachineEditorListing *machine_editor_listing;

  GList *start_pad, *pad;

  machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_listing->connectable_flags)) == 0){
    return;
  }
  
  machine_editor_listing->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  pad =
    start_pad = ags_machine_editor_listing_get_pad(machine_editor_listing);

  while(pad != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(pad->data));

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);
}

void
ags_machine_editor_listing_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineEditorListing *machine_editor_listing;

  GList *start_pad, *pad;

  machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(applicable);

  pad =
    start_pad = ags_machine_editor_listing_get_pad(machine_editor_listing);

  while(pad != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(pad->data),
			      update);

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);
}

void
ags_machine_editor_listing_apply(AgsApplicable *applicable)
{
  AgsMachineEditorListing *machine_editor_listing;

  GList *start_pad, *pad;

  machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(applicable);

  pad =
    start_pad = ags_machine_editor_listing_get_pad(machine_editor_listing);

  while(pad != NULL){
    ags_applicable_apply(AGS_APPLICABLE(pad->data));

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);
}

void
ags_machine_editor_listing_reset(AgsApplicable *applicable)
{
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;

  AgsChannel *start_output, *start_input;

  GList *start_pad, *pad;

  guint output_pads, input_pads;
  guint i;
  
  machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(applicable);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(machine_editor_listing,
								AGS_TYPE_MACHINE_EDITOR);
  
  pad =
    start_pad = ags_machine_editor_listing_get_pad(machine_editor_listing);

  while(pad != NULL){
    ags_machine_editor_listing_remove_pad(machine_editor_listing,
					  pad->data);

    /* iterate */
    pad = pad->next;
  }

  g_list_free(start_pad);

  if(!AGS_IS_MACHINE_EDITOR(machine_editor) ||
     machine_editor->machine == NULL){
    return;
  }

  machine = machine_editor->machine;

  if(machine == NULL){
    return;
  }

  start_output = ags_audio_get_output(machine->audio);
  output_pads = ags_audio_get_output_pads(machine->audio);
  
  input_pads = ags_audio_get_input_pads(machine->audio);
  start_input = ags_audio_get_input(machine->audio);

  if(g_type_is_a(machine_editor_listing->channel_type, AGS_TYPE_OUTPUT)){
    for(i = 0; i < output_pads; i++){
      AgsMachineEditorPad *pad;

      AgsChannel *current_channel;

      current_channel = ags_channel_pad_nth(start_output,
					    i);
      
      pad = ags_machine_editor_pad_new(current_channel);
      ags_machine_editor_listing_add_pad(machine_editor_listing,
					 pad);

      if(current_channel != NULL){
	g_object_unref(current_channel);
      }
    }
  }else{
    for(i = 0; i < input_pads; i++){
      AgsMachineEditorPad *pad;

      AgsChannel *current_channel;

      current_channel = ags_channel_pad_nth(start_input,
					    i);
      
      pad = ags_machine_editor_pad_new(current_channel);
      ags_machine_editor_listing_add_pad(machine_editor_listing,
					 pad);

      if(current_channel != NULL){
	g_object_unref(current_channel);
      }
    }
  }

  /* reset */
  pad =
    start_pad = ags_machine_editor_listing_get_pad(machine_editor_listing);

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
 * ags_machine_editor_listing_get_pad:
 * @machine_editor_listing: the #AgsMachineEditorListing
 * 
 * Get pad.
 * 
 * Returns: the #GList-struct containig #AgsMachineEditorPad
 * 
 * Since: 4.0.0
 */
GList*
ags_machine_editor_listing_get_pad(AgsMachineEditorListing *machine_editor_listing)
{
  g_return_val_if_fail(AGS_IS_MACHINE_EDITOR_LISTING(machine_editor_listing), NULL);

  return(g_list_reverse(g_list_copy(machine_editor_listing->pad)));
}

/**
 * ags_machine_editor_listing_add_pad:
 * @machine_editor_listing: the #AgsMachineEditorListing
 * @pad: the #AgsMachineEditorPad
 * 
 * Add @pad to @machine_editor_listing.
 * 
 * Since: 4.0.0
 */
void
ags_machine_editor_listing_add_pad(AgsMachineEditorListing *machine_editor_listing,
				   AgsMachineEditorPad *pad)
{
  g_return_if_fail(AGS_IS_MACHINE_EDITOR_LISTING(machine_editor_listing));
  g_return_if_fail(AGS_IS_MACHINE_EDITOR_PAD(pad));

  if(g_list_find(machine_editor_listing->pad, pad) == NULL){
    machine_editor_listing->pad = g_list_prepend(machine_editor_listing->pad,
						 pad);
    
    gtk_box_append(machine_editor_listing->pad_box,
		   pad);
  }
}

/**
 * ags_machine_editor_listing_remove_pad:
 * @machine_editor_listing: the #AgsMachineEditorListing
 * @pad: the #AgsMachineEditorPad
 * 
 * Remove @pad from @machine_editor_listing.
 * 
 * Since: 4.0.0
 */
void
ags_machine_editor_listing_remove_pad(AgsMachineEditorListing *machine_editor_listing,
				      AgsMachineEditorPad *pad)
{
  g_return_if_fail(AGS_IS_MACHINE_EDITOR_LISTING(machine_editor_listing));
  g_return_if_fail(AGS_IS_MACHINE_EDITOR_PAD(pad));

  if(g_list_find(machine_editor_listing->pad, pad) != NULL){
    machine_editor_listing->pad = g_list_remove(machine_editor_listing->pad,
						pad);
    
    gtk_box_remove(machine_editor_listing->pad_box,
		   pad);
  }
}

/**
 * ags_machine_editor_listing_new:
 * @channel_type: the channel type
 *
 * Creates an #AgsMachineEditorListing
 *
 * Returns: a new #AgsMachineEditorListing
 *
 * Since: 4.0.0
 */
AgsMachineEditorListing*
ags_machine_editor_listing_new(GType channel_type)
{
  AgsMachineEditorListing *machine_editor_listing;

  machine_editor_listing = (AgsMachineEditorListing *) g_object_new(AGS_TYPE_MACHINE_EDITOR_LISTING,
								    "channel-type", channel_type,
								    NULL);

  return(machine_editor_listing);
}
