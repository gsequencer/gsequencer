/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_two_pass_aliase_channel.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_two_pass_aliase_channel_class_init(AgsFxTwoPassAliaseChannelClass *fx_two_pass_aliase_channel);
void ags_fx_two_pass_aliase_channel_init(AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel);
void ags_fx_two_pass_aliase_channel_set_property(GObject *gobject,
						 guint prop_id,
						 const GValue *value,
						 GParamSpec *param_spec);
void ags_fx_two_pass_aliase_channel_get_property(GObject *gobject,
						 guint prop_id,
						 GValue *value,
						 GParamSpec *param_spec);
void ags_fx_two_pass_aliase_channel_dispose(GObject *gobject);
void ags_fx_two_pass_aliase_channel_finalize(GObject *gobject);

void ags_fx_two_pass_aliase_channel_notify_buffer_size_callback(GObject *gobject,
								GParamSpec *pspec,
								gpointer user_data);

static AgsPluginPort* ags_fx_two_pass_aliase_channel_get_enabled_plugin_port();
static AgsPluginPort* ags_fx_two_pass_aliase_channel_get_a_amount_plugin_port();
static AgsPluginPort* ags_fx_two_pass_aliase_channel_get_a_phase_plugin_port();
static AgsPluginPort* ags_fx_two_pass_aliase_channel_get_b_amount_plugin_port();
static AgsPluginPort* ags_fx_two_pass_aliase_channel_get_b_phase_plugin_port();

