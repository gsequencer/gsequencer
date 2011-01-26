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

#include <ags/audio/recall/ags_remove_audio_run.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_recall_audio.h>

#include <ags/audio/recall/ags_remove_audio.h>

void ags_remove_audio_run_class_init(AgsRemoveAudioRunClass *remove_audio_run);
void ags_remove_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_remove_audio_run_init(AgsRemoveAudioRun *remove_audio_run);
void ags_remove_audio_run_connect(AgsRunConnectable *run_connectable);
void ags_remove_audio_run_disconnect(AgsRunConnectable *run_connectable);
void ags_remove_audio_run_run_connect(AgsRunConnectable *run_connectable);
void ags_remove_audio_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_remove_audio_run_finalize(GObject *gobject);

void ags_remove_audio_run_remove(AgsRecall *recall);

static gpointer ags_remove_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_remove_audio_run_parent_connectable_interface;
static AgsRunConnectableInterface *ags_remove_audio_run_parent_run_connectable_interface;

GType
ags_remove_audio_run_get_type()
{
  static GType ags_type_remove_audio_run = 0;

  if(!ags_type_remove_audio_run){
    static const GTypeInfo ags_remove_audio_run_info = {
      sizeof (AgsRemoveAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_audio_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
						       "AgsRemoveAudioRun\0",
						       &ags_remove_audio_run_info,
						       0);
    
    g_type_add_interface_static(ags_type_remove_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_remove_audio_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_remove_audio_run);
}

void
ags_remove_audio_run_class_init(AgsRemoveAudioRunClass *remove_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_remove_audio_run_parent_class = g_type_class_peek_parent(remove_audio_run);

  gobject = (GObjectClass *) remove_audio_run;

  gobject->finalize = ags_remove_audio_run_finalize;

  recall = (AgsRecallClass *) remove_audio_run;

  recall->remove = ags_remove_audio_run_remove;
}

void
ags_remove_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remove_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remove_audio_run_connect;
  connectable->disconnect = ags_remove_audio_run_disconnect;
}

void
ags_remove_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_remove_audio_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_remove_audio_run_run_connect;
  run_connectable->disconnect = ags_remove_audio_run_run_disconnect;
}

GQuark
ags_remove_audio_run_error_quark()
{
  return(g_quark_from_static_string("ags-remove-audio-run-error-quark\0"));
}

void
ags_remove_audio_run_init(AgsRemoveAudioRun *remove_audio_run)
{
  remove_audio_run->flags = 0;

  remove_audio_run->fifo_wait_ref = 0;
  pthread_cond_init(&(remove_audio_run->fifo_cond), NULL);
  pthread_mutex_init(&(remove_audio_run->fifo_mutex), NULL);

  remove_audio_run->unref_list = NULL;
}

void
ags_remove_audio_run_connect(AgsRunConnectable *run_connectable)
{
}

void
ags_remove_audio_run_disconnect(AgsRunConnectable *run_connectable)
{
}

void
ags_remove_audio_run_run_connect(AgsRunConnectable *run_connectable)
{
}

void
ags_remove_audio_run_run_disconnect(AgsRunConnectable *run_connectable)
{
}

void
ags_remove_audio_run_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_remove_audio_run_parent_class)->finalize(gobject);
}

void
ags_remove_audio_run_enter_fifo(AgsRemoveAudioRun *remove_audio_run)
{
  gboolean was_waiting;
  void ags_remove_audio_run_fifo_wait(){
    while((AGS_REMOVE_AUDIO_RUN_FIFO_IS_BUSY & (remove_audio_run->flags)) != 0){
      pthread_cond_wait(&(remove_audio_run->fifo_cond),
			&(remove_audio_run->fifo_mutex));
    }
  }

  pthread_mutex_lock(&(remove_audio_run->fifo_mutex));

  if((AGS_REMOVE_AUDIO_RUN_FIFO_IS_BUSY & (remove_audio_run->flags)) != 0){
    remove_audio_run->fifo_wait_ref++;
    was_waiting = TRUE;

    ags_remove_audio_run_fifo_wait();
  }else{
    was_waiting = FALSE;
  }

  /* tell that the fifo is busy */
  remove_audio_run->flags |= AGS_REMOVE_AUDIO_RUN_FIFO_IS_BUSY;
  pthread_mutex_unlock(&(remove_audio_run->fifo_mutex));

  if(was_waiting)
    remove_audio_run->fifo_wait_ref--;
}

