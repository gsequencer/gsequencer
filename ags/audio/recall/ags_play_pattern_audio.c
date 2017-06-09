/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/audio/recall/ags_play_pattern_audio.h>
#include <ags/audio/recall/ags_play_pattern_audio_run.h>

#include <ags/object/ags_plugin.h>

void ags_play_pattern_audio_class_init(AgsPlayPatternAudioClass *play_pattern_audio);
void ags_play_pattern_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_pattern_audio_init(AgsPlayPatternAudio *play_pattern_audio);
void ags_play_pattern_audio_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_play_pattern_audio_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_play_pattern_audio_set_ports(AgsPlugin *plugin, GList *port);
void ags_play_pattern_audio_dispose(GObject *gobject);
void ags_play_pattern_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_play_pattern_audio
 * @short_description: play audio pattern
 * @title: AgsPlayPatternAudio
 * @section_id:
 * @include: ags/audio/recall/ags_play_pattern_audio.h
 *
 * The #AgsPlayPatternAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_BANK_INDEX_0,
  PROP_BANK_INDEX_1,
};

static gpointer ags_play_pattern_audio_parent_class = NULL;

static const gchar *ags_play_pattern_audio_plugin_name = "ags-play-pattern";
static const gchar *ags_play_pattern_audio_specifier[] = {
  "./bank-index-0[0]",
  "./bank-index-1[0]"  
};
static const gchar *ags_play_pattern_audio_control_port[] = {
  "1/2",
  "2/2"
};

GType
ags_play_pattern_audio_get_type()
{
  static GType ags_type_play_pattern_audio = 0;

  if(!ags_type_play_pattern_audio){
    static const GTypeInfo ags_play_pattern_audio_info = {
      sizeof (AgsPlayPatternAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_pattern_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayPatternAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_pattern_audio_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_pattern_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_pattern_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							 "AgsPlayPatternAudio",
							 &ags_play_pattern_audio_info,
							 0);

    g_type_add_interface_static(ags_type_play_pattern_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_play_pattern_audio);
}

void
ags_play_pattern_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_play_pattern_audio_set_ports;
}

void
ags_play_pattern_audio_class_init(AgsPlayPatternAudioClass *play_pattern_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_pattern_audio_parent_class = g_type_class_peek_parent(play_pattern_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_pattern_audio;

  gobject->set_property = ags_play_pattern_audio_set_property;
  gobject->get_property = ags_play_pattern_audio_get_property;

  gobject->dispose = ags_play_pattern_audio_dispose;
  gobject->finalize = ags_play_pattern_audio_finalize;

  /* properties */
  /**
   * AgsPlayPatternAudio:bank-index-0:
   *
   * The bank index 0 port.
   * 
   * Since: 0.7.122.9
   */
  param_spec = g_param_spec_object("bank-index-0",
				   "current bank index 0",
				   "The current bank index 0 of the AgsPattern",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK_INDEX_0,
				  param_spec);

  /**
   * AgsPlayPatternAudio:bank-index-1:
   *
   * The bank index 1 port.
   * 
   * Since: 0.7.122.9
   */
  param_spec = g_param_spec_object("bank-index-1",
				   "current bank index 1",
				   "The current bank index 1 of the AgsPattern",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK_INDEX_1,
				  param_spec);
}

void
ags_play_pattern_audio_init(AgsPlayPatternAudio *play_pattern_audio)
{
  GList *port;

  AGS_RECALL(play_pattern_audio)->name = "ags-play-pattern";
  AGS_RECALL(play_pattern_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_pattern_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_pattern_audio)->xml_type = "ags-play-pattern-audio";

  port = NULL;

  /* bank index 0 */
  play_pattern_audio->bank_index_0 = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_play_pattern_audio_plugin_name,
						  "specifier", ags_play_pattern_audio_specifier[0],
						  "control-port", ags_play_pattern_audio_control_port[0],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_UINT64,
						  "port-value-size", sizeof(guint),
						  "port-value-length", 1,
						  NULL);
  g_object_ref(play_pattern_audio->bank_index_0);

  play_pattern_audio->bank_index_0->port_value.ags_port_uint = 0;

  /* add to port */
  port = g_list_prepend(port, play_pattern_audio->bank_index_0);
  g_object_ref(play_pattern_audio->bank_index_0);

  /* bank index 1 */
  play_pattern_audio->bank_index_1 = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_play_pattern_audio_plugin_name,
						  "specifier", ags_play_pattern_audio_specifier[1],
						  "control-port", ags_play_pattern_audio_control_port[1],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_UINT64,
						  "port-value-size", sizeof(guint),
						  "port-value-length", 1,
						  NULL);
  g_object_ref(play_pattern_audio->bank_index_1);

  play_pattern_audio->bank_index_1->port_value.ags_port_uint = 0;

  /* add to port */
  port = g_list_prepend(port, play_pattern_audio->bank_index_1);
  g_object_ref(play_pattern_audio->bank_index_1);

  /* set port */
  AGS_RECALL(play_pattern_audio)->port = port;
}

