/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_eq10_channel.h>

#include <ags/i18n.h>

void ags_fx_eq10_channel_class_init(AgsFxEq10ChannelClass *fx_eq10_channel);
void ags_fx_eq10_channel_init(AgsFxEq10Channel *fx_eq10_channel);
void ags_fx_eq10_channel_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_fx_eq10_channel_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_fx_eq10_channel_dispose(GObject *gobject);
void ags_fx_eq10_channel_finalize(GObject *gobject);

void ags_fx_eq10_channel_notify_buffer_size_callback(GObject *gobject,
						     GParamSpec *pspec,
						     gpointer user_data);

/**
 * SECTION:ags_fx_eq10_channel
 * @short_description: fx eq10 channel
 * @title: AgsFxEq10Channel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_eq10_channel.h
 *
 * The #AgsFxEq10Channel class provides ports to the effect processor.
 */

static gpointer ags_fx_eq10_channel_parent_class = NULL;

static const gchar *ags_fx_eq10_channel_plugin_name = "ags-fx-eq10";

static const gchar *ags_fx_eq10_channel_specifier[] = {
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
  NULL,
};

static const gchar *ags_fx_eq10_channel_control_port[] = {
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
  NULL,
};

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

GType
ags_fx_eq10_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_eq10_channel = 0;

    static const GTypeInfo ags_fx_eq10_channel_info = {
      sizeof (AgsFxEq10ChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_eq10_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxEq10Channel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_eq10_channel_init,
    };

    ags_type_fx_eq10_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						      "AgsFxEq10Channel",
						      &ags_fx_eq10_channel_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_eq10_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_eq10_channel_class_init(AgsFxEq10ChannelClass *fx_eq10_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_eq10_channel_parent_class = g_type_class_peek_parent(fx_eq10_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_eq10_channel;

  gobject->set_property = ags_fx_eq10_channel_set_property;
  gobject->get_property = ags_fx_eq10_channel_get_property;

  gobject->dispose = ags_fx_eq10_channel_dispose;
  gobject->finalize = ags_fx_eq10_channel_finalize;

  /* properties */
  /**
   * AgsFxEq10Channel:peak-28hz:
   * 
   * The peak 28Hz port.
   * 
   * Since: 3.3.0 
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
   * AgsFxEq10Channel:peak-56hz:
   * 
   * The peak 56Hz port.
   * 
   * Since: 3.3.0 
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
   * AgsFxEq10Channel:peak-112hz:
   * 
   * The peak 112Hz port.
   * 
   * Since: 3.3.0 
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
   * AgsFxEq10Channel:peak-224hz:
   * 
   * The peak 224Hz port.
   * 
   * Since: 3.3.0 
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
   * AgsFxEq10Channel:peak-448hz:
   * 
   * The peak 448Hz port.
   * 
   * Since: 3.3.0 
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
   * AgsFxEq10Channel:peak-896hz:
   * 
   * The peak 896Hz port.
   * 
   * Since: 3.3.0 
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
   * AgsFxEq10Channel:peak-1792hz:
   * 
   * The peak 1792Hz port.
   * 
   * Since: 3.3.0 
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
   * AgsFxEq10Channel:peak-3584hz:
   * 
   * The peak 3584Hz port.
   * 
   * Since: 3.3.0 
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
   * AgsFxEq10Channel:peak-7168hz:
   * 
   * The peak 7168Hz port.
   * 
   * Since: 3.3.0 
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
   * AgsFxEq10Channel:peak-14336hz:
   * 
   * The peak 14336Hz port.
   * 
   * Since: 3.3.0 
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
   * AgsFxEq10Channel:pressure:
   * 
   * The pressure port.
   * 
   * Since: 3.3.0 
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
ags_fx_eq10_channel_init(AgsFxEq10Channel *fx_eq10_channel)
{
  guint buffer_size;
  guint i;

  g_signal_connect(fx_eq10_channel, "notify::buffer-size",
		   G_CALLBACK(ags_fx_eq10_channel_notify_buffer_size_callback), NULL);

  AGS_RECALL(fx_eq10_channel)->name = "ags-fx-eq10";
  AGS_RECALL(fx_eq10_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_eq10_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_eq10_channel)->xml_type = "ags-fx-eq10-channel";

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_eq10_channel,
	       "buffer-size", &buffer_size,
	       NULL);

  /* peak 28hz */
  fx_eq10_channel->peak_28hz = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_fx_eq10_channel_plugin_name,
					    "specifier", ags_fx_eq10_channel_specifier[0],
					    "control-port", ags_fx_eq10_channel_control_port[0],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_FLOAT,
					    "port-value-size", sizeof(gfloat),
					    "port-value-length", 1,
					    NULL);

  fx_eq10_channel->peak_28hz->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->peak_28hz);

  /* peak 56hz */
  fx_eq10_channel->peak_56hz = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_fx_eq10_channel_plugin_name,
					    "specifier", ags_fx_eq10_channel_specifier[0],
					    "control-port", ags_fx_eq10_channel_control_port[0],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_FLOAT,
					    "port-value-size", sizeof(gfloat),
					    "port-value-length", 1,
					    NULL);

  fx_eq10_channel->peak_56hz->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->peak_56hz);

  /* peak 112hz */
  fx_eq10_channel->peak_112hz = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_eq10_channel_plugin_name,
					     "specifier", ags_fx_eq10_channel_specifier[0],
					     "control-port", ags_fx_eq10_channel_control_port[0],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_FLOAT,
					     "port-value-size", sizeof(gfloat),
					     "port-value-length", 1,
					     NULL);

  fx_eq10_channel->peak_112hz->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->peak_112hz);

  /* peak 224hz */
  fx_eq10_channel->peak_224hz = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_eq10_channel_plugin_name,
					     "specifier", ags_fx_eq10_channel_specifier[0],
					     "control-port", ags_fx_eq10_channel_control_port[0],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_FLOAT,
					     "port-value-size", sizeof(gfloat),
					     "port-value-length", 1,
					     NULL);

  fx_eq10_channel->peak_224hz->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->peak_224hz);

  /* peak 448hz */
  fx_eq10_channel->peak_448hz = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_eq10_channel_plugin_name,
					     "specifier", ags_fx_eq10_channel_specifier[0],
					     "control-port", ags_fx_eq10_channel_control_port[0],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_FLOAT,
					     "port-value-size", sizeof(gfloat),
					     "port-value-length", 1,
					     NULL);

  fx_eq10_channel->peak_448hz->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->peak_448hz);

  /* peak 896hz */
  fx_eq10_channel->peak_896hz = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_eq10_channel_plugin_name,
					     "specifier", ags_fx_eq10_channel_specifier[0],
					     "control-port", ags_fx_eq10_channel_control_port[0],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_FLOAT,
					     "port-value-size", sizeof(gfloat),
					     "port-value-length", 1,
					     NULL);

  fx_eq10_channel->peak_896hz->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->peak_896hz);

  /* peak 1792hz */
  fx_eq10_channel->peak_1792hz = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_fx_eq10_channel_plugin_name,
					      "specifier", ags_fx_eq10_channel_specifier[0],
					      "control-port", ags_fx_eq10_channel_control_port[0],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_FLOAT,
					      "port-value-size", sizeof(gfloat),
					      "port-value-length", 1,
					      NULL);

  fx_eq10_channel->peak_1792hz->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->peak_1792hz);

  /* peak 3584hz */
  fx_eq10_channel->peak_3584hz = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_fx_eq10_channel_plugin_name,
					      "specifier", ags_fx_eq10_channel_specifier[0],
					      "control-port", ags_fx_eq10_channel_control_port[0],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_FLOAT,
					      "port-value-size", sizeof(gfloat),
					      "port-value-length", 1,
					      NULL);

  fx_eq10_channel->peak_3584hz->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->peak_3584hz);

  /* peak 7168hz */
  fx_eq10_channel->peak_7168hz = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_fx_eq10_channel_plugin_name,
					      "specifier", ags_fx_eq10_channel_specifier[0],
					      "control-port", ags_fx_eq10_channel_control_port[0],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_FLOAT,
					      "port-value-size", sizeof(gfloat),
					      "port-value-length", 1,
					      NULL);

  fx_eq10_channel->peak_7168hz->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->peak_7168hz);

  /* peak 14336hz */
  fx_eq10_channel->peak_14336hz = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_fx_eq10_channel_plugin_name,
					       "specifier", ags_fx_eq10_channel_specifier[0],
					       "control-port", ags_fx_eq10_channel_control_port[0],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_FLOAT,
					       "port-value-size", sizeof(gfloat),
					       "port-value-length", 1,
					       NULL);

  fx_eq10_channel->peak_14336hz->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->peak_14336hz);


  /* pressure */
  fx_eq10_channel->pressure = g_object_new(AGS_TYPE_PORT,
					   "plugin-name", ags_fx_eq10_channel_plugin_name,
					   "specifier", ags_fx_eq10_channel_specifier[0],
					   "control-port", ags_fx_eq10_channel_control_port[0],
					   "port-value-is-pointer", FALSE,
					   "port-value-type", G_TYPE_FLOAT,
					   "port-value-size", sizeof(gfloat),
					   "port-value-length", 1,
					   NULL);

  fx_eq10_channel->pressure->port_value.ags_port_float = 1.0;

  ags_recall_add_port((AgsRecall *) fx_eq10_channel,
		      fx_eq10_channel->pressure);

  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    fx_eq10_channel->input_data[i] = ags_fx_eq10_channel_input_data_alloc();
      
    fx_eq10_channel->input_data[i]->parent = fx_eq10_channel;

    fx_eq10_channel->input_data[i]->output = (gdouble *) ags_stream_alloc(buffer_size,
 AGS_SOUNDCARD_DOUBLE);
    fx_eq10_channel->input_data[i]->input = (gdouble *) ags_stream_alloc(buffer_size,
AGS_SOUNDCARD_DOUBLE);
  }
}

