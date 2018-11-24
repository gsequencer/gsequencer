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

#ifndef __AGS_APPLY_SOUND_CONFIG_H__
#define __AGS_APPLY_SOUND_CONFIG_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#define AGS_TYPE_APPLY_SOUND_CONFIG                (ags_apply_sound_config_get_type())
#define AGS_APPLY_SOUND_CONFIG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_SOUND_CONFIG, AgsApplySoundConfig))
#define AGS_APPLY_SOUND_CONFIG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_SOUND_CONFIG, AgsApplySoundConfigClass))
#define AGS_IS_APPLY_SOUND_CONFIG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_SOUND_CONFIG))
#define AGS_IS_APPLY_SOUND_CONFIG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_SOUND_CONFIG))
#define AGS_APPLY_SOUND_CONFIG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_SOUND_CONFIG, AgsApplySoundConfigClass))

typedef struct _AgsApplySoundConfig AgsApplySoundConfig;
typedef struct _AgsApplySoundConfigClass AgsApplySoundConfigClass;

struct _AgsApplySoundConfig
{
  AgsTask task;

  gchar *config_data;
};

struct _AgsApplySoundConfigClass
{
  AgsTaskClass task;
};

GType ags_apply_sound_config_get_type();

AgsApplySoundConfig* ags_apply_sound_config_new(gchar *config_data);

#endif /*__AGS_APPLY_SOUND_CONFIG_H__*/
