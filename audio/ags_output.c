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

#include <ags/audio/ags_output.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>

void ags_output_class_init(AgsOutputClass *output_class);
void ags_output_init(AgsOutput *output);
void ags_output_finalize(GObject *gobject);

extern void ags_file_write_output(AgsFile *file, AgsChannel *channel);

static gpointer ags_output_parent_class = NULL;

GType
ags_output_get_type (void)
{
  static GType ags_type_output = 0;

  if(!ags_type_output){
    static const GTypeInfo ags_output_info = {
      sizeof (AgsOutputClass),
      (GBaseInitFunc) NULL, /* base_init */
      (GBaseFinalizeFunc) NULL, /* base_finalize */
      (GClassInitFunc) ags_output_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOutput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_output_init,
    };

    ags_type_output = g_type_register_static(AGS_TYPE_CHANNEL,
					     "AgsOutput\0",
					     &ags_output_info,
					     0);
  }

  return (ags_type_output);
}

void
ags_output_class_init(AgsOutputClass *output)
{
  GObjectClass *gobject;
  AgsChannelClass *channel;

  ags_output_parent_class = g_type_class_peek_parent(output);

  gobject = (GObjectClass *) output;
  gobject->finalize = ags_output_finalize;

  channel = (AgsChannelClass *) output;
  channel->write_file = ags_file_write_output;
}

void
ags_output_init(AgsOutput *output)
{
}

void
ags_output_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_output_parent_class)->finalize(gobject);
}

void
ags_output_connect(AgsOutput *output)
{
}

GList*
ags_output_map_audio_signal(AgsOutput *output, AgsRecallID *recall_id)
{
  AgsDevout *devout;
  AgsAudioSignal *audio_signal;
  GList *list_destination;

  if(output != NULL){
    list_destination = g_list_alloc();
    goto ags_copy_pattern_map_destination0;
  }else
    return(NULL);

  devout = AGS_DEVOUT(AGS_AUDIO(AGS_CHANNEL(output)->audio)->devout);
  
  while(output != NULL){
    list_destination->next = g_list_alloc();
    list_destination->next->prev = list_destination;
    list_destination = list_destination->next;
  ags_copy_pattern_map_destination0:
    fprintf(stdout, "ags_output_map_audio_signal\n\0");

    audio_signal = ags_audio_signal_new((GObject *) devout,
					(GObject *) output->channel.first_recycling,
					(GObject *) recall_id);
    ags_connectable_connect(AGS_CONNECTABLE(audio_signal));

    ags_recycling_add_audio_signal(output->channel.first_recycling,
				   audio_signal);
    audio_signal->stream_current = audio_signal->stream_beginning;
    list_destination->data = (gpointer) audio_signal;

    output = (AgsOutput *) output->channel.next_pad;
  }
  
  return(list_destination);
}

AgsOutput*
ags_output_new(GObject *audio)
{
  AgsOutput *output;

  output = (AgsOutput *) g_object_new(AGS_TYPE_OUTPUT,
				      "audio\0", audio,
				      NULL);

  return(output);
}
