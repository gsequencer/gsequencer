/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_PRESET_H__
#define __AGS_PRESET_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_PRESET                (ags_preset_get_type())
#define AGS_PRESET(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PRESET, AgsPreset))
#define AGS_PRESET_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PRESET, AgsPresetClass))
#define AGS_IS_PRESET(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PRESET))
#define AGS_IS_PRESET_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PRESET))
#define AGS_PRESET_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PRESET, AgsPresetClass))

#define AGS_PRESET_GET_OBJ_MUTEX(obj) (&(((AgsPreset *) obj)->obj_mutex))

typedef struct _AgsPreset AgsPreset;
typedef struct _AgsPresetClass AgsPresetClass;

/**
 * AgsPresetFlags:
 * @AGS_PRESET_CONNECTED: indicates the playback was connected by calling #AgsConnectable::connect()
 * 
 * Enum values to control the behavior or indicate internal state of #AgsPreset by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PRESET_ADDED_TO_REGISTRY  = 1,
  AGS_PRESET_CONNECTED          = 1 <<  1,
}AgsPresetFlags;

#define AGS_PRESET_ERROR (ags_preset_error_quark())

typedef enum{
  AGS_PRESET_ERROR_NO_SUCH_PARAMETER,
}AgsPresetError;

struct _AgsPreset
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;
  
  GObject *audio;

  gchar *scope;
  gchar *preset_name;

  guint audio_channel_start;
  guint audio_channel_end;

  guint pad_start;
  guint pad_end;

  guint x_start;
  guint x_end;
  
  guint n_params;
  gchar **parameter_name;
  GValue *value;
};

struct _AgsPresetClass
{
  GObjectClass gobject;
};

GType ags_preset_get_type();

GQuark ags_preset_error_quark();

gboolean ags_preset_test_flags(AgsPreset *preset, guint flags);
void ags_preset_set_flags(AgsPreset *preset, guint flags);
void ags_preset_unset_flags(AgsPreset *preset, guint flags);

GList* ags_preset_find_scope(GList *preset,
			     gchar *scope);
GList* ags_preset_find_name(GList *preset,
			    gchar *preset_name);

gboolean ags_preset_add_parameter(AgsPreset *preset,
				  gchar *param_name, GValue *value);
void ags_preset_remove_parameter(AgsPreset *preset,
				 guint nth);

void ags_preset_get_parameter(AgsPreset *preset,
			      gchar *param_name, GValue *value,
			      GError **error);

AgsPreset* ags_preset_new();

G_END_DECLS

#endif /*__AGS_PRESET_H__*/
