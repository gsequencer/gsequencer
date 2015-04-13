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

#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

void ags_copy_pattern_channel_class_init(AgsCopyPatternChannelClass *copy_pattern_channel);
void ags_copy_pattern_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_pattern_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_copy_pattern_channel_init(AgsCopyPatternChannel *copy_pattern_channel);
void ags_copy_pattern_channel_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_copy_pattern_channel_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_copy_pattern_channel_connect(AgsConnectable *connectable);
void ags_copy_pattern_channel_disconnect(AgsConnectable *connectable);
void ags_copy_pattern_channel_set_ports(AgsPlugin *plugin, GList *port);
void ags_copy_pattern_channel_finalize(GObject *gobject);

void ags_copy_pattern_channel_bank_index_0_safe_write_callback(AgsPort *pattern,
							       GValue *value,
							       AgsCopyPatternChannel *copy_pattern_channel);
void ags_copy_pattern_channel_bank_index_1_safe_write_callback(AgsPort *pattern,
							       GValue *value,
							       AgsCopyPatternChannel *copy_pattern_channel);

/**
 * SECTION:ags_copy_pattern_channel
 * @short_description: copy patterns channel
 * @title: AgsCopyPatternChannel
 * @section_id:
 * @include: ags/audio/recall/ags_copy_pattern_channel.h
 *
 * The #AgsCopyPatternChannel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_PATTERN,
  PROP_BANK_INDEX_0,
  PROP_BANK_INDEX_1,
  PROP_CURRENT_BIT,
};

static gpointer ags_copy_pattern_channel_parent_class = NULL;
static AgsConnectableInterface* ags_copy_pattern_channel_parent_connectable_interface;

static const gchar *ags_copy_pattern_channel_plugin_name = "ags-copy-pattern\0";
static const gchar *ags_copy_pattern_channel_specifier[] = {
  "./pattern[0]\0",
  "./bank-index-0[0]\0",
  "./bank-index-1[0]\0",
  "./current-bit[0]\0",
};
static const gchar *ags_copy_pattern_channel_control_port[] = {
  "1/1\0",
  "2/4\0",
  "3/4\0",
  "4/4\0",
};

GType
ags_copy_pattern_channel_get_type()
{
  static GType ags_type_copy_pattern_channel = 0;

  if(!ags_type_copy_pattern_channel){
    static const GTypeInfo ags_copy_pattern_channel_info = {
      sizeof (AgsCopyPatternChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_pattern_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							   "AgsCopyPatternChannel\0",
							   &ags_copy_pattern_channel_info,
							   0);

    g_type_add_interface_static(ags_type_copy_pattern_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_pattern_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_copy_pattern_channel);
}

void
ags_copy_pattern_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_pattern_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_pattern_channel_connect;
  connectable->disconnect = ags_copy_pattern_channel_disconnect;
}

void
ags_copy_pattern_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_copy_pattern_channel_set_ports;
}

void
ags_copy_pattern_channel_class_init(AgsCopyPatternChannelClass *copy_pattern_channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_copy_pattern_channel_parent_class = g_type_class_peek_parent(copy_pattern_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_pattern_channel;

  gobject->set_property = ags_copy_pattern_channel_set_property;
  gobject->get_property = ags_copy_pattern_channel_get_property;

  gobject->finalize = ags_copy_pattern_channel_finalize;

  /* properties */
  param_spec = g_param_spec_object("pattern\0",
				   "pattern to play\0",
				   "The pattern which has to be played\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PATTERN,
				  param_spec);

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

  param_spec = g_param_spec_object("current-bit\0",
				   "current playing bit\0",
				   "The current playing bit is set\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CURRENT_BIT,
				  param_spec);
}

