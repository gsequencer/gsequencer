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

#ifndef __AGS_MIDI_FILE_H__
#define __AGS_MIDI_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <stdio.h>

#define AGS_TYPE_MIDI_FILE                (ags_midi_file_get_type ())
#define AGS_MIDI_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_FILE, AgsMidiFile))
#define AGS_MIDI_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_FILE, AgsMidiFileClass))
#define AGS_IS_MIDI_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI_FILE))
#define AGS_IS_MIDI_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI_FILE))
#define AGS_MIDI_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_MIDI_FILE, AgsMidiFileClass))

#define AGS_MIDI_FILE_TRACK(ptr) ((AgsMidiFileTrack *)(ptr))

#define USEC_PER_SEC (1000000)

#define AGS_MIDI_FILE_MAX_TEXT_LENGTH (4096)
#define AGS_MIDI_FILE_MTHD "MThd\0"
#define AGS_MIDI_FILE_MTRK "MTrk\0"

#define AGS_MIDI_FILE_DEFAULT_OFFSET (0)
#define AGS_MIDI_FILE_DEFAULT_FORMAT (1)
#define AGS_MIDI_FILE_DEFAULT_BEATS (120)
#define AGS_MIDI_FILE_DEFAULT_FPS (30)
#define AGS_MIDI_FILE_DEFAULT_TICKS (384)

typedef struct _AgsMidiFile AgsMidiFile;
typedef struct _AgsMidiFileClass AgsMidiFileClass;
typedef struct _AgsMidiFileTrack AgsMidiFileTrack;

typedef enum{
  /* channel messages */
  AGS_MIDI_FILE_KEY_OFF           = 0x80,
  AGS_MIDI_FILE_KEY_ON            = 0x90,
  AGS_MIDI_FILE_KEY_PRESSURE      = 0xa0,
  AGS_MIDI_FILE_CHANGE_PARAMETER  = 0xb0,
  AGS_MIDI_FILE_CHANGE_PROGRAM    = 0xc0,
  AGS_MIDI_FILE_CHANNEL_PRESSURE  = 0xd0,
  AGS_MIDI_FILE_CHANGE_PITCH_BEND = 0xe0,
  /* status messages */
  AGS_MIDI_FILE_SYSEX             = 0xf0,
  AGS_MIDI_FILE_QUARTER_FRAME     = 0xf1,
  AGS_MIDI_FILE_SONG_POSITION     = 0xf2,
  AGS_MIDI_FILE_SONG_SELECT       = 0xf3,
  AGS_MIDI_FILE_UNDEFINED_0       = 0xf4,
  AGS_MIDI_FILE_UNDEFINED_1       = 0xf5,
  AGS_MIDI_FILE_TUNE_REQUEST      = 0xf6,
  AGS_MIDI_FILE_SYSEX_END         = 0xf7,
  AGS_MIDI_FILE_META_EVENT        = 0xff,
}AgsMidiFileStatus;

typedef enum{
  AGS_MIDI_FILE_EOF               = 1,
  AGS_MIDI_FILE_SMTPE             = 1 <<  1,
  AGS_MIDI_FILE_DROP_FRAME        = 1 <<  2,
}AgsMidiFileFlags;

struct _AgsMidiFile
{
  GObject gobject;

  guint flags;
  
  FILE *file;
  gchar *filename;
  
  unsigned char *buffer;
  guint buffer_length;

  unsigned char *iter;
  
  guint offset;
  guint format;
  guint count;
  guint division;
  guint times;
  guint beat;
  guint clicks;

  GList *track;
  
  AgsMidiFileTrack *current_track;
};

struct _AgsMidiFileClass
{
  GObjectClass gobject;
};

struct _AgsMidiFileTrack
{
  GObject *sequencer;

  gchar *track_name;
  unsigned char *buffer;
};

GType ags_midi_file_get_type(void);

gboolean ags_midi_file_open(AgsMidiFile *midi_file);
gboolean ags_midi_file_open_from_data(AgsMidiFile *midi_file,
				      unsigned char *data, guint buffer_length);
gboolean ags_midi_file_rw_open(AgsMidiFile *midi_file);

void ags_midi_file_close(AgsMidiFile *midi_file);

gchar* ags_midi_file_read(AgsMidiFile *midi_file);
void ags_midi_file_write(AgsMidiFile *midi_file,
			 unsigned char *data, guint buffer_length);
void ags_midi_file_seek(AgsMidiFile *midi_file, guint position, gint whence);
void ags_midi_file_flush(AgsMidiFile *midi_file);

gint16 ags_midi_file_read_gint16(AgsMidiFile *midi_file);
gint32 ags_midi_file_read_gint24(AgsMidiFile *midi_file);
gint32 ags_midi_file_read_gint32(AgsMidiFile *midi_file);
long ags_midi_file_read_varlength(AgsMidiFile *midi_file);
gchar* ags_midi_file_read_text(AgsMidiFile *midi_file,
			       gint length);

void ags_midi_file_write_gint16(AgsMidiFile *midi_file, gint16 val);
void ags_midi_file_write_gint24(AgsMidiFile *midi_file, gint32 val);
void ags_midi_file_write_gint32(AgsMidiFile *midi_file, gint32 val);
void ags_midi_file_write_varlength(AgsMidiFile *midi_file, long val);
void ags_midi_file_write_text(AgsMidiFile *midi_file,
			      gchar *text, guint length);

unsigned char* ags_midi_file_read_header(AgsMidiFile *midi_file,
					 guint *buffer_length);
void ags_midi_file_write_header(AgsMidiFile *midi_file,
				unsigned char *buffer, guint buffer_length);

unsigned char* ags_midi_file_read_track_data(AgsMidiFile *midi_file,
					     guint *buffer_length);
void ags_midi_file_write_track_data(AgsMidiFile *midi_file,
				    unsigned char *buffer, guint buffer_length);

AgsMidiFile* ags_midi_file_new(gchar *filename);

#endif /*__AGS_MIDI_FILE_H__*/
