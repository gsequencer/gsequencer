/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/machine/ags_replicator_bridge.h>
#include <ags/X/machine/ags_replicator_bridge_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_clone_channel.h>
#include <ags/audio/recall/ags_clone_channel_run.h>

void ags_replicator_bridge_class_init(AgsReplicatorBridgeClass *replicator_bridge);
void ags_replicator_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_replicator_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_replicator_bridge_init(AgsReplicatorBridge *replicator_bridge);
void ags_replicator_bridge_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_replicator_bridge_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_replicator_bridge_connect(AgsConnectable *connectable);
void ags_replicator_bridge_disconnect(AgsConnectable *connectable);
gchar* ags_replicator_bridge_get_version(AgsPlugin *plugin);
void ags_replicator_bridge_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_replicator_bridge_get_build_id(AgsPlugin *plugin);
void ags_replicator_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_replicator_bridge_set_audio_channels(AgsAudio *audio,
					      guint audio_channels, guint audio_channels_old,
					      AgsReplicatorBridge *replicator_bridge);
void ags_replicator_bridge_set_pads(AgsAudio *audio, GType type,
				    guint pads, guint pads_old,
				    AgsReplicatorBridge *replicator_bridge);

/**
 * SECTION:ags_replicator_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsReplicatorBridge
 * @section_id:
 * @include: ags/X/ags_replicator_bridge.h
 *
 * #AgsReplicatorBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

static gpointer ags_replicator_bridge_parent_class = NULL;

static AgsConnectableInterface *ags_replicator_bridge_parent_connectable_interface;

GType
ags_replicator_bridge_get_type(void)
{
  static GType ags_type_replicator_bridge = 0;

  if(!ags_type_replicator_bridge){
    static const GTypeInfo ags_replicator_bridge_info = {
      sizeof(AgsReplicatorBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_replicator_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsReplicatorBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_replicator_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_replicator_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_replicator_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_replicator_bridge = g_type_register_static(AGS_TYPE_MACHINE,
							"AgsReplicatorBridge\0", &ags_replicator_bridge_info,
							0);

    g_type_add_interface_static(ags_type_replicator_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_replicator_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_replicator_bridge);
}

void
ags_replicator_bridge_class_init(AgsReplicatorBridgeClass *replicator_bridge)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_replicator_bridge_parent_class = g_type_class_peek_parent(replicator_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(replicator_bridge);

  gobject->set_property = ags_replicator_bridge_set_property;
  gobject->get_property = ags_replicator_bridge_get_property;
}

void
ags_replicator_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_replicator_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_replicator_bridge_connect;
  connectable->disconnect = ags_replicator_bridge_disconnect;
}

void
ags_replicator_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_replicator_bridge_get_version;
  plugin->set_version = ags_replicator_bridge_set_version;
  plugin->get_build_id = ags_replicator_bridge_get_build_id;
  plugin->set_build_id = ags_replicator_bridge_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_replicator_bridge_init(AgsReplicatorBridge *replicator_bridge)
{
  GtkVBox *vbox;
  GtkExpander *expander;

  AgsAudio *audio;

  audio = AGS_MACHINE(replicator_bridge)->audio;
  audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_HAS_RECYCLING |
		   AGS_AUDIO_SYNC |
		   AGS_AUDIO_ASYNC);

  AGS_MACHINE(replicator_bridge)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(replicator_bridge)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(replicator_bridge)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(replicator_bridge)->output_line_type = G_TYPE_NONE;

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(replicator_bridge)->audio), "set-audio-channels\0",
			 G_CALLBACK(ags_replicator_bridge_set_audio_channels), replicator_bridge);

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(replicator_bridge)->audio), "set-pads\0",
			 G_CALLBACK(ags_replicator_bridge_set_pads), replicator_bridge);

  /* create widgets */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer *) (gtk_bin_get_child((GtkBin *) replicator_bridge)),
		    (GtkWidget *) vbox);


  /* replicator_bridge */
  expander = (GtkExpander *) gtk_expander_new("conversion replicator_bridge\0");
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) expander,
		     FALSE, FALSE,
		     0);

  replicator_bridge->matrix = (GtkTable *) gtk_table_new(1, 1,
							 FALSE);
  gtk_container_add((GtkContainer *) expander,
		    (GtkWidget *) replicator_bridge->matrix);
}

void
ags_replicator_bridge_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsReplicatorBridge *replicator_bridge;

  replicator_bridge = AGS_REPLICATOR_BRIDGE(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_replicator_bridge_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsReplicatorBridge *replicator_bridge;

  replicator_bridge = AGS_REPLICATOR_BRIDGE(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_replicator_bridge_connect(AgsConnectable *connectable)
{
  AgsReplicatorBridge *replicator_bridge;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_replicator_bridge_parent_connectable_interface->connect(connectable);

  replicator_bridge = AGS_REPLICATOR_BRIDGE(connectable);
}

void
ags_replicator_bridge_disconnect(AgsConnectable *connectable)
{
  AgsReplicatorBridge *replicator_bridge;

  ags_replicator_bridge_parent_connectable_interface->disconnect(connectable);

  /* AgsReplicator_Bridge */
  replicator_bridge = AGS_REPLICATOR_BRIDGE(connectable);

  //TODO:JK: implement me
}

gchar*
ags_replicator_bridge_get_version(AgsPlugin *plugin)
{
  return(AGS_REPLICATOR_BRIDGE(plugin)->version);
}

void
ags_replicator_bridge_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsReplicatorBridge *replicator_bridge;

  replicator_bridge = AGS_REPLICATOR_BRIDGE(plugin);

  replicator_bridge->version = version;
}

gchar*
ags_replicator_bridge_get_build_id(AgsPlugin *plugin)
{
  return(AGS_REPLICATOR_BRIDGE(plugin)->build_id);
}

void
ags_replicator_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsReplicatorBridge *replicator_bridge;

  replicator_bridge = AGS_REPLICATOR_BRIDGE(plugin);

  replicator_bridge->build_id = build_id;
}

