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

#include <ags/audio/recall/ags_mute_audio.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_mutable.h>
#include <ags/object/ags_plugin.h>

#include <ags/plugin/ags_base_plugin.h>

#include <ags/i18n.h>

void ags_mute_audio_class_init(AgsMuteAudioClass *mute_audio);
void ags_mute_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mute_audio_mutable_interface_init(AgsMutableInterface *mutable);
void ags_mute_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_mute_audio_init(AgsMuteAudio *mute_audio);
void ags_mute_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_mute_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_mute_audio_connect(AgsConnectable *connectable);
void ags_mute_audio_disconnect(AgsConnectable *connectable);
void ags_mute_audio_set_ports(AgsPlugin *plugin, GList *port);
void ags_mute_audio_dispose(GObject *gobject);
void ags_mute_audio_finalize(GObject *gobject);

void ags_mute_audio_set_muted(AgsMutable *mutable, gboolean muted);

static AgsPortDescriptor* ags_mute_audio_get_muted_port_descriptor();

/**
 * SECTION:ags_mute_audio
 * @short_description: mute audio 
 * @title: AgsMuteAudio
 * @section_id:
 * @include: ags/audio/recall/ags_mute_audio.h
 *
 * The #AgsMuteAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_MUTED,
};

static gpointer ags_mute_audio_parent_class = NULL;
static AgsConnectableInterface *ags_mute_audio_parent_connectable_interface;
static AgsMutableInterface *ags_mute_audio_parent_mutable_interface;

GType
ags_mute_audio_get_type()
{
  static GType ags_type_mute_audio = 0;

  if(!ags_type_mute_audio){
    static const GTypeInfo ags_mute_audio_info = {
      sizeof (AgsMuteAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mute_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsMuteAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mute_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mute_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_mute_audio_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_mute_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_mute_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						 "AgsMuteAudio",
						 &ags_mute_audio_info,
						 0);

    g_type_add_interface_static(ags_type_mute_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_mute_audio,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_type_add_interface_static(ags_type_mute_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_mute_audio);
}

void
ags_mute_audio_class_init(AgsMuteAudioClass *mute_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_mute_audio_parent_class = g_type_class_peek_parent(mute_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) mute_audio;

  gobject->set_property = ags_mute_audio_set_property;
  gobject->get_property = ags_mute_audio_get_property;

  gobject->dispose = ags_mute_audio_dispose;
  gobject->finalize = ags_mute_audio_finalize;

  /* properties */
  /**
   * AgsMuteAudio:muted:
   *
   * The mute port.
   * 
   * Since: 1.0.0.7
   */
  param_spec = g_param_spec_object("muted",
				   i18n_pspec("mute audio"),
				   i18n_pspec("Mute the audio"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MUTED,
				  param_spec);
}

void
ags_mute_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_mute_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_mute_audio_connect;
  connectable->disconnect = ags_mute_audio_disconnect;
}

void
ags_mute_audio_mutable_interface_init(AgsMutableInterface *mutable)
{
  ags_mute_audio_parent_mutable_interface = g_type_interface_peek_parent(mutable);

  mutable->set_muted = ags_mute_audio_set_muted;
}

void
ags_mute_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_mute_audio_set_ports;
}

