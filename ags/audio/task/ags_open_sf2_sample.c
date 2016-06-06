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

#include <ags/audio/task/ags_open_sf2_sample.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_playable.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_ipatch.h>

void ags_open_sf2_sample_class_init(AgsOpenSf2SampleClass *open_sf2_sample);
void ags_open_sf2_sample_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_open_sf2_sample_init(AgsOpenSf2Sample *open_sf2_sample);
void ags_open_sf2_sample_connect(AgsConnectable *connectable);
void ags_open_sf2_sample_disconnect(AgsConnectable *connectable);
void ags_open_sf2_sample_finalize(GObject *gobject);
void ags_open_sf2_sample_launch(AgsTask *task);

/**
 * SECTION:ags_open_sf2_sample
 * @short_description: open sf2_sample object
 * @title: AgsOpenSf2Sample
 * @section_id:
 * @include: ags/audio/task/ags_open_sf2_sample.h
 *
 * The #AgsOpenSf2Sample task opens Soundfont2 samples.
 */

static gpointer ags_open_sf2_sample_parent_class = NULL;
static AgsConnectableInterface *ags_open_sf2_sample_parent_connectable_interface;

GType
ags_open_sf2_sample_get_type()
{
  static GType ags_type_open_sf2_sample = 0;

  if(!ags_type_open_sf2_sample){
    static const GTypeInfo ags_open_sf2_sample_info = {
      sizeof (AgsOpenSf2SampleClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_open_sf2_sample_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOpenSf2Sample),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_open_sf2_sample_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_open_sf2_sample_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_open_sf2_sample = g_type_register_static(AGS_TYPE_TASK,
						"AgsOpenSf2Sample\0",
						&ags_open_sf2_sample_info,
						0);

    g_type_add_interface_static(ags_type_open_sf2_sample,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_open_sf2_sample);
}

void
ags_open_sf2_sample_class_init(AgsOpenSf2SampleClass *open_sf2_sample)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_open_sf2_sample_parent_class = g_type_class_peek_parent(open_sf2_sample);

  /* GObject */
  gobject = (GObjectClass *) open_sf2_sample;

  gobject->finalize = ags_open_sf2_sample_finalize;

  /* AgsTask */
  task = (AgsTaskClass *) open_sf2_sample;

  task->launch = ags_open_sf2_sample_launch;
}

void
ags_open_sf2_sample_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_open_sf2_sample_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_open_sf2_sample_connect;
  connectable->disconnect = ags_open_sf2_sample_disconnect;
}

void
ags_open_sf2_sample_init(AgsOpenSf2Sample *open_sf2_sample)
{
  open_sf2_sample->channel = NULL;
  open_sf2_sample->filename = NULL;
  open_sf2_sample->preset = NULL;
  open_sf2_sample->instrument = NULL;
  open_sf2_sample->sample = NULL;
}

void
ags_open_sf2_sample_connect(AgsConnectable *connectable)
{
  ags_open_sf2_sample_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_open_sf2_sample_disconnect(AgsConnectable *connectable)
{
  ags_open_sf2_sample_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_open_sf2_sample_finalize(GObject *gobject)
{
  AgsOpenSf2Sample *open_sf2_sample;

  open_sf2_sample = AGS_OPEN_SF2_SAMPLE(gobject);

  g_free(open_sf2_sample->filename);
  g_free(open_sf2_sample->preset);
  g_free(open_sf2_sample->instrument);
  g_free(open_sf2_sample->sample);
  
  G_OBJECT_CLASS(ags_open_sf2_sample_parent_class)->finalize(gobject);
}

void
ags_open_sf2_sample_launch(AgsTask *task)
{
  AgsOpenSf2Sample *open_sf2_sample;

  AgsChannel *channel;
  AgsAudioSignal *audio_signal, *audio_signal_old;
  AgsPlayable *playable;

  AgsIpatch *ipatch;

  GList *list;
  
  GError *error;

  open_sf2_sample = AGS_OPEN_SF2_SAMPLE(task);

  g_message("Open Soundfont2 [%s] - %s %s %s\0",
	    open_sf2_sample->filename,
	    open_sf2_sample->preset,
	    open_sf2_sample->instrument,
	    open_sf2_sample->sample);

  channel = open_sf2_sample->channel;

  ipatch = g_object_new(AGS_TYPE_IPATCH,
			"mode\0", AGS_IPATCH_READ,
			"filename\0", open_sf2_sample->filename,
			NULL);
  ipatch->soundcard = channel->soundcard;

  playable = AGS_PLAYABLE(ipatch);
  
  error = NULL;
  ags_playable_level_select(playable,
			    0, open_sf2_sample->filename,
			    &error);

  /* select first - preset */
  ipatch->nth_level = 1;
  
  error = NULL;
  ags_playable_level_select(playable,
			    1, open_sf2_sample->preset,
			    &error);

  if(error != NULL){
    return;
  }

  /* select second - instrument */
  ipatch->nth_level = 2;

  error = NULL;
  ags_playable_level_select(playable,
			    2, open_sf2_sample->instrument,
			    &error);

  if(error != NULL){
    return;
  }

  /* select third - sample */
  ipatch->nth_level = 3;

  error = NULL;
  ags_playable_level_select(playable,
			    3, open_sf2_sample->sample,
			    &error);

  if(error != NULL){
    return;
  }

  /* set link */
  if(channel->link != NULL){
    error = NULL;
    
    ags_channel_set_link(channel, NULL,
			 &error);
    
    if(error != NULL){
      g_warning("%s\0", error->message);
    }
  }

  /* read audio signal */
  list = ags_playable_read_audio_signal(playable,
					channel->soundcard,
					channel->audio_channel, 1);
  audio_signal = list->data;
  
  /* replace template audio signal */
  audio_signal_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
  ags_recycling_remove_audio_signal(channel->first_recycling,
				    (gpointer) audio_signal_old);
  
  audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
  ags_recycling_add_audio_signal(channel->first_recycling,
				 audio_signal); 
}

/**
 * ags_open_sf2_sample_new:
 * @channel: the #AgsChannel
 * @filename: the Soundfont2 file
 * @preset: the preset
 * @instrument: the instrument
 * @sample: the sample
 *
 * Creates an #AgsOpenSf2Sample.
 *
 * Returns: an new #AgsOpenSf2Sample.
 *
 * Since: 0.7.15
 */
AgsOpenSf2Sample*
ags_open_sf2_sample_new(AgsChannel *channel,
			gchar *filename,
			gchar *preset,
			gchar *instrument,
			gchar *sample)
{
  AgsOpenSf2Sample *open_sf2_sample;

  open_sf2_sample = (AgsOpenSf2Sample *) g_object_new(AGS_TYPE_OPEN_SF2_SAMPLE,
						      NULL);

  open_sf2_sample->channel = channel;
  open_sf2_sample->filename = filename;
  open_sf2_sample->preset = preset;
  open_sf2_sample->instrument = instrument;
  open_sf2_sample->sample = sample;

  return(open_sf2_sample);
}
