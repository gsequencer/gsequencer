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

#include <ags/audio/midi/ags_midi_file.h>

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
  midi_file->filename = NULL;

  midi_file->parser = ags_midi_parser_new();
  midi_file->notation = NULL;
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
}

gboolean
ags_midi_file_open(AgsMidiFile *midi_file)
{
}

gboolean
ags_midi_file_open_from_data(AgsMidiFile *midi_file)
{
}

gboolean
ags_midi_file_rw_open(AgsMidiFile *midi_file)
{
}

void
ags_midi_file_close(AgsMidiFile *midi_file)
{
}

GList*
ags_midi_file_read(AgsMidiFile *midi_file, GError **error)
{
}

void
ags_midi_file_write(AgsMidiFile *midi_file,
		    GList *notation)
{
}

void
ags_audio_file_seek(AgsMidiFile *midi_file, guint offset, gint whence)
{
}

void
ags_audio_file_flush(AgsMidiFile *midi_file)
{
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
