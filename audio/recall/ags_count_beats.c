/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/audio/recall/ags_count_beats.h>

#include <ags/audio/recall/ags_count_beats_run.h>

void ags_count_beats_class_init(AgsCountBeatsClass *count_beats);
void ags_count_beats_init(AgsCountBeats *count_beats);
void ags_count_beats_finalize(GObject *gobject);

static gpointer ags_count_beats_parent_class = NULL;

GType
ags_count_beats_get_type()
{
  static GType ags_type_count_beats = 0;

  if(!ags_type_count_beats){
    static const GTypeInfo ags_count_beats_info = {
      sizeof (AgsCountBeatsClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_count_beats_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCountBeats),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_count_beats_init,
    };

    ags_type_count_beats = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						  "AgsCountBeats\0",
						  &ags_count_beats_info,
						  0);
  }

  return(ags_type_count_beats);
}

void
ags_count_beats_class_init(AgsCountBeatsClass *count_beats)
{
  GObjectClass *gobject;

  ags_count_beats_parent_class = g_type_class_peek_parent(count_beats);

  gobject = (GObjectClass *) count_beats;

  gobject->finalize = ags_count_beats_finalize;
}

void
ags_count_beats_init(AgsCountBeats *count_beats)
{
  count_beats->devout = NULL;
}

void
ags_count_beats_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_count_beats_parent_class)->finalize(gobject);
}

AgsCountBeats*
ags_count_beats_new(AgsAudio *audio)
{
  AgsCountBeats *count_beats;

  count_beats = (AgsCountBeats *) g_object_new(AGS_TYPE_COUNT_BEATS,
					       "audio\0", audio,
					       NULL);
  
  return(count_beats);
}