void
ags_replicator_bridge_set_audio_channels(AgsAudio *audio,
					 guint audio_channels, guint audio_channels_old,
					 AgsReplicatorBridge *replicator_bridge)
{
  GtkToggleButton *toggle_button;
  GtkLabel *label;
  
  gchar *str;
  guint i, j, k;
  
  if(audio_channels > audio_channels_old){
    /* resize label and matrix table */
    gtk_table_resize(replicator_bridge->v_label,
		     1,
		     audio->output_pads * audio_channels);

    gtk_table_resize(replicator_bridge->h_label,
		     audio->input_pads * audio_channels,
		     1);

    gtk_table_resize(replicator_bridge->matrix,
		     audio->input_pads * audio_channels,
		     audio->output_pads * audio_channels);

    /* create vertical labels */
    for(i = 0; i < audio->input_pads; i++){
      for(j = audio_channels_old; j < audio_channels; j++){
	label = gtk_label_new(g_strdup_printf("%d\0", i * audio_channels + j));
	gtk_table_attach(replicator_bridge->v_label,
			 label,
			 0, 1,
			 i * audio_channels + j, i * audio_channels + j + 1,
			 GTK_FILL, GTK_FILL,
			 0, 0);
      }
    }

    /* create vertical labels */
    for(i = 0; i < audio->input_pads; i++){
      for(j = audio_channels_old; j < audio_channels; j++){
	label = gtk_label_new(g_strdup_printf("%d\0", i * audio_channels + j));
	gtk_table_attach(replicator_bridge->h_label,
			 label,
			 i * audio_channels + j, i * audio_channels + j + 1,
			 0, 1,
			 GTK_FILL, GTK_FILL,
			 0, 0);
      }
    }

    /* create matrix */
    for(i = 0; i < audio->output_pads; i++){
      for(j = 0; j < audio->input_pads; j++){
	for(k = audio_channels_old; k < audio_channels; k++){
	  toggle_button = gtk_toggle_button_new();
	  gtk_table_attach(replicator_bridge->matrix,
			   toggle_button,
			   j * audio_channels + k, j * audio_channels + k + 1,
			   i * audio_channels, i * audio_channels + 1,
			   GTK_FILL, GTK_FILL,
			   0, 0);
	}
      }
    }
  }else{
    gtk_table_resize(replicator_bridge->v_label,
		     1,
		     audio->output_pads * audio_channels);

    gtk_table_resize(replicator_bridge->h_label,
		     audio->input_pads * audio_channels,
		     1);

    gtk_table_resize(replicator_bridge->matrix,
		     audio->output_pads *audio_channels,
		     audio->input_pads * audio_channels);
  }

}

void
ags_replicator_bridge_set_pads(AgsAudio *audio, GType type,
			       guint pads, guint pads_old,
			       AgsReplicatorBridge *replicator_bridge)
{
  GtkToggleButton *toggle_button;
  GtkLabel *label;
  
  gchar *str;
  guint i, j, k;
  
  if(pads > pads_old){
    if(type == AGS_TYPE_INPUT){
      /* resize label and matrix table */
      gtk_table_resize(replicator_bridge->h_label,
		       pads * audio->audio_channels,
		       1);
      
      gtk_table_resize(replicator_bridge->matrix,
		       pads * audio->audio_channels,
		       pads * audio->audio_channels);
    }else{
      /* resize label and matrix table */
      gtk_table_resize(replicator_bridge->v_label,
		       1,
		       pads * audio->audio_channels);
      
      gtk_table_resize(replicator_bridge->matrix,
		       pads * audio->audio_channels,
		       pads * audio->audio_channels);
    }
  }else{
    if(type == AGS_TYPE_INPUT){
    }else{
    }
  }
}

/**
 * ags_replicator_bridge_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsReplicatorBridge
 *
 * Returns: a new #AgsReplicatorBridge
 *
 * Since: 0.4.3
 */
AgsReplicatorBridge*
ags_replicator_bridge_new(GObject *soundcard)
{
  AgsReplicatorBridge *replicator_bridge;
  GValue value = {0,};

  replicator_bridge = (AgsReplicatorBridge *) g_object_new(AGS_TYPE_REPLICATOR_BRIDGE,
							   NULL);
  
  if(soundcard != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, soundcard);
    g_object_set_property(G_OBJECT(AGS_MACHINE(replicator_bridge)->audio),
			  "soundcard\0", &value);
    g_value_unset(&value);
  }

  return(replicator_bridge);
}