/**
 * SECTION:ags_fx_two_pass_aliase_channel
 * @short_description: fx two pass aliase channel
 * @title: AgsFxTwoPassAliaseChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_two_pass_aliase_channel.h
 *
 * The #AgsFxTwoPassAliaseChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_two_pass_aliase_channel_parent_class = NULL;

const gchar *ags_fx_two_pass_aliase_channel_plugin_name = "ags-fx-two-pass-aliase";

const gchar* ags_fx_two_pass_aliase_channel_specifier[] = {
  "./enabled[0]",
  "./a-amount[0]",
  "./a-phase[0]",
  "./b-amount[0]",
  "./b-phase[0]",
  NULL,
};

const gchar* ags_fx_two_pass_aliase_channel_control_port[] = {
  "1/5",
  "2/5",
  "3/5",
  "4/5",
  "5/5",
  NULL,
};

enum{
  PROP_0,
  PROP_ENABLED,
  PROP_A_AMOUNT,
  PROP_A_PHASE,
  PROP_B_AMOUNT,
  PROP_B_PHASE,
};

GType
ags_fx_two_pass_aliase_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_two_pass_aliase_channel = 0;

    static const GTypeInfo ags_fx_two_pass_aliase_channel_info = {
      sizeof (AgsFxTwoPassAliaseChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_two_pass_aliase_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxTwoPassAliaseChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_two_pass_aliase_channel_init,
    };

    ags_type_fx_two_pass_aliase_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
								 "AgsFxTwoPassAliaseChannel",
								 &ags_fx_two_pass_aliase_channel_info,
								 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_two_pass_aliase_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_two_pass_aliase_channel_class_init(AgsFxTwoPassAliaseChannelClass *fx_two_pass_aliase_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_two_pass_aliase_channel_parent_class = g_type_class_peek_parent(fx_two_pass_aliase_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_two_pass_aliase_channel;

  gobject->set_property = ags_fx_two_pass_aliase_channel_set_property;
  gobject->get_property = ags_fx_two_pass_aliase_channel_get_property;

  gobject->dispose = ags_fx_two_pass_aliase_channel_dispose;
  gobject->finalize = ags_fx_two_pass_aliase_channel_finalize;

  /* properties */
  /**
   * AgsFxTwoPassAliaseChannel:enabled:
   *
   * The enable control of two pass aliase.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("enabled",
				   i18n_pspec("two pass aliase enabled"),
				   i18n_pspec("The enabled port to control two pass aliase"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ENABLED,
				  param_spec);

  /**
   * AgsFxTwoPassAliaseChannel:a-amount:
   *
   * The first pass amount.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("a-amount",
				   i18n_pspec("a's amount"),
				   i18n_pspec("The amount of a"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_A_AMOUNT,
				  param_spec);
  
  /**
   * AgsFxTwoPassAliaseChannel:a-phase:
   *
   * The first pass phase.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("a-phase",
				   i18n_pspec("a's phase"),
				   i18n_pspec("The phase of a"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_A_PHASE,
				  param_spec);

  /**
   * AgsFxTwoPassAliaseChannel:b-amount:
   *
   * The second pass amount.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("b-amount",
				   i18n_pspec("b's amount"),
				   i18n_pspec("The amount of b"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_B_AMOUNT,
				  param_spec);

  /**
   * AgsFxTwoPassAliaseChannel:b-phase:
   *
   * The second pass phase.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_object("b-phase",
				   i18n_pspec("b's phase"),
				   i18n_pspec("The phase of b"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_B_PHASE,
				  param_spec);
}

void
ags_fx_two_pass_aliase_channel_init(AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel)
{
  guint buffer_size;
  guint i;

  g_signal_connect(fx_two_pass_aliase_channel, "notify::buffer-size",
		   G_CALLBACK(ags_fx_two_pass_aliase_channel_notify_buffer_size_callback), NULL);
  
  AGS_RECALL(fx_two_pass_aliase_channel)->name = "ags-fx-two-pass-aliase";
  AGS_RECALL(fx_two_pass_aliase_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_two_pass_aliase_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_two_pass_aliase_channel)->xml_type = "ags-fx-two-pass-aliase-channel";

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_two_pass_aliase_channel,
	       "buffer-size", &buffer_size,
	       NULL);

  /* enabled */
  fx_two_pass_aliase_channel->enabled = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_two_pass_aliase_channel_plugin_name,
						     "specifier", ags_fx_two_pass_aliase_channel_specifier[0],
						     "control-port", ags_fx_two_pass_aliase_channel_control_port[0],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_two_pass_aliase_channel->enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_two_pass_aliase_channel->enabled,
	       "plugin-port", ags_fx_two_pass_aliase_channel_get_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_two_pass_aliase_channel,
		      fx_two_pass_aliase_channel->enabled);

  /* a amount */
  fx_two_pass_aliase_channel->a_amount = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_two_pass_aliase_channel_plugin_name,
						      "specifier", ags_fx_two_pass_aliase_channel_specifier[0],
						      "control-port", ags_fx_two_pass_aliase_channel_control_port[0],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_two_pass_aliase_channel->a_amount->port_value.ags_port_float = 0.0;

  g_object_set(fx_two_pass_aliase_channel->a_amount,
	       "plugin-port", ags_fx_two_pass_aliase_channel_get_a_amount_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_two_pass_aliase_channel,
		      fx_two_pass_aliase_channel->a_amount);

  /* a phase */
  fx_two_pass_aliase_channel->a_phase = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_two_pass_aliase_channel_plugin_name,
						     "specifier", ags_fx_two_pass_aliase_channel_specifier[0],
						     "control-port", ags_fx_two_pass_aliase_channel_control_port[0],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_two_pass_aliase_channel->a_phase->port_value.ags_port_float = 0.0;

  g_object_set(fx_two_pass_aliase_channel->a_phase,
	       "plugin-port", ags_fx_two_pass_aliase_channel_get_a_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_two_pass_aliase_channel,
		      fx_two_pass_aliase_channel->a_phase);


  /* b amount */
  fx_two_pass_aliase_channel->b_amount = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_two_pass_aliase_channel_plugin_name,
						      "specifier", ags_fx_two_pass_aliase_channel_specifier[0],
						      "control-port", ags_fx_two_pass_aliase_channel_control_port[0],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_two_pass_aliase_channel->b_amount->port_value.ags_port_float = 0.0;

  g_object_set(fx_two_pass_aliase_channel->b_amount,
	       "plugin-port", ags_fx_two_pass_aliase_channel_get_b_amount_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_two_pass_aliase_channel,
		      fx_two_pass_aliase_channel->b_amount);

  /* b phase */
  fx_two_pass_aliase_channel->b_phase = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_two_pass_aliase_channel_plugin_name,
						     "specifier", ags_fx_two_pass_aliase_channel_specifier[0],
						     "control-port", ags_fx_two_pass_aliase_channel_control_port[0],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_two_pass_aliase_channel->b_phase->port_value.ags_port_float = 0.0;

  g_object_set(fx_two_pass_aliase_channel->b_phase,
	       "plugin-port", ags_fx_two_pass_aliase_channel_get_b_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_two_pass_aliase_channel,
		      fx_two_pass_aliase_channel->b_phase);


  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    fx_two_pass_aliase_channel->input_data[i] = ags_fx_two_pass_aliase_channel_input_data_alloc();
      
    fx_two_pass_aliase_channel->input_data[i]->parent = fx_two_pass_aliase_channel;

    fx_two_pass_aliase_channel->input_data[i]->orig_buffer = (gdouble *) ags_stream_alloc(buffer_size,
											  AGS_SOUNDCARD_DOUBLE);

    fx_two_pass_aliase_channel->input_data[i]->a_buffer = (gdouble *) ags_stream_alloc(buffer_size,
										       AGS_SOUNDCARD_DOUBLE);
    fx_two_pass_aliase_channel->input_data[i]->b_buffer = (gdouble *) ags_stream_alloc(buffer_size,
										       AGS_SOUNDCARD_DOUBLE);

    fx_two_pass_aliase_channel->input_data[i]->a_mix = (gdouble *) ags_stream_alloc(buffer_size,
										    AGS_SOUNDCARD_DOUBLE);
    fx_two_pass_aliase_channel->input_data[i]->b_mix = (gdouble *) ags_stream_alloc(buffer_size,
										    AGS_SOUNDCARD_DOUBLE);

    fx_two_pass_aliase_channel->input_data[i]->final_mix = (gdouble *) ags_stream_alloc(buffer_size,
											AGS_SOUNDCARD_DOUBLE);
  }
}

