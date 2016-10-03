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

#include <ags/audio/task/ags_apply_presets.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synth_generator.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_synth_util.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_export_thread.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/file/ags_ipatch.h>

void ags_apply_presets_class_init(AgsApplyPresetsClass *apply_presets);
void ags_apply_presets_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_presets_init(AgsApplyPresets *apply_presets);
void ags_apply_presets_connect(AgsConnectable *connectable);
void ags_apply_presets_disconnect(AgsConnectable *connectable);
void ags_apply_presets_finalize(GObject *gobject);

void ags_apply_presets_launch(AgsTask *task);

void ags_apply_presets_audio(AgsApplyPresets *apply_presets,
			     AgsAudio *audio);
void ags_apply_presets_channel(AgsApplyPresets *apply_presets,
			       AgsChannel *channel);
void ags_apply_presets_audio_signal(AgsApplyPresets *apply_presets,
				    AgsAudioSignal *audio_signal);

/**
 * SECTION:ags_apply_presets
 * @short_description: apply presets
 * @title: AgsApplyPresets
 * @section_id:
 * @include: ags/audio/task/ags_apply_presets.h
 *
 * The #AgsApplyPresets task apply the specified presets.
 */

static gpointer ags_apply_presets_parent_class = NULL;
static AgsConnectableInterface *ags_apply_presets_parent_connectable_interface;

GType
ags_apply_presets_get_type()
{
  static GType ags_type_apply_presets = 0;

  if(!ags_type_apply_presets){
    static const GTypeInfo ags_apply_presets_info = {
      sizeof (AgsApplyPresetsClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_presets_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsApplyPresets),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_presets_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_apply_presets_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_apply_presets = g_type_register_static(AGS_TYPE_TASK,
						    "AgsApplyPresets\0",
						    &ags_apply_presets_info,
						    0);

    g_type_add_interface_static(ags_type_apply_presets,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_apply_presets);
}

void
ags_apply_presets_class_init(AgsApplyPresetsClass *apply_presets)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_apply_presets_parent_class = g_type_class_peek_parent(apply_presets);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_presets;

  gobject->finalize = ags_apply_presets_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_presets;

  task->launch = ags_apply_presets_launch;
}

void
ags_apply_presets_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_apply_presets_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_apply_presets_connect;
  connectable->disconnect = ags_apply_presets_disconnect;
}

void
ags_apply_presets_init(AgsApplyPresets *apply_presets)
{
  apply_presets->scope = NULL;

  apply_presets->pcm_channels = 0;
  apply_presets->samplerate = 0;
  apply_presets->buffer_size = 0;
  apply_presets->format = 0;
}

