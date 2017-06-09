/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/recall/ags_route_dssi_audio.h>
#include <ags/audio/recall/ags_route_dssi_audio_run.h>

#include <ags/object/ags_plugin.h>

void ags_route_dssi_audio_class_init(AgsRouteDssiAudioClass *route_dssi_audio);
void ags_route_dssi_audio_init(AgsRouteDssiAudio *route_dssi_audio);
void ags_route_dssi_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_route_dssi_audio_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_route_dssi_audio_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_route_dssi_audio_set_ports(AgsPlugin *plugin, GList *port);
void ags_route_dssi_audio_dispose(GObject *gobject);
void ags_route_dssi_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_route_dssi_audio
 * @short_description: route MIDI to DSSI
 * @title: AgsRouteDssiAudio
 * @section_id:
 * @include: ags/audio/recall/ags_route_dssi_audio.h
 *
 * The #AgsRouteDssiAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_NOTATION_INPUT,
  PROP_SEQUENCER_INPUT,
};

static gpointer ags_route_dssi_audio_parent_class = NULL;

static const gchar *ags_route_dssi_audio_plugin_name = "ags-count-beats";
static const gchar *ags_route_dssi_audio_specifier[] = {
  "./notation-input[0]",
  "./sequencer-input[0]",
};
static const gchar *ags_route_dssi_audio_control_port[] = {
  "1/2",
  "2/2",
};

GType
ags_route_dssi_audio_get_type()
{
  static GType ags_type_route_dssi_audio = 0;

  if(!ags_type_route_dssi_audio){
    static const GTypeInfo ags_route_dssi_audio_info = {
      sizeof (AgsRouteDssiAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_route_dssi_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRouteDssiAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_route_dssi_audio_init,
    };
    
    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_route_dssi_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_route_dssi_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						       "AgsRouteDssiAudio",
						       &ags_route_dssi_audio_info,
						       0);

    g_type_add_interface_static(ags_type_route_dssi_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_route_dssi_audio);
}

void
ags_route_dssi_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_route_dssi_audio_set_ports;
}

void
ags_route_dssi_audio_class_init(AgsRouteDssiAudioClass *route_dssi_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_route_dssi_audio_parent_class = g_type_class_peek_parent(route_dssi_audio);

  gobject = (GObjectClass *) route_dssi_audio;

  gobject->set_property = ags_route_dssi_audio_set_property;
  gobject->get_property = ags_route_dssi_audio_get_property;

  gobject->dispose = ags_route_dssi_audio_dispose;
  gobject->finalize = ags_route_dssi_audio_finalize;

  /* properties */
  /**
   * AgsRouteDssiAudio:notation-input:
   *
   * If enabled input is taken of #AgsNotation.
   * 
   * Since: 0.7.1
   */
  param_spec = g_param_spec_object("notation-input",
				   "route notation input",
				   "Route notation as input to the DSSI recall.",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_INPUT,
				  param_spec);

  /**
   * AgsRouteDssiAudio:sequencer-input:
   *
   * If enabled input is taken of #AgsSequencer.
   * 
   * Since: 0.7.1
   */
  param_spec = g_param_spec_object("sequencer-input",
				   "route sequencer input",
				   "Route sequencer as input to the DSSI recall.",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_INPUT,
				  param_spec);
}

void
ags_route_dssi_audio_init(AgsRouteDssiAudio *route_dssi_audio)
{
  GList *port;

  AGS_RECALL(route_dssi_audio)->name = "ags-count-beats";
  AGS_RECALL(route_dssi_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(route_dssi_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(route_dssi_audio)->xml_type = "ags-count-beats-audio";

  port = NULL;

  /* notation input */
  route_dssi_audio->notation_input = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_route_dssi_audio_plugin_name,
						  "specifier", ags_route_dssi_audio_specifier[0],
						  "control-port", ags_route_dssi_audio_control_port[0],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_BOOLEAN,
						  "port-value-size", sizeof(gboolean),
						  "port-value-length", 1,
						  NULL);
  g_object_ref(route_dssi_audio->notation_input);
  
  route_dssi_audio->notation_input->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, route_dssi_audio->notation_input);
  g_object_ref(route_dssi_audio->notation_input);

  /* sequencer input */
  route_dssi_audio->sequencer_input = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_route_dssi_audio_plugin_name,
						  "specifier", ags_route_dssi_audio_specifier[1],
						  "control-port", ags_route_dssi_audio_control_port[1],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_BOOLEAN,
						  "port-value-size", sizeof(gboolean),
						  "port-value-length", 1,
						  NULL);
  g_object_ref(route_dssi_audio->sequencer_input);

  route_dssi_audio->sequencer_input->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, route_dssi_audio->sequencer_input);
  g_object_ref(route_dssi_audio->sequencer_input);

  /* port */
  AGS_RECALL(route_dssi_audio)->port = port;
}