void
ags_fx_two_pass_aliase_channel_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;

  GRecMutex *recall_mutex;

  fx_two_pass_aliase_channel = AGS_FX_TWO_PASS_ALIASE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_two_pass_aliase_channel);

  switch(prop_id){
  case PROP_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_two_pass_aliase_channel->enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_two_pass_aliase_channel->enabled != NULL){
      g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_two_pass_aliase_channel->enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_A_AMOUNT:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_two_pass_aliase_channel->a_amount){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_two_pass_aliase_channel->a_amount != NULL){
      g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->a_amount));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_two_pass_aliase_channel->a_amount = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_A_PHASE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_two_pass_aliase_channel->a_phase){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_two_pass_aliase_channel->a_phase != NULL){
      g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->a_phase));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_two_pass_aliase_channel->a_phase = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_B_AMOUNT:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_two_pass_aliase_channel->b_amount){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_two_pass_aliase_channel->b_amount != NULL){
      g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->b_amount));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_two_pass_aliase_channel->b_amount = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_B_PHASE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_two_pass_aliase_channel->b_phase){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_two_pass_aliase_channel->b_phase != NULL){
      g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->b_phase));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_two_pass_aliase_channel->b_phase = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_two_pass_aliase_channel_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;

  GRecMutex *recall_mutex;

  fx_two_pass_aliase_channel = AGS_FX_TWO_PASS_ALIASE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_two_pass_aliase_channel);

  switch(prop_id){
  case PROP_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_two_pass_aliase_channel->enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_A_AMOUNT:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_two_pass_aliase_channel->a_amount);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_A_PHASE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_two_pass_aliase_channel->a_phase);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_B_AMOUNT:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_two_pass_aliase_channel->b_amount);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_B_PHASE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_two_pass_aliase_channel->b_phase);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_two_pass_aliase_channel_dispose(GObject *gobject)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;
  
  fx_two_pass_aliase_channel = AGS_FX_TWO_PASS_ALIASE_CHANNEL(gobject);

  /* enabled */
  if(fx_two_pass_aliase_channel->enabled != NULL){
    g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->enabled));

    fx_two_pass_aliase_channel->enabled = NULL;
  }

  /* a amount */
  if(fx_two_pass_aliase_channel->a_amount != NULL){
    g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->a_amount));

    fx_two_pass_aliase_channel->a_amount = NULL;
  }

  /* a phase */
  if(fx_two_pass_aliase_channel->a_phase != NULL){
    g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->a_phase));

    fx_two_pass_aliase_channel->a_phase = NULL;
  }

  /* b amount */
  if(fx_two_pass_aliase_channel->b_amount != NULL){
    g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->b_amount));

    fx_two_pass_aliase_channel->b_amount = NULL;
  }

  /* b phase */
  if(fx_two_pass_aliase_channel->b_phase != NULL){
    g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->b_phase));

    fx_two_pass_aliase_channel->b_phase = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_two_pass_aliase_channel_parent_class)->dispose(gobject);
}