void
ags_copy_pattern_channel_init(AgsCopyPatternChannel *copy_pattern_channel)
{
  GList *port;

  AGS_RECALL(copy_pattern_channel)->name = "ags-copy-pattern\0";
  AGS_RECALL(copy_pattern_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(copy_pattern_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(copy_pattern_channel)->xml_type = "ags-copy-pattern-channel\0";
  
  port = NULL;

  copy_pattern_channel->pattern = g_object_new(AGS_TYPE_PORT,
					       "plugin-name\0", ags_copy_pattern_channel_plugin_name,
					       "specifier\0", ags_copy_pattern_channel_specifier[0],
					       "control-port\0", ags_copy_pattern_channel_control_port[0],
					       "port-value-is-pointer\0", FALSE,
					       "port-value-type\0", G_TYPE_OBJECT,
					       NULL);

  copy_pattern_channel->pattern->port_value.ags_port_object = NULL;

  port = g_list_prepend(port, copy_pattern_channel->pattern);

  copy_pattern_channel->bank_index_0 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name\0", ags_copy_pattern_channel_plugin_name,
						    "specifier\0", ags_copy_pattern_channel_specifier[1],
						    "control-port\0", ags_copy_pattern_channel_control_port[1],
						    "port-value-is-pointer\0", FALSE,
						    "port-value-type\0", G_TYPE_UINT64,
						    "port-value-size\0", sizeof(guint64),
						    "port-value-length\0", 1,
						    NULL);
  copy_pattern_channel->bank_index_0->port_value.ags_port_uint = 0;

  port = g_list_prepend(port, copy_pattern_channel->bank_index_0);

  copy_pattern_channel->bank_index_1 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name\0", ags_copy_pattern_channel_plugin_name,
						    "specifier\0", ags_copy_pattern_channel_specifier[2],
						    "control-port\0", ags_copy_pattern_channel_control_port[2],
						    "port-value-is-pointer\0", FALSE,
						    "port-value-type\0", G_TYPE_UINT64,
						    "port-value-size\0", sizeof(guint64),
						    "port-value-length\0", 1,
						    NULL);
  copy_pattern_channel->bank_index_1->port_value.ags_port_uint = 0;

  port = g_list_prepend(port, copy_pattern_channel->current_bit);

  copy_pattern_channel->current_bit = g_object_new(AGS_TYPE_PORT,
						   "plugin-name\0", ags_copy_pattern_channel_plugin_name,
						   "specifier\0", ags_copy_pattern_channel_specifier[3],
						   "control-port\0", ags_copy_pattern_channel_control_port[3],
						   "port-value-is-pointer\0", FALSE,
						   "port-value-type\0", G_TYPE_BOOLEAN,
						   "port-value-size\0", sizeof(gboolean),
						   "port-value-length\0", 1,
						   NULL);
  copy_pattern_channel->current_bit->port_value.ags_port_boolean = FALSE;

  port = g_list_prepend(port, copy_pattern_channel->current_bit);
  
  AGS_RECALL(copy_pattern_channel)->port = port;
}

