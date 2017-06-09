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
#include <string.h>

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
 * @short_description: the MIDI file
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
						"AgsMidiFile", &ags_midi_file_info,
						0);
  }

  return(ags_type_midi_file);
}

void
ags_midi_file_class_init(AgsMidiFileClass *midi_file)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_midi_file_parent_class = g_type_class_peek_parent(midi_file);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_file;

  gobject->set_property = ags_midi_file_set_property;
  gobject->get_property = ags_midi_file_get_property;
  
  gobject->finalize = ags_midi_file_finalize;

  /* properties */
  /**
   * AgsMidiFile:filename:
   *
   * The assigned filename to perform input/output on.
   * 
   * Since: 0.7.74
   */
  param_spec = g_param_spec_string("filename",
				   "assigned filename",
				   "The filename to read or write",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);
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
  AgsMidiFile *midi_file;

  midi_file = (AgsMidiFile *) gobject;
  
  g_free(midi_file->filename);
  
  G_OBJECT_CLASS(ags_midi_file_parent_class)->finalize(gobject);
}

/**
 * ags_midi_file_open:
 * @midi_file: the #AgsMidiFile
 *
 * Opens a MIDI file read-only.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 0.7.1
 */
gboolean
ags_midi_file_open(AgsMidiFile *midi_file)
{
  if(midi_file == NULL ||
     midi_file->filename == NULL){
    return(FALSE);
  }

  midi_file->file = fopen(midi_file->filename,
			  "r");

  return(TRUE);
}

/**
 * ags_midi_file_open_from_data:
 * @midi_file: the #AgsMidiFile
 * @data: the buffer to set
 * @buffer_length: the length of the buffer
 *
 * Opens a virtual MIDI file residing in @data's array.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 0.7.1
 */
gboolean
ags_midi_file_open_from_data(AgsMidiFile *midi_file,
			     unsigned char *data, guint buffer_length)
{
  if(midi_file == NULL){
    return(FALSE);
  }

  midi_file->buffer = data;
  midi_file->buffer_length = buffer_length;

  return(TRUE);
}

/**
 * ags_midi_file_rw_open:
 * @midi_file: the #AgsMidiFile
 * 
 * Opens a MIDI file with read-write permission.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 0.7.1
 */
gboolean
ags_midi_file_rw_open(AgsMidiFile *midi_file)
{
  if(midi_file == NULL ||
     midi_file->filename == NULL){
    return(FALSE);
  }
  
  midi_file->file = fopen(midi_file->filename,
			  "r+");

  return(TRUE);
}

/**
 * ags_midi_file_close:
 * @midi_file: the #AgsMidiFile
 *
 * Closes the file stream.
 *
 * Since: 0.7.1
 */
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

/**
 * ags_midi_file_read:
 * @midi_file: the #AgsMidiFile
 *
 * Reads all bytes of the file's stream and stores them in the internal buffer.
 *
 * Returns: the data array just read
 * 
 * Since: 0.7.1
 */
unsigned char*
ags_midi_file_read(AgsMidiFile *midi_file)
{
  struct stat sb;

  size_t n_read;
  
  if(midi_file == NULL ||
     midi_file->filename == NULL ||
     midi_file->file == NULL){
    return(NULL);
  }
  
  stat(midi_file->filename, &sb);

  midi_file->buffer_length = sb.st_size + 1;
  midi_file->buffer = (unsigned char *) malloc(midi_file->buffer_length * sizeof(unsigned char));
  midi_file->buffer[sb.st_size] = EOF;
  n_read = fread(midi_file->buffer, sizeof(unsigned char), sb.st_size, midi_file->file);

  if(n_read != sb.st_size){
    g_critical("fread() number of bytes read doesn't match buffer size");
  }
  
  midi_file->iter = midi_file->buffer;
  
  return(midi_file->buffer);
}

/**
 * ags_midi_file_write:
 * @midi_file: the #AgsMidiFile
 * @data: the data array to write
 * @buffer_length: n-bytes to write
 *
 * Writes @data to the file stream and to internal buffer, reallocates it if necessary.
 * 
 * Since: 0.7.1
 */
