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

#ifndef __AGS_INSTANTIATE_VST3_PLUGIN_H__
#define __AGS_INSTANTIATE_VST3_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/fx/ags_fx_vst3_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_INSTANTIATE_VST3_PLUGIN                (ags_instantiate_vst3_plugin_get_type())
#define AGS_INSTANTIATE_VST3_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INSTANTIATE_VST3_PLUGIN, AgsInstantiateVst3Plugin))
#define AGS_INSTANTIATE_VST3_PLUGIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INSTANTIATE_VST3_PLUGIN, AgsInstantiateVst3PluginClass))
#define AGS_IS_INSTANTIATE_VST3_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_INSTANTIATE_VST3_PLUGIN))
#define AGS_IS_INSTANTIATE_VST3_PLUGIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_INSTANTIATE_VST3_PLUGIN))
#define AGS_INSTANTIATE_VST3_PLUGIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INSTANTIATE_VST3_PLUGIN, AgsInstantiateVst3PluginClass))

typedef struct _AgsInstantiateVst3Plugin AgsInstantiateVst3Plugin;
typedef struct _AgsInstantiateVst3PluginClass AgsInstantiateVst3PluginClass;

struct _AgsInstantiateVst3Plugin
{
  AgsTask task;

  AgsFxVst3Audio *fx_vst3_audio;

  gint sound_scope;
  gint audio_channel;

  gboolean do_replace;
};

struct _AgsInstantiateVst3PluginClass
{
  AgsTaskClass task;
};

GType ags_instantiate_vst3_plugin_get_type();

AgsInstantiateVst3Plugin* ags_instantiate_vst3_plugin_new(AgsFxVst3Audio *fx_vst3_audio,
							  gint sound_scope,
							  gint audio_channel,
							  gboolean do_replace);

G_END_DECLS

#endif /*__AGS_INSTANTIATE_VST3_PLUGIN_H__*/
