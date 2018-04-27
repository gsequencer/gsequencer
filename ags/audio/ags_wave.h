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

#ifndef __AGS_WAVE_H__
#define __AGS_WAVE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/libags.h>

#include <ags/audio/ags_buffer.h>

#define AGS_TYPE_WAVE                (ags_wave_get_type())
#define AGS_WAVE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE, AgsWave))
#define AGS_WAVE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE, AgsWaveClass))
#define AGS_IS_WAVE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WAVE))
#define AGS_IS_WAVE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WAVE))
#define AGS_WAVE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_WAVE, AgsWaveClass))

#define AGS_WAVE_DEFAULT_BPM (120.0)

#define AGS_WAVE_TICS_PER_BEAT (1.0)
#define AGS_WAVE_MINIMUM_BUFFER_LENGTH (1.0 / 16.0)

#define AGS_WAVE_DEFAULT_LENGTH (65535.0 / AGS_WAVE_TICS_PER_BEAT - AGS_WAVE_MINIMUM_BUFFER_LENGTH)
#define AGS_WAVE_DEFAULT_JIFFIE (60.0 / AGS_WAVE_DEFAULT_BPM / AGS_WAVE_TICS_PER_BEAT)
#define AGS_WAVE_DEFAULT_DURATION (AGS_WAVE_DEFAULT_LENGTH * AGS_WAVE_DEFAULT_JIFFIE * AGS_MICROSECONDS_PER_SECOND)
#define AGS_WAVE_DEFAULT_OFFSET (64 * (1 / AGS_WAVE_MINIMUM_BUFFER_LENGTH))

#define AGS_WAVE_CLIPBOARD_VERSION "1.4.0"
#define AGS_WAVE_CLIPBOARD_TYPE "AgsWaveClipboardXml"
#define AGS_WAVE_CLIPBOARD_FORMAT "AgsWaveNativeLevel"

typedef struct _AgsWave AgsWave;
typedef struct _AgsWaveClass AgsWaveClass;

/**
 * AgsWaveFlags:
 * @AGS_WAVE_BYPASS: ignore any wave data
 * 
 * Enum values to control the behavior or indicate internal state of #AgsWave by
 * enable/disable as flags.
 */
typedef enum{
  AGS_WAVE_BYPASS            = 1,
}AgsWaveFlags;

struct _AgsWave
{
  GObject gobject;

  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  GObject *audio;
  guint audio_channel;

  guint samplerate;
  guint buffer_size;
  guint format;

  AgsTimestamp *timestamp;
  
  GList *buffer;
  GList *selection;
};

struct _AgsWaveClass
{
  GObjectClass gobject;
};

GType ags_wave_get_type(void);

pthread_mutex_t* ags_wave_get_class_mutex();

gboolean ags_wave_test_flags(AgsWave *wave, guint flags);
void ags_wave_set_flags(AgsWave *wave, guint flags);
void ags_wave_unset_flags(AgsWave *wave, guint flags);

void ags_wave_set_samplerate(AgsWave *wave,
			     guint samplerate);
void ags_wave_set_buffer_size(AgsWave *wave,
			      guint buffer_size);
void ags_wave_set_format(AgsWave *wave,
			 guint format);

GList* ags_wave_find_near_timestamp(GList *wave, guint audio_channel,
				    AgsTimestamp *timestamp);

GList* ags_wave_add(GList *wave,
		    AgsWave *new_wave);

void ags_wave_add_buffer(AgsWave *wave,
			 AgsBuffer *buffer,
			 gboolean use_selection_list);
void ags_wave_remove_buffer(AgsWave *wave,
			    AgsBuffer *buffer,
			    gboolean use_selection_list);

GList* ags_wave_get_selection(AgsWave *wave);

gboolean ags_wave_is_buffer_selected(AgsWave *wave, AgsBuffer *buffer);

AgsBuffer* ags_wave_find_point(AgsWave *wave,
			       guint64 x,
			       gboolean use_selection_list);
GList* ags_wave_find_region(AgsWave *wave,
			    guint64 x0,
			    guint64 x1,
			    gboolean use_selection_list);

void ags_wave_free_selection(AgsWave *wave);

void ags_wave_add_region_to_selection(AgsWave *wave,
				      guint64 x0, guint64 x1,
				      gboolean replace_current_selection);
void ags_wave_remove_region_from_selection(AgsWave *wave,
					   guint64 x0, guint64 x1);

void ags_wave_add_all_to_selection(AgsWave *wave);

xmlNode* ags_wave_copy_selection(AgsWave *wave);
xmlNode* ags_wave_cut_selection(AgsWave *wave);

void ags_wave_insert_from_clipboard(AgsWave *wave,
				    xmlNode *wave_node,
				    gboolean reset_x_offset, guint64 x_offset,
				    gdouble delay, guint attack);

void ags_wave_insert_from_clipboard_extended(AgsWave *wave,
					     xmlNode *wave_node,
					     gboolean reset_x_offset, guint64 x_offset,
					     gdouble delay, guint attack,
					     gboolean match_audio_channel, gboolean do_replace);

AgsWave* ags_wave_new(GObject *audio,
		      guint audio_channel);

#endif /*__AGS_WAVE_H__*/
