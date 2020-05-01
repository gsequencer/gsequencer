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

#include <ags/audio/fx/ags_fx_envelope_channel.h>

#include <complex.h>

#include <ags/i18n.h>

void ags_fx_envelope_channel_class_init(AgsFxEnvelopeChannelClass *fx_envelope_channel);
void ags_fx_envelope_channel_init(AgsFxEnvelopeChannel *fx_envelope_channel);
void ags_fx_envelope_channel_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_fx_envelope_channel_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_fx_envelope_channel_dispose(GObject *gobject);
void ags_fx_envelope_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_envelope_channel
 * @short_description: fx envelope channel
 * @title: AgsFxEnvelopeChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_envelope_channel.h
 *
 * The #AgsFxEnvelopeChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_envelope_channel_parent_class = NULL;

static const gchar *ags_fx_envelope_channel_plugin_name = "ags-fx-envelope";

static const gchar *ags_fx_envelope_channel_specifier[] = {
  "./fixed-length[0]",
  "./attack[0]",
  "./decay[0]",
  "./sustain[0]",
  "./release[0]",
  "./ratio[0]",
  NULL,
};

static const gchar *ags_fx_envelope_channel_control_port[] = {
  "1/6",
  "2/6",
  "3/6",
  "4/6",
  "5/6",
  "6/6",
  NULL,
};

enum{
  PROP_0,
  PROP_FIXED_LENGTH,
  PROP_ATTACK,
  PROP_DECAY,
  PROP_SUSTAIN,
  PROP_RELEASE,
  PROP_RATIO,
};