void
ags_fx_two_pass_aliase_channel_finalize(GObject *gobject)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;

  guint i;

  fx_two_pass_aliase_channel = AGS_FX_TWO_PASS_ALIASE_CHANNEL(gobject);

  /* enabled */
  if(fx_two_pass_aliase_channel->enabled != NULL){
    g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->enabled));
  }

  /* a amount */
  if(fx_two_pass_aliase_channel->a_amount != NULL){
    g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->a_amount));
  }

  /* a phase */
  if(fx_two_pass_aliase_channel->a_phase != NULL){
    g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->a_phase));
  }

  /* b amount */
  if(fx_two_pass_aliase_channel->b_amount != NULL){
    g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->b_amount));
  }

  /* b phase */
  if(fx_two_pass_aliase_channel->b_phase != NULL){
    g_object_unref(G_OBJECT(fx_two_pass_aliase_channel->b_phase));
  }
  
  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    ags_fx_two_pass_aliase_channel_input_data_free(fx_two_pass_aliase_channel->input_data[i]);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_two_pass_aliase_channel_parent_class)->finalize(gobject);
}

static AgsPluginPort*
ags_fx_two_pass_aliase_channel_get_enabled_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_TOGGLED);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_two_pass_aliase_channel_get_a_amount_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
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
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0 * M_PI);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_two_pass_aliase_channel_get_a_phase_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
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
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0 * M_PI);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_two_pass_aliase_channel_get_b_amount_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
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
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0 * M_PI);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_two_pass_aliase_channel_get_b_phase_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
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
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0 * M_PI);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

void
ags_fx_two_pass_aliase_channel_notify_buffer_size_callback(GObject *gobject,
							   GParamSpec *pspec,
							   gpointer user_data)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;

  guint buffer_size;
  guint i;
  
  GRecMutex *recall_mutex;
  
  fx_two_pass_aliase_channel = AGS_FX_TWO_PASS_ALIASE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_two_pass_aliase_channel);

  /* get buffer size */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_two_pass_aliase_channel,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxTwoPassAliaseChannelInputData *input_data;

    input_data = fx_two_pass_aliase_channel->input_data[i];

    /* buffer */
    ags_stream_free(input_data->orig_buffer);
    
    ags_stream_free(input_data->a_buffer);
    ags_stream_free(input_data->b_buffer);

    if(buffer_size > 0){
      input_data->orig_buffer = (gdouble *) ags_stream_alloc(buffer_size,
							     AGS_SOUNDCARD_DOUBLE);
      
      input_data->a_buffer = (gdouble *) ags_stream_alloc(buffer_size,
							  AGS_SOUNDCARD_DOUBLE);
      input_data->b_buffer = (gdouble *) ags_stream_alloc(buffer_size,
							  AGS_SOUNDCARD_DOUBLE);

      input_data->a_mix = (gdouble *) ags_stream_alloc(buffer_size,
						       AGS_SOUNDCARD_DOUBLE);
      input_data->b_mix = (gdouble *) ags_stream_alloc(buffer_size,
						       AGS_SOUNDCARD_DOUBLE);

      input_data->final_mix = (gdouble *) ags_stream_alloc(buffer_size,
							   AGS_SOUNDCARD_DOUBLE);
    }else{
      input_data->orig_buffer = NULL;
      
      input_data->a_buffer = NULL;
      input_data->b_buffer = NULL;

      input_data->a_mix = NULL;
      input_data->b_mix = NULL;

      input_data->final_mix = NULL;
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_two_pass_aliase_channel_input_data_alloc:
 * 
 * Allocate #AgsFxTwoPassAliaseChannelInputData-struct
 * 
 * Returns: (type gpointer) (transfer full): the new #AgsFxTwoPassAliaseChannelInputData-struct
 * 
 * Since: 3.8.0
 */
AgsFxTwoPassAliaseChannelInputData*
ags_fx_two_pass_aliase_channel_input_data_alloc()
{
  AgsFxTwoPassAliaseChannelInputData *input_data;

  input_data = (AgsFxTwoPassAliaseChannelInputData *) g_malloc(sizeof(AgsFxTwoPassAliaseChannelInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));
  
  input_data->parent = NULL;

  input_data->orig_buffer = NULL;

  input_data->a_buffer = NULL;
  input_data->b_buffer = NULL;

  input_data->a_mix = NULL;
  input_data->b_mix = NULL;

  input_data->final_mix = NULL;

  return(input_data);
}

/**
 * ags_fx_two_pass_aliase_channel_input_data_free:
 * @input_data: (type gpointer) (transfer full): the #AgsFxTwoPassAliaseChannelInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.8.0
 */
void
ags_fx_two_pass_aliase_channel_input_data_free(AgsFxTwoPassAliaseChannelInputData *input_data)
{
  if(input_data == NULL){
    return;
  }

  ags_stream_free(input_data->orig_buffer);

  ags_stream_free(input_data->a_buffer);
  ags_stream_free(input_data->b_buffer);

  ags_stream_free(input_data->a_mix);
  ags_stream_free(input_data->b_mix);

  ags_stream_free(input_data->final_mix);
  
  g_free(input_data);
}

/**
 * ags_fx_two_pass_aliase_channel_input_data_get_strct_mutex:
 * @input_data: (type gpointer) (transfer none): the #AgsFxTwoPassAliaseChannelInputData
 * 
 * Get structure mutex.
 * 
 * Returns: (type gpointer) (transfer none): the #GRecMutex to lock @input_data
 * 
 * Since: 3.8.0
 */
GRecMutex*
ags_fx_two_pass_aliase_channel_input_data_get_strct_mutex(AgsFxTwoPassAliaseChannelInputData *input_data)
{
  if(input_data == NULL){
    return(NULL);
  }

  return(AGS_FX_TWO_PASS_ALIASE_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data));
}