void
ags_mute_audio_init(AgsMuteAudio *mute_audio)
{
  GList *port;

  AGS_RECALL(mute_audio)->name = "ags-mute";
  AGS_RECALL(mute_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(mute_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(mute_audio)->xml_type = "ags-mute-audio";

  port = NULL;

  /* muted */
  mute_audio->muted = g_object_new(AGS_TYPE_PORT,
				   "plugin-name", "ags-mute",
				   "specifier", "./muted[0]",
				   "control-port", "1/1",
				   "port-value-is-pointer", FALSE,
				   "port-value-type", G_TYPE_FLOAT,
				   "port-value-size", sizeof(gfloat),
				   "port-value-length", 1,
				   NULL);
  g_object_ref(mute_audio->muted);

  mute_audio->muted->port_value.ags_port_float = (float) FALSE;

  /* port descriptor */
  mute_audio->muted->port_descriptor = ags_mute_audio_get_muted_port_descriptor();

  /* add to port */
  port = g_list_prepend(port, mute_audio->muted);
  g_object_ref(mute_audio->muted);

  /* set port */
  AGS_RECALL(mute_audio)->port = port;
}


void
ags_mute_audio_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsMuteAudio *mute_audio;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  switch(prop_id){
  case PROP_MUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == mute_audio->muted){
	return;
      }

      if(mute_audio->muted != NULL){
	g_object_unref(G_OBJECT(mute_audio->muted));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      mute_audio->muted = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_mute_audio_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsMuteAudio *mute_audio;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  switch(prop_id){
  case PROP_MUTED:
    {
      g_value_set_object(value, mute_audio->muted);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_mute_audio_connect(AgsConnectable *connectable)
{
  AgsRecall *recall;
  
  recall = AGS_RECALL(connectable);
  
  if((AGS_RECALL_CONNECTED & (recall->flags)) != 0){
    return;
  }

  /* load automation */
  ags_recall_load_automation(recall,
			     g_list_copy(recall->port));

  /* call parent */
  ags_mute_audio_parent_connectable_interface->connect(connectable);
}

void
ags_mute_audio_disconnect(AgsConnectable *connectable)
{
  ags_mute_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_mute_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"muted[0]",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "muted", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_mute_audio_dispose(GObject *gobject)
{
  AgsMuteAudio *mute_audio;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  /* muted */
  if(mute_audio->muted != NULL){
    g_object_unref(G_OBJECT(mute_audio->muted));

    mute_audio->muted = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_mute_audio_parent_class)->dispose(gobject);
}

void
ags_mute_audio_finalize(GObject *gobject)
{
  AgsMuteAudio *mute_audio;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  /* muted */
  if(mute_audio->muted != NULL){
    g_object_unref(G_OBJECT(mute_audio->muted));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_mute_audio_parent_class)->finalize(gobject);
}

void
ags_mute_audio_set_muted(AgsMutable *mutable, gboolean muted)
{
  GValue value = {0,};

  g_value_init(&value, G_TYPE_FLOAT);
  g_value_set_float(&value, (muted ? 1.0: 0.0));

  ags_port_safe_write(AGS_MUTE_AUDIO(mutable)->muted, &value);
}

static AgsPortDescriptor*
ags_mute_audio_get_muted_port_descriptor()
{
  static AgsPortDescriptor *port_descriptor = NULL;

  if(port_descriptor == NULL){
    port_descriptor = ags_port_descriptor_alloc();

    port_descriptor->flags |= (AGS_PORT_DESCRIPTOR_INPUT |
			       AGS_PORT_DESCRIPTOR_CONTROL |
			       AGS_PORT_DESCRIPTOR_TOGGLED);

    port_descriptor->port_index = 0;

    /* range */
    g_value_init(port_descriptor->default_value,
		 G_TYPE_FLOAT);
    g_value_init(port_descriptor->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(port_descriptor->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(port_descriptor->default_value,
		      0.0);
    g_value_set_float(port_descriptor->lower_value,
		      0.0);
    g_value_set_float(port_descriptor->upper_value,
		      1.0);
  }
  
  return(port_descriptor);
}

/**
 * ags_mute_audio_new:
 *
 * Creates an #AgsMuteAudio
 *
 * Returns: a new #AgsMuteAudio
 *
 * Since: 1.0.0
 */
AgsMuteAudio*
ags_mute_audio_new()
{
  AgsMuteAudio *mute_audio;

  mute_audio = (AgsMuteAudio *) g_object_new(AGS_TYPE_MUTE_AUDIO,
					     NULL);

  return(mute_audio);
}
