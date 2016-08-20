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

#include <ags/audio/midi/ags_midi_file_reader.h>

void ags_midi_file_reader_class_init(AgsMidiFileReaderClass *midi_file_reader);
void ags_midi_file_reader_init(AgsMidiFileReader *midi_file_reader);
void ags_midi_file_reader_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_midi_file_reader_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_midi_file_reader_finalize(GObject *gobject);

/**
 * SECTION:ags_midi_file_reader
 * @short_description: the MIDI file reader
 * @title: AgsMidiFileReader
 * @section_id:
 * @include: ags/audio/midi/ags_midi_file_reader.h
 *
 * #AgsMidiFileReader reads your midi files.
 */

enum{
  PROP_0,
  PROP_MIDI_FILE,
};

static gpointer ags_midi_file_reader_parent_class = NULL;

GType
ags_midi_file_reader_get_type(void)
{
  static GType ags_type_midi_file_reader = 0;

  if(!ags_type_midi_file_reader){
    static const GTypeInfo ags_midi_file_reader_info = {
      sizeof (AgsMidiFileReaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_file_reader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiFileReader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_file_reader_init,
    };

    ags_type_midi_file_reader = g_type_register_static(G_TYPE_OBJECT,
						       "AgsMidiFileReader\0", &ags_midi_file_reader_info,
						       0);
  }

  return(ags_type_midi_file_reader);
}

void
ags_midi_file_reader_class_init(AgsMidiFileReaderClass *midi_file_reader)
{
  GObjectClass *gobject;

  ags_midi_file_reader_parent_class = g_type_class_peek_parent(midi_file_reader);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_file_reader;
  
  gobject->finalize = ags_midi_file_reader_finalize;
}

void
ags_midi_file_reader_init(AgsMidiFileReader *midi_file_reader)
{
  midi_file_reader->midi_file = NULL;
}

void
ags_midi_file_reader_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsMidiFileReader *midi_file_reader;

  midi_file_reader = AGS_MIDI_FILE_READER(gobject);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_file_reader_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsMidiFileReader *midi_file_reader;

  midi_file_reader = AGS_MIDI_FILE_READER(gobject);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_file_reader_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_midi_file_reader_parent_class)->finalize(gobject);
}

gchar*
ags_midi_file_reader_read_channel_message(AgsMidiFileReader *midi_file_reader,
					  GParameter *parameter, guint *n_params)
{
  //TODO:JK: implement me
  
  return(NULL);
}


gchar*
ags_midi_file_reader_read_status_message(AgsMidiFileReader *midi_file_reader,
					 GParameter *parameter, guint *n_params)
{
  //TODO:JK: implement me
  
  return(NULL);
}

AgsMidiFileReader*
ags_midi_file_reader_new(gchar *filename)
{
  AgsMidiFileReader *midi_file_reader;

  midi_file_reader = (AgsMidiFileReader *) g_object_new(AGS_TYPE_MIDI_FILE_READER,
						  NULL);

  if(filename != NULL){
    midi_file_reader->midi_file = g_object_new(AGS_TYPE_MIDI_FILE,
					       "filename\0", filename,
					       NULL);
  }
  
  return(midi_file_reader);
}
