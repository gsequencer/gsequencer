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

#include <ags/audio/midi/ags_midi_file_writer.h>

#include <ags/i18n.h>

void ags_midi_file_writer_class_init(AgsMidiFileWriterClass *midi_file_writer);
void ags_midi_file_writer_init(AgsMidiFileWriter *midi_file_writer);
void ags_midi_file_writer_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_midi_file_writer_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_midi_file_writer_finalize(GObject *gobject);

/**
 * SECTION:ags_midi_file_writer
 * @short_description: the MIDI file writer
 * @title: AgsMidiFileWriter
 * @section_id:
 * @include: ags/audio/midi/ags_midi_file_writer.h
 *
 * #AgsMidiFileWriter reads your midi files.
 */

enum{
  PROP_0,
  PROP_MIDI_FILE,
};

static gpointer ags_midi_file_writer_parent_class = NULL;

GType
ags_midi_file_writer_get_type(void)
{
  static GType ags_type_midi_file_writer = 0;

  if(!ags_type_midi_file_writer){
    static const GTypeInfo ags_midi_file_writer_info = {
      sizeof (AgsMidiFileWriterClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_file_writer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiFileWriter),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_file_writer_init,
    };

    ags_type_midi_file_writer = g_type_register_static(G_TYPE_OBJECT,
						       "AgsMidiFileWriter", &ags_midi_file_writer_info,
						       0);
  }

  return(ags_type_midi_file_writer);
}

void
ags_midi_file_writer_class_init(AgsMidiFileWriterClass *midi_file_writer)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_midi_file_writer_parent_class = g_type_class_peek_parent(midi_file_writer);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_file_writer;
  
  gobject->set_property = ags_midi_file_writer_set_property;
  gobject->get_property = ags_midi_file_writer_get_property;

  gobject->finalize = ags_midi_file_writer_finalize;

  /* properties */
  /**
   * AgsMidiFileWriter:midi-file:
   *
   * The assigned #AgsMidiFile to read from.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("midi-file",
				   i18n_pspec("assigned midi file"),
				   i18n_pspec("The midi file to read"),
				   AGS_TYPE_MIDI_FILE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_FILE,
				  param_spec);
}

void
ags_midi_file_writer_init(AgsMidiFileWriter *midi_file_writer)
{
  midi_file_writer->midi_file = NULL;
}

void
ags_midi_file_writer_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsMidiFileWriter *midi_file_writer;

  midi_file_writer = AGS_MIDI_FILE_WRITER(gobject);
  
  switch(prop_id){
  case PROP_MIDI_FILE:
    {
      AgsMidiFile *midi_file;

      midi_file = (AgsMidiFile *) g_value_get_object(value);

      if(midi_file_writer->midi_file == midi_file){
	return;
      }

      if(midi_file_writer->midi_file != NULL){
	g_object_unref(midi_file_writer->midi_file);
      }

      if(midi_file != NULL){
	g_object_ref(midi_file);
      }

      midi_file_writer->midi_file = midi_file;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_file_writer_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsMidiFileWriter *midi_file_writer;

  midi_file_writer = AGS_MIDI_FILE_WRITER(gobject);
  
  switch(prop_id){
  case PROP_MIDI_FILE:
    {
      g_value_set_object(value,
			 midi_file_writer->midi_file);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_file_writer_finalize(GObject *gobject)
{
  AgsMidiFileWriter *midi_file_writer;

  midi_file_writer = (AgsMidiFileWriter *) gobject;

  if(midi_file_writer->midi_file != NULL){
    g_object_unref(midi_file_writer->midi_file);
  }

  G_OBJECT_CLASS(ags_midi_file_writer_parent_class)->finalize(gobject);
}

/**
 * ags_midi_file_writer_write_bytes:
 * @midi_file_writer: the #AgsMidiFileWriter
 * @buffer: the buffer
 * @buffer_length: the buffer length
 *
 * Write @buffer_length count bytes from @buffer to MIDI file.
 *
 * Since: 1.0.0
 */
void
ags_midi_file_writer_write_bytes(AgsMidiFileWriter *midi_file_writer,
				 unsigned char *buffer,
				 guint buffer_length)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_file_writer_write_channel_message:
 * @midi_file_writer: the #AgsMidiFileWriter
 * @track_name: the track name
 * @status: the status
 * @ap: parameters
 * 
 * Writes a channel message.
 * 
 * Since: 1.0.0
 */
void
ags_midi_file_writer_write_channel_message(AgsMidiFileWriter *midi_file_writer,
					   gchar *track_name,
					   guint status, va_list ap)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_file_writer_write_status_message:
 * @midi_file_writer: the #AgsMidiFileWriter
 * @track_name: the track name
 * @status: the status
 * @ap: parameters
 * 
 * Writes a status message.
 * 
 * Since: 1.0.0
 */
void
ags_midi_file_writer_write_status_message(AgsMidiFileWriter *midi_file_writer,
					  gchar *track_name,
					  guint status, va_list ap)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_file_writer_new:
 * @filename: the filename, may be %NULL
 * 
 * Instantiate a new #AgsMidiFileWriter assigned to @filename.
 *
 * Returns: a new #AgsMidiFileWriter
 * 
 * Since: 1.0.0
 */
AgsMidiFileWriter*
ags_midi_file_writer_new(gchar *filename)
{
  AgsMidiFileWriter *midi_file_writer;

  midi_file_writer = (AgsMidiFileWriter *) g_object_new(AGS_TYPE_MIDI_FILE_WRITER,
							"midi-file", g_object_new(AGS_TYPE_MIDI_FILE,
										    "filename", filename,
										    NULL),
							NULL);
  
  return(midi_file_writer);
}
