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

#include <ags/audio/task/ags_instantiate_vst3_plugin.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/i18n.h>

void ags_instantiate_vst3_plugin_class_init(AgsInstantiateVst3PluginClass *instantiate_vst3_plugin);
void ags_instantiate_vst3_plugin_init(AgsInstantiateVst3Plugin *instantiate_vst3_plugin);
void ags_instantiate_vst3_plugin_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec);
void ags_instantiate_vst3_plugin_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec);
void ags_instantiate_vst3_plugin_dispose(GObject *gobject);
void ags_instantiate_vst3_plugin_finalize(GObject *gobject);

void ags_instantiate_vst3_plugin_launch(AgsTask *task);

/**
 * SECTION:ags_instantiate_vst3_plugin
 * @short_description: start audio object to audio loop
 * @title: AgsInstantiateVst3Plugin
 * @section_id:
 * @include: ags/audio/task/ags_instantiate_vst3_plugin.h
 *
 * The #AgsInstantiateVst3Plugin task starts #AgsAudio to #AgsAudioLoop.
 */

static gpointer ags_instantiate_vst3_plugin_parent_class = NULL;

enum{
  PROP_0,
  PROP_FX_VST3_AUDIO,
  PROP_SOUND_SCOPE,
  PROP_AUDIO_CHANNEL,
  PROP_DO_REPLACE,
};

GType
ags_instantiate_vst3_plugin_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_instantiate_vst3_plugin = 0;

    static const GTypeInfo ags_instantiate_vst3_plugin_info = {
      sizeof(AgsInstantiateVst3PluginClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_instantiate_vst3_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsInstantiateVst3Plugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_instantiate_vst3_plugin_init,
    };

    ags_type_instantiate_vst3_plugin = g_type_register_static(AGS_TYPE_TASK,
							      "AgsInstantiateVst3Plugin",
							      &ags_instantiate_vst3_plugin_info,
							      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_instantiate_vst3_plugin);
  }

  return g_define_type_id__volatile;
}

