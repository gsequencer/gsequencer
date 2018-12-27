/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_eq10_channel.h>

#include <ags/libags.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_eq10_channel_class_init(AgsEq10ChannelClass *eq10_channel);
void ags_eq10_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_eq10_channel_init(AgsEq10Channel *eq10_channel);
void ags_eq10_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_eq10_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_eq10_channel_dispose(GObject *gobject);
void ags_eq10_channel_finalize(GObject *gobject);

void ags_eq10_channel_set_ports(AgsPlugin *plugin, GList *port);

static AgsPluginPort* ags_eq10_channel_get_peak_generic_plugin_port();

/**
 * SECTION:ags_eq10_channel
 * @short_description: 10 band equalizer channel
 * @title: AgsEq10Channel
 * @section_id:
 * @include: ags/audio/recall/ags_eq10_channel.h
 *
 * The #AgsEq10Channel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_PEAK_28HZ,
  PROP_PEAK_56HZ,
  PROP_PEAK_112HZ,
  PROP_PEAK_224HZ,
  PROP_PEAK_448HZ,
  PROP_PEAK_896HZ,
  PROP_PEAK_1792HZ,
  PROP_PEAK_3584HZ,
  PROP_PEAK_7168HZ,
  PROP_PEAK_14336HZ,
  PROP_PRESSURE,
};

static gpointer ags_eq10_channel_parent_class = NULL;
static AgsPluginInterface *ags_eq10_channel_parent_plugin_interface;

static const gchar *ags_eq10_channel_plugin_name = "ags-eq10";
static const gchar *ags_eq10_channel_specifier[] = {
  "./peak-28hz[0]",
  "./peak-56hz[0]",
  "./peak-112hz[0]",
  "./peak-224hz[0]",
  "./peak-448hz[0]",
  "./peak-896hz[0]",
  "./peak-1792hz[0]",
  "./peak-3584hz[0]",
  "./peak-7168hz[0]",
  "./peak-14336hz[0]",
  "./pressure[0]",
};
static const gchar *ags_eq10_channel_control_port[] = {
  "1/11",
  "2/11",
  "3/11",
  "4/11",
  "5/11",
  "6/11",
  "7/11",
  "8/11",
  "9/11",
  "10/11",
  "11/11",
};

GType
ags_eq10_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_eq10_channel = 0;

    static const GTypeInfo ags_eq10_channel_info = {
      sizeof(AgsEq10ChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_eq10_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEq10Channel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_eq10_channel_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_eq10_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_eq10_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsEq10Channel",
						   &ags_eq10_channel_info,
						   0);
    
    g_type_add_interface_static(ags_type_eq10_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_eq10_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_eq10_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_eq10_channel_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_eq10_channel_set_ports;
}

void
ags_eq10_channel_class_init(AgsEq10ChannelClass *eq10_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  GParamSpec *param_spec;

  ags_eq10_channel_parent_class = g_type_class_peek_parent(eq10_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) eq10_channel;

  gobject->set_property = ags_eq10_channel_set_property;
  gobject->get_property = ags_eq10_channel_get_property;

  gobject->dispose = ags_eq10_channel_dispose;
  gobject->finalize = ags_eq10_channel_finalize;

  /* properties */
  /**
   * AgsEq10Channel:peak-28hz:
   * 
   * The peak 28Hz port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("peak-28hz",
				   i18n_pspec("28Hz peak to apply"),
				   i18n_pspec("The 28Hz peak to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK_28HZ,
				  param_spec);

  /**
   * AgsEq10Channel:peak-56hz:
   * 
   * The peak 56Hz port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("peak-56hz",
				   i18n_pspec("56Hz peak to apply"),
				   i18n_pspec("The 56Hz peak to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK_56HZ,
				  param_spec);

  /**
   * AgsEq10Channel:peak-112hz:
   * 
   * The peak 112Hz port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("peak-112hz",
				   i18n_pspec("112Hz peak to apply"),
				   i18n_pspec("The 112Hz peak to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK_112HZ,
				  param_spec);

  /**
   * AgsEq10Channel:peak-224hz:
   * 
   * The peak 224Hz port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("peak-224hz",
				   i18n_pspec("224Hz peak to apply"),
				   i18n_pspec("The 224Hz peak to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK_224HZ,
				  param_spec);

  /**
   * AgsEq10Channel:peak-448hz:
   * 
   * The peak 448Hz port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("peak-448hz",
				   i18n_pspec("448Hz peak to apply"),
				   i18n_pspec("The 448Hz peak to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK_448HZ,
				  param_spec);

  /**
   * AgsEq10Channel:peak-896hz:
   * 
   * The peak 896Hz port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("peak-896hz",
				   i18n_pspec("896Hz peak to apply"),
				   i18n_pspec("The 896Hz peak to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK_896HZ,
				  param_spec);

  /**
   * AgsEq10Channel:peak-1792hz:
   * 
   * The peak 1792Hz port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("peak-1792hz",
				   i18n_pspec("1792Hz peak to apply"),
				   i18n_pspec("The 1792Hz peak to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK_1792HZ,
				  param_spec);

  /**
   * AgsEq10Channel:peak-3584hz:
   * 
   * The peak 3584Hz port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("peak-3584hz",
				   i18n_pspec("3584Hz peak to apply"),
				   i18n_pspec("The 3584Hz peak to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK_3584HZ,
				  param_spec);

  /**
   * AgsEq10Channel:peak-7168hz:
   * 
   * The peak 7168Hz port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("peak-7168hz",
				   i18n_pspec("7168Hz peak to apply"),
				   i18n_pspec("The 7168Hz peak to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK_7168HZ,
				  param_spec);

  /**
   * AgsEq10Channel:peak-14336hz:
   * 
   * The peak 14336Hz port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("peak-14336hz",
				   i18n_pspec("14336Hz peak to apply"),
				   i18n_pspec("The 14336Hz peak to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK_14336HZ,
				  param_spec);

  /**
   * AgsEq10Channel:pressure:
   * 
   * The pressure port.
   * 
   * Since: 2.0.0 
   */
  param_spec = g_param_spec_object("pressure",
				   i18n_pspec("pressure to apply"),
				   i18n_pspec("The pressure to apply on the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESSURE,
				  param_spec);
}

