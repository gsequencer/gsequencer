/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_SET_PRESETS_H__
#define __AGS_SET_PRESETS_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_SET_PRESETS                (ags_set_presets_get_type())
#define AGS_SET_PRESETS(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SET_PRESETS, AgsSetPresets))
#define AGS_SET_PRESETS_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SET_PRESETS, AgsSetPresetsClass))
#define AGS_IS_SET_PRESETS(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SET_PRESETS))
#define AGS_IS_SET_PRESETS_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SET_PRESETS))
#define AGS_SET_PRESETS_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SET_PRESETS, AgsSetPresetsClass))

typedef struct _AgsSetPresets AgsSetPresets;
typedef struct _AgsSetPresetsClass AgsSetPresetsClass;

struct _AgsSetPresets
{
  AgsTask task;

  GObject *scope;

  guint buffer_size;
  AgsSoundcardFormat format;
  guint samplerate;
};

struct _AgsSetPresetsClass
{
  AgsTaskClass task;
};

GType ags_set_presets_get_type();

AgsSetPresets* ags_set_presets_new(GObject *scope,
				   guint buffer_size,
				   AgsSoundcardFormat format,
				   guint samplerate);

G_END_DECLS

#endif /*__AGS_SET_PRESETS_H__*/
