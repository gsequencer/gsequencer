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

#include <ags/audio/midi/ags_midi_builder.h>

#include <ags/object/ags_marshal.h>

#include <fcntl.h>
#include <sys/stat.h>

void ags_midi_builder_class_init(AgsMidiBuilderClass *midi_builder);
void ags_midi_builder_init(AgsMidiBuilder *midi_builder);
void ags_midi_builder_finalize(GObject *gobject);

enum{
  LAST_SIGNAL,
};

static gpointer ags_midi_builder_parent_class = NULL;
static guint midi_builder_signals[LAST_SIGNAL];

GType
ags_midi_builder_get_type(void)
{
  static GType ags_type_midi_builder = 0;

  if(!ags_type_midi_builder){
    static const GTypeInfo ags_midi_builder_info = {
      sizeof (AgsMidiBuilderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_builder_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiBuilder),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_builder_init,
    };

    ags_type_midi_builder = g_type_register_static(G_TYPE_OBJECT,
						   "AgsMidiBuilder\0", &ags_midi_builder_info,
						   0);
  }

  return(ags_type_midi_builder);
}

void
ags_midi_builder_class_init(AgsMidiBuilderClass *midi_builder)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_midi_builder_parent_class = g_type_class_peek_parent(midi_builder);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_builder;
  
  gobject->finalize = ags_midi_builder_finalize;
}

void
ags_midi_builder_init(AgsMidiBuilder *midi_builder)
{
}

void
ags_midi_builder_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_midi_builder_parent_class)->finalize(gobject);
}

AgsMidiBuilder*
ags_midi_builder_new(FILE *file)
{
  AgsMidiBuilder *midi_builder;
  
  midi_builder = (AgsMidiBuilder *) g_object_new(AGS_TYPE_MIDI_BUILDER,
						 NULL);

  
  return(midi_builder);
}