void
ags_remove_audio_run_leave_fifo(AgsRemoveAudioRun *remove_audio_run)
{
  /* unlock fifo */
  remove_audio_run->flags &= (~AGS_REMOVE_AUDIO_RUN_FIFO_IS_BUSY);
  pthread_cond_signal(&(remove_audio_run->fifo_cond));
}

void
ags_remove_audio_run_remove(AgsRecall *recall)
{
  AgsDevout *devout;
  AgsRemoveAudio *remove_audio;
  AgsRemoveAudioRun *remove_audio_run;
  GObject *object;
  GList *list;

  AGS_RECALL_CLASS(ags_remove_audio_run_parent_class)->remove(recall);

  remove_audio = AGS_REMOVE_AUDIO(recall->recall_audio);
  remove_audio_run = AGS_REMOVE_AUDIO_RUN(recall);

  /* beginning of thread synchronization */
  ags_remove_audio_run_enter_fifo(remove_audio_run);

  /* unref all objects */
  list = remove_audio_run->unref_list;

  while(list != NULL){
    object = G_OBJECT(list->data);

    g_object_unref(object);

    list = list->next;
  }

  list = remove_audio_run->unref_list;
  remove_audio_run->unref_list = NULL;

  /* end of thread synchronization  */
  ags_remove_audio_run_leave_fifo(remove_audio_run);

  /* free the list */
  g_list_free(list);
}

/*
 * Note you have to call ags_remove_audio_run_leave_fifo after this function else other threads
 * will be blocked as soon they call one of these thread safe functions
 */
gboolean
ags_remove_audio_run_unref_exists(AgsRemoveAudioRun *remove_audio_run, GObject *object)
{
  GObject *current_object;
  GList *list;

  /* beginning of thread synchronization */
  ags_remove_audio_run_enter_fifo(remove_audio_run);

  list = remove_audio_run->unref_list;

  while(list != NULL){
    current_object = G_OBJECT(list->data);

    if(current_object == object)
      return(TRUE);

    list = list->next;
  }

  return(FALSE);
}

void
ags_remove_audio_run_unref_append(AgsRemoveAudioRun *remove_audio_run, GObject *object)
{
  /* beginning of thread synchronization */
  ags_remove_audio_run_enter_fifo(remove_audio_run);

  /* add object */
  remove_audio_run->unref_list = g_list_prepend(remove_audio_run->unref_list, object);

  /* end of thread synchronization  */
  ags_remove_audio_run_leave_fifo(remove_audio_run);
}

void
ags_remove_audio_run_unref_remove(AgsRemoveAudioRun *remove_audio_run, GObject *object,
				  GError **error)
{
  GList *list;
  gboolean object_not_found;

  /* beginning of thread synchronization */
  ags_remove_audio_run_enter_fifo(remove_audio_run);

  /* find object */
  list = g_list_find(remove_audio_run->unref_list, object);

  /* check if object it's possible to remove the object */
  if(list == NULL)
    object_not_found = TRUE;
  else{
    object_not_found = FALSE;

    remove_audio_run->unref_list = g_list_delete_link(remove_audio_run->unref_list, list);
  }

  /* end of thread synchronization  */
  ags_remove_audio_run_leave_fifo(remove_audio_run);

  /* */
  if(object_not_found){
    g_set_error(error,
		AGS_REMOVE_AUDIO_RUN_ERROR,
		AGS_REMOVE_AUDIO_RUN_ERROR_REMOVE_FAILED,
		"failed to remove object %s from unref_list\0",
		G_OBJECT_TYPE_NAME(object));
  }
}

AgsRemoveAudioRun*
ags_remove_audio_run_new(AgsRecallAudio *recall_audio)
{
  AgsRemoveAudioRun *remove_audio_run;

  remove_audio_run = (AgsRemoveAudioRun *) g_object_new(AGS_TYPE_REMOVE_AUDIO_RUN,
							"recall_audio_type\0", AGS_TYPE_REMOVE_AUDIO,
							"recall_audio\0", recall_audio,
							"recall_audio_run_type\0", AGS_TYPE_REMOVE_AUDIO_RUN,
							NULL);

  return(remove_audio_run);
}