void
ags_apply_presets_connect(AgsConnectable *connectable)
{
  ags_apply_presets_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_apply_presets_disconnect(AgsConnectable *connectable)
{
  ags_apply_presets_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_apply_presets_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_apply_presets_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_apply_presets_launch(AgsTask *task)
{
  AgsApplyPresets *apply_presets;

  apply_presets = AGS_APPLY_PRESETS(task);
  
  if(AGS_IS_SOUNDCARD(apply_presets->scope)){
    ags_apply_presets_soundcard(apply_presets,
				apply_presets->scope);
  }else if(AGS_IS_AUDIO(apply_presets->scope)){
    ags_apply_presets_audio(apply_presets,
			    (AgsAudio *) apply_presets->scope);
  }else if(AGS_IS_CHANNEL(apply_presets->scope)){
    ags_apply_presets_channel(apply_presets,
			      (AgsChannel *) apply_presets->scope);
  }else if(AGS_IS_AUDIO_SIGNAL(apply_presets->scope)){
    ags_apply_presets_audio_signal(apply_presets,
				   (AgsAudioSignal *) apply_presets->scope);
  }else{
    if(apply_presets->scope != NULL){
      g_warning("ags_apply_presets_launch() - unsupported scope %s\0", G_OBJECT_TYPE_NAME(apply_presets->scope));
    }else{
      g_warning("ags_apply_presets_launch() - unsupported scope (null)0");
    }
  }
}

void
ags_apply_presets_soundcard(AgsApplyPresets *apply_presets,
			    GObject *soundcard)
{
  AgsThread *main_loop;
  AgsThread *export_thread;
  AgsThread *soundcard_thread;
  AgsThread *audio_thread;
  AgsThread *channel_thread;
  
  AgsApplicationContext *application_context;
  
  GList *audio;

  gdouble freq;
  guint channels;
  
  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(soundcard));
  main_loop = (AgsThread *) application_context->main_loop;
  
  freq = ceil((gdouble) apply_presets->samplerate / (gdouble) apply_presets->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  /* reset soundcard */
  ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
			    apply_presets->pcm_channels,
			    apply_presets->samplerate,
			    apply_presets->buffer_size,
			    apply_presets->format);

  /* reset audio loop frequency */
  g_object_set(G_OBJECT(main_loop),
	       "frequency\0", freq,
	       NULL);
    
  /* reset export thread frequency */
  export_thread = main_loop;
  
  while((export_thread = ags_thread_find_type(export_thread,
					      AGS_TYPE_EXPORT_THREAD)) != NULL){
    g_object_set(export_thread,
		 "frequency\0", freq,
		 NULL);

    /* iterate */
    export_thread = g_atomic_pointer_get(&(export_thread->next));
  }

  /* reset soundcard thread frequency */
  soundcard_thread = main_loop;
  
  while((soundcard_thread = ags_thread_find_type(soundcard_thread,
						 AGS_TYPE_SOUNDCARD_THREAD)) != NULL){
    g_object_set(soundcard_thread,
		 "frequency\0", freq,
		 NULL);

    /* iterate */
    soundcard_thread = g_atomic_pointer_get(&(soundcard_thread->next));
  }

  /* reset playback on soundcard */
  if(ags_soundcard_is_playing(AGS_SOUNDCARD(soundcard))){
    ags_soundcard_stop(AGS_SOUNDCARD(soundcard));
    ags_soundcard_play_init(AGS_SOUNDCARD(soundcard),
			    &(AGS_SOUNDCARD_THREAD(soundcard_thread)->error));
  }
  
  /* descend children */
  audio = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));

  while(audio != NULL){
    ags_apply_presets_audio(apply_presets,
			    AGS_AUDIO(audio->data));
    
    audio = audio->next;
  }
}

void
ags_apply_presets_audio(AgsApplyPresets *apply_presets,
			AgsAudio *audio)
{
  g_object_set(audio,
	       "samplerate\0", apply_presets->samplerate,
	       "buffer-size\0", apply_presets->buffer_size,
	       "format\0", apply_presets->format,
	       NULL);
}

void
ags_apply_presets_channel(AgsApplyPresets *apply_presets,
			  AgsChannel *channel)
{
  g_object_set(channel,
	       "samplerate\0", apply_presets->samplerate,
	       "buffer-size\0", apply_presets->buffer_size,
	       "format\0", apply_presets->format,
	       NULL);
}

void
ags_apply_presets_audio_signal(AgsApplyPresets *apply_presets,
			       AgsAudioSignal *audio_signal)
{
  g_object_set(audio_signal,
	       "samplerate\0", apply_presets->samplerate,
	       "buffer-size\0", apply_presets->buffer_size,
	       "format\0", apply_presets->format,
	       NULL);
}

/**
 * ags_apply_presets_new:
 * @scope: a #GObject
 * @pcm_channels: pcm channels
 * @samplerate: samplerate
 * @buffer_size: buffer size
 * @format: format
 *
 * Creates an #AgsApplyPresets.
 *
 * Returns: an new #AgsApplyPresets.
 *
 * Since: 0.7.45
 */
AgsApplyPresets*
ags_apply_presets_new(GObject *scope,
		      guint pcm_channels,
		      guint samplerate,
		      guint buffer_size,
		      guint format)
{
  AgsApplyPresets *apply_presets;

  apply_presets = (AgsApplyPresets *) g_object_new(AGS_TYPE_APPLY_PRESETS,
						   NULL);

  apply_presets->scope = scope;

  apply_presets->pcm_channels = pcm_channels;
  apply_presets->samplerate = samplerate;
  apply_presets->buffer_size = buffer_size;
  apply_presets->format = format;

  return(apply_presets);
}
