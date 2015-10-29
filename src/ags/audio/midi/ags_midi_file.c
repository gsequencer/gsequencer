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

#include <ags/audio/midi/ags_midi_file.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

void ags_midi_file_class_init(AgsMidiFileClass *midi_file);
void ags_midi_file_init(AgsMidiFile *midi_file);
void ags_midi_file_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_midi_file_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_midi_file_finalize(GObject *gobject);

/**
 * SECTION:ags_midi_file
 * @short_description: the menu bar.
 * @title: AgsMidiFile
 * @section_id:
 * @include: ags/X/ags_midi_file.h
 *
 * #AgsMidiFile reads your midi files.
 */

enum{
  PROP_0,
  PROP_FILENAME,
};

static gpointer ags_midi_file_parent_class = NULL;

GType
ags_midi_file_get_type(void)
{
  static GType ags_type_midi_file = 0;

  if(!ags_type_midi_file){
    static const GTypeInfo ags_midi_file_info = {
      sizeof (AgsMidiFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_file_init,
    };

    ags_type_midi_file = g_type_register_static(G_TYPE_OBJECT,
						"AgsMidiFile\0", &ags_midi_file_info,
						0);
  }

  return(ags_type_midi_file);
}

void
ags_midi_file_class_init(AgsMidiFileClass *midi_file)
{
  GObjectClass *gobject;

  ags_midi_file_parent_class = g_type_class_peek_parent(midi_file);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_file;
  
  gobject->finalize = ags_midi_file_finalize;
}

void
ags_midi_file_init(AgsMidiFile *midi_file)
{
  midi_file->flags = 0;
  
  midi_file->file = NULL;
  midi_file->filename = NULL;
  
  midi_file->buffer = NULL;
  midi_file->buffer_length = 0;
  
  midi_file->offset = AGS_MIDI_FILE_DEFAULT_OFFSET;
  midi_file->format = AGS_MIDI_FILE_DEFAULT_FORMAT;
  midi_file->count = 0;
  midi_file->division = (60 * USEC_PER_SEC) / AGS_MIDI_FILE_DEFAULT_BEATS;
  midi_file->times = 0;
  midi_file->beat = AGS_MIDI_FILE_DEFAULT_BEATS;
  midi_file->clicks = AGS_MIDI_FILE_DEFAULT_TICKS;

  midi_file->track = NULL;

  midi_file->current_track = NULL;
}

void
ags_midi_file_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsMidiFile *midi_file;

  midi_file = AGS_MIDI_FILE(gobject);
  
  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);
      
      if(filename == midi_file->filename){
	return;
      }

      if(midi_file->filename != NULL){
	g_free(midi_file->filename);
      }

      midi_file->filename = g_strdup(filename);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_file_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsMidiFile *midi_file;

  midi_file = AGS_MIDI_FILE(gobject);
  
  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value,
			 midi_file->filename);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_file_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_midi_file_parent_class)->finalize(gobject);
}

gboolean
ags_midi_file_open(AgsMidiFile *midi_file)
{
  if(midi_file == NULL ||
     midi_file->filename == NULL){
    return(FALSE);
  }

  midi_file->file = fopen(midi_file->filename,
			  "r\0");

  return(TRUE);
}

gboolean
ags_midi_file_open_from_data(AgsMidiFile *midi_file,
			     char *data, guint buffer_length)
{
  //TODO:JK: implement me
}

gboolean
ags_midi_file_rw_open(AgsMidiFile *midi_file)
{
  if(midi_file == NULL ||
     midi_file->filename == NULL){
    return(FALSE);
  }
  
  midi_file->file = fopen(midi_file->filename,
			  "r+\0");

  return(TRUE);
}

void
ags_midi_file_close(AgsMidiFile *midi_file)
{
  if(midi_file == NULL ||
     midi_file->file == NULL){
    return;
  }
  
  fclose(midi_file->file);
  
  midi_file->file = NULL;
}

