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

#include <ags/audio/task/ags_add_audio_signal.h>

#include <ags-lib/object/ags_connectable.h>

void ags_add_audio_signal_class_init(AgsAddAudioSignalClass *add_audio_signal);
void ags_add_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_audio_signal_init(AgsAddAudioSignal *add_audio_signal);
void ags_add_audio_signal_connect(AgsConnectable *connectable);
void ags_add_audio_signal_disconnect(AgsConnectable *connectable);
void ags_add_audio_signal_finalize(GObject *gobject);

void ags_add_audio_signal_launch(AgsTask *task);

/**
 * SECTION:ags_add_audio_signal
 * @short_description: add audio_signal object to recycling
 * @title: AgsAddAudioSignal
 * @section_id:
 * @include: ags/audio/task/ags_add_audio_signal.h
 *
 * The #AgsAddAudioSignal task adds #AgsAudioSignal to #AgsRecycling.
 */

static gpointer ags_add_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_add_audio_signal_parent_connectable_interface;

GType
ags_add_audio_signal_get_type()
{
  static GType ags_type_add_audio_signal = 0;

  if(!ags_type_add_audio_signal){
    static const GTypeInfo ags_add_audio_signal_info = {
      sizeof (AgsAddAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_audio_signal = g_type_register_static(AGS_TYPE_TASK,
						       "AgsAddAudioSignal\0",
						       &ags_add_audio_signal_info,
						       0);
    
    g_type_add_interface_static(ags_type_add_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_audio_signal);
}

void
ags_add_audio_signal_class_init(AgsAddAudioSignalClass *add_audio_signal)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_add_audio_signal_parent_class = g_type_class_peek_parent(add_audio_signal);

  /* gobject */
  gobject = (GObjectClass *) add_audio_signal;

  gobject->finalize = ags_add_audio_signal_finalize;

  /* task */
  task = (AgsTaskClass *) add_audio_signal;

  task->launch = ags_add_audio_signal_launch;
}

void
ags_add_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_audio_signal_connect;
  connectable->disconnect = ags_add_audio_signal_disconnect;
}

void
ags_add_audio_signal_init(AgsAddAudioSignal *add_audio_signal)
{
  add_audio_signal->recycling = NULL;
  add_audio_signal->audio_signal = NULL;
  add_audio_signal->devout = NULL;
  add_audio_signal->recall_id = NULL;
  add_audio_signal->audio_signal_flags = 0;
}

void
ags_add_audio_signal_connect(AgsConnectable *connectable)
{
  ags_add_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_add_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_audio_signal_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_audio_signal_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_audio_signal_launch(AgsTask *task)
{
  AgsDevout *devout;
  AgsAddAudioSignal *add_audio_signal;
  AgsAudioSignal *audio_signal, *old_template;
  AgsRecallID *recall_id;
  guint attack, delay;
  guint tic_counter_incr;

  add_audio_signal = AGS_ADD_AUDIO_SIGNAL(task);

  devout = AGS_DEVOUT(add_audio_signal->devout);

  /* check for template to remove */
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (add_audio_signal->audio_signal_flags)) != 0){
    old_template = ags_audio_signal_get_template(add_audio_signal->recycling->audio_signal);
  }else{
    old_template = NULL;
  }

  recall_id = add_audio_signal->recall_id;

  /* create audio signal */
  if(add_audio_signal->audio_signal == NULL){
    add_audio_signal->audio_signal = 
      audio_signal = ags_audio_signal_new((GObject *) devout,
					  (GObject *) add_audio_signal->recycling,
					  (GObject *) recall_id);
    audio_signal->flags = add_audio_signal->audio_signal_flags;
  }else{
    audio_signal = add_audio_signal->audio_signal;
  }

  /* delay and attack */
  tic_counter_incr = devout->tic_counter + 1;

  attack = 0; //devout->attack[((tic_counter_incr == AGS_NOTATION_TICS_PER_BEAT) ?
    //		   0:
    //			   tic_counter_incr)];
  delay = 0; //devout->delay[((tic_counter_incr == AGS_NOTATION_TICS_PER_BEAT) ?
  //		 0:
  //			 tic_counter_incr)];
  
  /* add audio signal */
  ags_recycling_create_audio_signal_with_defaults(add_audio_signal->recycling,
						  audio_signal,
						  delay, attack);
  audio_signal->stream_current = audio_signal->stream_beginning;
  ags_audio_signal_connect(audio_signal);
  
  /*
   * emit add_audio_signal on AgsRecycling
   */
  ags_recycling_add_audio_signal(add_audio_signal->recycling,
				 audio_signal);

  /* remove template */
  if(old_template != NULL){
    ags_recycling_remove_audio_signal(add_audio_signal->recycling,
				      old_template);
  }
}

/**
 * ags_add_audio_signal_new:
 * @recycling: the #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to add
 * @devout: the #AgsDevout defaulting to
 * @recall_id: the #AgsRecallID, may be %NULL if %AGS_AUDIO_SIGNAL_TEMPLATE set
 * @audio_signal_flags: the flags to set
 *
 * Creates an #AgsAddAudioSignal.
 *
 * Returns: an new #AgsAddAudioSignal.
 *
 * Since: 0.4
 */
AgsAddAudioSignal*
ags_add_audio_signal_new(AgsRecycling *recycling,
			 AgsAudioSignal *audio_signal,
			 AgsDevout *devout,
			 AgsRecallID *recall_id,
			 guint audio_signal_flags)
{
  AgsAddAudioSignal *add_audio_signal;

  add_audio_signal = (AgsAddAudioSignal *) g_object_new(AGS_TYPE_ADD_AUDIO_SIGNAL,
							NULL);

  add_audio_signal->recycling = recycling;
  add_audio_signal->audio_signal = audio_signal;
  add_audio_signal->devout = devout;
  add_audio_signal->recall_id = recall_id;
  add_audio_signal->audio_signal_flags = audio_signal_flags;

  return(add_audio_signal);
}
