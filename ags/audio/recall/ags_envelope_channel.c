/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/recall/ags_envelope_channel.h>

#include <ags/i18n.h>

void ags_envelope_channel_class_init(AgsEnvelopeChannelClass *envelope_channel);
void ags_envelope_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_envelope_channel_init(AgsEnvelopeChannel *envelope_channel);
void ags_envelope_channel_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_envelope_channel_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_envelope_channel_dispose(GObject *gobject);
void ags_envelope_channel_finalize(GObject *gobject);

void ags_envelope_channel_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_envelope_channel
 * @short_description: envelopes channel
 * @title: AgsEnvelopeChannel
 * @section_id:
 * @include: ags/audio/recall/ags_envelope_channel.h
 *
 * The #AgsEnvelopeChannel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_USE_NOTE_LENGTH,
  PROP_USE_FIXED_LENGTH,
  PROP_FIXED_LENGTH,
  PROP_ATTACK,
  PROP_DECAY,
  PROP_SUSTAIN,
  PROP_RELEASE,
  PROP_RATIO,
};

static gpointer ags_envelope_channel_parent_class = NULL;

static const gchar *ags_envelope_channel_plugin_name = "ags-envelope";
static const gchar *ags_envelope_channel_specifier[] = {
  "./use-note-length[0]",
  "./use-fixed-length[0]",
  "./fixed-length[0]",
  "./attack[0]",
  "./decay[0]",
  "./sustain[0]",
  "./release[0]",
  "./ratio[0]"
};

static const gchar *ags_envelope_channel_control_port[] = {
  "1/8",
  "2/8",
  "3/8",
  "4/8",
  "5/8",
  "6/8",
  "7/8",
  "8/8",
};

GType
ags_envelope_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_envelope_channel = 0;

    static const GTypeInfo ags_envelope_channel_info = {
      sizeof (AgsEnvelopeChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_envelope_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEnvelopeChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_envelope_channel_init,
    };
    
    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_envelope_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_envelope_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						       "AgsEnvelopeChannel",
						       &ags_envelope_channel_info,
						       0);

    g_type_add_interface_static(ags_type_envelope_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_envelope_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_envelope_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_envelope_channel_set_ports;
}

