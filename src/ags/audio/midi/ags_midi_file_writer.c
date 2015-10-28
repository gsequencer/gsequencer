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

void ags_midi_file_writer_class_init(AgsMidiFileClass *midi_file_writer);
void ags_midi_file_writer_init(AgsMidiFile *midi_file_writer);
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
 * @short_description: the menu bar.
 * @title: AgsMidiFile
 * @section_id:
 * @include: ags/X/ags_midi_file_writer.h
 *
 * #AgsMidiFile reads your midi files.
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
      sizeof (AgsMidiFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_file_writer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_file_writer_init,
    };

    ags_type_midi_file_writer = g_type_register_static(G_TYPE_OBJECT,
						       "AgsMidiFile\0", &ags_midi_file_writer_info,
						       0);
  }

  return(ags_type_midi_file_writer);
}

void
ags_midi_file_writer_class_init(AgsMidiFileClass *midi_file_writer)
{
  GObjectClass *gobject;

  ags_midi_file_writer_parent_class = g_type_class_peek_parent(midi_file_writer);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_file_writer;
  
  gobject->finalize = ags_midi_file_writer_finalize;
}

void
ags_midi_file_writer_init(AgsMidiFile *midi_file_writer)
{
  midi_file_writer->midi_file = NULL;
}

void
ags_midi_file_writer_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsMidiFile *midi_file_writer;

  midi_file_writer = AGS_MIDI_FILE_WRITER(gobject);
  
  switch(prop_id){
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
  AgsMidiFile *midi_file_writer;

  midi_file_writer = AGS_MIDI_FILE_WRITER(gobject);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_file_writer_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_midi_file_writer_parent_class)->finalize(gobject);
}

void
ags_midi_file_writer_write_channel_message(AgsMidiFileWriter *midi_file_writer,
					   gchar *track_name,
					   guint status, ...)
{
}

void
ags_midi_file_writer_write_status_message(AgsMidiFileWriter *midi_file_writer,
					  gchar *track_name,
					  guint status, ...)
{
}

AgsMidiFile*
ags_midi_file_writer_new(gchar *filename)
{
  AgsMidiFile *midi_file_writer;

  midi_file_writer = (AgsMidiFile *) g_object_new(AGS_TYPE_MIDI_FILE_WRITER,
						  NULL);
  
  if(filename != NULL){
    midi_file_writer->midi_file = g_object_new(AGS_TYPE_MIDI_FILE,
					       "filename\0", filename,
					       NULL);
  }
  
  return(midi_file_writer);
}
