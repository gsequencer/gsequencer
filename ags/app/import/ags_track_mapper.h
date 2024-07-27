/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_TRACK_MAPPER_H__
#define __AGS_TRACK_MAPPER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_TRACK_MAPPER                (ags_track_mapper_get_type())
#define AGS_TRACK_MAPPER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TRACK_MAPPER, AgsTrackMapper))
#define AGS_TRACK_MAPPER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TRACK_MAPPER, AgsTrackMapperClass))
#define AGS_IS_TRACK_MAPPER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TRACK_MAPPER))
#define AGS_IS_TRACK_MAPPER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TRACK_MAPPER))
#define AGS_TRACK_MAPPER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_TRACK_MAPPER, AgsTrackMapperClass))

#define AGS_TRACK_MAPPER_DEFAULT_BEATS (16.0)
#define AGS_MIDI_DEFAULT_BEATS (4.0)

typedef struct _AgsTrackMapper AgsTrackMapper;
typedef struct _AgsTrackMapperClass AgsTrackMapperClass;

struct _AgsTrackMapper
{
  GtkGrid grid;

  gchar *instrument;
  gchar *sequence;
  
  GList *track;

  GList *notation;
  
  GtkCheckButton *enabled;

  GtkBox *info;

  GtkLabel *info_instrument;
  GtkLabel *info_sequence;
  
  GtkComboBoxText *machine_type;

  GtkSpinButton *midi_channel;
  
  GtkSpinButton *audio_channels;

  GtkSpinButton *offset;
};

struct _AgsTrackMapperClass
{
  GtkGridClass grid;
};

GType ags_track_mapper_get_type(void);

GList* ags_track_mapper_find_instrument_with_sequence(GList *track_mapper,
						      gchar *instrument, gchar *sequence);
GList* ags_track_mapper_find_midi_channel(GList *track_mapper,
					  gint midi_channel);

void ags_track_mapper_map(AgsTrackMapper *track_mapper);

AgsTrackMapper* ags_track_mapper_new();

G_END_DECLS

#endif /*__AGS_TRACK_MAPPER_H__*/
