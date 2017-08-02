/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_NOTE_H__
#define __AGS_NOTE_H__

#include <glib.h>
#include <glib-object.h>

#include <alsa/asoundlib.h>

#include <ags/lib/ags_time.h>
#include <ags/lib/ags_complex.h>

#define AGS_TYPE_NOTE                (ags_note_get_type())
#define AGS_NOTE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTE, AgsNote))
#define AGS_NOTE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTE, AgsNoteClass))
#define AGS_IS_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_NOTE))
#define AGS_IS_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_NOTE))
#define AGS_NOTE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NOTE, AgsNoteClass))

#define AGS_NOTE_DEFAULT_TICKS_PER_QUARTER_NOTE (16.0)

typedef struct _AgsNote AgsNote;
typedef struct _AgsNoteClass AgsNoteClass;

typedef enum{
  AGS_NOTE_CONNECTED       = 1,
  AGS_NOTE_GUI             = 1 <<  1,
  AGS_NOTE_RUNTIME         = 1 <<  2,
  AGS_NOTE_HUMAN_READABLE  = 1 <<  3,
  AGS_NOTE_DEFAULT_LENGTH  = 1 <<  4,
  AGS_NOTE_IS_SELECTED     = 1 <<  5,
  AGS_NOTE_FEED            = 1 <<  6,
  AGS_NOTE_ENVELOPE        = 1 <<  7,
}AgsNoteFlags;

struct _AgsNote
{
  GObject object;

  guint flags;

  // gui format, convert easy to visualization
  guint x[2];
  guint y;

  gdouble stream_delay;
  gdouble stream_attack;
  
  AgsComplex attack;
  AgsComplex decay;
  AgsComplex sustain;
  AgsComplex release;
  
  AgsComplex ratio;
  
  gchar *note_name;
  gdouble frequency;
};

struct _AgsNoteClass
{
  GObjectClass object;
};

GType ags_note_get_type();

gint ags_note_sort_func(gconstpointer a,
			gconstpointer b);

GList* ags_note_find_prev(GList *note,
			  guint x0, guint y);
GList* ags_note_find_next(GList *note,
			  guint x0, guint y);

glong ags_note_length_to_smf_delta_time(guint note_length,
					gdouble bpm, gdouble delay_factor,
					glong nn, glong dd, glong cc, glong bb,
					glong tempo);
guint ags_note_smf_delta_time_to_length(glong delta_time,
					glong nn, glong dd, glong cc, glong bb,
					glong tempo,
					gdouble bpm, gdouble delay_factor);

unsigned char* ags_note_to_raw_midi(AgsNote *note,
				    gdouble bpm, gdouble delay_factor,
				    guint *buffer_length);
unsigned char* ags_note_to_raw_midi_extended(AgsNote *note,
					     gdouble bpm, gdouble delay_factor,
					     glong nn, glong dd, glong cc, glong bb,
					     glong tempo,
					     guint *buffer_length);
snd_seq_event_t* ags_note_to_seq_event(AgsNote *note,
				       gdouble bpm, gdouble delay_factor,
				       guint *n_events);
snd_seq_event_t* ags_note_to_seq_event_extended(AgsNote *note,
						gdouble bpm, gdouble delay_factor,
						glong nn, glong dd, glong cc, glong bb,
						glong tempo,
						guint *n_events);

GList* ags_note_from_raw_midi(unsigned char *raw_midi,
			      gdouble bpm, gdouble delay_factor,
			      guint length);
GList* ags_note_from_raw_midi_extended(unsigned char *raw_midi,
				       glong nn, glong dd, glong cc, glong bb,
				       glong tempo,
				       gdouble bpm, gdouble delay_factor,
				       guint length);
GList* ags_note_from_seq_event(snd_seq_event_t *event,
			       gdouble bpm, gdouble delay_factor,
			       guint n_events);
GList* ags_note_from_seq_event_extended(snd_seq_event_t *event,
					glong nn, glong dd, glong cc, glong bb,
					glong tempo,
					gdouble bpm, gdouble delay_factor,
					guint n_events);

AgsNote* ags_note_duplicate(AgsNote *note);

AgsNote* ags_note_new();
AgsNote* ags_note_new_with_offset(guint x0, guint x1,
				  guint y,
				  gdouble stream_delay, guint stream_attack);

#endif /*__AGS_NOTE_H__*/
