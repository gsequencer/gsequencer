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

#ifndef __AGS_WAVE_META_H__
#define __AGS_WAVE_META_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_WAVE_META                (ags_wave_meta_get_type())
#define AGS_WAVE_META(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_META, AgsWaveMeta))
#define AGS_WAVE_META_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_META, AgsWaveMetaClass))
#define AGS_IS_WAVE_META(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WAVE_META))
#define AGS_IS_WAVE_META_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WAVE_META))
#define AGS_WAVE_META_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_WAVE_META, AgsWaveMetaClass))

typedef struct _AgsWaveMeta AgsWaveMeta;
typedef struct _AgsWaveMetaClass AgsWaveMetaClass;

typedef enum{
  AGS_WAVE_META_CONNECTED   = 1,
}AgsWaveMetaFlags;

struct _AgsWaveMeta
{
  GtkVBox vbox;

  guint flags;

  GtkLabel *machine_type;
  GtkLabel *machine_name;

  GtkLabel *audio_channels;
  GtkLabel *output_pads;
  GtkLabel *input_pads;
  
  GtkLabel *editor_tool;

  GtkLabel *active_audio_channel;
  
  GtkLabel *cursor_x_position;
};

struct _AgsWaveMetaClass
{
  GtkVBoxClass vbox;
};

GType ags_wave_meta_get_type(void);

void ags_wave_meta_refresh(AgsWaveMeta *wave_meta);

AgsWaveMeta* ags_wave_meta_new();

G_END_DECLS

#endif /*__AGS_WAVE_META_H__*/