void
ags_envelope_channel_class_init(AgsEnvelopeChannelClass *envelope_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_envelope_channel_parent_class = g_type_class_peek_parent(envelope_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) envelope_channel;

  gobject->set_property = ags_envelope_channel_set_property;
  gobject->get_property = ags_envelope_channel_get_property;

  gobject->dispose = ags_envelope_channel_dispose;
  gobject->finalize = ags_envelope_channel_finalize;

  /* properties */
  /**
   * AgsEnvelopeChannel:use-note-length:
   *
   * Use note length to compute envelope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("use-note-length",
				   i18n_pspec("use note length"),
				   i18n_pspec("Use note length to compute envelope"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_USE_NOTE_LENGTH,
				  param_spec);

  /**
   * AgsEnvelopeChannel:use-fixed-length:
   *
   * Use fixed length to compute envelope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("use-fixed-length",
				   i18n_pspec("use fixed length"),
				   i18n_pspec("Use fixed length to compute envelope"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_USE_FIXED_LENGTH,
				  param_spec);

  /**
   * AgsEnvelopeChannel:fixed-length:
   *
   * The fixed length to compute envelope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("fixed-length",
				   i18n_pspec("fixed length"),
				   i18n_pspec("The fixed length to compute envelope"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIXED_LENGTH,
				  param_spec);
  
  /**
   * AgsEnvelopeChannel:attack:
   *
   * The attack of envelope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("attack",
				   i18n_pspec("attack channel"),
				   i18n_pspec("Attack of the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /**
   * AgsEnvelopeChannel:decay:
   *
   * The decay of envelope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("decay",
				   i18n_pspec("decay channel"),
				   i18n_pspec("Decay of the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DECAY,
				  param_spec);

  /**
   * AgsEnvelopeChannel:sustain:
   *
   * The sustain of envelope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("sustain",
				   i18n_pspec("sustain channel"),
				   i18n_pspec("Sustain of the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SUSTAIN,
				  param_spec);

  /**
   * AgsEnvelopeChannel:release:
   *
   * The release of envelope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("release",
				   i18n_pspec("release channel"),
				   i18n_pspec("Release of the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RELEASE,
				  param_spec);

  /**
   * AgsEnvelopeChannel:ratio:
   *
   * The ratio of envelope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("ratio",
				   i18n_pspec("envelope ratio"),
				   i18n_pspec("The ratio of the envelope"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RATIO,
				  param_spec);
}

void
ags_envelope_channel_init(AgsEnvelopeChannel *envelope_channel)
{
  GList *port;

  AGS_RECALL(envelope_channel)->name = "ags-envelope";
  AGS_RECALL(envelope_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(envelope_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(envelope_channel)->xml_type = "ags-envelope-channel";

  /* ports */
  port = NULL;
  
  /* use note length */
  envelope_channel->use_note_length = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_envelope_channel_plugin_name,
						   "specifier", ags_envelope_channel_specifier[0],
						   "control-port", ags_envelope_channel_control_port[0],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_BOOLEAN,
						   "port-value-size", sizeof(gboolean),
						   "port-value-length", 1,
						   NULL);
  g_object_ref(envelope_channel->use_note_length);
  
  envelope_channel->use_note_length->port_value.ags_port_boolean = FALSE;

  /* use fixed length */
  envelope_channel->use_fixed_length = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_envelope_channel_plugin_name,
						    "specifier", ags_envelope_channel_specifier[1],
						    "control-port", ags_envelope_channel_control_port[1],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_BOOLEAN,
						    "port-value-size", sizeof(gboolean),
						    "port-value-length", 1,
						    NULL);
  g_object_ref(envelope_channel->use_fixed_length);
  
  envelope_channel->use_fixed_length->port_value.ags_port_boolean = FALSE;

  /* fixed length */
  envelope_channel->fixed_length = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_envelope_channel_plugin_name,
						"specifier", ags_envelope_channel_specifier[2],
						"control-port", ags_envelope_channel_control_port[2],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_DOUBLE,
						"port-value-size", sizeof(gdouble),
						"port-value-length", 1,
						NULL);
  g_object_ref(envelope_channel->fixed_length);
  
  envelope_channel->fixed_length->port_value.ags_port_double = 0.0;

  /* attack */
  envelope_channel->attack = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", ags_envelope_channel_plugin_name,
					  "specifier", ags_envelope_channel_specifier[3],
					  "control-port", ags_envelope_channel_control_port[3],
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_DOUBLE,
					  "port-value-size", sizeof(gdouble),
					  "port-value-length", 1,
					  NULL);
  g_object_ref(envelope_channel->attack);
  
  envelope_channel->attack->port_value.ags_port_double = 1.0;

  /* add to port */
  port = g_list_prepend(port, envelope_channel->attack);
  g_object_ref(envelope_channel->attack);

  /* decay */
  envelope_channel->decay = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_envelope_channel_plugin_name,
					 "specifier", ags_envelope_channel_specifier[4],
					 "control-port", ags_envelope_channel_control_port[4],
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_DOUBLE,
					 "port-value-size", sizeof(gdouble),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(envelope_channel->decay);

  envelope_channel->decay->port_value.ags_port_double = 1.0;

  /* add to port */
  port = g_list_prepend(port, envelope_channel->decay);
  g_object_ref(envelope_channel->decay);

  /* sustain */
  envelope_channel->sustain = g_object_new(AGS_TYPE_PORT,
					   "plugin-name", ags_envelope_channel_plugin_name,
					   "specifier", ags_envelope_channel_specifier[5],
					   "control-port", ags_envelope_channel_control_port[5],
					   "port-value-is-pointer", FALSE,
					   "port-value-type", G_TYPE_DOUBLE,
					   "port-value-size", sizeof(gdouble),
					   "port-value-length", 1,
					   NULL);
  g_object_ref(envelope_channel->sustain);

  envelope_channel->sustain->port_value.ags_port_double = 1.0;
  
  /* add to port */
  port = g_list_prepend(port, envelope_channel->sustain);
  g_object_ref(envelope_channel->sustain);

  /* release */
  envelope_channel->release = g_object_new(AGS_TYPE_PORT,
					   "plugin-name", ags_envelope_channel_plugin_name,
					   "specifier", ags_envelope_channel_specifier[6],
					   "control-port", ags_envelope_channel_control_port[6],
					   "port-value-is-pointer", FALSE,
					   "port-value-type", G_TYPE_DOUBLE,
					   "port-value-size", sizeof(gdouble),
					   "port-value-length", 1,
					   NULL);
  g_object_ref(envelope_channel->release);
  
  envelope_channel->release->port_value.ags_port_double = 1.0;
  
  /* add to port */
  port = g_list_prepend(port, envelope_channel->release);
  g_object_ref(envelope_channel->release);

  /* ratio */  
  envelope_channel->ratio = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_envelope_channel_plugin_name,
					 "specifier", ags_envelope_channel_specifier[7],
					 "control-port", ags_envelope_channel_control_port[7],
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_DOUBLE,
					 "port-value-size", sizeof(gdouble),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(envelope_channel->ratio);
  
  envelope_channel->ratio->port_value.ags_port_double = 1.0;

  /* add to port */
  port = g_list_prepend(port, envelope_channel->ratio);
  g_object_ref(envelope_channel->ratio);

  /* set ports */
  AGS_RECALL(envelope_channel)->port = port;
}

