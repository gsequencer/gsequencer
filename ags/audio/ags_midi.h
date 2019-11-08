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

#ifndef __AGS_MIDI_H__
#define __AGS_MIDI_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/libags.h>

#include <ags/audio/ags_track.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI                (ags_midi_get_type())
#define AGS_MIDI(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI, AgsMidi))
#define AGS_MIDI_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI, AgsMidiClass))
#define AGS_IS_MIDI(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI))
#define AGS_IS_MIDI_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI))
#define AGS_MIDI_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_MIDI, AgsMidiClass))

#define AGS_MIDI_GET_OBJ_MUTEX(obj) (&(((AgsMidi *) obj)->obj_mutex))

#define AGS_MIDI_DEFAULT_BPM (120.0)

#define AGS_MIDI_TICS_PER_BEAT (1.0)
#define AGS_MIDI_MINIMUM_TRACK_LENGTH (1.0 / 16.0)

#define AGS_MIDI_DEFAULT_LENGTH (65535.0 / AGS_MIDI_TICS_PER_BEAT - AGS_MIDI_MINIMUM_TRACK_LENGTH)
#define AGS_MIDI_DEFAULT_JIFFIE (60.0 / AGS_MIDI_DEFAULT_BPM / AGS_MIDI_TICS_PER_BEAT)
#define AGS_MIDI_DEFAULT_DURATION (AGS_MIDI_DEFAULT_LENGTH * AGS_MIDI_DEFAULT_JIFFIE * USEC_PER_SEC)
#define AGS_MIDI_DEFAULT_OFFSET (64 * (1 / AGS_MIDI_MINIMUM_TRACK_LENGTH))

#define AGS_MIDI_CLIPBOARD_VERSION "1.4.0"
#define AGS_MIDI_CLIPBOARD_TYPE "AgsMidiClipboardXml"
#define AGS_MIDI_CLIPBOARD_FORMAT "AgsMidiNativeLevel"

typedef struct _AgsMidi AgsMidi;
typedef struct _AgsMidiClass AgsMidiClass;

/**
 * AgsMidiFlags:
 * @AGS_MIDI_BYPASS: ignore any midi data
 * 
 * Enum values to control the behavior or indicate internal state of #AgsMidi by
 * enable/disable as flags.
 */
typedef enum{
  AGS_MIDI_BYPASS            = 1,
}AgsMidiFlags;

struct _AgsMidi
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  GObject *audio;
  guint audio_channel;

  AgsTimestamp *timestamp;
  
  GList *track;  
  GList *selection;
};

struct _AgsMidiClass
{
  GObjectClass gobject;
};

GType ags_midi_get_type(void);

gboolean ags_midi_test_flags(AgsMidi *midi, guint flags);
void ags_midi_set_flags(AgsMidi *midi, guint flags);
void ags_midi_unset_flags(AgsMidi *midi, guint flags);

GList* ags_midi_find_near_timestamp(GList *midi, guint audio_channel,
				    AgsTimestamp *timestamp);

GList* ags_midi_add(GList *midi,
		    AgsMidi *new_midi);

void ags_midi_add_track(AgsMidi *midi,
			 AgsTrack *track,
			 gboolean use_selection_list);
void ags_midi_remove_track(AgsMidi *midi,
			    AgsTrack *track,
			    gboolean use_selection_list);

AgsMidi* ags_midi_new(GObject *audio,
		      guint audio_channel);

G_END_DECLS

#endif /*__AGS_MIDI_H__*/
