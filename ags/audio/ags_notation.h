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

#ifndef __AGS_NOTATION_H__
#define __AGS_NOTATION_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/libags.h>

#include <ags/audio/ags_note.h>

G_BEGIN_DECLS

#define AGS_TYPE_NOTATION                (ags_notation_get_type())
#define AGS_NOTATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTATION, AgsNotation))
#define AGS_NOTATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTATION, AgsNotationClass))
#define AGS_IS_NOTATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_NOTATION))
#define AGS_IS_NOTATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_NOTATION))
#define AGS_NOTATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NOTATION, AgsNotationClass))

#define AGS_NOTATION_GET_OBJ_MUTEX(obj) (&(((AgsNotation *) obj)->obj_mutex))

#define AGS_NOTATION_DEFAULT_BPM (120.0)

#define AGS_NOTATION_TICS_PER_BEAT (1.0)
#define AGS_NOTATION_MINIMUM_NOTE_LENGTH (1.0 / 16.0)
#define AGS_NOTATION_MAXIMUM_NOTE_LENGTH (16.0)

#define AGS_NOTATION_DEFAULT_LENGTH (16 * 16 * 1200 / AGS_NOTATION_TICS_PER_BEAT)
#define AGS_NOTATION_DEFAULT_JIFFIE (60.0 / AGS_NOTATION_DEFAULT_BPM / AGS_NOTATION_TICS_PER_BEAT)
#define AGS_NOTATION_DEFAULT_DURATION (AGS_NOTATION_DEFAULT_LENGTH * AGS_NOTATION_DEFAULT_JIFFIE * AGS_USEC_PER_SEC)
#define AGS_NOTATION_DEFAULT_OFFSET (64 * (1 / AGS_NOTATION_MINIMUM_NOTE_LENGTH))

#define AGS_NOTATION_DEFAULT_END (64 * 64 * 1200)

#define AGS_NOTATION_DEFAULT_DIVISION (96)

#define AGS_NOTATION_CLIPBOARD_VERSION "1.2.0"
#define AGS_NOTATION_CLIPBOARD_TYPE "AgsNotationClipboardXml"
#define AGS_NOTATION_CLIPBOARD_FORMAT "AgsNotationNativePiano"

typedef struct _AgsNotation AgsNotation;
typedef struct _AgsNotationClass AgsNotationClass;

/**
 * AgsNotationFlags:
 * @AGS_NOTATION_PATTERN_MODE: pattern mode
 * @AGS_NOTATION_BYPASS: ignore any notation data
 *
 * Enum values to control the behavior or indicate internal state of #AgsNotation by
 * enable/disable as flags.
 */
typedef enum{
  AGS_NOTATION_PATTERN_MODE         = 1,
  AGS_NOTATION_BYPASS               = 1 <<  1,
}AgsNotationFlags;

struct _AgsNotation
{
  GObject gobject;

  guint flags;
  guint key_format;

  GRecMutex obj_mutex;

  GObject *audio;
  guint audio_channel;

  gboolean is_minor;
  guint sharp_flats;

  AgsTimestamp *timestamp;

  gdouble maximum_note_length;

  GList *note;
  GList *selection;
};

struct _AgsNotationClass
{
  GObjectClass gobject;
};

GType ags_notation_get_type();
GType ags_notation_flags_get_type();

GRecMutex* ags_notation_get_obj_mutex(AgsNotation *notation);

gboolean ags_notation_test_flags(AgsNotation *notation, guint flags);
void ags_notation_set_flags(AgsNotation *notation, guint flags);
void ags_notation_unset_flags(AgsNotation *notation, guint flags);

GList* ags_notation_find_near_timestamp(GList *notation, guint audio_channel,
					AgsTimestamp *timestamp);

GList* ags_notation_add(GList *notation,
			AgsNotation *new_notation);

gint ags_notation_sort_func(gconstpointer a,
			    gconstpointer b);

GObject* ags_notation_get_audio(AgsNotation *notation);
void ags_notation_set_audio(AgsNotation *notation,
			    GObject *audio);

guint ags_notation_get_audio_channel(AgsNotation *notation);
void ags_notation_set_audio_channel(AgsNotation *notation,
				    guint audio_channel);

gboolean ags_notation_get_is_minor(AgsNotation *notation);
void ags_notation_set_is_minor(AgsNotation *notation, gboolean is_minor);

guint ags_notation_get_sharp_flats(AgsNotation *notation);
void ags_notation_set_sharp_flats(AgsNotation *notation, guint sharp_flats);

AgsTimestamp* ags_notation_get_timestamp(AgsNotation *notation);
void ags_notation_set_timestamp(AgsNotation *notation,
				AgsTimestamp *timestamp);

GList* ags_notation_get_note(AgsNotation *notation);
void ags_notation_set_note(AgsNotation *notation,
			   GList *note);

void ags_notation_add_note(AgsNotation *notation,
			   AgsNote *note,
			   gboolean use_selection_list);
void ags_notation_remove_note(AgsNotation *notation,
			      AgsNote *note,
			      gboolean use_selection_list);

gboolean ags_notation_remove_note_at_position(AgsNotation *notation,
					      guint x, guint y);

GList* ags_notation_get_selection(AgsNotation *notation);

gboolean ags_notation_is_note_selected(AgsNotation *notation, AgsNote *note);

AgsNote* ags_notation_find_point(AgsNotation *notation,
				 guint x, guint y,
				 gboolean use_selection_list);
GList* ags_notation_find_region(AgsNotation *notation,
				guint x0, guint y0,
				guint x1, guint y1,
				gboolean use_selection_list);

GList* ags_notation_find_offset(AgsNotation *notation,
				guint x,
				gboolean use_selection_list);

void ags_notation_free_selection(AgsNotation *notation);
void ags_notation_free_all_selection(GList *notation);

void ags_notation_add_point_to_selection(AgsNotation *notation,
					 guint x, guint y,
					 gboolean replace_current_selection);
void ags_notation_remove_point_from_selection(AgsNotation *notation,
					      guint x, guint y);

void ags_notation_add_region_to_selection(AgsNotation *notation,
					  guint x0, guint y0,
					  guint x1, guint y1,
					  gboolean replace_current_selection);
void ags_notation_remove_region_from_selection(AgsNotation *notation,
					       guint x0, guint y0,
					       guint x1, guint y1);

void ags_notation_add_all_to_selection(AgsNotation *notation);

xmlNode* ags_notation_copy_selection(AgsNotation *notation);
xmlNode* ags_notation_cut_selection(AgsNotation *notation);

void ags_notation_insert_from_clipboard(AgsNotation *notation,
					xmlNode *notation_node,
					gboolean reset_x_offset, guint x_offset,
					gboolean reset_y_offset, guint y_offset);

void ags_notation_insert_from_clipboard_extended(AgsNotation *notation,
						 xmlNode *notation_node,
						 gboolean reset_x_offset, guint x_offset,
						 gboolean reset_y_offset, guint y_offset,
						 gboolean match_channel, gboolean no_duplicates);

guchar* ags_notation_to_raw_midi(AgsNotation *notation,
				 gdouble bpm, gdouble delay_factor,
				 glong nn, glong dd, glong cc, glong bb,
				 glong tempo,
				 guint *buffer_length);
AgsNotation* ags_notation_from_raw_midi(guchar *raw_midi,
					glong nn, glong dd, glong cc, glong bb,
					glong tempo,
					gdouble bpm, gdouble delay_factor,
					guint buffer_length);

AgsNotation* ags_notation_new(GObject *audio,
			      guint audio_channel);

G_END_DECLS

#endif /*__AGS_NOTATION_H__*/