void
ags_play_pattern_audio_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsPlayPatternAudio *play_pattern_audio;

  play_pattern_audio = AGS_PLAY_PATTERN_AUDIO(gobject);

  switch(prop_id){
  case PROP_BANK_INDEX_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == play_pattern_audio->bank_index_0){
	return;
      }

      if(play_pattern_audio->bank_index_0 != NULL){
	g_object_unref(G_OBJECT(play_pattern_audio->bank_index_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_pattern_audio->bank_index_0 = port;
    }
    break;
  case PROP_BANK_INDEX_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == play_pattern_audio->bank_index_1){
	return;
      }

      if(play_pattern_audio->bank_index_1 != NULL){
	g_object_unref(G_OBJECT(play_pattern_audio->bank_index_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_pattern_audio->bank_index_1 = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_pattern_audio_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsPlayPatternAudio *play_pattern_audio;
  
  play_pattern_audio = AGS_PLAY_PATTERN_AUDIO(gobject);

  switch(prop_id){
  case PROP_BANK_INDEX_0:
    {
      g_value_set_object(value, play_pattern_audio->bank_index_0);
    }
    break;
  case PROP_BANK_INDEX_1:
    {
      g_value_set_object(value, play_pattern_audio->bank_index_1);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_pattern_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./bank-index-0[0]",
		16)){
      g_object_set(G_OBJECT(plugin),
		   "bank-index-0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./bank-index-1[0]",
		      16)){
      g_object_set(G_OBJECT(plugin),
		   "bank-index-1", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_play_pattern_audio_dispose(GObject *gobject)
{
  AgsPlayPatternAudio *play_pattern_audio;
  
  play_pattern_audio = AGS_PLAY_PATTERN_AUDIO(gobject);

  /* bank index 0 */
  if(play_pattern_audio->bank_index_0 != NULL){
    g_object_unref(play_pattern_audio->bank_index_0);

    play_pattern_audio->bank_index_0 = NULL;
  }

  /* bank index 1 */
  if(play_pattern_audio->bank_index_1 != NULL){
    g_object_unref(play_pattern_audio->bank_index_1);

    play_pattern_audio->bank_index_1 = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_pattern_audio_parent_class)->dispose(gobject);
}

void
ags_play_pattern_audio_finalize(GObject *gobject)
{
  AgsPlayPatternAudio *play_pattern_audio;
  
  play_pattern_audio = AGS_PLAY_PATTERN_AUDIO(gobject);

  /* bank index 0 */
  if(play_pattern_audio->bank_index_0 != NULL){
    g_object_unref(play_pattern_audio->bank_index_0);
  }

  /* bank index 1 */
  if(play_pattern_audio->bank_index_1 != NULL){
    g_object_unref(play_pattern_audio->bank_index_1);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_pattern_audio_parent_class)->finalize(gobject);
}

/**
 * ags_play_pattern_audio_new:
 * @soundcard: the #GObject defaulting to
 * @tact: the offset
 * @i: bank index 0
 * @j: bank index 1
 *
 * Creates an #AgsPlayPatternAudio
 *
 * Returns: a new #AgsPlayPatternAudio
 *
 * Since: 0.7.122.9
 */
AgsPlayPatternAudio*
ags_play_pattern_audio_new(GObject *soundcard,
			   gdouble tact,
			   guint i, guint j)
{
  AgsPlayPatternAudio *play_pattern_audio;

  GValue *value;
  
  play_pattern_audio = (AgsPlayPatternAudio *) g_object_new(AGS_TYPE_PLAY_PATTERN_AUDIO,
							    "soundcard", soundcard,
							    "tact", tact,
							    NULL);

  value = g_new0(GValue,
		 1);
  g_value_init(value, G_TYPE_UINT);

  g_value_set_uint(value, i);
  ags_port_safe_write(copy_pattern_audio->bank_index_0,
		      value);

  g_value_set_uint(value, j);
  ags_port_safe_write(copy_pattern_audio->bank_index_1,
		      value);

  g_value_unset(value);
  g_free(value);
  
  return(play_pattern_audio);
}
