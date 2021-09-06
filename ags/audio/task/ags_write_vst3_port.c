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

#include <ags/audio/task/ags_write_vst3_port.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/i18n.h>

void ags_write_vst3_port_class_init(AgsWriteVst3PortClass *write_vst3_port);
void ags_write_vst3_port_init(AgsWriteVst3Port *write_vst3_port);
void ags_write_vst3_port_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_write_vst3_port_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_write_vst3_port_dispose(GObject *gobject);
void ags_write_vst3_port_finalize(GObject *gobject);

void ags_write_vst3_port_launch(AgsTask *task);

/**
 * SECTION:ags_write_vst3_port
 * @short_description: start audio object to audio loop
 * @title: AgsWriteVst3Port
 * @section_id:
 * @include: ags/audio/task/ags_write_vst3_port.h
 *
 * The #AgsWriteVst3Port task starts #AgsAudio to #AgsAudioLoop.
 */

static gpointer ags_write_vst3_port_parent_class = NULL;

enum{
  PROP_0,
  PROP_FX_VST3_AUDIO,
  PROP_PORT,
  PROP_VALUE,
  PROP_SOUND_SCOPE,
  PROP_AUDIO_CHANNEL,
  PROP_DO_REPLACE,
};

GType
ags_write_vst3_port_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_write_vst3_port = 0;

    static const GTypeInfo ags_write_vst3_port_info = {
      sizeof(AgsWriteVst3PortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_write_vst3_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsWriteVst3Port),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_write_vst3_port_init,
    };

    ags_type_write_vst3_port = g_type_register_static(AGS_TYPE_TASK,
						      "AgsWriteVst3Port",
						      &ags_write_vst3_port_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_write_vst3_port);
  }

  return g_define_type_id__volatile;
}

