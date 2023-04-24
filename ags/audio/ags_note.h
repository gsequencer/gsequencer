/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_NOTE                (ags_note_get_type())
#define AGS_NOTE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTE, AgsNote))
#define AGS_NOTE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTE, AgsNoteClass))
#define AGS_IS_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_NOTE))
#define AGS_IS_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_NOTE))
#define AGS_NOTE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NOTE, AgsNoteClass))

#define AGS_NOTE_GET_OBJ_MUTEX(obj) (&(((AgsNote *) obj)->obj_mutex))

#define AGS_NOTE_DEFAULT_TICKS_PER_QUARTER_NOTE (16.0)

typedef struct _AgsNote AgsNote;
typedef struct _AgsNoteClass AgsNoteClass;

/**
 * AgsNoteFlags:
 * @AGS_NOTE_GUI: gui format
 * @AGS_NOTE_RUNTIME: runtime format
 * @AGS_NOTE_HUMAN_READABLE: human readable format
 * @AGS_NOTE_DEFAULT_LENGTH: default length
 * @AGS_NOTE_IS_SELECTED: is selected
 * @AGS_NOTE_FEED: feed note
 * @AGS_NOTE_ENVELOPE: do envelope
 *
 * Enum values to control the behavior or indicate internal state of #AgsNote by
 * enable/disable as flags.
 */
typedef enum{
  AGS_NOTE_GUI             = 1,
  AGS_NOTE_RUNTIME         = 1 <<  1,
  AGS_NOTE_HUMAN_READABLE  = 1 <<  2,
  AGS_NOTE_DEFAULT_LENGTH  = 1 <<  3,
  AGS_NOTE_IS_SELECTED     = 1 <<  4,
  AGS_NOTE_FEED            = 1 <<  5,
  AGS_NOTE_ENVELOPE        = 1 <<  6,
}AgsNoteFlags;

struct _AgsNote
{
  GObject gobject;

  AgsNoteFlags flags;
  AgsSoundKeyFormat key_format;

  GRecMutex obj_mutex;
  
  gboolean is_minor;
  guint sharp_flats;

  // gui format, convert easy to visualization
  guint x[2];
  guint y;

  guint64 rt_offset;
  guint rt_attack;
  
  gdouble stream_delay;
  gdouble stream_attack;
  guint64 stream_frame_count;
  
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
  GObjectClass gobject;
};

GType ags_note_get_type();

GRecMutex* ags_note_get_obj_mutex(AgsNote *note);

gboolean ags_note_test_flags(AgsNote *note, AgsNoteFlags flags);
void ags_note_set_flags(AgsNote *note, AgsNoteFlags flags);
void ags_note_unset_flags(AgsNote *note, AgsNoteFlags flags);

gint ags_note_sort_func(gconstpointer a,
			gconstpointer b);

gboolean ags_note_get_is_minor(AgsNote *note);
void ags_note_set_is_minor(AgsNote *note, gboolean is_minor);

guint ags_note_get_sharp_flats(AgsNote *note);
void ags_note_set_sharp_flats(AgsNote *note, guint sharp_flats);

guint ags_note_get_x0(AgsNote *note);
void ags_note_set_x0(AgsNote *note, guint x0);

guint ags_note_get_x1(AgsNote *note);
void ags_note_set_x1(AgsNote *note, guint x1);

guint ags_note_get_y(AgsNote *note);
void ags_note_set_y(AgsNote *note, guint y);

guint64 ags_note_get_rt_offset(AgsNote *note);
void ags_note_set_rt_offset(AgsNote *note, guint64 rt_offset);

guint ags_note_get_rt_attack(AgsNote *note);
void ags_note_set_rt_attack(AgsNote *note, guint rt_attack);

AgsComplex* ags_note_get_attack(AgsNote *note);
void ags_note_set_attack(AgsNote *note, AgsComplex *attack);

AgsComplex* ags_note_get_sustain(AgsNote *note);
void ags_note_set_sustain(AgsNote *note, AgsComplex *sustain);

AgsComplex* ags_note_get_decay(AgsNote *note);
void ags_note_set_decay(AgsNote *note, AgsComplex *decay);

AgsComplex* ags_note_get_release(AgsNote *note);
void ags_note_set_release(AgsNote *note, AgsComplex *release);

AgsComplex* ags_note_get_ratio(AgsNote *note);
void ags_note_set_ratio(AgsNote *note, AgsComplex *ratio);

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

guchar* ags_note_to_raw_midi(AgsNote *note,
			     gdouble bpm, gdouble delay_factor,
			     guint *buffer_length);
guchar* ags_note_to_raw_midi_extended(AgsNote *note,
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

GList* ags_note_from_raw_midi(guchar *raw_midi,
			      gdouble bpm, gdouble delay_factor,
			      guint length);
GList* ags_note_from_raw_midi_extended(guchar *raw_midi,
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
				  gdouble stream_delay, gdouble stream_attack);

G_END_DECLS

#endif /*__AGS_NOTE_H__*/