void
ags_route_dssi_audio_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsRouteDssiAudio *route_dssi_audio;

  route_dssi_audio = AGS_ROUTE_DSSI_AUDIO(gobject);

  switch(prop_id){
  case PROP_NOTATION_INPUT:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == route_dssi_audio->notation_input){
	return;
      }

      if(route_dssi_audio->notation_input != NULL){
	g_object_unref(G_OBJECT(route_dssi_audio->notation_input));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      route_dssi_audio->notation_input = port;
    }
    break;
  case PROP_SEQUENCER_INPUT:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == route_dssi_audio->sequencer_input){
	return;
      }

      if(route_dssi_audio->sequencer_input != NULL){
	g_object_unref(G_OBJECT(route_dssi_audio->sequencer_input));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      route_dssi_audio->sequencer_input = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_route_dssi_audio_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsRouteDssiAudio *route_dssi;
  
  route_dssi = AGS_ROUTE_DSSI_AUDIO(gobject);

  switch(prop_id){
  case PROP_NOTATION_INPUT:
    {
      g_value_set_object(value, route_dssi->notation_input);
    }
    break;
  case PROP_SEQUENCER_INPUT:
    {
      g_value_set_object(value, route_dssi->sequencer_input);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_route_dssi_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./notation-input[0]",
		18)){
      g_object_set(G_OBJECT(plugin),
		   "notation-input", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"./sequencer-input[0]",
		18)){
      g_object_set(G_OBJECT(plugin),
		   "sequencer-input", AGS_PORT(port->data),
		   NULL);
    }
    
    port = port->next;
  }
}

void
ags_route_dssi_audio_dispose(GObject *gobject)
{
  AgsRouteDssiAudio *route_dssi_audio;

  route_dssi_audio = AGS_ROUTE_DSSI_AUDIO(gobject);

  /* notation input */
  if(route_dssi_audio->notation_input != NULL){
    g_object_unref(G_OBJECT(route_dssi_audio->notation_input));

    route_dssi_audio->notation_input = NULL;
  }

  /* sequencer input */
  if(route_dssi_audio->sequencer_input != NULL){
    g_object_unref(G_OBJECT(route_dssi_audio->sequencer_input));

    route_dssi_audio->sequencer_input = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_route_dssi_audio_parent_class)->dispose(gobject);
}

void
ags_route_dssi_audio_finalize(GObject *gobject)
{
  AgsRouteDssiAudio *route_dssi_audio;

  route_dssi_audio = AGS_ROUTE_DSSI_AUDIO(gobject);

  if(route_dssi_audio->notation_input != NULL){
    g_object_unref(G_OBJECT(route_dssi_audio->notation_input));
  }

  if(route_dssi_audio->sequencer_input != NULL){
    g_object_unref(G_OBJECT(route_dssi_audio->sequencer_input));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_route_dssi_audio_parent_class)->finalize(gobject);
}

/**
 * ags_route_dssi_audio_new:
 *
 * Creates an #AgsRouteDssiAudio
 *
 * Returns: a new #AgsRouteDssiAudio
 *
 * Since: 0.7.1
 */
AgsRouteDssiAudio*
ags_route_dssi_audio_new()
{
  AgsRouteDssiAudio *route_dssi_audio;

  route_dssi_audio = (AgsRouteDssiAudio *) g_object_new(AGS_TYPE_ROUTE_DSSI_AUDIO,
							NULL);
  
  return(route_dssi_audio);
}