void
ags_fx_eq10_channel_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsFxEq10Channel *fx_eq10_channel;

  GRecMutex *recall_mutex;

  fx_eq10_channel = AGS_FX_EQ10_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_eq10_channel);

  switch(prop_id){
  case PROP_PEAK_28HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_eq10_channel->peak_28hz){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_eq10_channel->peak_28hz != NULL){
	g_object_unref(G_OBJECT(fx_eq10_channel->peak_28hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_eq10_channel->peak_28hz = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_56HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_eq10_channel->peak_56hz){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_eq10_channel->peak_56hz != NULL){
	g_object_unref(G_OBJECT(fx_eq10_channel->peak_56hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_eq10_channel->peak_56hz = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_112HZ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_eq10_channel->peak_112hz){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_eq10_channel->peak_112hz != NULL){
	g_object_unref(G_OBJECT(fx_eq10_channel->peak_112hz));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_eq10_channel->peak_112hz = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PEAK_224HZ:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_eq10_channel->peak_224hz){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_eq10_channel->peak_224hz != NULL){
      g_object_unref(G_OBJECT(fx_eq10_channel->peak_224hz));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_eq10_channel->peak_224hz = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_448HZ:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_eq10_channel->peak_448hz){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_eq10_channel->peak_448hz != NULL){
      g_object_unref(G_OBJECT(fx_eq10_channel->peak_448hz));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_eq10_channel->peak_448hz = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_896HZ:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_eq10_channel->peak_896hz){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_eq10_channel->peak_896hz != NULL){
      g_object_unref(G_OBJECT(fx_eq10_channel->peak_896hz));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_eq10_channel->peak_896hz = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_1792HZ:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_eq10_channel->peak_1792hz){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_eq10_channel->peak_1792hz != NULL){
      g_object_unref(G_OBJECT(fx_eq10_channel->peak_1792hz));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_eq10_channel->peak_1792hz = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_3584HZ:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_eq10_channel->peak_3584hz){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_eq10_channel->peak_3584hz != NULL){
      g_object_unref(G_OBJECT(fx_eq10_channel->peak_3584hz));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_eq10_channel->peak_3584hz = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_7168HZ:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_eq10_channel->peak_7168hz){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_eq10_channel->peak_7168hz != NULL){
      g_object_unref(G_OBJECT(fx_eq10_channel->peak_7168hz));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_eq10_channel->peak_7168hz = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_14336HZ:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_eq10_channel->peak_14336hz){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_eq10_channel->peak_14336hz != NULL){
      g_object_unref(G_OBJECT(fx_eq10_channel->peak_14336hz));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_eq10_channel->peak_14336hz = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PRESSURE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_eq10_channel->pressure){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_eq10_channel->pressure != NULL){
      g_object_unref(G_OBJECT(fx_eq10_channel->pressure));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_eq10_channel->pressure = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_eq10_channel_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsFxEq10Channel *fx_eq10_channel;

  GRecMutex *recall_mutex;

  fx_eq10_channel = AGS_FX_EQ10_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_eq10_channel);

  switch(prop_id){
  case PROP_PEAK_28HZ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->peak_28hz);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_56HZ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->peak_56hz);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_112HZ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->peak_112hz);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_224HZ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->peak_224hz);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_448HZ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->peak_448hz);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_896HZ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->peak_896hz);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_PEAK_1792HZ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->peak_1792hz);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_3584HZ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->peak_3584hz);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_7168HZ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->peak_7168hz);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PEAK_14336HZ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->peak_14336hz);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PRESSURE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_eq10_channel->pressure);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_eq10_channel_dispose(GObject *gobject)
{
  AgsFxEq10Channel *fx_eq10_channel;
  
  fx_eq10_channel = AGS_FX_EQ10_CHANNEL(gobject);

  /* peak 28hz */
  if(fx_eq10_channel->peak_28hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_28hz));

    fx_eq10_channel->peak_28hz = NULL;
  }

  /* peak 56hz */
  if(fx_eq10_channel->peak_56hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_56hz));

    fx_eq10_channel->peak_56hz = NULL;
  }

  /* peak 112hz */
  if(fx_eq10_channel->peak_112hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_112hz));

    fx_eq10_channel->peak_112hz = NULL;
  }

  /* peak 224hz */
  if(fx_eq10_channel->peak_224hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_224hz));

    fx_eq10_channel->peak_224hz = NULL;
  }

  /* peak 448hz */
  if(fx_eq10_channel->peak_448hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_448hz));

    fx_eq10_channel->peak_448hz = NULL;
  }

  /* peak 896hz */
  if(fx_eq10_channel->peak_896hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_896hz));

    fx_eq10_channel->peak_896hz = NULL;
  }

  /* peak 1792hz */
  if(fx_eq10_channel->peak_1792hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_1792hz));

    fx_eq10_channel->peak_1792hz = NULL;
  }

  /* peak 3584hz */
  if(fx_eq10_channel->peak_3584hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_3584hz));

    fx_eq10_channel->peak_3584hz = NULL;
  }

  /* peak 7168hz */
  if(fx_eq10_channel->peak_7168hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_7168hz));

    fx_eq10_channel->peak_7168hz = NULL;
  }

  /* peak 14336hz */
  if(fx_eq10_channel->peak_14336hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_14336hz));

    fx_eq10_channel->peak_14336hz = NULL;
  }

  /* pressure */
  if(fx_eq10_channel->pressure != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->pressure));

    fx_eq10_channel->pressure = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_eq10_channel_parent_class)->dispose(gobject);
}