void
ags_eq10_channel_init(AgsEq10Channel *eq10_channel)
{
  GList *port;

  AGS_RECALL(eq10_channel)->name = "ags-eq10";
  AGS_RECALL(eq10_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(eq10_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(eq10_channel)->xml_type = "ags-eq10-channel";

  /* initialize the port */
  port = NULL;

  /* peak 28hz */
  eq10_channel->peak_28hz = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_eq10_channel_plugin_name,
					 "specifier", ags_eq10_channel_specifier[0],
					 "control-port", ags_eq10_channel_control_port[0],
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_FLOAT,
					 "port-value-size", sizeof(gfloat),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(eq10_channel->peak_28hz);
  
  eq10_channel->peak_28hz->port_value.ags_port_float = 1.0;

  /* plugin port */
  g_object_set(eq10_channel->peak_28hz,
	       "plugin-port", ags_eq10_channel_get_peak_generic_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, eq10_channel->peak_28hz);
  g_object_ref(eq10_channel->peak_28hz);

  /* peak 56hz */
  eq10_channel->peak_56hz = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_eq10_channel_plugin_name,
					 "specifier", ags_eq10_channel_specifier[1],
					 "control-port", ags_eq10_channel_control_port[1],
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_FLOAT,
					 "port-value-size", sizeof(gfloat),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(eq10_channel->peak_56hz);
  
  eq10_channel->peak_56hz->port_value.ags_port_float = 1.0;

  /* plugin port */
  g_object_set(eq10_channel->peak_56hz,
	       "plugin-port", ags_eq10_channel_get_peak_generic_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, eq10_channel->peak_56hz);
  g_object_ref(eq10_channel->peak_56hz);

  /* peak 112hz */
  eq10_channel->peak_112hz = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", ags_eq10_channel_plugin_name,
					  "specifier", ags_eq10_channel_specifier[2],
					  "control-port", ags_eq10_channel_control_port[2],
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_FLOAT,
					  "port-value-size", sizeof(gfloat),
					  "port-value-length", 1,
					  NULL);
  g_object_ref(eq10_channel->peak_112hz);
  
  eq10_channel->peak_112hz->port_value.ags_port_float = 1.0;

  /* plugin port */
  g_object_set(eq10_channel->peak_112hz,
	       "plugin-port", ags_eq10_channel_get_peak_generic_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, eq10_channel->peak_112hz);
  g_object_ref(eq10_channel->peak_112hz);

  /* peak 224hz */
  eq10_channel->peak_224hz = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", ags_eq10_channel_plugin_name,
					  "specifier", ags_eq10_channel_specifier[3],
					  "control-port", ags_eq10_channel_control_port[3],
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_FLOAT,
					  "port-value-size", sizeof(gfloat),
					  "port-value-length", 1,
					  NULL);
  g_object_ref(eq10_channel->peak_224hz);
  
  eq10_channel->peak_224hz->port_value.ags_port_float = 1.0;

  /* plugin port */
  g_object_set(eq10_channel->peak_224hz,
	       "plugin-port", ags_eq10_channel_get_peak_generic_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, eq10_channel->peak_224hz);
  g_object_ref(eq10_channel->peak_224hz);

  /* peak 448hz */
  eq10_channel->peak_448hz = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", ags_eq10_channel_plugin_name,
					  "specifier", ags_eq10_channel_specifier[4],
					  "control-port", ags_eq10_channel_control_port[4],
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_FLOAT,
					  "port-value-size", sizeof(gfloat),
					  "port-value-length", 1,
					  NULL);
  g_object_ref(eq10_channel->peak_448hz);
  
  eq10_channel->peak_448hz->port_value.ags_port_float = 1.0;

  /* plugin port */
  g_object_set(eq10_channel->peak_448hz,
	       "plugin-port", ags_eq10_channel_get_peak_generic_plugin_port(),
	       NULL);
 
  /* add to port */  
  port = g_list_prepend(port, eq10_channel->peak_448hz);
  g_object_ref(eq10_channel->peak_448hz);

  /* peak 896hz */
  eq10_channel->peak_896hz = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", ags_eq10_channel_plugin_name,
					  "specifier", ags_eq10_channel_specifier[5],
					  "control-port", ags_eq10_channel_control_port[5],
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_FLOAT,
					  "port-value-size", sizeof(gfloat),
					  "port-value-length", 1,
					  NULL);
  g_object_ref(eq10_channel->peak_896hz);
  
  eq10_channel->peak_896hz->port_value.ags_port_float = 1.0;

  /* plugin port */
  eq10_channel->peak_896hz->plugin_port = ags_eq10_channel_get_peak_generic_plugin_port();

  /* add to port */  
  port = g_list_prepend(port, eq10_channel->peak_896hz);
  g_object_ref(eq10_channel->peak_896hz);

  /* peak 1792hz */
  eq10_channel->peak_1792hz = g_object_new(AGS_TYPE_PORT,
					   "plugin-name", ags_eq10_channel_plugin_name,
					   "specifier", ags_eq10_channel_specifier[6],
					   "control-port", ags_eq10_channel_control_port[6],
					   "port-value-is-pointer", FALSE,
					   "port-value-type", G_TYPE_FLOAT,
					   "port-value-size", sizeof(gfloat),
					   "port-value-length", 1,
					   NULL);
  g_object_ref(eq10_channel->peak_1792hz);
  
  eq10_channel->peak_1792hz->port_value.ags_port_float = 1.0;

  /* plugin port */
  g_object_set(eq10_channel->peak_1792hz,
	       "plugin-port", ags_eq10_channel_get_peak_generic_plugin_port(),
	       NULL);
  
  /* add to port */  
  port = g_list_prepend(port, eq10_channel->peak_1792hz);
  g_object_ref(eq10_channel->peak_1792hz);

  /* peak 3584hz */
  eq10_channel->peak_3584hz = g_object_new(AGS_TYPE_PORT,
					   "plugin-name", ags_eq10_channel_plugin_name,
					   "specifier", ags_eq10_channel_specifier[7],
					   "control-port", ags_eq10_channel_control_port[7],
					   "port-value-is-pointer", FALSE,
					   "port-value-type", G_TYPE_FLOAT,
					   "port-value-size", sizeof(gfloat),
					   "port-value-length", 1,
					   NULL);
  g_object_ref(eq10_channel->peak_3584hz);
  
  eq10_channel->peak_3584hz->port_value.ags_port_float = 1.0;

  /* plugin port */
  g_object_set(eq10_channel->peak_3584hz,
	       "plugin-port", ags_eq10_channel_get_peak_generic_plugin_port(),
	       NULL);
  
  /* add to port */  
  port = g_list_prepend(port, eq10_channel->peak_3584hz);
  g_object_ref(eq10_channel->peak_3584hz);

  /* peak 7168hz */
  eq10_channel->peak_7168hz = g_object_new(AGS_TYPE_PORT,
					   "plugin-name", ags_eq10_channel_plugin_name,
					   "specifier", ags_eq10_channel_specifier[8],
					   "control-port", ags_eq10_channel_control_port[8],
					   "port-value-is-pointer", FALSE,
					   "port-value-type", G_TYPE_FLOAT,
					   "port-value-size", sizeof(gfloat),
					   "port-value-length", 1,
					   NULL);
  g_object_ref(eq10_channel->peak_7168hz);
  
  eq10_channel->peak_7168hz->port_value.ags_port_float = 1.0;

  /* plugin port */
  g_object_set(eq10_channel->peak_7168hz,
	       "plugin-port", ags_eq10_channel_get_peak_generic_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, eq10_channel->peak_7168hz);
  g_object_ref(eq10_channel->peak_7168hz);

  /* peak 14336hz */
  eq10_channel->peak_14336hz = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_eq10_channel_plugin_name,
					    "specifier", ags_eq10_channel_specifier[9],
					    "control-port", ags_eq10_channel_control_port[9],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_FLOAT,
					    "port-value-size", sizeof(gfloat),
					    "port-value-length", 1,
					    NULL);
  g_object_ref(eq10_channel->peak_14336hz);
  
  eq10_channel->peak_14336hz->port_value.ags_port_float = 1.0;

  /* plugin port */
  g_object_set(eq10_channel->peak_14336hz,
	       "plugin-port", ags_eq10_channel_get_peak_generic_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, eq10_channel->peak_14336hz);
  g_object_ref(eq10_channel->peak_14336hz);

  /* pressure */
  eq10_channel->pressure = g_object_new(AGS_TYPE_PORT,
					"plugin-name", ags_eq10_channel_plugin_name,
					"specifier", ags_eq10_channel_specifier[10],
					"control-port", ags_eq10_channel_control_port[10],
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					"port-value-size", sizeof(gfloat),
					"port-value-length", 1,
					NULL);
  g_object_ref(eq10_channel->pressure);
  
  eq10_channel->pressure->port_value.ags_port_float = 1.0;

  /* plugin port */
  g_object_set(eq10_channel->pressure,
	       "plugin-port", ags_eq10_channel_get_peak_generic_plugin_port(),
	       NULL);

  /* add to port */  
  port = g_list_prepend(port, eq10_channel->pressure);
  g_object_ref(eq10_channel->pressure);

  /* set port */
  AGS_RECALL(eq10_channel)->port = port;
}

