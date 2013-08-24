/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_AUDIO_SIGNAL_H__
#define __AGS_AUDIO_SIGNAL_H__

#include <glib-object.h>

#define AGS_TYPE_AUDIO_SIGNAL                (ags_audio_signal_get_type())
#define AGS_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_SIGNAL, AgsAudioSignal))
#define AGS_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_SIGNAL, AgsAudioSignalClass))
#define AGS_IS_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_SIGNAL))
#define AGS_IS_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_SIGNAL))
#define AGS_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_AUDIO_SIGNAL, AgsAudioSignalClass))

typedef struct _AgsAudioSignal AgsAudioSignal;
typedef struct _AgsAudioSignalClass AgsAudioSignalClass;
typedef struct _AgsAttack AgsAttack;

typedef enum{
  AGS_AUDIO_SIGNAL_TEMPLATE             = 1,
  AGS_AUDIO_SIGNAL_PLAY_DONE            = 1 << 1,
  AGS_AUDIO_SIGNAL_STANDALONE           = 1 << 2,
}AgsAudioSignalFlags;

typedef enum{
  AGS_ATTACK_INITIAL_RUN  = 1,
  AGS_ATTACK_SWAP         = 1 << 1,
}AgsAttackFlags;

struct _AgsAudioSignal
{
  GObject object;

  guint flags;

  GObject *devout;

  GObject *recycling;
  GObject *recall_id; // AGS_TYPE_RECALL_ID to identify the AgsAudioSignal

  guint samplerate;
  guint buffer_size;

  guint length;
  guint last_frame; // the last frame at stream_end
  guint loop_start;
  guint loop_end;

  GList *stream_beginning;
  GList *stream_current;
  GList *stream_end;
};

struct _AgsAudioSignalClass
{
  GObjectClass object;

  void (*realloc_buffer_size)(AgsAudioSignal *audio_signal, guint buffer_size);
  void (*morph_samplerate)(AgsAudioSignal *audio_signal, guint samplerate, double k_morph);
};

struct _AgsAttack
{
  guint flags;

  guint first_start;
  guint first_length;

  guint second_start;
  guint second_length;
};

GType ags_audio_signal_get_type();

signed short* ags_stream_alloc(guint buffer_size);

AgsAttack* ags_attack_alloc(guint first_start, guint first_length,
			    guint second_start, guint second_length);
AgsAttack* ags_attack_duplicate(AgsAttack *attack);
AgsAttack* ags_attack_duplicate_from_devout(GObject *devout);

guint ags_audio_signal_get_length_till_current(AgsAudioSignal *audio_signal);

void ags_audio_signal_add_stream(AgsAudioSignal *audio_signal);
void ags_audio_signal_stream_resize(AgsAudioSignal *audio_signal, guint length);
void ags_audio_signal_stream_safe_resize(AgsAudioSignal *audio_signal, guint length);

void ags_audio_signal_realloc_buffer_size(AgsAudioSignal *audio_signal, guint buffer_size);
void ags_audio_signal_morph_samplerate(AgsAudioSignal *audio_signal, guint samplerate, double k_morph);

void ags_audio_signal_copy_buffer_to_buffer(signed short *destination, guint dchannels,
					    signed short *source, guint schannels, guint size);

void ags_audio_signal_duplicate_stream(AgsAudioSignal *audio_signal,
				       AgsAudioSignal *template,
				       guint attack);

//TODO:JK: rename these functions name it rather find than get
AgsAudioSignal* ags_audio_signal_get_template(GList *audio_signal);
GList* ags_audio_signal_get_stream_current(GList *audio_signal,
					   GObject *recall_id);
GList* ags_audio_signal_get_by_recall_id(GList *audio_signal,
					 GObject *recall_id);

void ags_audio_signal_tile(AgsAudioSignal *audio_signal,
			   AgsAudioSignal *template,
			   guint length);
void ags_audio_signal_scale(AgsAudioSignal *audio_signal,
			    AgsAudioSignal *template,
			    guint length);

AgsAudioSignal* ags_audio_signal_new(GObject *devout,
				     GObject *recycling,
				     GObject *recall_id);

AgsAudioSignal* ags_audio_signal_new_with_length(GObject *devout,
						 GObject *recycling,
						 GObject *recall_id,
						 guint length);

#endif /*__AGS_AUDIO_SIGNAL_H__*/