GType
ags_fx_envelope_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_envelope_channel = 0;

    static const GTypeInfo ags_fx_envelope_channel_info = {
      sizeof (AgsFxEnvelopeChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_envelope_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxEnvelopeChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_envelope_channel_init,
    };

    ags_type_fx_envelope_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							  "AgsFxEnvelopeChannel",
							  &ags_fx_envelope_channel_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_envelope_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_envelope_channel_class_init(AgsFxEnvelopeChannelClass *fx_envelope_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_envelope_channel_parent_class = g_type_class_peek_parent(fx_envelope_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_envelope_channel;

  gobject->set_property = ags_fx_envelope_channel_set_property;
  gobject->get_property = ags_fx_envelope_channel_get_property;

  gobject->dispose = ags_fx_envelope_channel_dispose;
  gobject->finalize = ags_fx_envelope_channel_finalize;

  /* properties */
  /**
   * AgsFxEnvelopeChannel:fixed-length:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("fixed-length",
				   i18n_pspec("fixed length of recall"),
				   i18n_pspec("The recall's fixed length"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIXED_LENGTH,
				  param_spec);

  /**
   * AgsFxEnvelopeChannel:attack:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("attack",
				   i18n_pspec("attack of recall"),
				   i18n_pspec("The recall's attack"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /**
   * AgsFxEnvelopeChannel:decay:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("decay",
				   i18n_pspec("decay of recall"),
				   i18n_pspec("The recall's decay"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DECAY,
				  param_spec);

  /**
   * AgsFxEnvelopeChannel:sustain:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("sustain",
				   i18n_pspec("sustain of recall"),
				   i18n_pspec("The recall's sustain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SUSTAIN,
				  param_spec);

  /**
   * AgsFxEnvelopeChannel:release:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("release",
				   i18n_pspec("release of recall"),
				   i18n_pspec("The recall's release"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RELEASE,
				  param_spec);

  /**
   * AgsFxEnvelopeChannel:ratio:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("ratio",
				   i18n_pspec("ratio of recall"),
				   i18n_pspec("The recall's ratio"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RATIO,
				  param_spec);
}

void
ags_fx_envelope_channel_init(AgsFxEnvelopeChannel *fx_envelope_channel)
{
  AGS_RECALL(fx_envelope_channel)->name = "ags-fx-envelope";
  AGS_RECALL(fx_envelope_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_envelope_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_envelope_channel)->xml_type = "ags-fx-envelope-channel";

  fx_envelope_channel->envelope_mode = AGS_FX_ENVELOPE_CHANNEL_MODE_STREAM_LENGTH;

  /* fixed length */
  fx_envelope_channel->fixed_length = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_envelope_channel_plugin_name,
						   "specifier", ags_fx_envelope_channel_specifier[0],
						   "control-port", ags_fx_envelope_channel_control_port[0],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_envelope_channel->fixed_length->port_value.ags_port_float = (gfloat) -1.0;

  ags_recall_add_port((AgsRecall *) fx_envelope_channel,
		      fx_envelope_channel->fixed_length);

  /* attack */
  fx_envelope_channel->attack = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_envelope_channel_plugin_name,
					     "specifier", ags_fx_envelope_channel_specifier[1],
					     "control-port", ags_fx_envelope_channel_control_port[1],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", AGS_TYPE_COMPLEX,
					     "port-value-size", sizeof(AgsComplex),
					     "port-value-length", 1,
					     NULL);

  ags_complex_set(&(fx_envelope_channel->attack->port_value.ags_port_complex),
		  1.0 + 0.25 * I);

  ags_recall_add_port((AgsRecall *) fx_envelope_channel,
		      fx_envelope_channel->attack);

  /* decay */
  fx_envelope_channel->decay = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_fx_envelope_channel_plugin_name,
					    "specifier", ags_fx_envelope_channel_specifier[2],
					    "control-port", ags_fx_envelope_channel_control_port[2],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", AGS_TYPE_COMPLEX,
					    "port-value-size", sizeof(AgsComplex),
					    "port-value-length", 1,
					    NULL);
  
  ags_complex_set(&(fx_envelope_channel->decay->port_value.ags_port_complex),
		  1.0 + 0.25 * I);

  ags_recall_add_port((AgsRecall *) fx_envelope_channel,
		      fx_envelope_channel->decay);

  /* sustain */
  fx_envelope_channel->sustain = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_fx_envelope_channel_plugin_name,
					      "specifier", ags_fx_envelope_channel_specifier[3],
					      "control-port", ags_fx_envelope_channel_control_port[3],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", AGS_TYPE_COMPLEX,
					      "port-value-size", sizeof(AgsComplex),
					      "port-value-length", 1,
					      NULL);
  
  ags_complex_set(&(fx_envelope_channel->sustain->port_value.ags_port_complex),
		  1.0 + 0.25 * I);

  ags_recall_add_port((AgsRecall *) fx_envelope_channel,
		      fx_envelope_channel->sustain);
  
  /* release */
  fx_envelope_channel->release = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_fx_envelope_channel_plugin_name,
					      "specifier", ags_fx_envelope_channel_specifier[4],
					      "control-port", ags_fx_envelope_channel_control_port[4],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", AGS_TYPE_COMPLEX,
					      "port-value-size", sizeof(AgsComplex),
					      "port-value-length", 1,
					      NULL);
  
  ags_complex_set(&(fx_envelope_channel->release->port_value.ags_port_complex),
		  1.0 + 0.25 * I);

  ags_recall_add_port((AgsRecall *) fx_envelope_channel,
		      fx_envelope_channel->release);

  /* ratio */
  fx_envelope_channel->ratio = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_fx_envelope_channel_plugin_name,
					    "specifier", ags_fx_envelope_channel_specifier[5],
					    "control-port", ags_fx_envelope_channel_control_port[5],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", AGS_TYPE_COMPLEX,
					    "port-value-size", sizeof(AgsComplex),
					    "port-value-length", 1,
					    NULL);
  
  ags_complex_set(&(fx_envelope_channel->ratio->port_value.ags_port_complex),
		  1.0 + 0.25 * I);

  ags_recall_add_port((AgsRecall *) fx_envelope_channel,
		      fx_envelope_channel->ratio);
}

