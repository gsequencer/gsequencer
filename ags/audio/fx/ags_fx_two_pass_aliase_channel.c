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

#include <ags/audio/task/ags_reset_fx_two_pass_aliase.h>

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

static AgsPluginPort* ags_fx_volume_channel_get_a_amount_plugin_port();
static AgsPluginPort* ags_fx_volume_channel_get_a_phase_plugin_port();
static AgsPluginPort* ags_fx_volume_channel_get_b_amount_plugin_port();
static AgsPluginPort* ags_fx_volume_channel_get_b_phase_plugin_port();

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
  "./a-amount[0]",
  "./a-phase[0]",
  "./b-amount[0]",
  "./b-phase[0]",
  NULL,
};

const gchar* ags_fx_two_pass_aliase_channel_control_port[] = {
  "1/4",
  "2/4",
  "3/4",
  "4/4",
  NULL,
};

enum{
  PROP_0,
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
  AGS_RECALL(fx_two_pass_aliase_channel)->name = "ags-fx-two-pass-aliase";
  AGS_RECALL(fx_two_pass_aliase_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_two_pass_aliase_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_two_pass_aliase_channel)->xml_type = "ags-fx-two-pass-aliase-channel";

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
  
  fx_two_pass_aliase_channel->a_amount->port_value.ags_port_float = (gfloat) FALSE;

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
  
  fx_two_pass_aliase_channel->a_phase->port_value.ags_port_float = (gfloat) FALSE;

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
  
  fx_two_pass_aliase_channel->b_amount->port_value.ags_port_float = (gfloat) FALSE;

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
  
  fx_two_pass_aliase_channel->b_phase->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_two_pass_aliase_channel->b_phase,
	       "plugin-port", ags_fx_two_pass_aliase_channel_get_b_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_two_pass_aliase_channel,
		      fx_two_pass_aliase_channel->b_phase);
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

  /* call parent */
  G_OBJECT_CLASS(ags_fx_two_pass_aliase_channel_parent_class)->dispose(gobject);
}

void
ags_fx_two_pass_aliase_channel_finalize(GObject *gobject)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;

  fx_two_pass_aliase_channel = AGS_FX_TWO_PASS_ALIASE_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_two_pass_aliase_channel_parent_class)->finalize(gobject);
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