void
ags_envelope_channel_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsEnvelopeChannel *envelope_channel;

  GRecMutex *recall_mutex;

  envelope_channel = AGS_ENVELOPE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(envelope_channel);

  switch(prop_id){
  case PROP_USE_NOTE_LENGTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == envelope_channel->use_note_length){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(envelope_channel->use_note_length != NULL){
	g_object_unref(G_OBJECT(envelope_channel->use_note_length));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->use_note_length = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_USE_FIXED_LENGTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == envelope_channel->use_fixed_length){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(envelope_channel->use_fixed_length != NULL){
	g_object_unref(G_OBJECT(envelope_channel->use_fixed_length));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->use_fixed_length = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FIXED_LENGTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == envelope_channel->fixed_length){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(envelope_channel->fixed_length != NULL){
	g_object_unref(G_OBJECT(envelope_channel->fixed_length));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->fixed_length = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == envelope_channel->attack){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(envelope_channel->attack != NULL){
	g_object_unref(G_OBJECT(envelope_channel->attack));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->attack = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DECAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == envelope_channel->decay){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(envelope_channel->decay != NULL){
	g_object_unref(G_OBJECT(envelope_channel->decay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->decay = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SUSTAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == envelope_channel->sustain){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(envelope_channel->sustain != NULL){
	g_object_unref(G_OBJECT(envelope_channel->sustain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->sustain = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RELEASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == envelope_channel->release){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(envelope_channel->release != NULL){
	g_object_unref(G_OBJECT(envelope_channel->release));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->release = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RATIO:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == envelope_channel->ratio){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(envelope_channel->ratio != NULL){
	g_object_unref(G_OBJECT(envelope_channel->ratio));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->ratio = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_envelope_channel_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsEnvelopeChannel *envelope_channel;

  GRecMutex *recall_mutex;

  envelope_channel = AGS_ENVELOPE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(envelope_channel);

  switch(prop_id){
  case PROP_USE_NOTE_LENGTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, envelope_channel->use_note_length);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_USE_FIXED_LENGTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, envelope_channel->use_fixed_length);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FIXED_LENGTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, envelope_channel->fixed_length);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, envelope_channel->attack);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DECAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, envelope_channel->decay);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SUSTAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, envelope_channel->sustain);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RELEASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, envelope_channel->release);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RATIO:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, envelope_channel->ratio);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_envelope_channel_dispose(GObject *gobject)
{
  AgsEnvelopeChannel *envelope_channel;

  envelope_channel = AGS_ENVELOPE_CHANNEL(gobject);

  if(envelope_channel->use_note_length != NULL){
    g_object_unref(G_OBJECT(envelope_channel->use_note_length));

    envelope_channel->use_note_length = NULL;
  }

  if(envelope_channel->use_fixed_length != NULL){
    g_object_unref(G_OBJECT(envelope_channel->use_fixed_length));

    envelope_channel->use_fixed_length = NULL;
  }

  if(envelope_channel->fixed_length != NULL){
    g_object_unref(G_OBJECT(envelope_channel->fixed_length));

    envelope_channel->fixed_length = NULL;
  }

  if(envelope_channel->attack != NULL){
    g_object_unref(G_OBJECT(envelope_channel->attack));

    envelope_channel->attack = NULL;
  }

  if(envelope_channel->decay != NULL){
    g_object_unref(G_OBJECT(envelope_channel->decay));

    envelope_channel->decay = NULL;
  }

  if(envelope_channel->sustain != NULL){
    g_object_unref(G_OBJECT(envelope_channel->sustain));

    envelope_channel->sustain = NULL;
  }

  if(envelope_channel->release != NULL){
    g_object_unref(G_OBJECT(envelope_channel->release));

    envelope_channel->release = NULL;
  }

  if(envelope_channel->ratio != NULL){
    g_object_unref(G_OBJECT(envelope_channel->ratio));

    envelope_channel->ratio = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_envelope_channel_parent_class)->dispose(gobject);
}

void
ags_envelope_channel_finalize(GObject *gobject)
{
  AgsEnvelopeChannel *envelope_channel;

  envelope_channel = AGS_ENVELOPE_CHANNEL(gobject);

  if(envelope_channel->use_note_length != NULL){
    g_object_unref(G_OBJECT(envelope_channel->use_note_length));
  }

  if(envelope_channel->use_fixed_length != NULL){
    g_object_unref(G_OBJECT(envelope_channel->use_fixed_length));
  }

  if(envelope_channel->fixed_length != NULL){
    g_object_unref(G_OBJECT(envelope_channel->fixed_length));
  }

  if(envelope_channel->attack != NULL){
    g_object_unref(G_OBJECT(envelope_channel->attack));
  }

  if(envelope_channel->decay != NULL){
    g_object_unref(G_OBJECT(envelope_channel->decay));
  }

  if(envelope_channel->sustain != NULL){
    g_object_unref(G_OBJECT(envelope_channel->sustain));
  }

  if(envelope_channel->release != NULL){
    g_object_unref(G_OBJECT(envelope_channel->release));
  }

  if(envelope_channel->ratio != NULL){
    g_object_unref(G_OBJECT(envelope_channel->ratio));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_envelope_channel_parent_class)->finalize(gobject);
}


void
ags_envelope_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"attack[0]",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "attack", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"decay[0]",
		8)){
      g_object_set(G_OBJECT(plugin),
		   "decay", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"sustain[0]",
		10)){
      g_object_set(G_OBJECT(plugin),
		   "sustain", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"release[0]",
		10)){
      g_object_set(G_OBJECT(plugin),
		   "release", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"ratio[0]",
		8)){
      g_object_set(G_OBJECT(plugin),
		   "ratio", AGS_PORT(port->data),
		   NULL);
    }


    port = port->next;
  }
}

/**
 * ags_envelope_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsEnvelopeChannel
 *
 * Returns: the new #AgsEnvelopeChannel
 *
 * Since: 3.0.0
 */
AgsEnvelopeChannel*
ags_envelope_channel_new(AgsChannel *source)
{
  AgsEnvelopeChannel *envelope_channel;

  envelope_channel = (AgsEnvelopeChannel *) g_object_new(AGS_TYPE_ENVELOPE_CHANNEL,
							 "source", source,
							 NULL);

  return(envelope_channel);
}