void
ags_write_vst3_port_class_init(AgsWriteVst3PortClass *write_vst3_port)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_write_vst3_port_parent_class = g_type_class_peek_parent(write_vst3_port);

  /* gobject */
  gobject = (GObjectClass *) write_vst3_port;

  gobject->set_property = ags_write_vst3_port_set_property;
  gobject->get_property = ags_write_vst3_port_get_property;

  gobject->dispose = ags_write_vst3_port_dispose;
  gobject->finalize = ags_write_vst3_port_finalize;

  /* properties */
  /**
   * AgsWriteVst3Port:fx-vst3-audio:
   *
   * The assigned #AgsFxVst3Audio
   * 
   * Since: 3.11.0
   */
  param_spec = g_param_spec_object("fx-vst3-audio",
				   i18n_pspec("ags-fx-vst3 audio object"),
				   i18n_pspec("The ags-vst-vst3 audio object"),
				   AGS_TYPE_FX_VST3_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FX_VST3_AUDIO,
				  param_spec);

  /**
   * AgsWriteVst3Port:port:
   *
   * The assigned #AgsPort
   * 
   * Since: 3.11.0
   */
  param_spec = g_param_spec_object("port",
				   i18n_pspec("port"),
				   i18n_pspec("The port to write"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsWriteVst3Port:value:
   *
   * The effects value.
   * 
   * Since: 3.11.0
   */
  param_spec =  g_param_spec_double("value",
				    i18n_pspec("value"),
				    i18n_pspec("The value to write"),
				    -1.0 * G_MAXDOUBLE,
				    G_MAXDOUBLE,
				    0.0,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VALUE,
				  param_spec);
  
  /**
   * AgsWriteVst3Port:sound-scope:
   *
   * The effects sound-scope.
   * 
   * Since: 3.11.0
   */
  param_spec =  g_param_spec_int("sound-scope",
				 i18n_pspec("sound scope"),
				 i18n_pspec("The sound scope"),
				 -1,
				 AGS_SOUND_SCOPE_LAST,
				 -1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUND_SCOPE,
				  param_spec);

  /**
   * AgsWriteVst3Port:audio-channel:
   *
   * The effects audio channel.
   * 
   * Since: 3.11.0
   */
  param_spec =  g_param_spec_int("audio-channel",
				 i18n_pspec("audio channel"),
				 i18n_pspec("The audio channel"),
				 -1,
				 G_MAXINT,
				 -1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);
  
  /**
   * AgsWriteVst3Port:do-replace:
   *
   * Do replace.
   * 
   * Since: 3.11.0
   */
  param_spec =  g_param_spec_boolean("do-replace",
				     i18n_pspec("do replace"),
				     i18n_pspec("Do replace"),
				     TRUE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);
  
  /* task */
  task = (AgsTaskClass *) write_vst3_port;

  task->launch = ags_write_vst3_port_launch;
}

void
ags_write_vst3_port_init(AgsWriteVst3Port *write_vst3_port)
{
  write_vst3_port->fx_vst3_audio = NULL;

  write_vst3_port->sound_scope = -1;
  write_vst3_port->audio_channel = -1;

  write_vst3_port->do_replace = TRUE;
}

void
ags_write_vst3_port_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsWriteVst3Port *write_vst3_port;

  write_vst3_port = AGS_WRITE_VST3_PORT(gobject);

  switch(prop_id){
  case PROP_FX_VST3_AUDIO:
  {
    AgsFxVst3Audio *fx_vst3_audio;

    fx_vst3_audio = (AgsFxVst3Audio *) g_value_get_object(value);

    if(write_vst3_port->fx_vst3_audio == fx_vst3_audio){
      return;
    }

    if(write_vst3_port->fx_vst3_audio != NULL){
      g_object_unref(write_vst3_port->fx_vst3_audio);
    }

    if(fx_vst3_audio != NULL){
      g_object_ref(fx_vst3_audio);
    }

    write_vst3_port->fx_vst3_audio = fx_vst3_audio;
  }
  break;
  case PROP_PORT:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    if(write_vst3_port->port == port){
      return;
    }

    if(write_vst3_port->port != NULL){
      g_object_unref(write_vst3_port->port);
    }

    if(port != NULL){
      g_object_ref(port);
    }

    write_vst3_port->port = port;
  }
  break;
  case PROP_VALUE:
  {
    write_vst3_port->value = g_value_get_int(value);
  }
  break;
  case PROP_SOUND_SCOPE:
  {
    write_vst3_port->sound_scope = g_value_get_int(value);
  }
  break;
  case PROP_AUDIO_CHANNEL:
  {
    write_vst3_port->audio_channel = g_value_get_int(value);
  }
  break;
  case PROP_DO_REPLACE:
  {
    write_vst3_port->do_replace = g_value_get_int(value);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_write_vst3_port_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsWriteVst3Port *write_vst3_port;

  write_vst3_port = AGS_WRITE_VST3_PORT(gobject);

  switch(prop_id){
  case PROP_FX_VST3_AUDIO:
  {
    g_value_set_object(value, write_vst3_port->fx_vst3_audio);
  }
  break;
  case PROP_PORT:
  {
    g_value_set_object(value, write_vst3_port->port);
  }
  break;
  case PROP_VALUE:
  {
    g_value_set_int(value, write_vst3_port->value);
  }
  break;
  case PROP_SOUND_SCOPE:
  {
    g_value_set_int(value, write_vst3_port->sound_scope);
  }
  break;
  case PROP_AUDIO_CHANNEL:
  {
    g_value_set_int(value, write_vst3_port->audio_channel);
  }
  break;
  case PROP_DO_REPLACE:
  {
    g_value_set_int(value, write_vst3_port->do_replace);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_write_vst3_port_dispose(GObject *gobject)
{
  AgsWriteVst3Port *write_vst3_port;

  write_vst3_port = AGS_WRITE_VST3_PORT(gobject);

  if(write_vst3_port->fx_vst3_audio != NULL){
    g_object_unref(write_vst3_port->fx_vst3_audio);

    write_vst3_port->fx_vst3_audio = NULL;
  }

  if(write_vst3_port->port != NULL){
    g_object_unref(write_vst3_port->port);

    write_vst3_port->port = NULL;
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_write_vst3_port_parent_class)->dispose(gobject);
}

void
ags_write_vst3_port_finalize(GObject *gobject)
{
  AgsWriteVst3Port *write_vst3_port;

  write_vst3_port = AGS_WRITE_VST3_PORT(gobject);

  if(write_vst3_port->fx_vst3_audio != NULL){
    g_object_unref(write_vst3_port->fx_vst3_audio);
  }

  if(write_vst3_port->port != NULL){
    g_object_unref(write_vst3_port->port);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_write_vst3_port_parent_class)->finalize(gobject);
}

void
ags_write_vst3_port_launch(AgsTask *task)
{
  AgsWriteVst3Port *write_vst3_port;
  
  write_vst3_port = AGS_WRITE_VST3_PORT(task);

  g_return_if_fail(AGS_IS_FX_VST3_AUDIO(write_vst3_port->fx_vst3_audio));
  g_return_if_fail(AGS_IS_PORT(write_vst3_port->port));

  //TODO:JK: implement me
}

/**
 * ags_write_vst3_port_new:
 * @fx_vst3_audio: the #AgsFxVst3Audio the port belongs to
 * @port: the #AgsPort to be written
 * @value: the value to write
 * @sound_scope: the #AgsSoundScope-enum or -1 for all
 * @audio_channel: the specific audio channel or -1 for all
 * @do_replace: do replace any prior added tasks of task launcher
 *
 * Create a new instance of #AgsWriteVst3Port.
 *
 * Returns: the new #AgsWriteVst3Port.
 *
 * Since: 3.11.0
 */
AgsWriteVst3Port*
ags_write_vst3_port_new(AgsFxVst3Audio *fx_vst3_audio,
			AgsPort *port,
			gdouble value,
			gint sound_scope,
			gint audio_channel,
			gboolean do_replace)
{
  AgsWriteVst3Port *write_vst3_port;

  write_vst3_port = (AgsWriteVst3Port *) g_object_new(AGS_TYPE_WRITE_VST3_PORT,
						      "fx-vst3-audio", fx_vst3_audio,
						      "port", port,
						      "value", value,
						      "sound-scope", sound_scope,
						      "audio-channel", audio_channel,
						      "do-replace", do_replace,
						      NULL);
  
  return(write_vst3_port);
}
