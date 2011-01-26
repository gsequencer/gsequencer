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

#include <ags/audio/recall/ags_remove_audio.h>

#include <ags/audio/recall/ags_remove_audio_run.h>

void ags_remove_audio_class_init(AgsRemoveAudioClass *remove_audio);
void ags_remove_audio_init(AgsRemoveAudio *remove_audio);
void ags_remove_audio_finalize(GObject *gobject);

static gpointer ags_remove_audio_parent_class = NULL;

GType
ags_remove_audio_get_type()
{
  static GType ags_type_remove_audio = 0;

  if(!ags_type_remove_audio){
    static const GTypeInfo ags_remove_audio_info = {
      sizeof (AgsRemoveAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_audio_init,
    };

    ags_type_remove_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						   "AgsRemoveAudio\0",
						   &ags_remove_audio_info,
						   0);
  }

  return(ags_type_remove_audio);
}

void
ags_remove_audio_class_init(AgsRemoveAudioClass *remove_audio)
{
  GObjectClass *gobject;

  ags_remove_audio_parent_class = g_type_class_peek_parent(remove_audio);

  gobject = (GObjectClass *) remove_audio;

  gobject->finalize = ags_remove_audio_finalize;
}

void
ags_remove_audio_init(AgsRemoveAudio *remove_audio)
{
  remove_audio->devout = NULL;
}

void
ags_remove_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_remove_audio_parent_class)->finalize(gobject);
}

AgsRemoveAudio*
ags_remove_audio_new(AgsDevout *devout, AgsAudio *audio)
{
  AgsRemoveAudio *remove_audio;

  remove_audio = (AgsRemoveAudio *) g_object_new(AGS_TYPE_REMOVE_AUDIO,
						 "recall_audio_type\0", AGS_TYPE_REMOVE_AUDIO,
						 "recall_audio_run_type\0", AGS_TYPE_REMOVE_AUDIO_RUN,
						 "audio\0", audio,
						 NULL);

  remove_audio->devout = devout;

  return(remove_audio);
}