void
ags_fx_eq10_channel_finalize(GObject *gobject)
{
  AgsFxEq10Channel *fx_eq10_channel;

  guint i;
  
  fx_eq10_channel = AGS_FX_EQ10_CHANNEL(gobject);

  /* peak 28hz */
  if(fx_eq10_channel->peak_28hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_28hz));
  }

  /* peak 56hz */
  if(fx_eq10_channel->peak_56hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_56hz));
  }

  /* peak 112hz */
  if(fx_eq10_channel->peak_112hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_112hz));
  }

  /* peak 224hz */
  if(fx_eq10_channel->peak_224hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_224hz));
  }

  /* peak 448hz */
  if(fx_eq10_channel->peak_448hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_448hz));
  }

  /* peak 896hz */
  if(fx_eq10_channel->peak_896hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_896hz));
  }

  /* peak 1792hz */
  if(fx_eq10_channel->peak_1792hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_1792hz));
  }

  /* peak 3584hz */
  if(fx_eq10_channel->peak_3584hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_3584hz));
  }

  /* peak 7168hz */
  if(fx_eq10_channel->peak_7168hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_7168hz));
  }

  /* peak 14336hz */
  if(fx_eq10_channel->peak_14336hz != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->peak_14336hz));
  }

  /* pressure */
  if(fx_eq10_channel->pressure != NULL){
    g_object_unref(G_OBJECT(fx_eq10_channel->pressure));
  }

  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    ags_fx_eq10_channel_input_data_free(fx_eq10_channel->input_data[i]);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_eq10_channel_parent_class)->finalize(gobject);
}