void
ags_midi_file_write(AgsMidiFile *midi_file,
		    unsigned char *data, guint buffer_length)
{
  unsigned char *start;
  
  if(midi_file == NULL ||
     midi_file->file == NULL){
    return;
  }

  if(midi_file->buffer == NULL){
    start =
      midi_file->buffer = (unsigned char *) malloc((buffer_length + 1) * sizeof(unsigned char));
    midi_file->buffer_length = buffer_length + 1;
  }else{
    guint old_buffer_length;

    old_buffer_length = midi_file->buffer_length;

    midi_file->buffer = realloc(midi_file->buffer,
				(old_buffer_length + buffer_length) * sizeof(unsigned char));
    start = &(midi_file->buffer[old_buffer_length - 1]);
  }

  memcpy(start, data, buffer_length * sizeof(unsigned char));
  start[buffer_length] = EOF;
  
  fwrite(data, sizeof(unsigned char), buffer_length, midi_file->file);
}

/**
 * ags_midi_file_seek:
 * @midi_file: the #AgsMidiFile
 * @position: the offset
 * @whence: SEEK_SET, SEEK_END, or SEEK_CUR
 *
 * Seeks the file stream's offset.
 *
 * Since: 0.7.1
 */
void
ags_midi_file_seek(AgsMidiFile *midi_file, guint position, gint whence)
{
  if(midi_file == NULL ||
     midi_file->file == NULL){
    return;
  }

  fseek(midi_file->file, position, whence);
}

/**
 * ags_midi_file_flush:
 * @midi_file: the #AgsMidiFile
 * 
 * Flushes file stream's data buffer to disc.
 *
 * Since: 0.7.1
 */
void
ags_midi_file_flush(AgsMidiFile *midi_file)
{
  if(midi_file == NULL ||
     midi_file->file == NULL){
    return;
  }

  fflush(midi_file->file);
}

/**
 * ags_midi_file_read_gint16:
 * @midi_file: the #AgsMidiFile
 *  
 * Reads a gint16 quantity.
 *
 * Returns: the current value at file's iteration pointer
 * 
 * Since: 0.7.1
 */
gint16
ags_midi_file_read_gint16(AgsMidiFile *midi_file)
{
  unsigned char str[2];
  gint16 value = 0;

  str[0] = (midi_file->iter[0]);
  str[1] = (midi_file->iter[1]);

  midi_file->iter += 2;
  
  value = (str[0] & 0xff);
  value = (value<<8) + (str[1] & 0xff);
  
  return(value);
}

/**
 * ags_midi_file_read_gint24:
 * @midi_file: the #AgsMidiFile
 *  
 * Reads a 24-bit quantity.
 *
 * Returns: the current value at file's iteration pointer
 * 
 * Since: 0.7.1
 */
gint32
ags_midi_file_read_gint24(AgsMidiFile *midi_file)
{
  unsigned char str[4];
  gint32 value = 0;
  
  str[0] = (unsigned char) 0x00;
  str[1] = (midi_file->iter[0]);
  str[2] = (midi_file->iter[1]);
  str[3] = (midi_file->iter[2]);

  midi_file->iter += 3;
  
  value = (value<<8) + (str[1] & 0xff);
  value = (value<<8) + (str[2] & 0xff);
  value = (value<<8) + (str[3] & 0xff);
  
  return(value);
}

/**
 * ags_midi_file_read_gint32:
 * @midi_file: the #AgsMidiFile
 *  
 * Reads a gint32 quantity.
 *
 * Returns: the current value at file's iteration pointer
 * 
 * Since: 0.7.1
 */
