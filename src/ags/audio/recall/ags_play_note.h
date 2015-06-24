/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_PLAY_NOTE_H__
#define __AGS_PLAY_NOTE_H__

#include <glib-object.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_PLAY_NOTE             (ags_play_note_get_type())
#define AGS_PLAY_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_NOTE, AgsPlayNote))
#define AGS_PLAY_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_NOTE, AgsPlayNote))
#define AGS_IS_PLAY_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_NOTE))
#define AGS_IS_PLAY_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_NOTE))
#define AGS_PLAY_NOTE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_NOTE, AgsPlayNoteClass))

typedef struct _AgsPlayNote AgsPlayNote;
typedef struct _AgsPlayNoteClass AgsPlayNoteClass;

struct _AgsPlayNote
{
  AgsRecall recall;

  AgsDevout *devout;

  AgsChannel *channel;
  AgsNote *note;
};

struct _AgsPlayNoteClass
{
  AgsRecallClass recall;
};

GType ags_play_note_get_type();

AgsPlayNote* ags_play_note_new();

#endif /*__AGS_PLAY_NOTE_H__*/
