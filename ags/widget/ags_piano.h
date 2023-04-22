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

#ifndef __AGS_PIANO_H__
#define __AGS_PIANO_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_piano_keys.h>

G_BEGIN_DECLS

#define AGS_TYPE_PIANO                (ags_piano_get_type())
#define AGS_PIANO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PIANO, AgsPiano))
#define AGS_PIANO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PIANO, AgsPianoClass))
#define AGS_IS_PIANO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PIANO))
#define AGS_IS_PIANO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PIANO))
#define AGS_PIANO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PIANO, AgsPianoClass))

#define AGS_PIANO_DEFAULT_FONT_SIZE (12)

#define AGS_PIANO_DEFAULT_BASE_NOTE AGS_PIANO_KEYS_OCTAVE_0_C
#define AGS_PIANO_DEFAULT_BASE_KEY_CODE (0)

#define AGS_PIANO_DEFAULT_KEY_WIDTH (60)
#define AGS_PIANO_DEFAULT_KEY_HEIGHT (14)

#define AGS_PIANO_DEFAULT_KEY_COUNT (128)

typedef struct _AgsPiano AgsPiano;
typedef struct _AgsPianoClass AgsPianoClass;

typedef enum{
  AGS_PIANO_DRAW_FULL_SCALE      = 1,
  AGS_PIANO_DRAW_OCTAVE_SCALE    = 1 <<  1,
}AgsPianoFlags;

typedef enum{
  AGS_PIANO_BUTTON_1_PRESSED     = 1,
}AgsPianoButtonState;

typedef enum{
  AGS_PIANO_MOVE_CURSOR_UP,
  AGS_PIANO_MOVE_CURSOR_DOWN,
  AGS_PIANO_HIT_KEY,
}AgsPianoAction;

typedef enum{
  AGS_PIANO_NOTE_C,
  AGS_PIANO_NOTE_CIS,
  AGS_PIANO_NOTE_D,
  AGS_PIANO_NOTE_DIS,
  AGS_PIANO_NOTE_E,
  AGS_PIANO_NOTE_F,
  AGS_PIANO_NOTE_FIS,
  AGS_PIANO_NOTE_G,
  AGS_PIANO_NOTE_GIS,
  AGS_PIANO_NOTE_A,
  AGS_PIANO_NOTE_AIS,
  AGS_PIANO_NOTE_H,
}AgsPianoOctave;

struct _AgsPiano
{
  GtkWidget widget;

  AgsPianoFlags flags;

  GtkOrientation orientation;

  AgsPianoButtonState button_state;

  guint font_size;
  
  gchar *base_note;
  gint base_key_code;

  guint key_width;
  guint key_height;

  guint key_count;

  gint cursor_position;
  gint current_key;
  
  gint *active_key;
  guint active_key_count;
};

struct _AgsPianoClass
{
  GtkWidgetClass widget;

  void (*key_pressed)(AgsPiano *piano,
		      gchar *note, gint key_code);
  void (*key_released)(AgsPiano *piano,
		       gchar *note, gint key_code);

  void (*key_clicked)(AgsPiano *piano,
		      gchar *note, gint key_code);
};

GType ags_piano_get_type(void);

void ags_piano_set_button_state(AgsPiano *piano,
				guint button_state);
guint ags_piano_get_button_state(AgsPiano *piano);

/* getter and setter */
void ags_piano_set_base_note(AgsPiano *piano,
			     gchar *base_note);
gchar* ags_piano_get_base_note(AgsPiano *piano);

void ags_piano_set_base_key_code(AgsPiano *piano,
				 gint base_key_code);
gint ags_piano_get_base_key_code(AgsPiano *piano);

void ags_piano_set_key_width(AgsPiano *piano,
			     guint key_width);
guint ags_piano_get_key_width(AgsPiano *piano);

void ags_piano_set_key_height(AgsPiano *piano,
			      guint key_height);
guint ags_piano_get_key_height(AgsPiano *piano);

void ags_piano_set_key_count(AgsPiano *piano,
			     guint key_count);
guint ags_piano_get_key_count(AgsPiano *piano);

/* events related */
gchar* ags_piano_key_code_to_note(gint key_code);

void ags_piano_key_pressed(AgsPiano *piano,
			   gchar *note, gint key_code);
void ags_piano_key_released(AgsPiano *piano,
			    gchar *note, gint key_code);

void ags_piano_key_clicked(AgsPiano *piano,
			   gchar *note, gint key_code);

gint* ags_piano_get_active_key(AgsPiano *piano,
			       guint *active_key_count);

/* instantiate */
AgsPiano* ags_piano_new(GtkOrientation orientation,
			guint key_width,
			guint key_height);

G_END_DECLS

#endif /*__AGS_PIANO_H__*/
