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

#ifndef __AGS_SEQUENCER_H__
#define __AGS_SEQUENCER_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_SEQUENCER                    (ags_sequencer_get_type())
#define AGS_SEQUENCER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SEQUENCER, AgsSequencer))
#define AGS_SEQUENCER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_SEQUENCER, AgsSequencerInterface))
#define AGS_IS_SEQUENCER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SEQUENCER))
#define AGS_IS_SEQUENCER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_SEQUENCER))
#define AGS_SEQUENCER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_SEQUENCER, AgsSequencerInterface))

#define AGS_SEQUENCER_MAX_MIDI_CHANNELS (16)

#define AGS_SEQUENCER_MAX_MIDI_KEYS (128)

#define AGS_SEQUENCER_DEFAULT_SAMPLERATE (44100.0)
#define AGS_SEQUENCER_DEFAULT_BUFFER_SIZE (944)
#define AGS_SEQUENCER_DEFAULT_DEVICE "hw:0"
#define AGS_SEQUENCER_DEFAULT_BPM (120.0)
#define AGS_SEQUENCER_DEFAULT_DELAY_FACTOR (1.0 / 4.0)
#define AGS_SEQUENCER_DEFAULT_JIFFIE ((double) AGS_SEQUENCER_DEFAULT_SAMPLERATE / (double) AGS_SEQUENCER_DEFAULT_BUFFER_SIZE)

#define AGS_SEQUENCER_DEFAULT_TACT (1.0 / 1.0)
#define AGS_SEQUENCER_DEFAULT_TACT_JIFFIE (60.0 / AGS_SEQUENCER_DEFAULT_BPM * AGS_SEQUENCER_DEFAULT_TACT)
#define AGS_SEQUENCER_DEFAULT_TACTRATE (1.0 / AGS_SEQUENCER_DEFAULT_TACT_JIFFIE)

#define AGS_SEQUENCER_DEFAULT_SCALE (1.0)
#define AGS_SEQUENCER_DEFAULT_DELAY (AGS_SEQUENCER_DEFAULT_JIFFIE * (60.0 / AGS_SEQUENCER_DEFAULT_BPM))

#define AGS_SEQUENCER_DEFAULT_PERIOD (64.0)

#define AGS_SEQUENCER_DEFAULT_LATENCY (400)

typedef struct _AgsSequencer AgsSequencer;
typedef struct _AgsSequencerInterface AgsSequencerInterface;

/**
 * AgsSequencerMidiVersion:
 * @AGS_SEQUENCER_MIDI1: MIDI version 1
 * @AGS_SEQUENCER_MIDI2: MIDI version 2
 * 
 * Enum values to tell about version.
 */
typedef enum{
  AGS_SEQUENCER_MIDI1   = 1,
  AGS_SEQUENCER_MIDI2   = 1 <<  1,
}AgsSequencerMidiVersion;

struct _AgsSequencerInterface
{
  GTypeInterface ginterface;

  void (*set_device)(AgsSequencer *sequencer,
		     gchar *card_id);
  gchar* (*get_device)(AgsSequencer *sequencer);

  void (*list_cards)(AgsSequencer *sequencer,
		     GList **card_id, GList **card_name);

  gboolean (*is_starting)(AgsSequencer *sequencer);
  gboolean (*is_playing)(AgsSequencer *sequencer);
  gboolean (*is_recording)(AgsSequencer *sequencer);

  void (*play_init)(AgsSequencer *sequencer,
		    GError **error);
  void (*play)(AgsSequencer *sequencer,
	       GError **error);

  void (*record_init)(AgsSequencer *sequencer,
		      GError **error);
  void (*record)(AgsSequencer *sequencer,
		 GError **error);

  void (*stop)(AgsSequencer *sequencer);

  void (*tic)(AgsSequencer *sequencer);
  void (*offset_changed)(AgsSequencer *sequencer,
			 guint note_offset);

  void* (*get_buffer)(AgsSequencer *sequencer,
		      guint *buffer_length);
  void* (*get_next_buffer)(AgsSequencer *sequencer,
			   guint *buffer_length);

  void (*lock_buffer)(AgsSequencer *sequencer,
		      void *buffer);
  void (*unlock_buffer)(AgsSequencer *sequencer,
			void *buffer);
  
  void (*set_bpm)(AgsSequencer *sequencer,
		  gdouble bpm);
  gdouble (*get_bpm)(AgsSequencer *sequencer);

  void (*set_delay_factor)(AgsSequencer *sequencer,
			   gdouble delay_factor);
  gdouble (*get_delay_factor)(AgsSequencer *sequencer);

  void (*set_start_note_offset)(AgsSequencer *sequencer,
				guint start_note_offset);
  guint (*get_start_note_offset)(AgsSequencer *sequencer);

  void (*set_note_offset)(AgsSequencer *sequencer,
			  guint note_offset);
  guint (*get_note_offset)(AgsSequencer *sequencer);

  AgsSequencerMidiVersion (*get_midi_version)(AgsSequencer *sequencer);  
  void (*set_midi_version)(AgsSequencer *sequencer,
			   AgsSequencerMidiVersion midi_version);  
};

GType ags_sequencer_get_type();

void ags_sequencer_set_device(AgsSequencer *sequencer,
			      gchar *card_id);
gchar* ags_sequencer_get_device(AgsSequencer *sequencer);

void ags_sequencer_list_cards(AgsSequencer *sequencer,
			      GList **card_id, GList **card_name);

gboolean ags_sequencer_is_starting(AgsSequencer *sequencer);
gboolean ags_sequencer_is_playing(AgsSequencer *sequencer);
gboolean ags_sequencer_is_recording(AgsSequencer *sequencer);

void ags_sequencer_play_init(AgsSequencer *sequencer,
			     GError **error);
void ags_sequencer_play(AgsSequencer *sequencer,
			GError **error);

void ags_sequencer_record_init(AgsSequencer *sequencer,
			       GError **error);
void ags_sequencer_record(AgsSequencer *sequencer,
			  GError **error);

void ags_sequencer_stop(AgsSequencer *sequencer);

void ags_sequencer_tic(AgsSequencer *sequencer);
void ags_sequencer_offset_changed(AgsSequencer *sequencer,
				  guint note_offset);

void* ags_sequencer_get_buffer(AgsSequencer *sequencer,
			       guint *buffer_length);
void* ags_sequencer_get_next_buffer(AgsSequencer *sequencer,
				    guint *buffer_length);

void ags_sequencer_lock_buffer(AgsSequencer *sequencer,
			       void *buffer);
void ags_sequencer_unlock_buffer(AgsSequencer *sequencer,
				 void *buffer);

void ags_sequencer_set_bpm(AgsSequencer *sequencer,
			   gdouble bpm);
gdouble ags_sequencer_get_bpm(AgsSequencer *sequencer);

void ags_sequencer_set_delay_factor(AgsSequencer *sequencer,
				    gdouble delay_factor);
gdouble ags_sequencer_get_delay_factor(AgsSequencer *sequencer);

void ags_sequencer_set_start_note_offset(AgsSequencer *sequencer,
					 guint start_note_offset);
guint ags_sequencer_get_start_note_offset(AgsSequencer *sequencer);

void ags_sequencer_set_note_offset(AgsSequencer *sequencer,
				   guint note_offset);
guint ags_sequencer_get_note_offset(AgsSequencer *sequencer);

AgsSequencerMidiVersion ags_sequencer_get_midi_version(AgsSequencer *sequencer);  

G_END_DECLS

#endif /*__AGS_SEQUENCER_H__*/