void
ags_eq10_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsEq10Channel *eq10_channel;

  pthread_mutex_t *recall_mutex;

  eq10_channel = AGS_EQ10_CHANNEL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_PEAK_28HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->peak_28hz){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->peak_28hz != NULL){
	g_object_unref(G_OBJECT(eq10_channel->peak_28hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->peak_28hz = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_56HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->peak_56hz){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->peak_56hz != NULL){
	g_object_unref(G_OBJECT(eq10_channel->peak_56hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->peak_56hz = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_112HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->peak_112hz){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->peak_112hz != NULL){
	g_object_unref(G_OBJECT(eq10_channel->peak_112hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->peak_112hz = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_224HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->peak_224hz){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->peak_224hz != NULL){
	g_object_unref(G_OBJECT(eq10_channel->peak_224hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->peak_224hz = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_448HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->peak_448hz){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->peak_448hz != NULL){
	g_object_unref(G_OBJECT(eq10_channel->peak_448hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->peak_448hz = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_896HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->peak_896hz){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->peak_896hz != NULL){
	g_object_unref(G_OBJECT(eq10_channel->peak_896hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->peak_896hz = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_1792HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->peak_1792hz){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->peak_1792hz != NULL){
	g_object_unref(G_OBJECT(eq10_channel->peak_1792hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->peak_1792hz = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_3584HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->peak_3584hz){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->peak_3584hz != NULL){
	g_object_unref(G_OBJECT(eq10_channel->peak_3584hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->peak_3584hz = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_7168HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->peak_7168hz){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->peak_7168hz != NULL){
	g_object_unref(G_OBJECT(eq10_channel->peak_7168hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->peak_7168hz = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_14336HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->peak_14336hz){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->peak_14336hz != NULL){
	g_object_unref(G_OBJECT(eq10_channel->peak_14336hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->peak_14336hz = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PRESSURE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == eq10_channel->pressure){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(eq10_channel->pressure != NULL){
	g_object_unref(G_OBJECT(eq10_channel->pressure));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      eq10_channel->pressure = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_eq10_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsEq10Channel *eq10_channel;

  pthread_mutex_t *recall_mutex;

  eq10_channel = AGS_EQ10_CHANNEL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_PEAK_28HZ:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->peak_28hz);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_56HZ:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->peak_56hz);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_112HZ:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->peak_112hz);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_224HZ:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->peak_224hz);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_448HZ:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->peak_448hz);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_896HZ:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->peak_896hz);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PEAK_1792HZ:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->peak_1792hz);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_3584HZ:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->peak_3584hz);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_7168HZ:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->peak_7168hz);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_14336HZ:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->peak_14336hz);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PRESSURE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, eq10_channel->pressure);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_eq10_channel_dispose(GObject *gobject)
{
  AgsEq10Channel *eq10_channel;

  eq10_channel = AGS_EQ10_CHANNEL(gobject);

  /* peak 28hz */
  if(eq10_channel->peak_28hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_28hz));

    eq10_channel->peak_28hz = NULL;
  }

  /* peak 56hz */
  if(eq10_channel->peak_56hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_56hz));

    eq10_channel->peak_56hz = NULL;
  }

  /* peak 112hz */
  if(eq10_channel->peak_112hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_112hz));

    eq10_channel->peak_112hz = NULL;
  }

  /* peak 224hz */
  if(eq10_channel->peak_224hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_224hz));

    eq10_channel->peak_224hz = NULL;
  }

  /* peak 448hz */
  if(eq10_channel->peak_448hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_448hz));

    eq10_channel->peak_448hz = NULL;
  }

  /* peak 896hz */
  if(eq10_channel->peak_896hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_896hz));

    eq10_channel->peak_896hz = NULL;
  }

  /* peak 1792hz */
  if(eq10_channel->peak_1792hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_1792hz));

    eq10_channel->peak_1792hz = NULL;
  }

  /* peak 3584hz */
  if(eq10_channel->peak_3584hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_3584hz));

    eq10_channel->peak_3584hz = NULL;
  }

  /* peak 7168hz */
  if(eq10_channel->peak_7168hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_7168hz));

    eq10_channel->peak_7168hz = NULL;
  }

  /* peak 14336hz */
  if(eq10_channel->peak_14336hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_14336hz));

    eq10_channel->peak_14336hz = NULL;
  }

  /* pressure */
  if(eq10_channel->pressure != NULL){
    g_object_unref(G_OBJECT(eq10_channel->pressure));

    eq10_channel->pressure = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_eq10_channel_parent_class)->dispose(gobject);
}