/**
 * ags_fx_two_pass_aliase_channel_input_get_parent:
 * @input_data: (type gpointer) (transfer none): the #AgsFxTwoPassAliaseChannelInputData-struct
 * 
 * Get parent of @input_data.
 * 
 * Returns: (type gpointer) (transfer none): the parent
 * 
 * Since: 3.8.0
 */
gpointer
ags_fx_two_pass_aliase_channel_input_get_parent(AgsFxTwoPassAliaseChannelInputData *input_data)
{
  gpointer parent;
  
  GRecMutex *input_data_mutex;
  
  if(input_data == NULL){
    return(NULL);
  }

  input_data_mutex = AGS_FX_TWO_PASS_ALIASE_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data);

  /* parent */
  g_rec_mutex_lock(input_data_mutex);

  parent = input_data->parent;
  
  g_rec_mutex_unlock(input_data_mutex);

  return(parent);
}

/**
 * ags_fx_two_pass_aliase_channel_input_get_orig_buffer:
 * @input_data: (type gpointer) (transfer none): the #AgsFxTwoPassAliaseChannelInputData-struct
 * 
 * Get orig buffer of @input_data.
 * 
 * Returns: (type gpointer) (transfer none): the orig buffer
 * 
 * Since: 3.8.0
 */
gpointer
ags_fx_two_pass_aliase_channel_input_get_orig_buffer(AgsFxTwoPassAliaseChannelInputData *input_data)
{
  gpointer orig_buffer;
  
  GRecMutex *input_data_mutex;
  
  if(input_data == NULL){
    return(NULL);
  }

  input_data_mutex = AGS_FX_TWO_PASS_ALIASE_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data);

  /* orig buffer */
  g_rec_mutex_lock(input_data_mutex);

  orig_buffer = input_data->orig_buffer;
  
  g_rec_mutex_unlock(input_data_mutex);

  return(orig_buffer);
}

/**
 * ags_fx_two_pass_aliase_channel_input_get_a_buffer:
 * @input_data: (type gpointer) (transfer none): the #AgsFxTwoPassAliaseChannelInputData-struct
 * 
 * Get a buffer of @input_data.
 * 
 * Returns: (type gpointer) (transfer none): the a buffer
 * 
 * Since: 3.8.0
 */
gpointer
ags_fx_two_pass_aliase_channel_input_get_a_buffer(AgsFxTwoPassAliaseChannelInputData *input_data)
{
  gpointer a_buffer;
  
  GRecMutex *input_data_mutex;
  
  if(input_data == NULL){
    return(NULL);
  }

  input_data_mutex = AGS_FX_TWO_PASS_ALIASE_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data);

  /* a buffer */
  g_rec_mutex_lock(input_data_mutex);

  a_buffer = input_data->a_buffer;
  
  g_rec_mutex_unlock(input_data_mutex);

  return(a_buffer);
}

