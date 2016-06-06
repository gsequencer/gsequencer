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

#ifndef __AGS_MIDI_FILE_READER_H__
#define __AGS_MIDI_FILE_READER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/midi/ags_midi_file.h>

#define AGS_TYPE_MIDI_FILE_READER                (ags_midi_file_reader_get_type ())
#define AGS_MIDI_FILE_READER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_FILE_READER, AgsMidiFileReader))
#define AGS_MIDI_FILE_READER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_FILE_READER, AgsMidiFileReaderClass))
#define AGS_IS_MIDI_FILE_READER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI_FILE_READER))
#define AGS_IS_MIDI_FILE_READER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI_FILE_READER))
#define AGS_MIDI_FILE_READER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_MIDI_FILE_READER, AgsMidiFileReaderClass))

typedef struct _AgsMidiFileReader AgsMidiFileReader;
typedef struct _AgsMidiFileReaderClass AgsMidiFileReaderClass;

struct _AgsMidiFileReader
{
  GObject gobject;

  AgsMidiFile *midi_file;
};

struct _AgsMidiFileReaderClass
{
  GObjectClass gobject;
};

GType ags_midi_file_reader_get_type(void);

gchar* ags_midi_file_reader_read_channel_message(AgsMidiFileReader *midi_file_reader,
						 GParameter *parameter, guint *n_params);


gchar* ags_midi_file_reader_read_status_message(AgsMidiFileReader *midi_file_reader,
						GParameter *parameter, guint *n_params);

AgsMidiFileReader* ags_midi_file_reader_new(gchar *filename);

#endif /*__AGS_MIDI_FILE_READER_H__*/