void
ags_eq10_channel_finalize(GObject *gobject)
{
  AgsEq10Channel *eq10_channel;

  eq10_channel = AGS_EQ10_CHANNEL(gobject);

  /* peak 28hz */
  if(eq10_channel->peak_28hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_28hz));
  }

  /* peak 56hz */
  if(eq10_channel->peak_56hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_56hz));
  }

  /* peak 112hz */
  if(eq10_channel->peak_112hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_112hz));
  }

  /* peak 224hz */
  if(eq10_channel->peak_224hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_224hz));
  }

  /* peak 448hz */
  if(eq10_channel->peak_448hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_448hz));
  }

  /* peak 896hz */
  if(eq10_channel->peak_896hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_896hz));
  }

  /* peak 1792hz */
  if(eq10_channel->peak_1792hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_1792hz));
  }

  /* peak 3584hz */
  if(eq10_channel->peak_3584hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_3584hz));
  }

  /* peak 7168hz */
  if(eq10_channel->peak_7168hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_7168hz));
  }

  /* peak 14336hz */
  if(eq10_channel->peak_14336hz != NULL){
    g_object_unref(G_OBJECT(eq10_channel->peak_14336hz));
  }

  /* pressure */
  if(eq10_channel->pressure != NULL){
    g_object_unref(G_OBJECT(eq10_channel->pressure));
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_eq10_channel_parent_class)->finalize(gobject);
}


