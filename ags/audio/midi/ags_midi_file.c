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

#include <ags/audio/midi/ags_midi_file.h>

#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include <ags/i18n.h>

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
 * @include: ags/audio/midi/ags_midi_file.h
 *
 * #AgsMidiFile reads or writes your midi files.
 */

enum{
  PROP_0,
  PROP_FILENAME,
};

static gpointer ags_midi_file_parent_class = NULL;

GType
ags_midi_file_get_type(void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_file = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_file);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("assigned filename"),
				   i18n_pspec("The filename to read or write"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);
}

GQuark
ags_midi_file_error_quark()
{
  return(g_quark_from_static_string("ags-midi-file-error-quark"));
}

void
ags_midi_file_init(AgsMidiFile *midi_file)
{
  midi_file->flags = 0;

  /* midi file mutex */
  g_rec_mutex_init(&(midi_file->obj_mutex));

  midi_file->file = NULL;
  midi_file->filename = NULL;
  
  midi_file->buffer = NULL;
  midi_file->buffer_length = 0;
  
  midi_file->offset = AGS_MIDI_FILE_DEFAULT_OFFSET;
  midi_file->format = AGS_MIDI_FILE_DEFAULT_FORMAT;
  midi_file->count = 0;
  midi_file->division = (60 * AGS_USEC_PER_SEC) / AGS_MIDI_FILE_DEFAULT_BEATS;
  midi_file->times = 0;
  midi_file->beat = AGS_MIDI_FILE_DEFAULT_BEATS;
  midi_file->clicks = AGS_MIDI_FILE_DEFAULT_TICKS;

  midi_file->track = NULL;

  midi_file->current_track = NULL;

  midi_file->notation = NULL;
  midi_file->midi = NULL;
}