/**
 * ags_fx_two_pass_aliase_channel_input_get_b_buffer:
 * @input_data: (type gpointer) (transfer none): the #AgsFxTwoPassAliaseChannelInputData-struct
 * 
 * Get b buffer of @input_data.
 * 
 * Returns: (type gpointer) (transfer none): the b buffer
 * 
 * Since: 3.8.0
 */
gpointer
ags_fx_two_pass_aliase_channel_input_get_b_buffer(AgsFxTwoPassAliaseChannelInputData *input_data)
{
  gpointer b_buffer;
  
  GRecMutex *input_data_mutex;
  
  if(input_data == NULL){
    return(NULL);
  }

  input_data_mutex = AGS_FX_TWO_PASS_ALIASE_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data);

  /* b buffer */
  g_rec_mutex_lock(input_data_mutex);

  b_buffer = input_data->b_buffer;
  
  g_rec_mutex_unlock(input_data_mutex);

  return(b_buffer);
}

/**
 * ags_fx_two_pass_aliase_channel_input_get_a_mix:
 * @input_data: (type gpointer) (transfer none): the #AgsFxTwoPassAliaseChannelInputData-struct
 * 
 * Get a mix of @input_data.
 * 
 * Returns: (type gpointer) (transfer none): the a mix
 * 
 * Since: 3.8.0
 */
gpointer
ags_fx_two_pass_aliase_channel_input_get_a_mix(AgsFxTwoPassAliaseChannelInputData *input_data)
{
  gpointer a_mix;
  
  GRecMutex *input_data_mutex;
  
  if(input_data == NULL){
    return(NULL);
  }

  input_data_mutex = AGS_FX_TWO_PASS_ALIASE_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data);

  /* a mix */
  g_rec_mutex_lock(input_data_mutex);

  a_mix = input_data->a_mix;
  
  g_rec_mutex_unlock(input_data_mutex);

  return(a_mix);
}

/**
 * ags_fx_two_pass_aliase_channel_input_get_b_mix:
 * @input_data: (type gpointer) (transfer none): the #AgsFxTwoPassAliaseChannelInputData-struct
 * 
 * Get b mix of @input_data.
 * 
 * Returns: (type gpointer) (transfer none): the b mix
 * 
 * Since: 3.8.0
 */
gpointer
ags_fx_two_pass_aliase_channel_input_get_b_mix(AgsFxTwoPassAliaseChannelInputData *input_data)
{
  gpointer b_mix;
  
  GRecMutex *input_data_mutex;
  
  if(input_data == NULL){
    return(NULL);
  }

  input_data_mutex = AGS_FX_TWO_PASS_ALIASE_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data);

  /* b mix */
  g_rec_mutex_lock(input_data_mutex);

  b_mix = input_data->b_mix;
  
  g_rec_mutex_unlock(input_data_mutex);

  return(b_mix);
}

/**
 * ags_fx_two_pass_aliase_channel_input_get_final_mix:
 * @input_data: (type gpointer) (transfer none): the #AgsFxTwoPassAliaseChannelInputData-struct
 * 
 * Get final mix of @input_data.
 * 
 * Returns: (type gpointer) (transfer none): the final mix
 * 
 * Since: 3.8.0
 */
gpointer
ags_fx_two_pass_aliase_channel_input_get_final_mix(AgsFxTwoPassAliaseChannelInputData *input_data)
{
  gpointer final_mix;
  
  GRecMutex *input_data_mutex;
  
  if(input_data == NULL){
    return(NULL);
  }

  input_data_mutex = AGS_FX_TWO_PASS_ALIASE_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data);

  /* final mix */
  g_rec_mutex_lock(input_data_mutex);

  final_mix = input_data->final_mix;
  
  g_rec_mutex_unlock(input_data_mutex);

  return(final_mix);
}

/**
 * ags_fx_two_pass_aliase_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxTwoPassAliaseChannel
 *
 * Returns: the new #AgsFxTwoPassAliaseChannel
 *
 * Since: 3.8.0
 */
AgsFxTwoPassAliaseChannel*
ags_fx_two_pass_aliase_channel_new(AgsChannel *channel)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;

  fx_two_pass_aliase_channel = (AgsFxTwoPassAliaseChannel *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL,
									  "source", channel,
									  NULL);

  return(fx_two_pass_aliase_channel);
}