void
ags_fx_eq10_channel_notify_buffer_size_callback(GObject *gobject,
						GParamSpec *pspec,
						gpointer user_data)
{
  AgsFxEq10Channel *fx_eq10_channel;

  guint buffer_size;
  guint i;
  
  GRecMutex *recall_mutex;
  
  fx_eq10_channel = AGS_FX_EQ10_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_eq10_channel);

  /* get buffer size */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_eq10_channel,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxEq10ChannelInputData *input_data;

    input_data = fx_eq10_channel->input_data[i];

    /* buffer */
    ags_stream_free(input_data->output);
    ags_stream_free(input_data->input);
    
    if(buffer_size > 0){
      input_data->output = (gdouble *) ags_stream_alloc(buffer_size,
							AGS_SOUNDCARD_DOUBLE);
      input_data->input = (gdouble *) ags_stream_alloc(buffer_size,
						       AGS_SOUNDCARD_DOUBLE);
    }else{
      input_data->output = NULL;
      input_data->input = NULL;
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_eq10_channel_input_data_alloc:
 * 
 * Allocate #AgsFxEq10ChannelInputData-struct
 * 
 * Returns: (type gpointer) (transfer full): the new #AgsFxEq10ChannelInputData-struct
 * 
 * Since: 3.3.0
 */
AgsFxEq10ChannelInputData*
ags_fx_eq10_channel_input_data_alloc()
{
  AgsFxEq10ChannelInputData *input_data;

  input_data = (AgsFxEq10ChannelInputData *) g_malloc(sizeof(AgsFxEq10ChannelInputData));

  input_data->parent = NULL;

  input_data->output = NULL;
  input_data->input = NULL;

  return(input_data);
}

/**
 * ags_fx_eq10_channel_input_data_free:
 * @input_data: (type gpointer) (transfer full): the #AgsFxEq10ChannelInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_eq10_channel_input_data_free(AgsFxEq10ChannelInputData *input_data)
{
  if(input_data == NULL){
    return;
  }

  ags_stream_free(input_data->output);
  ags_stream_free(input_data->input);
  
  g_free(input_data);
}

/**
 * ags_fx_eq10_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxEq10Channel
 *
 * Returns: the new #AgsFxEq10Channel
 *
 * Since: 3.3.0
 */
AgsFxEq10Channel*
ags_fx_eq10_channel_new(AgsChannel *channel)
{
  AgsFxEq10Channel *fx_eq10_channel;

  fx_eq10_channel = (AgsFxEq10Channel *) g_object_new(AGS_TYPE_FX_EQ10_CHANNEL,
						      "source", channel,
						      NULL);

  return(fx_eq10_channel);
}
