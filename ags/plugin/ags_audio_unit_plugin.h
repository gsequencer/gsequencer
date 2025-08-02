/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_AUDIO_UNIT_PLUGIN_H__
#define __AGS_AUDIO_UNIT_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-vst.h>

#include <ags/plugin/ags_base_plugin.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_UNIT_PLUGIN                (ags_audio_unit_plugin_get_type())
#define AGS_AUDIO_UNIT_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_UNIT_PLUGIN, AgsAudioUnitPlugin))
#define AGS_AUDIO_UNIT_PLUGIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_UNIT_PLUGIN, AgsAudioUnitPluginClass))
#define AGS_IS_AUDIO_UNIT_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_UNIT_PLUGIN))
#define AGS_IS_AUDIO_UNIT_PLUGIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_UNIT_PLUGIN))
#define AGS_AUDIO_UNIT_PLUGIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_AUDIO_UNIT_PLUGIN, AgsAudioUnitPluginClass))

typedef struct _AgsAudioUnitPlugin AgsAudioUnitPlugin;
typedef struct _AgsAudioUnitPluginClass AgsAudioUnitPluginClass;

struct _AgsAudioUnitPlugin
{
  AgsBasePlugin base_plugin;

  gpointer component;
  gpointer audio_unit;
};

struct _AgsAudioUnitPluginClass
{
  AgsBasePluginClass base_plugin;
};

GType ags_audio_unit_plugin_get_type(void);

AgsAudioUnitPlugin* ags_audio_unit_plugin_new(gchar *filename, gchar *effect, guint effect_index);

G_END_DECLS

#endif /*__AGS_AUDIO_UNIT_PLUGIN_H__*/
