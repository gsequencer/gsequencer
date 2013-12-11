/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/main.h>

#include <ags/object/ags_plugin.h>

void ags_copy_pattern_audio_class_init(AgsCopyPatternAudioClass *copy_pattern_audio);
void ags_copy_pattern_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_copy_pattern_audio_init(AgsCopyPatternAudio *copy_pattern_audio);
void ags_copy_pattern_audio_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_copy_pattern_audio_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_copy_pattern_audio_set_ports(AgsPlugin *plugin, GList *port);
void ags_copy_pattern_audio_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_BANK_INDEX_0,
  PROP_BANK_INDEX_1,
};

static gpointer ags_copy_pattern_audio_parent_class = NULL;

static const gchar *ags_copy_pattern_audio_plugin_name = "ags-copy-pattern\0";
static const gchar *ags_copy_pattern_audio_specifier[] = {
  "./bank-index-0[0]\0",
  "./bank-index-1[0]\0"  
};
static const gchar *ags_copy_pattern_audio_control_port[] = {
  "1/2\0",
  "2/2\0"
};

GType
ags_copy_pattern_audio_get_type()
{
  static GType ags_type_copy_pattern_audio = 0;

  if(!ags_type_copy_pattern_audio){
    static const GTypeInfo ags_copy_pattern_audio_info = {
      sizeof (AgsCopyPatternAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_audio_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_pattern_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							 "AgsCopyPatternAudio\0",
							 &ags_copy_pattern_audio_info,
							 0);

    g_type_add_interface_static(ags_type_copy_pattern_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_copy_pattern_audio);
}

void
ags_copy_pattern_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_copy_pattern_audio_set_ports;
}

void
ags_copy_pattern_audio_class_init(AgsCopyPatternAudioClass *copy_pattern_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_copy_pattern_audio_parent_class = g_type_class_peek_parent(copy_pattern_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_pattern_audio;

  gobject->set_property = ags_copy_pattern_audio_set_property;
  gobject->get_property = ags_copy_pattern_audio_get_property;

  gobject->finalize = ags_copy_pattern_audio_finalize;

  /* properties */
  param_spec = g_param_spec_object("bank-index-0\0",
				   "current bank index 0\0",
				   "The current bank index 0 of the AgsPattern\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK_INDEX_0,
				  param_spec);

  param_spec = g_param_spec_object("bank-index-1\0",
				   "current bank index 1\0",
				   "The current bank index 1 of the AgsPattern\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK_INDEX_1,
				  param_spec);
}

void
ags_copy_pattern_audio_init(AgsCopyPatternAudio *copy_pattern_audio)
{
  GList *port;

  AGS_RECALL(copy_pattern_audio)->name = "ags-copy-pattern\0";
  AGS_RECALL(copy_pattern_audio)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(copy_pattern_audio)->build_id = AGS_BUILD_ID;
  AGS_RECALL(copy_pattern_audio)->xml_type = "ags-copy-pattern-audio\0";

  port = NULL;

  copy_pattern_audio->bank_index_0 = g_object_new(AGS_TYPE_PORT,
						  "plugin-name\0", ags_copy_pattern_audio_plugin_name,
						  "specifier\0", ags_copy_pattern_audio_specifier[0],
						  "control-port\0", ags_copy_pattern_audio_control_port[0],
						  "port-value-is-pointer\0", FALSE,
						  "port-value-type\0", G_TYPE_UINT64,
						  "port-value-size\0", sizeof(guint),
						  "port-value-length\0", 1,
						  NULL);
  copy_pattern_audio->bank_index_0->port_value.ags_port_uint = 0;

  port = g_list_prepend(port, copy_pattern_audio->bank_index_0);

  copy_pattern_audio->bank_index_1 = g_object_new(AGS_TYPE_PORT,
						  "plugin-name\0", ags_copy_pattern_audio_plugin_name,
						  "specifier\0", ags_copy_pattern_audio_specifier[1],
						  "control-port\0", ags_copy_pattern_audio_control_port[1],
						  "port-value-is-pointer\0", FALSE,
						  "port-value-type\0", G_TYPE_UINT64,
						  "port-value-size\0", sizeof(guint),
						  "port-value-length\0", 1,
						  NULL);
  copy_pattern_audio->bank_index_1->port_value.ags_port_uint = 0;

  port = g_list_prepend(port, copy_pattern_audio->bank_index_1);

  AGS_RECALL(copy_pattern_audio)->port = port;
}

void
ags_copy_pattern_audio_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsCopyPatternAudio *copy_pattern_audio;

  copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(gobject);

  switch(prop_id){
  case PROP_BANK_INDEX_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == copy_pattern_audio->bank_index_0){
	return;
      }

      if(copy_pattern_audio->bank_index_0 != NULL){
	g_object_unref(G_OBJECT(copy_pattern_audio->bank_index_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      copy_pattern_audio->bank_index_0 = port;
    }
    break;
  case PROP_BANK_INDEX_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == copy_pattern_audio->bank_index_1){
	return;
      }

      if(copy_pattern_audio->bank_index_1 != NULL){
	g_object_unref(G_OBJECT(copy_pattern_audio->bank_index_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      copy_pattern_audio->bank_index_1 = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_audio_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsCopyPatternAudio *copy_pattern_audio;
  
  copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(gobject);

  switch(prop_id){
  case PROP_BANK_INDEX_0:
    {
      g_value_set_object(value, copy_pattern_audio->bank_index_0);
    }
    break;
  case PROP_BANK_INDEX_1:
    {
      g_value_set_object(value, copy_pattern_audio->bank_index_1);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"bank-index-0[0]\0",
		16)){
      g_object_set(G_OBJECT(plugin),
		   "bank-index-0\0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "bank-index-1[0]\0",
		      16)){
      g_object_set(G_OBJECT(plugin),
		   "bank-index-1\0", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_copy_pattern_audio_finalize(GObject *gobject)
{
  AgsCopyPatternAudio *copy_pattern_audio;
  
  copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_copy_pattern_audio_parent_class)->finalize(gobject);
}

AgsCopyPatternAudio*
ags_copy_pattern_audio_new(AgsDevout *devout,
			   gdouble tact,
			   guint i, guint j)
{
  AgsCopyPatternAudio *copy_pattern_audio;

  copy_pattern_audio = (AgsCopyPatternAudio *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO,
							    "devout\0", devout,
							    "tact\0", tact,
							    "bank_index_0\0", i,
							    "bank_index_1\0", j,
							    NULL);

  return(copy_pattern_audio);
}
