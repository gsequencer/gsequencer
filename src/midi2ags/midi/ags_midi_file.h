/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_MIDI_FILE_H__
#define __AGS_MIDI_FILE_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_MIDI_FILE                (ags_midi_file_get_type ())
#define AGS_MIDI_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_FILE, AgsMidiFile))
#define AGS_MIDI_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_FILE, AgsMidiFileClass))
#define AGS_IS_MIDI_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI_FILE))
#define AGS_IS_MIDI_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI_FILE))
#define AGS_MIDI_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_MIDI_FILE, AgsMidiFileClass))

typedef struct _AgsMidiFile AgsMidiFile;
typedef struct _AgsMidiFileClass AgsMidiFileClass;

struct _AgsMidiFile
{
  GObject gobject;

  gchar *filename;
  
  AgsMidiParser *parser;
  GList *notation;
};

struct _AgsMidiFileClass
{
  GObjectClass gobject;
};

GType ags_midi_file_get_type(void);

gboolean ags_midi_file_open(AgsMidiFile *midi_file);
gboolean ags_midi_file_open_from_data(AgsMidiFile *midi_file);
gboolean ags_midi_file_rw_open(AgsMidiFile *midi_file);

void ags_midi_file_close(AgsMidiFile *midi_file);

GList* ags_midi_file_read(AgsMidiFile *midi_file, GError **error);
void ags_midi_file_write(AgsMidiFile *midi_file,
			  GList *notation);
void ags_midi_file_seek(AgsMidiFile *midi_file, guint note_offset, gint whence);
void ags_midi_file_flush(AgsMidiFile *midi_file);

AgsMidiFile* ags_midi_file_new(gchar *filename);

#endif /*__AGS_MIDI_FILE_H__*/
