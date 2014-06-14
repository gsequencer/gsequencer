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

#ifndef __AGS_NOTATION_H__
#define __AGS_NOTATION_H__

#include <glib-object.h>
#include <libxml/tree.h>

#include <ags/thread/ags_timestamp_thread.h>
#include <ags/audio/ags_timestamp.h>
#include <ags/audio/ags_note.h>

#define AGS_TYPE_NOTATION                (ags_notation_get_type())
#define AGS_NOTATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTATION, AgsNotation))
#define AGS_NOTATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTATION, AgsNotationClass))
#define AGS_IS_NOTATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_NOTATION))
#define AGS_IS_NOTATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_NOTATION))
#define AGS_NOTATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NOTATION, AgsNotationClass))

#define AGS_NOTATION_DEFAULT_BPM (120.0)

#define AGS_NOTATION_TICS_PER_BEAT (16.0)
#define AGS_NOTATION_MINIMUM_NOTE_LENGTH (exp2(-4.0)) // same as (1.0 / 16.0)
#define AGS_NOTATION_MAXIMUM_NOTE_LENGTH (256.0)

#define AGS_NOTATION_DEFAULT_LENGTH (65535.0 / AGS_NOTATION_TICS_PER_BEAT - AGS_NOTATION_MAXIMUM_NOTE_LENGTH)
#define AGS_NOTATION_DEFAULT_JIFFIE (60.0 / AGS_NOTATION_DEFAULT_BPM * AGS_NOTATION_TICS_PER_BEAT)
#define AGS_NOTATION_DEFAULT_DURATION (AGS_NOTATION_DEFAULT_LENGTH * AGS_NOTATION_DEFAULT_JIFFIE * AGS_MICROSECONDS_PER_SECOND)

typedef struct _AgsNotation AgsNotation;
typedef struct _AgsNotationClass AgsNotationClass;

typedef enum{
  AGS_NOTATION_STICKY              =  1,
  AGS_NOTATION_INDICATED_LENGTH    =  1 <<  1,
}AgsNotationFlags;

struct _AgsNotation
{
  GObject object;

  guint flags;

  GObject *timestamp;

  guint audio_channel;
  GObject *audio;

  gchar *key;
  gdouble base_frequency;

  gdouble tact;
  gdouble bpm;

  gdouble maximum_note_length;

  GList *notes;
  
  gdouble start_loop;
  gdouble end_loop;
  gdouble offset;

  GList *selection;

  GObject *port;

  GList *current_notes;
  GList *next_notes;
};

struct _AgsNotationClass
{
  GObjectClass object;
};

GType ags_notation_get_type();

GList* ags_notation_find_near_timestamp(GList *notation, guint audio_channel,
					GObject *timestamp);

void ags_notation_add_note(AgsNotation *notation,
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

void ags_notation_free_selection(AgsNotation *notation);

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

xmlNodePtr ags_notation_copy_selection(AgsNotation *notation);
xmlNodePtr ags_notation_cut_selection(AgsNotation *notation);

void ags_notation_insert_from_clipboard(AgsNotation *notation,
					xmlNodePtr content,
					gboolean reset_x_offset, guint x_offset,
					gboolean reset_y_offset, guint y_offset);

GList* ags_notation_get_current(AgsNotation *notation);

AgsNotation* ags_notation_new(guint audio_channel);

#endif /*__AGS_NOTATION_H__*/