void
ags_copy_pattern_channel_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  switch(prop_id){
  case PROP_PATTERN:
    {
      AgsPort *pattern;

      pattern = (AgsPort *) g_value_get_object(value);

      if(copy_pattern_channel->pattern == pattern)
	return;

      if(copy_pattern_channel->pattern != NULL)
	g_object_unref(G_OBJECT(copy_pattern_channel->pattern));
      
      if(pattern != NULL)
	g_object_ref(G_OBJECT(pattern));

      copy_pattern_channel->pattern = pattern;
    }
    break;
  case PROP_BANK_INDEX_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == copy_pattern_channel->bank_index_0){
	return;
      }

      if(copy_pattern_channel->bank_index_0 != NULL){
	g_object_unref(G_OBJECT(copy_pattern_channel->bank_index_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      copy_pattern_channel->bank_index_0 = port;
    }
    break;
  case PROP_BANK_INDEX_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == copy_pattern_channel->bank_index_1){
	return;
      }

      if(copy_pattern_channel->bank_index_1 != NULL){
	g_object_unref(G_OBJECT(copy_pattern_channel->bank_index_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      copy_pattern_channel->bank_index_1 = port;
    }
    break;
  case PROP_CURRENT_BIT:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == copy_pattern_channel->current_bit){
	return;
      }

      if(copy_pattern_channel->current_bit != NULL){
	g_object_unref(G_OBJECT(copy_pattern_channel->current_bit));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      copy_pattern_channel->current_bit = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_channel_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  switch(prop_id){
  case PROP_PATTERN:
    {
      g_value_set_object(value, copy_pattern_channel->pattern);
    }
    break;
  case PROP_BANK_INDEX_0:
    {
      g_value_set_object(value, copy_pattern_channel->bank_index_0);
    }
    break;
  case PROP_BANK_INDEX_1:
    {
      g_value_set_object(value, copy_pattern_channel->bank_index_1);
    }
    break;
  case PROP_CURRENT_BIT:
    {
      g_value_set_object(value, copy_pattern_channel->current_bit);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_copy_pattern_channel_connect(AgsConnectable *connectable)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(connectable);

  if((AGS_RECALL_CONNECTED & (AGS_RECALL(copy_pattern_channel)->flags)) != 0){
    return;
  }

  ags_copy_pattern_channel_parent_connectable_interface->connect(connectable);
  
  g_signal_connect(G_OBJECT(copy_pattern_channel->bank_index_0), "safe-write\0", 
		   G_CALLBACK(ags_copy_pattern_channel_bank_index_0_safe_write_callback), copy_pattern_channel);

  g_signal_connect(G_OBJECT(copy_pattern_channel->bank_index_1), "safe-write\0", 
		   G_CALLBACK(ags_copy_pattern_channel_bank_index_1_safe_write_callback), copy_pattern_channel);
}

void
ags_copy_pattern_channel_disconnect(AgsConnectable *connectable)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(connectable);

  if((AGS_RECALL_CONNECTED & (AGS_RECALL(copy_pattern_channel)->flags)) == 0){
    return;
  }

  //TODO:JK: implement me

  ags_copy_pattern_channel_parent_connectable_interface->disconnect(connectable);
}

void
ags_copy_pattern_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./pattern[0]\0",
		11)){
      g_object_set(G_OBJECT(plugin),
		   "pattern\0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./bank-index-0[0]\0",
		      16)){
      g_object_set(G_OBJECT(plugin),
		   "bank-index-0\0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./bank-index-1[0]\0",
		      16)){
      g_object_set(G_OBJECT(plugin),
		   "bank-index-1\0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./current-bit[0]\0",
		      16)){
      g_object_set(G_OBJECT(plugin),
		   "current-bit\0", AGS_PORT(port->data),
		   NULL);
    }


    port = port->next;
  }
}

void
ags_copy_pattern_channel_finalize(GObject *gobject)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  if(copy_pattern_channel->pattern != NULL){
    g_object_unref(copy_pattern_channel->pattern);
  }

  if(copy_pattern_channel->bank_index_0 != NULL){
    g_object_unref(copy_pattern_channel->bank_index_0);
  }

  if(copy_pattern_channel->bank_index_1 != NULL){
    g_object_unref(copy_pattern_channel->bank_index_1);
  }

  if(copy_pattern_channel->current_bit != NULL){
    g_object_unref(copy_pattern_channel->current_bit);
  }

  G_OBJECT_CLASS(ags_copy_pattern_channel_parent_class)->finalize(gobject);
}

GList*
ags_copy_pattern_channel_template_find_source_and_destination(GList *recall,
							      AgsChannel *destination,
							      AgsChannel *source)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  while(recall != NULL){
    recall = ags_recall_template_find_type(recall, AGS_TYPE_COPY_PATTERN_CHANNEL);

    if(recall == NULL)
      break;

    copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(recall->data);

    if(AGS_RECALL_CHANNEL(copy_pattern_channel)->destination == destination &&
       AGS_RECALL_CHANNEL(copy_pattern_channel)->source == source)
      break;

    recall = recall->next;
  }

  return(recall);
}

void
ags_copy_pattern_channel_bank_index_0_safe_write_callback(AgsPort *pattern,
							  GValue *value,
							  AgsCopyPatternChannel *copy_pattern_channel)
{
  //TODO:JK: implement me
}

void
ags_copy_pattern_channel_bank_index_1_safe_write_callback(AgsPort *pattern,
							  GValue *value,
							  AgsCopyPatternChannel *copy_pattern_channel)
{
  //TODO:JK: implement me
}

/**
 * ags_copy_pattern_channel_new:
 * @destination: the destination #AgsChannel
 * @source: the source #AgsChannel
 * @pattern: the pattern port 
 *
 * Creates an #AgsCopyPatternChannel
 *
 * Returns: a new #AgsCopyPatternChannel
 *
 * Since: 0.4
 */
AgsCopyPatternChannel*
ags_copy_pattern_channel_new(AgsChannel *destination,
			     AgsChannel *source,
			     AgsPort *pattern)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
								"destination\0", destination,
								"channel\0", source,
								"pattern\0", pattern,
								NULL);

  return(copy_pattern_channel);
}
