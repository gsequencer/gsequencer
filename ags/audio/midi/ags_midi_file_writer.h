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

#ifndef __AGS_MIDI_FILE_WRITER_H__
#define __AGS_MIDI_FILE_WRITER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/midi/ags_midi_file.h>

#define AGS_TYPE_MIDI_FILE_WRITER                (ags_midi_file_writer_get_type ())
#define AGS_MIDI_FILE_WRITER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_FILE_WRITER, AgsMidiFileWriter))
#define AGS_MIDI_FILE_WRITER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_FILE_WRITER, AgsMidiFileWriterClass))
#define AGS_IS_MIDI_FILE_WRITER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI_FILE_WRITER))
#define AGS_IS_MIDI_FILE_WRITER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI_FILE_WRITER))
#define AGS_MIDI_FILE_WRITER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_MIDI_FILE_WRITER, AgsMidiFileWriterClass))

typedef struct _AgsMidiFileWriter AgsMidiFileWriter;
typedef struct _AgsMidiFileWriterClass AgsMidiFileWriterClass;

struct _AgsMidiFileWriter
{
  GObject gobject;

  AgsMidiFile *midi_file;
};

struct _AgsMidiFileWriterClass
{
  GObjectClass gobject;
};

GType ags_midi_file_writer_get_type(void);

void ags_midi_file_writer_write_bytes(AgsMidiFileWriter *midi_file_writer,
				      unsigned char buffer,
				      guint buffer_length);

void ags_midi_file_writer_write_channel_message(AgsMidiFileWriter *midi_file_writer,
						gchar *track_name,
						guint status, ...);


void ags_midi_file_writer_write_status_message(AgsMidiFileWriter *midi_file_writer,
					       gchar *track_name,
					       guint status, ...);

AgsMidiFileWriter* ags_midi_file_writer_new(gchar *filename);

#endif /*__AGS_MIDI_FILE_WRITER_H__*/
