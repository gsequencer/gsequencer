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

#include <ags/audio/recall/ags_envelope_channel.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/i18n.h>

void ags_envelope_channel_class_init(AgsEnvelopeChannelClass *envelope_channel);
void ags_envelope_channel_connectable_interface_init(AgsConnectableInterface *connectable);
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
void ags_envelope_channel_connect(AgsConnectable *connectable);
void ags_envelope_channel_disconnect(AgsConnectable *connectable);
void ags_envelope_channel_set_ports(AgsPlugin *plugin, GList *port);
void ags_envelope_channel_dispose(GObject *gobject);
void ags_envelope_channel_finalize(GObject *gobject);

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
static AgsConnectableInterface *ags_envelope_channel_parent_connectable_interface;

GType
ags_envelope_channel_get_type()
{
  static GType ags_type_envelope_channel = 0;

  if(!ags_type_envelope_channel){
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
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
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_envelope_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_envelope_channel);
}

void
ags_envelope_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_envelope_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_envelope_channel_connect;
  connectable->disconnect = ags_envelope_channel_disconnect;
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
   * Since: 0.8.5
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
   * Since: 0.8.5
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
   * Since: 0.8.5
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
   * Since: 0.7.122.7
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
   * Since: 0.7.122.7
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
   * Since: 0.7.122.7
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
   * Since: 0.7.122.7
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
   * Since: 0.7.122.7
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
						   "plugin-name", "ags-envelope",
						   "specifier", "./use-note-length[0]",
						   "control-port", "1/8",
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_BOOLEAN,
						   "port-value-size", sizeof(gboolean),
						   "port-value-length", 1,
						   NULL);
  g_object_ref(envelope_channel->use_note_length);
  
  envelope_channel->use_note_length->port_value.ags_port_boolean = FALSE;

  /* use fixed length */
  envelope_channel->use_fixed_length = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", "ags-envelope",
						    "specifier", "./use-fixed-length[0]",
						    "control-port", "2/8",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_BOOLEAN,
						    "port-value-size", sizeof(gboolean),
						    "port-value-length", 1,
						    NULL);
  g_object_ref(envelope_channel->use_fixed_length);
  
  envelope_channel->use_fixed_length->port_value.ags_port_boolean = FALSE;

  /* fixed length */
  envelope_channel->fixed_length = g_object_new(AGS_TYPE_PORT,
						"plugin-name", "ags-envelope",
						"specifier", "./fixed-length[0]",
						"control-port", "3/8",
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_UINT,
						"port-value-size", sizeof(guint),
						"port-value-length", 1,
						NULL);
  g_object_ref(envelope_channel->fixed_length);
  
  envelope_channel->fixed_length->port_value.ags_port_uint = 0;

  /* attack */
  envelope_channel->attack = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", "ags-envelope",
					  "specifier", "./attack[0]",
					  "control-port", "4/8",
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
					 "plugin-name", "ags-envelope",
					 "specifier", "./decay[0]",
					 "control-port", "5/8",
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
					   "plugin-name", "ags-envelope",
					   "specifier", "./sustain[0]",
					   "control-port", "6/8",
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
					   "plugin-name", "ags-envelope",
					   "specifier", "./release[0]",
					   "control-port", "7/8",
					   "port-value-is-pointer", FALSE,
					   "port-value-type", G_TYPE_DOUBLE,
					   "port-value-size", sizeof(gdouble),
					   "port-value-length", 1,
					   NULL);
  g_object_ref(envelope_channel->release);
  
  envelope_channel->release->port_value.ags_port_boolean = 1.0;
  
  /* add to port */
  port = g_list_prepend(port, envelope_channel->release);
  g_object_ref(envelope_channel->release);

  /* ratio */  
  envelope_channel->ratio = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", "ags-envelope",
					 "specifier", "./ratio[0]",
					 "control-port", "8/8",
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_DOUBLE,
					 "port-value-size", sizeof(gdouble),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(envelope_channel->ratio);
  
  envelope_channel->ratio->port_value.ags_port_boolean = 1.0;

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

  envelope_channel = AGS_ENVELOPE_CHANNEL(gobject);

  switch(prop_id){
  case PROP_USE_NOTE_LENGTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == envelope_channel->use_note_length){
	return;
      }

      if(envelope_channel->use_note_length != NULL){
	g_object_unref(G_OBJECT(envelope_channel->use_note_length));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->use_note_length = port;
    }
    break;
  case PROP_USE_FIXED_LENGTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == envelope_channel->use_fixed_length){
	return;
      }

      if(envelope_channel->use_fixed_length != NULL){
	g_object_unref(G_OBJECT(envelope_channel->use_fixed_length));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->use_fixed_length = port;
    }
    break;
  case PROP_FIXED_LENGTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == envelope_channel->fixed_length){
	return;
      }

      if(envelope_channel->fixed_length != NULL){
	g_object_unref(G_OBJECT(envelope_channel->fixed_length));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->fixed_length = port;
    }
    break;
  case PROP_ATTACK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == envelope_channel->attack){
	return;
      }

      if(envelope_channel->attack != NULL){
	g_object_unref(G_OBJECT(envelope_channel->attack));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->attack = port;
    }
    break;
  case PROP_DECAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == envelope_channel->decay){
	return;
      }

      if(envelope_channel->decay != NULL){
	g_object_unref(G_OBJECT(envelope_channel->decay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->decay = port;
    }
    break;
  case PROP_SUSTAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == envelope_channel->sustain){
	return;
      }

      if(envelope_channel->sustain != NULL){
	g_object_unref(G_OBJECT(envelope_channel->sustain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->sustain = port;
    }
    break;
  case PROP_RELEASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == envelope_channel->release){
	return;
      }

      if(envelope_channel->release != NULL){
	g_object_unref(G_OBJECT(envelope_channel->release));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->release = port;
    }
    break;
  case PROP_RATIO:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == envelope_channel->ratio){
	return;
      }

      if(envelope_channel->ratio != NULL){
	g_object_unref(G_OBJECT(envelope_channel->ratio));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      envelope_channel->ratio = port;
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

  envelope_channel = AGS_ENVELOPE_CHANNEL(gobject);

  switch(prop_id){
  case PROP_USE_NOTE_LENGTH:
    {
      g_value_set_object(value, envelope_channel->use_note_length);
    }
    break;
  case PROP_USE_FIXED_LENGTH:
    {
      g_value_set_object(value, envelope_channel->use_fixed_length);
    }
    break;
  case PROP_FIXED_LENGTH:
    {
      g_value_set_object(value, envelope_channel->fixed_length);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_object(value, envelope_channel->attack);
    }
    break;
  case PROP_DECAY:
    {
      g_value_set_object(value, envelope_channel->decay);
    }
    break;
  case PROP_SUSTAIN:
    {
      g_value_set_object(value, envelope_channel->sustain);
    }
    break;
  case PROP_RELEASE:
    {
      g_value_set_object(value, envelope_channel->release);
    }
    break;
  case PROP_RATIO:
    {
      g_value_set_object(value, envelope_channel->ratio);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_envelope_channel_connect(AgsConnectable *connectable)
{
  ags_envelope_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_envelope_channel_disconnect(AgsConnectable *connectable)
{
  ags_envelope_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
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
		9)){
      g_object_set(G_OBJECT(plugin),
		   "decay", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"sustain[0]",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "sustain", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"release[0]",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "release", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"ratio[0]",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "ratio", AGS_PORT(port->data),
		   NULL);
    }


    port = port->next;
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

/**
 * ags_envelope_channel_new:
 *
 * Creates an #AgsEnvelopeChannel
 *
 * Returns: a new #AgsEnvelopeChannel
 *
 * Since: 0.6
 */
AgsEnvelopeChannel*
ags_envelope_channel_new()
{
  AgsEnvelopeChannel *envelope_channel;

  envelope_channel = (AgsEnvelopeChannel *) g_object_new(AGS_TYPE_ENVELOPE_CHANNEL,
							 NULL);

  return(envelope_channel);
}