void
ags_instantiate_vst3_plugin_class_init(AgsInstantiateVst3PluginClass *instantiate_vst3_plugin)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_instantiate_vst3_plugin_parent_class = g_type_class_peek_parent(instantiate_vst3_plugin);

  /* gobject */
  gobject = (GObjectClass *) instantiate_vst3_plugin;

  gobject->set_property = ags_instantiate_vst3_plugin_set_property;
  gobject->get_property = ags_instantiate_vst3_plugin_get_property;

  gobject->dispose = ags_instantiate_vst3_plugin_dispose;
  gobject->finalize = ags_instantiate_vst3_plugin_finalize;

  /* properties */
  /**
   * AgsInstantiateVst3Plugin:fx-vst3-audio:
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
   * AgsInstantiateVst3Plugin:sound-scope:
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
   * AgsInstantiateVst3Plugin:audio-channel:
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
   * AgsInstantiateVst3Plugin:do-replace:
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
  task = (AgsTaskClass *) instantiate_vst3_plugin;

  task->launch = ags_instantiate_vst3_plugin_launch;
}

void
ags_instantiate_vst3_plugin_init(AgsInstantiateVst3Plugin *instantiate_vst3_plugin)
{
  instantiate_vst3_plugin->fx_vst3_audio = NULL;

  instantiate_vst3_plugin->sound_scope = -1;
  instantiate_vst3_plugin->audio_channel = -1;

  instantiate_vst3_plugin->do_replace = TRUE;
}

void
ags_instantiate_vst3_plugin_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  instantiate_vst3_plugin = AGS_INSTANTIATE_VST3_PLUGIN(gobject);

  switch(prop_id){
  case PROP_FX_VST3_AUDIO:
  {
    AgsFxVst3Audio *fx_vst3_audio;

    fx_vst3_audio = (AgsFxVst3Audio *) g_value_get_object(value);

    if(instantiate_vst3_plugin->fx_vst3_audio == fx_vst3_audio){
      return;
    }

    if(instantiate_vst3_plugin->fx_vst3_audio != NULL){
      g_object_unref(instantiate_vst3_plugin->fx_vst3_audio);
    }

    if(fx_vst3_audio != NULL){
      g_object_ref(fx_vst3_audio);
    }

    instantiate_vst3_plugin->fx_vst3_audio = fx_vst3_audio;
  }
  break;
  case PROP_SOUND_SCOPE:
  {
    instantiate_vst3_plugin->sound_scope = g_value_get_int(value);
  }
  break;
  case PROP_AUDIO_CHANNEL:
  {
    instantiate_vst3_plugin->audio_channel = g_value_get_int(value);
  }
  break;
  case PROP_DO_REPLACE:
  {
    instantiate_vst3_plugin->do_replace = g_value_get_int(value);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_instantiate_vst3_plugin_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  instantiate_vst3_plugin = AGS_INSTANTIATE_VST3_PLUGIN(gobject);

  switch(prop_id){
  case PROP_FX_VST3_AUDIO:
  {
    g_value_set_object(value, instantiate_vst3_plugin->fx_vst3_audio);
  }
  break;
  case PROP_SOUND_SCOPE:
  {
    g_value_set_int(value, instantiate_vst3_plugin->sound_scope);
  }
  break;
  case PROP_AUDIO_CHANNEL:
  {
    g_value_set_int(value, instantiate_vst3_plugin->audio_channel);
  }
  break;
  case PROP_DO_REPLACE:
  {
    g_value_set_int(value, instantiate_vst3_plugin->do_replace);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_instantiate_vst3_plugin_dispose(GObject *gobject)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  instantiate_vst3_plugin = AGS_INSTANTIATE_VST3_PLUGIN(gobject);

  if(instantiate_vst3_plugin->fx_vst3_audio != NULL){
    g_object_unref(instantiate_vst3_plugin->fx_vst3_audio);

    instantiate_vst3_plugin->fx_vst3_audio = NULL;
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_instantiate_vst3_plugin_parent_class)->dispose(gobject);
}

void
ags_instantiate_vst3_plugin_finalize(GObject *gobject)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  instantiate_vst3_plugin = AGS_INSTANTIATE_VST3_PLUGIN(gobject);

  if(instantiate_vst3_plugin->fx_vst3_audio != NULL){
    g_object_unref(instantiate_vst3_plugin->fx_vst3_audio);
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_instantiate_vst3_plugin_parent_class)->finalize(gobject);
}

void
ags_instantiate_vst3_plugin_launch(AgsTask *task)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;
  
  instantiate_vst3_plugin = AGS_INSTANTIATE_VST3_PLUGIN(task);

  g_return_if_fail(AGS_IS_FX_VST3_AUDIO(instantiate_vst3_plugin->fx_vst3_audio));

  //TODO:JK: implement me
}

/**
 * ags_instantiate_vst3_plugin_new:
 * @fx_vst3_audio: the #AgsFxVst3Audio the port belongs to
 * @sound_scope: the #AgsSoundScope-enum or -1 for all
 * @audio_channel: the specific audio channel or -1 for all
 * @do_replace: do replace any prior added plugin instances
 *
 * Create a new instance of #AgsInstantiateVst3Plugin.
 *
 * Returns: the new #AgsInstantiateVst3Plugin.
 *
 * Since: 3.11.0
 */
AgsInstantiateVst3Plugin*
ags_instantiate_vst3_plugin_new(AgsFxVst3Audio *fx_vst3_audio,
				gint sound_scope,
				gint audio_channel,
				gboolean do_replace)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  instantiate_vst3_plugin = (AgsInstantiateVst3Plugin *) g_object_new(AGS_TYPE_INSTANTIATE_VST3_PLUGIN,
								      "fx-vst3-audio", fx_vst3_audio,
								      "sound-scope", sound_scope,
								      "audio-channel", audio_channel,
								      "do-replace", do_replace,
								      NULL);
  
  return(instantiate_vst3_plugin);
}
