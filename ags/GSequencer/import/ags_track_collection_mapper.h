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

#ifndef __AGS_TRACK_COLLECTION_MAPPER_H__
#define __AGS_TRACK_COLLECTION_MAPPER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_TRACK_COLLECTION_MAPPER                (ags_track_collection_mapper_get_type())
#define AGS_TRACK_COLLECTION_MAPPER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TRACK_COLLECTION_MAPPER, AgsTrackCollectionMapper))
#define AGS_TRACK_COLLECTION_MAPPER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TRACK_COLLECTION_MAPPER, AgsTrackCollectionMapperClass))
#define AGS_IS_TRACK_COLLECTION_MAPPER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TRACK_COLLECTION_MAPPER))
#define AGS_IS_TRACK_COLLECTION_MAPPER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TRACK_COLLECTION_MAPPER))
#define AGS_TRACK_COLLECTION_MAPPER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_TRACK_COLLECTION_MAPPER, AgsTrackCollectionMapperClass))

#define AGS_TRACK_COLLECTION_MAPPER_DEFAULT_BEATS (16.0)
#define AGS_MIDI_DEFAULT_BEATS (4.0)

typedef struct _AgsTrackCollectionMapper AgsTrackCollectionMapper;
typedef struct _AgsTrackCollectionMapperClass AgsTrackCollectionMapperClass;

struct _AgsTrackCollectionMapper
{
  GtkGrid grid;

  gchar *instrument;
  gchar *sequence;
  
  GList *track;

  GList *notation;
  
  GtkCheckButton *enabled;

  GtkBox *info;
  
  GtkComboBoxText *machine_type;
  GtkSpinButton *audio_channels;
  GtkSpinButton *offset;
};

struct _AgsTrackCollectionMapperClass
{
  GtkGridClass grid;
};

GType ags_track_collection_mapper_get_type(void);

GList* ags_track_collection_mapper_find_instrument_with_sequence(GList *track_collection_mapper,
								 gchar *instrument, gchar *sequence);

void ags_track_collection_mapper_map(AgsTrackCollectionMapper *track_collection_mapper);

AgsTrackCollectionMapper* ags_track_collection_mapper_new();

G_END_DECLS

#endif /*__AGS_TRACK_COLLECTION_MAPPER_H__*/