void
ags_fx_envelope_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsFxEnvelopeChannel *fx_envelope_channel;

  GRecMutex *recall_mutex;

  fx_envelope_channel = AGS_FX_ENVELOPE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_envelope_channel);

  switch(prop_id){
  case PROP_FIXED_LENGTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_envelope_channel->fixed_length){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_envelope_channel->fixed_length != NULL){
      g_object_unref(G_OBJECT(fx_envelope_channel->fixed_length));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_envelope_channel->fixed_length = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_ATTACK:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_envelope_channel->attack){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_envelope_channel->attack != NULL){
      g_object_unref(G_OBJECT(fx_envelope_channel->attack));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_envelope_channel->attack = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_DECAY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_envelope_channel->decay){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_envelope_channel->decay != NULL){
      g_object_unref(G_OBJECT(fx_envelope_channel->decay));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_envelope_channel->decay = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SUSTAIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_envelope_channel->sustain){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_envelope_channel->sustain != NULL){
      g_object_unref(G_OBJECT(fx_envelope_channel->sustain));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_envelope_channel->sustain = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_RELEASE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_envelope_channel->release){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_envelope_channel->release != NULL){
      g_object_unref(G_OBJECT(fx_envelope_channel->release));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_envelope_channel->release = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_RATIO:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_envelope_channel->ratio){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_envelope_channel->ratio != NULL){
      g_object_unref(G_OBJECT(fx_envelope_channel->ratio));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_envelope_channel->ratio = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_envelope_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsFxEnvelopeChannel *fx_envelope_channel;

  GRecMutex *recall_mutex;

  fx_envelope_channel = AGS_FX_ENVELOPE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_envelope_channel);

  switch(prop_id){
  case PROP_FIXED_LENGTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_envelope_channel->fixed_length);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_ATTACK:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_envelope_channel->attack);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_DECAY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_envelope_channel->decay);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SUSTAIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_envelope_channel->sustain);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_RELEASE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_envelope_channel->release);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_RATIO:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_envelope_channel->ratio);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_envelope_channel_dispose(GObject *gobject)
{
  AgsFxEnvelopeChannel *fx_envelope_channel;
  
  fx_envelope_channel = AGS_FX_ENVELOPE_CHANNEL(gobject);
  
  /* fixed length */
  if(fx_envelope_channel->fixed_length != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->fixed_length));

    fx_envelope_channel->fixed_length = NULL;
  }  

  /* attack */
  if(fx_envelope_channel->attack != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->attack));

    fx_envelope_channel->attack = NULL;
  }  

  /* decay */
  if(fx_envelope_channel->decay != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->decay));

    fx_envelope_channel->decay = NULL;
  }  

  /* sustain */
  if(fx_envelope_channel->sustain != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->sustain));

    fx_envelope_channel->sustain = NULL;
  }  

  /* release */
  if(fx_envelope_channel->release != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->release));

    fx_envelope_channel->release = NULL;
  }  

  /* ratio */
  if(fx_envelope_channel->ratio != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->ratio));

    fx_envelope_channel->ratio = NULL;
  }  

  /* call parent */
  G_OBJECT_CLASS(ags_fx_envelope_channel_parent_class)->dispose(gobject);
}

void
ags_fx_envelope_channel_finalize(GObject *gobject)
{
  AgsFxEnvelopeChannel *fx_envelope_channel;
  
  fx_envelope_channel = AGS_FX_ENVELOPE_CHANNEL(gobject);

  /* fixed length */
  if(fx_envelope_channel->fixed_length != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->fixed_length));
  }

  /* attack */
  if(fx_envelope_channel->attack != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->attack));
  }

  /* decay */
  if(fx_envelope_channel->decay != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->decay));
  }

  /* sustain */
  if(fx_envelope_channel->sustain != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->sustain));
  }

  /* release */
  if(fx_envelope_channel->release != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->release));
  }

  /* ratio */
  if(fx_envelope_channel->ratio != NULL){
    g_object_unref(G_OBJECT(fx_envelope_channel->ratio));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_envelope_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_envelope_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxEnvelopeChannel
 *
 * Returns: the new #AgsFxEnvelopeChannel
 *
 * Since: 3.3.0
 */
AgsFxEnvelopeChannel*
ags_fx_envelope_channel_new(AgsChannel *channel)
{
  AgsFxEnvelopeChannel *fx_envelope_channel;

  fx_envelope_channel = (AgsFxEnvelopeChannel *) g_object_new(AGS_TYPE_FX_ENVELOPE_CHANNEL,
							      "source", channel,
							      NULL);

  return(fx_envelope_channel);
}
