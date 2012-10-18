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

#include <ags/audio/ags_recycling.h>

#include <ags/lib/ags_list.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <string.h>
#include <math.h>

void ags_recycling_class_init(AgsRecyclingClass *recycling_class);
void ags_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recycling_init(AgsRecycling *recycling);
void ags_recycling_connect(AgsConnectable *connectable);
void ags_recycling_disconnect(AgsConnectable *connectable);
void ags_recycling_finalize(GObject *gobject);

void ags_recycling_real_add_audio_signal(AgsRecycling *recycling,
					 AgsAudioSignal *audio_signal);

void ags_recycling_real_remove_audio_signal(AgsRecycling *recycling,
					    AgsAudioSignal *audio_signal);

enum{
  ADD_AUDIO_SIGNAL,
  REMOVE_AUDIO_SIGNAL,
  LAST_SIGNAL,
};

static gpointer ags_recycling_parent_class = NULL;
static guint recycling_signals[LAST_SIGNAL];

GType
ags_recycling_get_type (void)
{
  static GType ags_type_recycling = 0;

  if(!ags_type_recycling){
    static const GTypeInfo ags_recycling_info = {
      sizeof (AgsRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recycling = g_type_register_static(G_TYPE_OBJECT,
						"AgsRecycling\0",
						&ags_recycling_info, 0);

    g_type_add_interface_static(ags_type_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_recycling);
}

void
ags_recycling_class_init(AgsRecyclingClass *recycling)
{
  GObjectClass *gobject;

  ags_recycling_parent_class = g_type_class_peek_parent(recycling);

  gobject = (GObjectClass *) recycling;
  gobject->finalize = ags_recycling_finalize;

  recycling->add_audio_signal = ags_recycling_real_add_audio_signal;
  recycling->remove_audio_signal = ags_recycling_real_remove_audio_signal;

  recycling_signals[ADD_AUDIO_SIGNAL] =
    g_signal_new("add_audio_signal\0",
		 G_TYPE_FROM_CLASS (recycling),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecyclingClass, add_audio_signal),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  recycling_signals[REMOVE_AUDIO_SIGNAL] =
    g_signal_new("remove_audio_signal\0",
		 G_TYPE_FROM_CLASS (recycling),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecyclingClass, remove_audio_signal),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_recycling_connect;
  connectable->disconnect = ags_recycling_disconnect;
}

void
ags_recycling_init(AgsRecycling *recycling)
{
  //  recycling->flags = 0;

  recycling->channel = NULL;

  recycling->parent = NULL;

  recycling->next = NULL;
  recycling->prev = NULL;

  recycling->audio_signal = NULL;
}

void
ags_recycling_connect(AgsConnectable *connectable)
{
}

void
ags_recycling_disconnect(AgsConnectable *connectable)
{
}

void
ags_recycling_finalize(GObject *gobject)
{
  AgsRecycling *recycling;
  GList *list, *list_next;

  g_warning("ags_recycling_finalize\0");

  recycling = AGS_RECYCLING(gobject);

  /* AgsAudioSignal */
  ags_list_free_and_unref_link(recycling->audio_signal);

  /* call parent */
  G_OBJECT_CLASS(ags_recycling_parent_class)->finalize(gobject);
}

void
ags_recycling_add_audio_signal(AgsRecycling *recycling,
			       AgsAudioSignal *audio_signal)
{
  g_return_if_fail(AGS_IS_RECYCLING(recycling));
  g_return_if_fail(AGS_IS_AUDIO_SIGNAL(audio_signal));

  g_object_ref(G_OBJECT(recycling));
  g_signal_emit(G_OBJECT(recycling),
		recycling_signals[ADD_AUDIO_SIGNAL], 0,
		audio_signal);
  g_object_unref(G_OBJECT(recycling));
}

void
ags_recycling_real_add_audio_signal(AgsRecycling *recycling,
				    AgsAudioSignal *audio_signal)
{
  recycling->audio_signal = g_list_prepend(recycling->audio_signal, (gpointer) audio_signal);
  audio_signal->recycling = (GObject *) recycling;
  g_object_ref(audio_signal);
}

void
ags_recycling_remove_audio_signal(AgsRecycling *recycling,
				  AgsAudioSignal *audio_signal)
{
  g_return_if_fail(AGS_IS_RECYCLING(recycling));

  g_object_ref((GObject *) recycling);
  g_signal_emit(G_OBJECT(recycling),
		recycling_signals[REMOVE_AUDIO_SIGNAL], 0,
		audio_signal);
  g_object_unref((GObject *) recycling);
}

void
ags_recycling_real_remove_audio_signal(AgsRecycling *recycling,
				       AgsAudioSignal *audio_signal)
{
  recycling->audio_signal = g_list_remove(recycling->audio_signal, (gpointer) audio_signal);
  g_object_unref(G_OBJECT(audio_signal));
}

void
ags_recycling_create_audio_signal_with_defaults(AgsRecycling *recycling,
						AgsAudioSignal *audio_signal,
						guint attack)
{
  AgsAudioSignal *template;

  template = ags_audio_signal_get_template(recycling->audio_signal);

  audio_signal->devout = template->devout;
  audio_signal->recycling = (GObject *) recycling;

  ags_audio_signal_duplicate_stream(audio_signal, template, attack);
}

void
ags_recycling_create_audio_signal_with_frame_count(AgsRecycling *recycling,
						   AgsAudioSignal *audio_signal,
						   guint frame_count, guint attack)
{
  AgsDevout *devout;
  AgsAudioSignal *template;
  GList *stream, *template_stream, *template_loop;
  guint k, template_k;
  guint loop_frames;
  guint frames_looped_copied, frames_copied;
  gboolean enter_loop;

  g_message("ags_recycling_create_audio_signal_with_frame_count: before\n\0");

  /* some init */
  template = ags_audio_signal_get_template(recycling->audio_signal);

  audio_signal->devout = template->devout;

  devout = AGS_DEVOUT(audio_signal->devout);

  audio_signal->recycling = (GObject *) recycling;

  /* resize */
  ags_audio_signal_stream_resize(audio_signal,
				 (guint) ceil((double) frame_count / (double) devout->buffer_size));
  
  if(template->length == 0)
    return;

  audio_signal->last_frame = frame_count % devout->buffer_size;

  /* generic copying */
  stream = audio_signal->stream_beginning;
  template_stream = template->stream_beginning;

  frames_copied = 0;

  /* loop related copying */
  template_loop = g_list_nth(template->stream_beginning,
			     (guint) floor((double)(frame_count - template->loop_start) / devout->buffer_size));

  if(frame_count - template->loop_start - template->loop_end > 0){
    loop_frames = frame_count - template->loop_start - template->loop_end;
  }else{
    loop_frames = 0;
  }

  frames_looped_copied = 0;
  k = attack;
  template_k = 0;

  enter_loop = TRUE;

  /* the copy loops */
  while(stream != NULL && template_stream != NULL){
    if(enter_loop && template->loop_start <= frames_copied && template->loop_end > frames_copied){
      for(; stream != NULL && frames_looped_copied < loop_frames;){
	template_k = template->loop_start % devout->buffer_size;
	template_stream = template_loop;

	if(k == devout->buffer_size){
	  k = 0;
	  stream = stream->next;
	}

	if(template_k == devout->buffer_size){
	  template_k = 0;
	  template_stream = template_stream->next;
	}

	for(;
	    template_stream != NULL && frames_looped_copied < loop_frames && k < devout->buffer_size && template_k < devout->buffer_size;
	    k++, template_k++, frames_looped_copied++){

	  /* copy audio data from template to new AgsAudioSignal */
	  ((short*) stream->data)[k] = ((short*) template_stream->data)[template_k];    
	}
      }

      frames_copied += frames_looped_copied;
      enter_loop = FALSE;
    }


    for(; stream != NULL && frames_copied < loop_frames;){
      if(k == devout->buffer_size){
	k = 0;
	stream = stream->next;
      }

      if(template_k == devout->buffer_size){
	template_k = 0;
	template_stream = template_stream->next;
      }

      for(;
	  template_stream != NULL && frames_looped_copied < loop_frames && k < devout->buffer_size && template_k < devout->buffer_size;
	  k++, template_k++, frames_copied++){
	/* copy audio data from template to new AgsAudioSignal */
	((short*) stream->data)[k] = ((short*) template_stream->data)[template_k];
      }
    }
  }

  g_message("ags_recycling_create_audio_signal_with_frame_count: after\n\0");
}

AgsRecycling*
ags_recycling_find_next_channel(AgsRecycling *start_region, AgsRecycling *end_region,
				GObject *prev_channel)
{
  AgsRecycling *recycling;

  recycling = start_region;

  while(recycling != end_region->next){
    if(recycling->channel != prev_channel){
      return(recycling);
    }

    recycling = recycling->next;
  }

  return(NULL);
}

AgsRecycling*
ags_recycling_new(GObject *devout)
{
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;

  recycling = (AgsRecycling *) g_object_new(AGS_TYPE_RECYCLING, NULL);

  audio_signal = ags_audio_signal_new(devout,
				      (GObject *) recycling,
				      NULL);
  audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;

  ags_connectable_connect(AGS_CONNECTABLE(audio_signal));

  recycling->audio_signal = g_list_alloc();
  recycling->audio_signal->data = (gpointer) audio_signal;

  return(recycling);
}
