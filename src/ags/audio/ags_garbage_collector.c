/* This file is part of GSequencer.
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

#include <ags/audio/ags_garbage_collector.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio_signal.h>

#include <stdlib.h>

void ags_garbage_collector_class_init (AgsGarbageCollectorClass *garbage_collector_class);
void ags_garbage_collector_init (AgsGarbageCollector *garbage_collector);
void ags_garbage_collector_destroy (GObject *gobject);

GType
ags_garbage_collector_get_type (void)
{
  static GType ags_type_garbage_collector = 0;

  if(!ags_type_garbage_collector){
    static const GTypeInfo ags_garbage_collector_info = {
      sizeof (AgsGarbageCollectorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_garbage_collector_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGarbageCollector),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_garbage_collector_init,
    };
    ags_type_garbage_collector = g_type_register_static (G_TYPE_OBJECT, "AgsGarbageCollector\0", &ags_garbage_collector_info, 0);
  }
  return(ags_type_garbage_collector);
}

void
ags_garbage_collector_class_init(AgsGarbageCollectorClass *garbage_collector_class)
{
}

void
ags_garbage_collector_init(AgsGarbageCollector *garbage_collector)
{
  garbage_collector->flags = 0;

  garbage_collector->devout = NULL;

  garbage_collector->list = NULL;
  garbage_collector->stopped = NULL;
  garbage_collector->start = NULL;
  garbage_collector->end = NULL;
}

void
ags_garbage_collector_destroy(GObject *gobject)
{
  g_object_unref(gobject);
}

void
ags_garbage_collector_add(AgsGarbageCollector *garbage_collector, AgsRecycling *recycling)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_static_mutex_lock(&mutex);
  garbage_collector->list = g_list_prepend(garbage_collector->list, recycling);

  if(garbage_collector->stopped == NULL)
    garbage_collector->stopped = garbage_collector->list;

  g_static_mutex_unlock(&mutex);
}

void
ags_garbage_collector_remove(AgsGarbageCollector *garbage_collector, AgsRecycling *recycling)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_static_mutex_lock(&mutex);

  if((AGS_GARBAGE_COLLECTOR_RUN & garbage_collector->flags) != 0){
    garbage_collector->flags |= AGS_GARBAGE_COLLECTOR_CANCEL_RUN;
    g_static_mutex_unlock(&mutex);

    if(recycling == garbage_collector->stopped->data){
    }

    if(garbage_collector->stopped == garbage_collector->start){
    }

    if(garbage_collector->stopped == garbage_collector->end){
    }

      garbage_collector->list = g_list_remove(garbage_collector->list, (gpointer) recycling);
  }else{
    garbage_collector->list = g_list_remove(garbage_collector->list, (gpointer) recycling);

    g_static_mutex_unlock(&mutex);
  }
}

/*
 * you should set the flag AGS_GARBAGE_COLLECTOR_RUN on before calling this function and wrap it with a mutex
 */
void
ags_garbage_collector_run(AgsGarbageCollector *garbage_collector)
{
  GList *audio_signal;
  gboolean stopped_is_control;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  /*
  void ags_garbage_collector_run_audio_signal(AgsAudioSignal *audio_signal){
    GList *stream, *stream_prev, *lock, *prev_lock;
    guint i;
    gboolean reset_lock_attack;
    static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

    stream = audio_signal->stream;
    i = audio_signal->lock_attack;
    reset_lock_attack = TRUE;

    if(i == AGS_STREAM_LOCK_SIZE){
      prev_lock = audio_signal->lock;
      audio_signal->lock = audio_signal->lock->next;
      audio_signal->lock->prev = NULL;
      free(prev_lock);
      i = 0;
    }else
      lock = audio_signal->lock;

    goto ags_garbage_collector_run_audio_signal0;

    while(stream != audio_signal->current_stream && stream != NULL){
      for(i = 0; i < AGS_STREAM_LOCK_SIZE;){
      ags_garbage_collector_run_audio_signal0:
	if((AGS_GARBAGE_COLLECTOR_CANCEL_RUN & garbage_collector->flags) != 0){
	  return;
	}

	if(AGS_STREAM_LOCK(lock->data)->lock[i] == 0){
	  if(reset_lock_attack){
	    audio_signal->stream = stream->next;
	    stream_prev = stream;
	    stream = stream->next;
	    stream->prev = NULL;
	    free(stream_prev);
	  }else{
	    stream_prev = stream;
	    stream = stream->next;
	    stream->prev = stream_prev->prev;
	    stream_prev->prev->next = stream;
	    free(stream_prev);
	  }

	  AGS_STREAM_LOCK(lock->data)->lock[i] = ~0;
	}else if(reset_lock_attack){
	  reset_lock_attack = FALSE;
	}

	if(AGS_STREAM_LOCK(lock->data)->lock[i] != ~0)
	  stream = stream->next;

	i++;

	if(reset_lock_attack){
	  audio_signal->lock_attack = i;
	  g_static_mutex_unlock(&mutex);
	}
      }

      if(reset_lock_attack){
	prev_lock = audio_signal->lock;
	audio_signal->lock = audio_signal->lock->next;
	audio_signal->lock->prev = NULL;
	free(prev_lock);
      }else
	lock = lock->next;
    }
  }

  if(garbage_collector->stopped == NULL)
    return;

  g_static_mutex_lock(&mutex);
  garbage_collector->start = garbage_collector->stopped;
  garbage_collector->end = NULL;
  g_static_mutex_unlock(&mutex);

  if(garbage_collector->start != garbage_collector->list)
    stopped_is_control = FALSE;
  else
    stopped_is_control = TRUE;

 ags_garbage_collector_run0:

  while(garbage_collector->stopped != garbage_collector->end){
    if((AGS_GARBAGE_COLLECTOR_CANCEL_RUN & garbage_collector->flags) != 0){
      garbage_collector->flags &= (~AGS_GARBAGE_COLLECTOR_RUN);
      return;
    }

    audio_signal = AGS_RECYCLING(garbage_collector->stopped->data)->audio_signal;

    while(audio_signal != NULL && (AGS_AUDIO_SIGNAL(audio_signal->data)->start < AGS_DEVOUT(garbage_collector->devout)->offset)){
      ags_garbage_collector_run_audio_signal((AgsAudioSignal *) audio_signal->data);

      if((AGS_GARBAGE_COLLECTOR_CANCEL_RUN & garbage_collector->flags) != 0){
	garbage_collector->flags &= (~AGS_GARBAGE_COLLECTOR_RUN);
	return;
      }

      audio_signal = audio_signal->next;
    }

    garbage_collector->stopped = garbage_collector->stopped->next;
  }

  if(!stopped_is_control){
    garbage_collector->stopped = garbage_collector->list;

    garbage_collector->end = garbage_collector->start;
    garbage_collector->start = garbage_collector->list;
    stopped_is_control = TRUE;
    goto ags_garbage_collector_run0;
  }

 ags_garbage_collector_runEnd:

  g_static_mutex_lock(&mutex);

  if(garbage_collector->stopped == NULL)
    garbage_collector->stopped = garbage_collector->list;

  garbage_collector->flags &= (~AGS_GARBAGE_COLLECTOR_RUN);

  g_static_mutex_unlock(&mutex);
  */
}

AgsGarbageCollector*
ags_garbage_collector_new()
{
  AgsGarbageCollector *garbage_collector;

  garbage_collector = (AgsGarbageCollector *) g_object_new(AGS_TYPE_GARBAGE_COLLECTOR, NULL);

  return(garbage_collector);
}