void
ags_midi_file_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsMidiFile *midi_file;

  GRecMutex *midi_file_mutex;

  midi_file = AGS_MIDI_FILE(gobject);

  /* get midi file mutex */
  midi_file_mutex = AGS_MIDI_FILE_GET_OBJ_MUTEX(midi_file);
  
  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);
      
      g_rec_mutex_lock(midi_file_mutex);

      if(filename == midi_file->filename){
	g_rec_mutex_unlock(midi_file_mutex);
	
	return;
      }

      if(midi_file->filename != NULL){
	g_free(midi_file->filename);
      }

      midi_file->filename = g_strdup(filename);

      g_rec_mutex_unlock(midi_file_mutex);
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

  GRecMutex *midi_file_mutex;

  midi_file = AGS_MIDI_FILE(gobject);

  /* get midi file mutex */
  midi_file_mutex = AGS_MIDI_FILE_GET_OBJ_MUTEX(midi_file);
  
  switch(prop_id){
  case PROP_FILENAME:
    {
      g_rec_mutex_lock(midi_file_mutex);

      g_value_set_string(value,
			 midi_file->filename);

      g_rec_mutex_unlock(midi_file_mutex);
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

  /* call parent */
  G_OBJECT_CLASS(ags_midi_file_parent_class)->finalize(gobject);
}

/**
 * ags_midi_file_open:
 * @midi_file: the #AgsMidiFile
 * @filename: the filename
 *
 * Opens a MIDI file read-only.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_midi_file_open(AgsMidiFile *midi_file,
		   gchar *filename)
{
  if(!AGS_IS_MIDI_FILE(midi_file) ||
     midi_file->filename == NULL){
    return(FALSE);
  }

  g_object_set(midi_file,
	       "filename", filename,
	       NULL);
  
  if(!g_file_test(filename,
		  (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))){
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
 * Since: 3.0.0
 */
gboolean
ags_midi_file_open_from_data(AgsMidiFile *midi_file,
			     unsigned char *data, guint buffer_length)
{
  if(!AGS_IS_MIDI_FILE(midi_file)){
    return(FALSE);
  }

  midi_file->buffer = data;
  midi_file->buffer_length = buffer_length;

  return(TRUE);
}

/**
 * ags_midi_file_rw_open:
 * @midi_file: the #AgsMidiFile
 * @filename: the filename
 * @create: %TRUE create file if not exists, else if %FALSE return
 * 
 * Opens a MIDI file with read-write permission.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_midi_file_rw_open(AgsMidiFile *midi_file,
		      gchar *filename,
		      gboolean create)
{
  if(!AGS_IS_MIDI_FILE(midi_file) ||
     filename == NULL){
    return(FALSE);
  }

  g_object_set(midi_file,
	       "filename", filename,
	       NULL);
  
  if(!create &&
     !g_file_test(filename,
		  (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))){
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
 * Since: 3.0.0
 */
void
ags_midi_file_close(AgsMidiFile *midi_file)
{
  if(!AGS_IS_MIDI_FILE(midi_file) ||
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
 * Since: 3.0.0
 */
unsigned char*
ags_midi_file_read(AgsMidiFile *midi_file)
{
  struct stat sb;

  size_t n_read;
  
  if(!AGS_IS_MIDI_FILE(midi_file) ||
     midi_file->filename == NULL ||
     midi_file->file == NULL){
    return(NULL);
  }
  
  stat(midi_file->filename, &sb);

  if(sb.st_size == 0){
    return(NULL);
  }
  
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
 * Since: 3.0.0
 */
void
ags_midi_file_write(AgsMidiFile *midi_file,
		    unsigned char *data, guint buffer_length)
{
  unsigned char *start;
  
  if(!AGS_IS_MIDI_FILE(midi_file) ||
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
 * Since: 3.0.0
 */
void
ags_midi_file_seek(AgsMidiFile *midi_file, guint position, gint whence)
{
  if(!AGS_IS_MIDI_FILE(midi_file) ||
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
 * Since: 3.0.0
 */
void
ags_midi_file_flush(AgsMidiFile *midi_file)
{
  if(!AGS_IS_MIDI_FILE(midi_file) ||
     midi_file->file == NULL){
    return;
  }

  fflush(midi_file->file);
}

/**
 * ags_midi_file_read_byte:
 * @midi_file: the #AgsMidiFile
 * @error: the #GError pointer return location
 *  
 * Reads a unsigned char quantity.
 *
 * Returns: the current value at file's iteration pointer
 * 
 * Since: 3.0.0
 */
unsigned char
ags_midi_file_read_byte(AgsMidiFile *midi_file,
			GError **error)
{
  unsigned char value;

  if(!AGS_IS_MIDI_FILE(midi_file)){
    return(0x0);
  }

  if(midi_file->iter + 1 < midi_file->buffer + midi_file->buffer_length){
    value = (midi_file->iter[0]);
    
    midi_file->iter += 1;
  }else{
    if(error != NULL){
      g_set_error(error,
		  AGS_MIDI_FILE_ERROR,
		  AGS_MIDI_FILE_ERROR_PREMATURE_EOF,
		  "no more data available in file buffer");
    }
    
    return(0x0);
  }
  
  return(value);
}

/**
 * ags_midi_file_read_gint16:
 * @midi_file: the #AgsMidiFile
 * @error: the #GError pointer return location
 *  
 * Reads a gint16 quantity.
 *
 * Returns: the current value at file's iteration pointer
 * 
 * Since: 3.0.0
 */
gint16
ags_midi_file_read_gint16(AgsMidiFile *midi_file,
			  GError **error)
{
  unsigned char str[2];
  gint16 value = 0;

  if(!AGS_IS_MIDI_FILE(midi_file)){
    return(0x0);
  }

  if(midi_file->iter + 2 < midi_file->buffer + midi_file->buffer_length){
    str[0] = (midi_file->iter[0]);
    str[1] = (midi_file->iter[1]);
    
    midi_file->iter += 2;
    
    value = (str[0] & 0xff);
    value = (value<<8) + (str[1] & 0xff);
  }else{
    if(error != NULL){
      g_set_error(error,
		  AGS_MIDI_FILE_ERROR,
		  AGS_MIDI_FILE_ERROR_PREMATURE_EOF,
		  "no more data available in file buffer");
    }
    
    return(0x0);
  }
  
  return(value);
}

/**
 * ags_midi_file_read_gint24:
 * @midi_file: the #AgsMidiFile
 * @error: the #GError pointer return location
 *  
 * Reads a 24-bit quantity.
 *
 * Returns: the current value at file's iteration pointer
 * 
 * Since: 3.0.0
 */
gint32
ags_midi_file_read_gint24(AgsMidiFile *midi_file,
			  GError **error)
{
  unsigned char str[4];
  gint32 value = 0;

  if(!AGS_IS_MIDI_FILE(midi_file)){
    return(0x0);
  }
  
  if(midi_file->iter + 3 < midi_file->buffer + midi_file->buffer_length){
    str[0] = (unsigned char) 0x00;
    str[1] = (midi_file->iter[0]);
    str[2] = (midi_file->iter[1]);
    str[3] = (midi_file->iter[2]);

    midi_file->iter += 3;
  
    value = (value<<8) + (str[1] & 0xff);
    value = (value<<8) + (str[2] & 0xff);
    value = (value<<8) + (str[3] & 0xff);
  }else{  
    if(error != NULL){
      g_set_error(error,
		  AGS_MIDI_FILE_ERROR,
		  AGS_MIDI_FILE_ERROR_PREMATURE_EOF,
		  "no more data available in file buffer");
    }
    
    return(0x0);
  }
  
  return(value);
}

/**
 * ags_midi_file_read_gint32:
 * @midi_file: the #AgsMidiFile
 * @error: the #GError pointer return location
 *  
 * Reads a gint32 quantity.
 *
 * Returns: the current value at file's iteration pointer
 * 
 * Since: 3.0.0
 */
gint32
ags_midi_file_read_gint32(AgsMidiFile *midi_file,
			  GError **error)
{
  unsigned char str[4];
  gint32 value;
  
  if(!AGS_IS_MIDI_FILE(midi_file)){
    return(0x0);
  }
  
  if(midi_file->iter + 4 < midi_file->buffer + midi_file->buffer_length){
    str[0] = (midi_file->iter[0]);
    str[1] = (midi_file->iter[1]);
    str[2] = (midi_file->iter[2]);
    str[3] = (midi_file->iter[3]);
  
    midi_file->iter += 4;
  
    value = (str[0] & 0xff);
    value = (value<<8) + (str[1] & 0xff);
    value = (value<<8) + (str[2] & 0xff);
    value = (value<<8) + (str[3] & 0xff);
  }else{
    if(error != NULL){
      g_set_error(error,
		  AGS_MIDI_FILE_ERROR,
		  AGS_MIDI_FILE_ERROR_PREMATURE_EOF,
		  "no more data available in file buffer");
    }
    
    return(0x0);
  }
  
  return(value);
}

/**
 * ags_midi_file_read_varlength:
 * @midi_file: the #AgsMidiFile
 * @error: the #GError pointer return location
 *  
 * Reads a variable length quantity.
 *
 * Returns: the current value at file's iteration pointer
 * 
 * Since: 3.0.0
 */
long
ags_midi_file_read_varlength(AgsMidiFile *midi_file,
			     GError **error)
{
  long value;
  guint i;
  unsigned char c;
  gboolean success;
  
  if(!AGS_IS_MIDI_FILE(midi_file)){
    return(0x0);
  }

  c = 0x0;
  success = TRUE;
  
  if(midi_file->iter + 1 < midi_file->buffer + midi_file->buffer_length){
    c = midi_file->iter[0];
    value = c;
    i = 1;

    midi_file->iter += 1;
  }else{
    success = FALSE;
  }
  
  if((c & 0x80) &&
     success){
    value &= 0x7F;
   
    do{
      if(midi_file->iter + 1 < midi_file->buffer + midi_file->buffer_length){
	//TODO:JK: unsafe
	value = (value << 7) + ((c = (midi_file->iter[0])) & 0x7F);
	i++;
	midi_file->iter += 1;
      }else{
	success = FALSE;
      }
    }while((c & 0x80) && success);
  }

  if(!success){
    if(error != NULL){
      g_set_error(error,
		  AGS_MIDI_FILE_ERROR,
		  AGS_MIDI_FILE_ERROR_PREMATURE_EOF,
		  "no more data available in file buffer");
    }
    
    return(0x0);
  }
  
  return(value);
}

/**
 * ags_midi_file_read_text:
 * @midi_file: the #AgsMidiFile
 * @length: the number of bytes to be read, or as long valid string for -1
 * @error: the #GError pointer return location
 *  
 * Reads a string.
 *
 * Returns: the string at file's iteration pointer
 * 
 * Since: 3.0.0
 */
unsigned char*
ags_midi_file_read_text(AgsMidiFile *midi_file,
			gint length,
			GError **error)
{
  unsigned char *text;
  gchar c;
  guint i;
  
  if(!AGS_IS_MIDI_FILE(midi_file)){
    return(NULL);
  }

  if(midi_file->iter + length >= midi_file->buffer + midi_file->buffer_length){
    if(error != NULL){
      g_set_error(error,
		  AGS_MIDI_FILE_ERROR,
		  AGS_MIDI_FILE_ERROR_PREMATURE_EOF,
		  "no more data available in file buffer");
    }

    return(NULL);
  }
  
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
 * ags_midi_file_write_byte:
 * @midi_file: the #AgsMidiFile
 * @val: the value to write
 *
 * Writes a unsigned char quantity to internal buffer.
 *
 * Since: 3.0.0
 */
void
ags_midi_file_write_byte(AgsMidiFile *midi_file, unsigned char val)
{
  if(midi_file->iter + 1 >= (midi_file->buffer + midi_file->buffer_length)){
    size_t new_length;

    new_length = (midi_file->iter + 1) - (midi_file->buffer + midi_file->buffer_length);
    
    midi_file->buffer = (unsigned char *) realloc(midi_file->buffer,
						  new_length * sizeof(unsigned char));

    midi_file->buffer_length = new_length;
  }

  midi_file->iter[0] = val;
  
  midi_file->iter += 1;
}

/**
 * ags_midi_file_write_gint16:
 * @midi_file: the #AgsMidiFile
 * @val: the value to write
 *
 * Writes a gint16 quantity to internal buffer.
 *
 * Since: 3.0.0
 */
void
ags_midi_file_write_gint16(AgsMidiFile *midi_file, gint16 val)
{
  if((midi_file->iter + 2) >= (midi_file->buffer + midi_file->buffer_length)){
    size_t new_length;

    new_length = (midi_file->iter + 2) - (midi_file->buffer + midi_file->buffer_length);
    
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
 * Since: 3.0.0
 */
void
ags_midi_file_write_gint24(AgsMidiFile *midi_file, gint32 val)
{
  if((midi_file->iter + 3) >= (midi_file->buffer + midi_file->buffer_length)){
    size_t new_length;

    new_length = (midi_file->iter + 3) - (midi_file->buffer + midi_file->buffer_length);
    
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
 * Since: 3.0.0
 */
void
ags_midi_file_write_gint32(AgsMidiFile *midi_file, gint32 val)
{
  if((midi_file->iter + 4) >= (midi_file->buffer + midi_file->buffer_length)){
    size_t new_length;

    new_length = (midi_file->iter + 4) - (midi_file->buffer + midi_file->buffer_length);
    
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
 * Since: 3.0.0
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
  if((midi_file->iter + i) >= (midi_file->buffer + midi_file->buffer_length)){
    size_t new_length;

    new_length = (midi_file->iter + i) - (midi_file->buffer + midi_file->buffer_length);
    
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
 * Since: 3.0.0
 */
void
ags_midi_file_write_text(AgsMidiFile *midi_file,
			 gchar *text, guint length)
{
  guint i;
  
  if(text == NULL){
    return;
  }

  if((midi_file->iter + length) >= (midi_file->buffer + midi_file->buffer_length)){
    size_t new_length;

    new_length = (midi_file->iter + length) - (midi_file->buffer + midi_file->buffer_length);
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
 * @error: the #GError pointer return location
 *
 * Reads the MIDI file's header and positions internal buffer pointer just behind it.
 *
 * Returns: the header's bytes
 *
 * Since: 3.0.0
 */
unsigned char*
ags_midi_file_read_header(AgsMidiFile *midi_file,
			  guint *buffer_length,
			  GError **error)
{
  static gchar header[] = "MThd";

  unsigned char *data;
  guint length;

  guint n;
  gchar c;

  GError *local_error;
  
  if(!AGS_IS_MIDI_FILE(midi_file)){
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
  local_error = NULL;
  midi_file->offset = (guint) ags_midi_file_read_gint32(midi_file,
							&local_error);
  
  local_error = NULL;
  midi_file->format = (guint) ags_midi_file_read_gint16(midi_file,
							&local_error);

  local_error = NULL;
  midi_file->count = (guint) ags_midi_file_read_gint16(midi_file,
						       &local_error);

  local_error = NULL;
  midi_file->division = (guint) ags_midi_file_read_gint16(midi_file,
							  &local_error);

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
 * Since: 3.0.0
 */
void
ags_midi_file_write_header(AgsMidiFile *midi_file,
			   unsigned char *buffer, guint length)
{
  guint i;
  
  if(!AGS_IS_MIDI_FILE(midi_file)){
    return;
  }

  if((midi_file->iter + length) >= (midi_file->buffer + midi_file->buffer_length)){
    size_t new_length;

    new_length = (midi_file->iter + length) - (midi_file->buffer + midi_file->buffer_length);
    midi_file->buffer = (unsigned char *) realloc(midi_file->buffer,
						  new_length * sizeof(unsigned char));

    midi_file->buffer_length = new_length;
  }

  for(i = 0; i < length; i++){
    midi_file->iter[i] = buffer[i];
  }
  
  midi_file->iter += length;
}

/**
 * ags_midi_file_read_track_data:
 * @midi_file: the #AgsMidiFile
 * @buffer_length: pointer to return buffer length or %NULL
 * @error: the #GError pointer return location
 *
 * Reads the MIDI file's track data.
 *
 * Returns: the track's bytes
 *
 * Since: 3.0.0
 */
unsigned char*
ags_midi_file_read_track_data(AgsMidiFile *midi_file,
			      guint *buffer_length,
			      GError **error)
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

  GError *local_error;
  
  if(!AGS_IS_MIDI_FILE(midi_file)){
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
  local_error = NULL;
  ags_midi_file_read_gint32(midi_file,
			    &local_error);

  end_of_track = FALSE;
  
  while(!end_of_track){
    local_error = NULL;
    delta_time = ags_midi_file_read_varlength(midi_file,
					      &local_error);
    
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
	      local_error = NULL;
	      text_length = ags_midi_file_read_varlength(midi_file,
							 &local_error);

	      local_error = NULL;
	      text = ags_midi_file_read_text(midi_file,
					     text_length,
					     &local_error);

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
  
  if(!AGS_IS_MIDI_FILE(midi_file)){
    return;
  }

  if((midi_file->iter + length) >= (midi_file->buffer + midi_file->buffer_length)){
    size_t new_length;

    new_length = (midi_file->iter + length) - (midi_file->buffer + midi_file->buffer_length);
    midi_file->buffer = (unsigned char *) realloc(midi_file->buffer,
						  new_length * sizeof(unsigned char));

    midi_file->buffer_length = new_length;
  }

  for(i = 0; i < length; i++){
    midi_file->iter[i] = buffer[i];
  }
  
  midi_file->iter += length;
}

void
ags_midi_file_read_notation(AgsMidiFile *midi_file)
{
  //TODO:JK: implement me
}

void
ags_mid_file_read_midi(AgsMidiFile *midi_file)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_file_new:
 *
 * Create a new instance of #AgsMidiFile
 * 
 * Returns: the new #AgsMidiFile
 *
 * Since: 3.0.0
 */
AgsMidiFile*
ags_midi_file_new()
{
  AgsMidiFile *midi_file;

  midi_file = (AgsMidiFile *) g_object_new(AGS_TYPE_MIDI_FILE,
					   NULL);

  return(midi_file);
}
