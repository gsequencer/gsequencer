/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#ifndef __AGS_MOVE_NOTE_256TH_H__
#define __AGS_MOVE_NOTE_256TH_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>

G_BEGIN_DECLS

#define AGS_TYPE_MOVE_NOTE_256TH                (ags_move_note_256th_get_type())
#define AGS_MOVE_NOTE_256TH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MOVE_NOTE_256TH, AgsMoveNote256th))
#define AGS_MOVE_NOTE_256TH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MOVE_NOTE_256TH, AgsMoveNote256thClass))
#define AGS_IS_MOVE_NOTE_256TH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MOVE_NOTE_256TH))
#define AGS_IS_MOVE_NOTE_256TH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MOVE_NOTE_256TH))
#define AGS_MOVE_NOTE_256TH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MOVE_NOTE_256TH, AgsMoveNote256thClass))

#define AGS_MOVE_NOTE_256TH_DEFAULT_X_LENGTH (16 * 16 * 16 * 1200)
#define AGS_MOVE_NOTE_256TH_DEFAULT_Y_LENGTH (1024)

typedef struct _AgsMoveNote256th AgsMoveNote256th;
typedef struct _AgsMoveNote256thClass AgsMoveNote256thClass;

struct _AgsMoveNote256th
{
  AgsTask task;

  AgsAudio *audio;
  AgsNotation *notation;

  GList *selection;
  
  guint first_x_256th;
  guint first_y;
  guint move_x_256th;
  guint move_y;
  
  gboolean relative;
  gboolean absolute;
};

struct _AgsMoveNote256thClass
{
  AgsTaskClass task;
};

GType ags_move_note_256th_get_type();

void ags_move_note_256th_set_selection(AgsMoveNote256th *move_note_256th,
				       GList *selection);

AgsMoveNote256th* ags_move_note_256th_new(AgsAudio *audio,
					  AgsNotation *notation,
					  GList *selection,
					  guint first_x_256th, guint first_y,
					  gint move_x_256th, gint move_y,
					  gboolean relative, gboolean absolute);

G_END_DECLS

#endif /*__AGS_MOVE_NOTE_256TH_H__*/