gchar*
ags_midi_file_read(AgsMidiFile *midi_file)
{
  struct stat sb;

  if(midi_file == NULL ||
     midi_file->filename == NULL ||
     midi_file->file == NULL){
    return(NULL);
  }
  
  stat(midi_file->filename, &sb);

  midi_file->buffer_length = sb.st_size + 1;
  midi_file->buffer = (gchar *) malloc(midi_file->buffer_length * sizeof(gchar));
  midi_file->buffer[sb.st_size] = EOF;
  fread(midi_file->buffer, sizeof(char), sb.st_size, midi_file->file);

  midi_file->iter = midi_file->buffer;
  
  return(midi_file->buffer);
}

void
ags_midi_file_write(AgsMidiFile *midi_file,
		    char *data, guint buffer_length)
{
  char *start;
  
  if(midi_file == NULL ||
     midi_file->file == NULL){
    return;
  }

  if(midi_file->buffer == NULL){
    start =
      midi_file->buffer = (char *) malloc((buffer_length + 1) * sizeof(char));
    midi_file->buffer_length = buffer_length + 1;
  }else{
    guint old_buffer_length;

    old_buffer_length = midi_file->buffer_length;

    midi_file->buffer = realloc(midi_file->buffer,
				(old_buffer_length + buffer_length) * sizeof(char));
    start = &(midi_file->buffer[old_buffer_length - 1]);
  }

  memcpy(start, data, buffer_length * sizeof(char));
  start[buffer_length] = EOF;
  
  fwrite(data, sizeof(char), buffer_length, midi_file->file);
}

void
ags_audio_file_seek(AgsMidiFile *midi_file, guint position, gint whence)
{
  if(midi_file == NULL ||
     midi_file->file == NULL){
    return;
  }

  fseek(midi_file->file, position, whence);
}

void
ags_audio_file_flush(AgsMidiFile *midi_file)
{
  if(midi_file == NULL ||
     midi_file->file == NULL){
    return;
  }

  fflush(midi_file->file);
}

gint16
ags_midi_file_read_gint16(AgsMidiFile *midi_file)
{
  char str[2];
  gint16 value = 0;

  str[0] = (midi_file->iter[0]);
  str[1] = (midi_file->iter[1]);

  midi_file->iter += 2;
  
  value = (str[0] & 0xff);
  value = (value<<8) + (str[1] & 0xff);
  
  return(value);
}

gint32
ags_midi_file_read_gint24(AgsMidiFile *midi_file)
{
  char str[4];
  gint32 value = 0;
  
  str[0] = (char) 0x00;
  str[1] = (midi_file->iter[0]);
  str[2] = (midi_file->iter[1]);
  str[3] = (midi_file->iter[2]);

  midi_file->iter += 3;
  
  value = (value<<8) + (str[1] & 0xff);
  value = (value<<8) + (str[2] & 0xff);
  value = (value<<8) + (str[3] & 0xff);
  
  return(value);
}

gint32
ags_midi_file_read_gint32(AgsMidiFile *midi_file)
{
  char str[4];
  gint32 value;
  
  str[0] = (midi_file->iter[0]);
  str[1] = (midi_file->iter[1]);
  str[2] = (midi_file->iter[2]);
  str[3] = (midi_file->iter[3]);
  
  midi_file->iter += 4;
  
  value = (str[0] & 0xff);
  value = (value<<8) + (str[1] & 0xff);
  value = (value<<8) + (str[2] & 0xff);
  value = (value<<8) + (str[3] & 0xff);
  
  return(value);
}

long
ags_midi_file_read_varlength(AgsMidiFile *midi_file)
{
  long value;
  guint i;
  char c;
  
  c = midi_file->iter[0];
  value = c;
  i = 1;

  midi_file->iter += 1;
  
  if(c & 0x80){
    value &= 0x7F;
   
    do{
      //TODO:JK: unsafe
      value = (value << 7) + ((c = (midi_file->iter[0])) & 0x7F);
      i++;
      midi_file->iter += 1;
    }while(c & 0x80);
  }
  
  return(value);
}

