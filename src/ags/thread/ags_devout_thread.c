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

#include <ags/thread/ags_devout_thread.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_timestamp_thread.h>

#include <ags/audio/ags_devout.h>

void ags_devout_thread_class_init(AgsDevoutThreadClass *devout_thread);
void ags_devout_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_devout_thread_init(AgsDevoutThread *devout_thread);
void ags_devout_thread_connect(AgsConnectable *connectable);
void ags_devout_thread_disconnect(AgsConnectable *connectable);
void ags_devout_thread_finalize(GObject *gobject);

void ags_devout_thread_start(AgsThread *thread);
void ags_devout_thread_run(AgsThread *thread);
void ags_devout_thread_stop(AgsThread *thread);

/**
 * SECTION:ags_devout_thread
 * @short_description: devout thread
 * @title: AgsDevoutThread
 * @section_id:
 * @include: ags/thread/ags_devout_thread.h
 *
 * The #AgsDevoutThread acts as audio output thread to soundcard.
 */

static gpointer ags_devout_thread_parent_class = NULL;
static AgsConnectableInterface *ags_devout_thread_parent_connectable_interface;

GType
ags_devout_thread_get_type()
{
  static GType ags_type_devout_thread = 0;

  if(!ags_type_devout_thread){
    static const GTypeInfo ags_devout_thread_info = {
      sizeof (AgsDevoutThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_devout_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDevoutThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_devout_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_devout_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_devout_thread = g_type_register_static(AGS_TYPE_THREAD,
						    "AgsDevoutThread\0",
						    &ags_devout_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_devout_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_devout_thread);
}

void
ags_devout_thread_class_init(AgsDevoutThreadClass *devout_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_devout_thread_parent_class = g_type_class_peek_parent(devout_thread);

  /* GObject */
  gobject = (GObjectClass *) devout_thread;

  gobject->finalize = ags_devout_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) devout_thread;

  thread->start = ags_devout_thread_start;
  thread->run = ags_devout_thread_run;
  thread->stop = ags_devout_thread_stop;
}

void
ags_devout_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_devout_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_devout_thread_connect;
  connectable->disconnect = ags_devout_thread_disconnect;
}

void
ags_devout_thread_init(AgsDevoutThread *devout_thread)
{
  AgsThread *thread;
  
  thread = AGS_THREAD(devout_thread);

  thread->freq = AGS_DEVOUT_THREAD_DEFAULT_JIFFIE;

  devout_thread->timestamp_thread = ags_timestamp_thread_new();
  ags_thread_add_child(thread, devout_thread->timestamp_thread);

  devout_thread->error = NULL;
}

void
ags_devout_thread_connect(AgsConnectable *connectable)
{
  ags_devout_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_devout_thread_disconnect(AgsConnectable *connectable)
{
  ags_devout_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_devout_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_devout_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_devout_thread_start(AgsThread *thread)
{
  AgsDevout *devout;
  AgsDevoutThread *devout_thread;
  static gboolean initialized = FALSE;
  GError *error;

  devout_thread = AGS_DEVOUT_THREAD(thread);

  devout = AGS_DEVOUT(thread->devout);

  /*  */
  devout->flags |= (AGS_DEVOUT_BUFFER3 |
		    AGS_DEVOUT_PLAY |
		    AGS_DEVOUT_NONBLOCKING);

  /*  */
  devout_thread->error = NULL;

  if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
    if(ags_soundcard_get_buffer(AGS_SOUNDCARD(devout)) == NULL){
      ags_soundcard_play_init(AGS_SOUNDCARD(devout),
			      &(devout_thread->error));
      
      if(devout_thread->error == NULL){
	devout->flags &= (~AGS_DEVOUT_START_PLAY);
      }else{
	/* preserve AgsAudioLoop from playing */
	
	return;
      }

#ifdef AGS_DEBUG
      g_message("ags_devout_alsa_play\0");
#endif
    }
  }

  memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(signed short));
  memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(signed short));
  memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(signed short));
  memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(signed short));

  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    AGS_THREAD_CLASS(ags_devout_thread_parent_class)->start(thread);
  }
}

void
ags_devout_thread_run(AgsThread *thread)
{
  AgsDevout *devout;
  AgsDevoutThread *devout_thread;
  long delay;
  GError *error;

  devout_thread = AGS_DEVOUT_THREAD(thread);

  devout = AGS_DEVOUT(thread->devout);

  //  delay = (long) floor(NSEC_PER_SEC / devout->frequency * devout->buffer_size);

  if((AGS_THREAD_INITIAL_RUN & (thread->flags)) != 0){
    //    time(&(devout_thread->time_val));
  }

  //  g_message("play\0");
  if((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
    error = NULL;
    ags_soundcard_play(AGS_SOUNDCARD(devout),
		       &error);
  }

  if(error != NULL){
    //TODO:JK: implement me
  }
}

void
ags_devout_thread_stop(AgsThread *thread)
{
  AgsDevout *devout;
  AgsAudioLoop *audio_loop;
  AgsDevoutThread *devout_thread;

  devout_thread = AGS_DEVOUT_THREAD(thread);

  devout = AGS_DEVOUT(thread->devout);
  audio_loop = AGS_AUDIO_LOOP(thread->parent);

  if((AGS_DEVOUT_START_PLAY & (devout->flags)) != 0){
#ifdef AGS_DEBUG
    g_message("ags_devout_thread_stop:  just starting\n\0");
#endif
    return;
  }

  AGS_THREAD_CLASS(ags_devout_thread_parent_class)->stop(thread);

  devout->flags &= ~(AGS_DEVOUT_PLAY);
  
  if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
    ags_devout_alsa_free(devout);
  }else{  
  }
}

/**
 * ags_devout_thread_new:
 * @devout: the #AgsDevout
 *
 * Create a new #AgsDevoutThread.
 *
 * Returns: the new #AgsDevoutThread
 *
 * Since: 0.4
 */
AgsDevoutThread*
ags_devout_thread_new(GObject *devout)
{
  AgsDevoutThread *devout_thread;

  devout_thread = (AgsDevoutThread *) g_object_new(AGS_TYPE_DEVOUT_THREAD,
						   "devout\0", devout,
						   NULL);


  return(devout_thread);
}