gint32
ags_midi_file_read_gint32(AgsMidiFile *midi_file)
{
  unsigned char str[4];
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

/**
 * ags_midi_file_read_varlength:
 * @midi_file: the #AgsMidiFile
 *  
 * Reads a variable length quantity.
 *
 * Returns: the current value at file's iteration pointer
 * 
 * Since: 0.7.1
 */
long
ags_midi_file_read_varlength(AgsMidiFile *midi_file)
{
  long value;
  guint i;
  unsigned char c;
  
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

/**
 * ags_midi_file_read_text:
 * @midi_file: the #AgsMidiFile
 * @length: the number of bytes to be read, or as long valid string for -1
 *  
 * Reads a string.
 *
 * Returns: the string at file's iteration pointer
 * 
 * Since: 0.7.1
 */
unsigned char*
ags_midi_file_read_text(AgsMidiFile *midi_file,
			gint length)
{
  unsigned char *text;
  gchar c;
  guint i;

  text = (unsigned char *) malloc((AGS_MIDI_FILE_MAX_TEXT_LENGTH + 1) * sizeof(unsigned char));
  memset(text, 0, AGS_MIDI_FILE_MAX_TEXT_LENGTH * sizeof(unsigned char));
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
    
  return(text);
}

/**
 * ags_midi_file_write_gint16:
 * @midi_file: the #AgsMidiFile
 * @val: the value to write
 *
 * Writes a gint16 quantity to internal buffer.
 *
 * Since: 0.7.1
 */
void
ags_midi_file_write_gint16(AgsMidiFile *midi_file, gint16 val)
{
  if(&(midi_file->iter[2]) >= &(midi_file->buffer[midi_file->buffer_length])){
    size_t new_length;

    new_length = &(midi_file->iter[2]) - &(midi_file->buffer[midi_file->buffer_length]);
    
    midi_file->buffer = (unsigned char *) realloc(midi_file->buffer,
						  new_length * sizeof(unsigned char));

    midi_file->buffer_length = new_length;
  }

  midi_file->iter[0] = 0xff & val;
  midi_file->iter[1] = (0xff00 & val) >> 8;
  
  midi_file->iter += 2;
}

/**
 * ags_midi_file_write_gint24:
 * @midi_file: the #AgsMidiFile
 * @val: the value to write
 *
 * Writes a 24-bit quantity to internal buffer.
 *
 * Since: 0.7.1
 */
void
ags_midi_file_write_gint24(AgsMidiFile *midi_file, gint32 val)
{
  if(&(midi_file->iter[3]) >= &(midi_file->buffer[midi_file->buffer_length])){
    size_t new_length;

    new_length = &(midi_file->iter[3]) - &(midi_file->buffer[midi_file->buffer_length]);
    
    midi_file->buffer = (unsigned char *) realloc(midi_file->buffer,
						  new_length * sizeof(unsigned char));

    midi_file->buffer_length = new_length;
  }

  midi_file->iter[0] = 0xff & val;
  midi_file->iter[1] = (0xff00 & val) >> 8;
  midi_file->iter[2] = (0xff0000 & val) >> 16;
  
  midi_file->iter += 3;
}

/**
 * ags_midi_file_write_gint32:
 * @midi_file: the #AgsMidiFile
 * @val: the value to write
 *
 * Writes a gint32 quantity to internal buffer.
 *
 * Since: 0.7.1
 */
void
ags_midi_file_write_gint32(AgsMidiFile *midi_file, gint32 val)
{
  if(&(midi_file->iter[4]) >= &(midi_file->buffer[midi_file->buffer_length])){
    size_t new_length;

    new_length = &(midi_file->iter[4]) - &(midi_file->buffer[midi_file->buffer_length]);
    
    midi_file->buffer = (unsigned char *) realloc(midi_file->buffer,
						  new_length * sizeof(unsigned char));

    midi_file->buffer_length = new_length;
  }

  midi_file->iter[0] = 0xff & val;
  midi_file->iter[1] = (0xff00 & val) >> 8;
  midi_file->iter[2] = (0xff0000 & val) >> 16;
  midi_file->iter[3] = (0xff000000 & val) >> 24;
  
  midi_file->iter += 4;
}

/**
 * ags_midi_file_write_varlenght:
 * @midi_file: the #AgsMidiFile
 * @val: the value to write
 *
 * Writes a variable length quantity to internal buffer.
 *
 * Since: 0.7.1
 */
void
ags_midi_file_write_varlength(AgsMidiFile *midi_file, long val)
{
  gchar c;
  long mask;
  guint i, j;
  
  mask = 0xff;

  /* retrieve new size */
  i = 0;

  do{
    c = ((mask << (i * 8)) & val) >> (i * 8);
    i++;
  }while(0x80 & c);

  /* realloc buffer if needed */
  if(&(midi_file->iter[i]) >= &(midi_file->buffer[midi_file->buffer_length])){
    size_t new_length;

    new_length = &(midi_file->iter[i]) - &(midi_file->buffer[midi_file->buffer_length]);
    
    midi_file->buffer = (unsigned char *) realloc(midi_file->buffer,
						  new_length * sizeof(unsigned char));

    midi_file->buffer_length = new_length;
  }

  /* write to internal buffer */
  for(j = 0; j < i; i++){
    midi_file->iter[j] = ((mask << (j * 8)) & val) >> (j * 8);
  }
  
  midi_file->iter += i;
}

/**
 * ags_midi_file_write_text:
 * @midi_file: the #AgsMidiFile
 * @text: the text
 * @length: the string's length
 *
 * Writes a string to internal buffer up to length bytes.
 *
 * Since: 0.7.1
 */
void
ags_midi_file_write_text(AgsMidiFile *midi_file,
			 gchar *text, guint length)
{
  guint i;
  
  if(text == NULL){
    return;
  }

  if(&(midi_file->iter[length]) >= &(midi_file->buffer[midi_file->buffer_length])){
    size_t new_length;

    new_length = &(midi_file->iter[length]) - &(midi_file->buffer[midi_file->buffer_length]);
    midi_file->buffer = (unsigned char *) realloc(midi_file->buffer,
						  new_length * sizeof(unsigned char));

    midi_file->buffer_length = new_length;
  }

  for(i = 0; i < length; i++){
    midi_file->iter[i] = text[i];
  }
  
  midi_file->iter += length;
}

/**
 * ags_midi_file_read_header:
 * @midi_file: the #AgsMidiFile
 * @buffer_length: pointer to return buffer length or %NULL
 *
 * Reads the MIDI file's header and positions internal buffer pointer just behind it.
 *
 * Returns: the header's bytes
 *
 * Since: 0.7.1
 */
unsigned char*
ags_midi_file_read_header(AgsMidiFile *midi_file,
			  guint *buffer_length)
{
  static gchar header[] = "MThd";

  unsigned char *data;
  guint length;

  guint n;
  gchar c;

  if(midi_file == NULL){
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
    c = midi_file->iter[n];
    
    if(c == header[n]){
      n++;
    }else{
      n = 0;
    }
  }

  /* position internal iteration pointer */
  midi_file->iter += 4;
  length += 4;

  /* get some values */
  midi_file->offset = (guint) ags_midi_file_read_gint32(midi_file);
  midi_file->format = (guint) ags_midi_file_read_gint16(midi_file);
  midi_file->count = (guint) ags_midi_file_read_gint16(midi_file);
  midi_file->division = (guint) ags_midi_file_read_gint16(midi_file);

  if((midi_file->division) & 0x8000){
    /* SMPTE */
    midi_file->times = 0; /* Can't do beats */
  }

  midi_file->beat =
    midi_file->clicks = midi_file->division;

  length += 10;

  /* return values */
  if(buffer_length != NULL){
    *buffer_length = length;
  }

  return(data);
}

/**
 * ags_midi_file_write_header:
 * @midi_file: the #AgsMidiFile
 * @buffer: the buffer to write
 * @buffer_length: the length of the buffer
 *
 * Write header bytes.
 *
 * Since: 0.7.1
 */
void
ags_midi_file_write_header(AgsMidiFile *midi_file,
			   unsigned char *buffer, guint length)
{
  guint i;
  
  if(midi_file == NULL){
    return;
  }

  if(&(midi_file->iter[length]) >= &(midi_file->buffer[midi_file->buffer_length])){
    size_t new_length;

    new_length = &(midi_file->iter[length]) - &(midi_file->buffer[midi_file->buffer_length]);
    midi_file->buffer = (unsigned char *) realloc(midi_file->buffer,
						  new_length * sizeof(unsigned char));

    midi_file->buffer_length = new_length;
  }

  for(i = 0; i < length; i++){
    midi_file->iter[i] = buffer[i];
  }
  
  midi_file->iter += length;
}

unsigned char*
ags_midi_file_read_track_data(AgsMidiFile *midi_file,
			      guint *buffer_length)
{
  gchar *track_name;
  unsigned char *data, *start;
  guint length;

  long delta_time;
  guint status;
  guint n;
  gchar c;
  gboolean end_of_track;

  static gchar track[] = "MTrk";
  
  if(midi_file == NULL){
    if(buffer_length != NULL){
      *buffer_length = 0;
    }
    
    return(NULL);
  }

  track_name = NULL;
  data = NULL;
  length = 0;
  
  /* midi track */
  n = 0;
  
  while(n < 4 &&
	midi_file->iter < &(midi_file->buffer[midi_file->buffer_length])){
    c = midi_file->iter[0];
    midi_file->iter += 1;
    
    if(c == track[n]){
      n++;
    }else{
      n = 0;
    }
  }

  start = midi_file->iter - 4;
  
  /* offset */
  ags_midi_file_read_gint32(midi_file);

  end_of_track = FALSE;
  
  while(!end_of_track){
    delta_time = ags_midi_file_read_varlength(midi_file);
    
    status = midi_file->iter[0];
    midi_file->iter += 1;
    
    if((0xf0 & (0xf0 & status)) != 0xf0){
#ifdef AGS_DEBUG
      g_message("channel message");
#endif
    }else{
#ifdef AGS_DEBUG
      g_message("status message");
#endif
      
      switch(status){
      case 0xf0:
	{
	  /* start of system exclusive */
	  while(midi_file->iter[0] != 0xf7 &&
		midi_file->iter < &(midi_file->buffer[midi_file->buffer_length])){
	    midi_file->iter += 1;
	  }
	}
      case 0xf1:
	{
	  /* quarter frame */
	  midi_file->iter += 1;
	}
	break;
      case 0xf2:
	{
	  /* song position */
	  midi_file->iter += 2;
	}
	break;
      case 0xf3:
	{
	  /* song select */
	  midi_file->iter += 1;
	}
	break;
      case 0xf4:
      case 0xf5:
	{
	  /* undefined */
	}
	break;
      case 0xf6:
	{
	  /* tune request */
	}
	break;
      case 0xf7:
	{
	  /* sysex continuation or arbitrary stuff */
#ifdef AGS_DEBUG
	  g_message("sysex end");
#endif
	}
	break;
      case 0xff:
	{
	  guint meta_type;
	  
	  /* meta event */
	  meta_type = midi_file->iter[0];
	  midi_file->iter += 1;

	  switch(meta_type){
	  case 0x00:
	    {
	      int c;

	      c = midi_file->iter[0];
	      midi_file->iter += 1;

	      if(c == 0x02){
		midi_file->iter += 2;
	      }
	    }
	    break;
	  case 0x01:      /* Text event */
	  case 0x02:      /* Copyright notice */
	  case 0x03:      /* Sequence/Track name */
	  case 0x04:      /* Instrument name */
	  case 0x05:      /* Lyric */
	  case 0x06:      /* Marker */
	  case 0x07:      /* Cue point */
	  case 0x08:
	  case 0x09:
	  case 0x0a:
	  case 0x0b:
	  case 0x0c:
	  case 0x0d:
	  case 0x0e:
	  case 0x0f:
	    {
	      gchar *text;
	      guint text_length;
	      
	      /* These are all text events */
	      text_length = ags_midi_file_read_varlength(midi_file);
	      text = ags_midi_file_read_text(midi_file,
					     text_length);

	      g_free(text);
	    }
	    break;
	  case 0x2f:
	    {
	      int c;

	      c = midi_file->iter[0];
	      midi_file->iter += 1;

	      if(c == 0x0){
		/* End of Track */
		end_of_track = TRUE;
	      }
	    }
	    break;
	  case 0x51:
	    {
	      int c;

	      c = midi_file->iter[0];
	      midi_file->iter += 1;

	      if(c == 0x03){
		/* Set tempo */
		midi_file->iter += 3;
	      }
	    }
	    break;
	  case 0x54:
	    {
	      int c;

	      c = midi_file->iter[0];
	      midi_file->iter += 1;

	      if(c == 0x05){
		midi_file->iter += 5;
	      }
	    }
	    break;
	  case 0x58:
	    {
	      int c;

	      c = midi_file->iter[0];
	      midi_file->iter += 1;
      
	      if(c == 0x04){
		/* time signature */
		midi_file->iter += 4;
	      }
	    }
	    break;
	  case 0x59:
	    {
	      int c;

	      c = midi_file->iter[0];
	      midi_file->iter += 1;

	      if(c == 0x02){
		/* key signature */
		midi_file->iter += 2;
	      }
	    }
	    break;
	  case 0x7f:
	    {
	      /* sequencer meta event */
	      midi_file->iter += 3;
	    }
	    break;
	  default:
	    {
	      /* misc */
	    }
	  }
	}
	break;
      default:
	g_warning("bad byte");
	break;
      }
    }
  }
  
  /* return value */
  length = midi_file->iter - start;
  
  if(buffer_length != NULL){
    *buffer_length = length;
  }

  data = malloc(length * sizeof(unsigned char));
  memcpy(data, start, length * sizeof(unsigned char));
  
  return(data);
}

void
ags_midi_file_write_track_data(AgsMidiFile *midi_file,
			       unsigned char *buffer, guint length)
{
  guint i;
  
  if(midi_file == NULL){
    return;
  }

  if(&(midi_file->iter[length]) >= &(midi_file->buffer[midi_file->buffer_length])){
    size_t new_length;

    new_length = &(midi_file->iter[length]) - &(midi_file->buffer[midi_file->buffer_length]);
    midi_file->buffer = (unsigned char *) realloc(midi_file->buffer,
						  new_length * sizeof(unsigned char));

    midi_file->buffer_length = new_length;
  }

  for(i = 0; i < length; i++){
    midi_file->iter[i] = buffer[i];
  }
  
  midi_file->iter += length;
}

AgsMidiFile*
ags_midi_file_new(gchar *filename)
{
  AgsMidiFile *midi_file;

  midi_file = (AgsMidiFile *) g_object_new(AGS_TYPE_MIDI_FILE,
					   "filename", filename,
					   NULL);

  return(midi_file);
}