gchar*
ags_midi_file_read_text(AgsMidiFile *midi_file,
			gint length)
{
  gchar text[AGS_MIDI_FILE_MAX_TEXT_LENGTH + 1];
  gchar c;
  guint i;

  memset(text, 0, AGS_MIDI_FILE_MAX_TEXT_LENGTH * sizeof(char));
  i = 0;
  
  while((length <= 0 ||
	 i < length) && (c = (midi_file->iter[0])) != EOF){
    midi_file->iter += 1;
    //TODO:JK: unsafe
    if(c == '\0' || !(g_ascii_isalnum(c) ||
		      g_ascii_ispunct(c) ||
		      c == ' ')){
      break;
    }

    text[i] = c;
    i++;
  }

  text[i] = '\0';
    
  return(g_strdup(text));
}

void
ags_midi_file_write_gint16(AgsMidiFile *midi_file, gint16 val)
{
  //TODO:JK: implement me
}

void
ags_midi_file_write_gint24(AgsMidiFile *midi_file, gint32 val)
{
  //TODO:JK: implement me
}

void
ags_midi_file_write_gint32(AgsMidiFile *midi_file, gint32 val)
{
  //TODO:JK: implement me
}

void
ags_midi_file_write_varlength(AgsMidiFile *midi_file, long val)
{
  //TODO:JK: implement me
}

void
ags_midi_file_write_text(AgsMidiFile *midi_file,
			 gchar *text)
{
  //TODO:JK: implement me
}

char*
ags_midi_file_read_header(AgsMidiFile *midi_file,
			  guint *buffer_length)
{
  static gchar header[] = "MThd";

  char *data;
  guint length;

  guint n;
  gchar c;

  if(midi_file == NULL ||
     midi_file->file == NULL){
    if(buffer_length != NULL){
      *buffer_length = 0;
    }
    
    return(NULL);
  }

  data = NULL;
  length = 0;

  /* read header */
  n = 0;
  
  while(n < 4 &&
	(AGS_MIDI_FILE_EOF & (midi_file->flags)) == 0){
    c = ags_midi_file_midi_getc(midi_file);
    
    if(c == header[n]){
      n++;
    }else{
      n = 0;
    }
  }

  length += 4;

  /* get some values */
  midi_file->offset = (guint) ags_midi_file_read_gint32(midi_file);
  midi_file->format = (guint) ags_midi_file_read_gint16(midi_file);
  midi_file->count = (guint) ags_midi_file_read_gint16(midi_file);
  midi_file->division = (guint) ags_midi_file_read_gint16(midi_file);

  if(division & 0x8000){
    /* SMPTE */
    midi_file->times = 0; /* Can't do beats */
  }

  midi_file->beat =
    midi_file->clicks = division;

  length += 10;

  /* return values */
  if(buffer_length != NULL){
    *buffer_length = length;
  }
  
  return(data);
}

void
ags_midi_file_write_header(AgsMidiFile *midi_file,
			   char *buffer, guint length)
{
  if(midi_file == NULL ||
     midi_file->file == NULL){
    return;
  }

  //TODO:JK: implement me
}

gchar*
ags_midi_file_read_track_data(AgsMidiFile *midi_file,
			      char **buffer,
			      guint *buffer_length)
{
  gchar *track_name;
  char *data;
  guint length;
  
  if(midi_file == NULL ||
     midi_file->file == NULL){
    if(buffer_length != NULL){
      *buffer_length = 0;
    }
    
    return(NULL);
  }

  track_name = NULL;
  data = NULL;
  length = 0;
  
  //TODO:JK: implement me

  if(buffer_length != NULL){
    *buffer_length = length;
  }
  
  return(track_name);
}

void
ags_midi_file_write_track_data(AgsMidiFile *midi_file,
			       gchar *track_name,
			       char *data, guint buffer_length)
{
  if(midi_file == NULL ||
     midi_file->file == NULL){
    return;
  }
  
  //TODO:JK: implement me
}

AgsMidiFile*
ags_midi_file_new(gchar *filename)
{
  AgsMidiFile *midi_file;

  midi_file = (AgsMidiFile *) g_object_new(AGS_TYPE_MIDI_FILE,
					   "filename\0", filename,
					   NULL);

  return(midi_file);
}