void
ags_eq10_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./peak-28hz[0]",
		15)){
      g_object_set(G_OBJECT(plugin),
		   "peak-28hz", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./peak-56hz[0]",
		      15)){
      g_object_set(G_OBJECT(plugin),
		   "peak-56hz", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./peak-112hz[0]",
		      16)){
      g_object_set(G_OBJECT(plugin),
		   "peak-112hz", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./peak-224hz[0]",
		      16)){
      g_object_set(G_OBJECT(plugin),
		   "peak-224hz", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./peak-448hz[0]",
		      16)){
      g_object_set(G_OBJECT(plugin),
		   "peak-448hz", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./peak-896hz[0]",
		      16)){
      g_object_set(G_OBJECT(plugin),
		   "peak-896hz", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./peak-1792hz[0]",
		      17)){
      g_object_set(G_OBJECT(plugin),
		   "peak-1792hz", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./peak-3584hz[0]",
		      17)){
      g_object_set(G_OBJECT(plugin),
		   "peak-3584hz", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./peak-7168hz[0]",
		      17)){
      g_object_set(G_OBJECT(plugin),
		   "peak-7168hz", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./peak-14336hz[0]",
		      18)){
      g_object_set(G_OBJECT(plugin),
		   "peak-14336hz", AGS_PORT(port->data),
		   NULL);
    }
    
    port = port->next;
  }
}

static AgsPluginPort*
ags_eq10_channel_get_peak_generic_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0);
  }
  
  pthread_mutex_unlock(&mutex);

  return(plugin_port);
}

/**
 * ags_eq10_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsEq10Channel
 *
 * Returns: the new #AgsEq10Channel
 *
 * Since: 2.0.0
 */
AgsEq10Channel*
ags_eq10_channel_new(AgsChannel *source)
{
  AgsEq10Channel *eq10_channel;

  eq10_channel = (AgsEq10Channel *) g_object_new(AGS_TYPE_EQ10_CHANNEL,
						 "source", source,
						 NULL);

  return(eq10_channel);
}
