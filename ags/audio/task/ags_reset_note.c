/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/task/ags_reset_note.h>

#include <ags/audio/ags_sound_provider.h>

#include <math.h>

#include <ags/i18n.h>

void ags_reset_note_class_init(AgsResetNoteClass *reset_note);
void ags_reset_note_init(AgsResetNote *reset_note);
void ags_reset_note_dispose(GObject *gobject);
void ags_reset_note_finalize(GObject *gobject);

void ags_reset_note_launch(AgsTask *task);

/**
 * SECTION:ags_reset_note
 * @short_description: reset note internal
 * @title: AgsResetNote
 * @section_id:
 * @include: ags/audio/task/ags_reset_note.h
 *
 * The #AgsResetNote task resets note to recompute the note during next run.
 */

static gpointer ags_reset_note_parent_class = NULL;

AgsResetNote *ags_reset_note = NULL;

GType
ags_reset_note_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_reset_note = 0;

    static const GTypeInfo ags_reset_note_info = {
      sizeof (AgsResetNoteClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_reset_note_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsResetNote),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_reset_note_init,
    };

    ags_type_reset_note = g_type_register_static(AGS_TYPE_TASK,
						 "AgsResetNote",
						 &ags_reset_note_info,
						 0);
    
    g_once_init_leave(&g_define_type_id__volatile, ags_type_reset_note);
  }

  return g_define_type_id__volatile;
}

void
ags_reset_note_class_init(AgsResetNoteClass *reset_note)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_reset_note_parent_class = g_type_class_peek_parent(reset_note);

  /* GObjectClass */
  gobject = (GObjectClass *) reset_note;

  gobject->dispose = ags_reset_note_dispose;
  gobject->finalize = ags_reset_note_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) reset_note;
  
  task->launch = ags_reset_note_launch;
}

void
ags_reset_note_init(AgsResetNote *reset_note)
{
  AGS_TASK(reset_note)->flags |= AGS_TASK_CYCLIC;

  reset_note->note_offset = 0;
  
  reset_note->note = NULL;
}

void
ags_reset_note_dispose(GObject *gobject)
{
  AgsResetNote *reset_note;

  reset_note = AGS_RESET_NOTE(gobject);

  if(reset_note->note != NULL){
    g_list_free_full(reset_note->note,
		     g_object_unref);

    reset_note->note = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_reset_note_parent_class)->dispose(gobject);
}

void
ags_reset_note_finalize(GObject *gobject)
{
  AgsResetNote *reset_note;

  reset_note = AGS_RESET_NOTE(gobject);

  if(reset_note->note != NULL){
    g_list_free_full(reset_note->note,
		     g_object_unref);
  }

  if(reset_note == ags_reset_note){
    ags_reset_note = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_reset_note_parent_class)->finalize(gobject);
}

void
ags_reset_note_launch(AgsTask *task)
{
  AgsResetNote *reset_note;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  GList *note;

  guint note_offset;
  guint x1;
  
  pthread_mutex_t *task_mutex;

  reset_note = AGS_RESET_NOTE(task);

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_note);

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  pthread_mutex_lock(task_mutex);
  
  if((note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(default_soundcard))) == reset_note->note_offset){
    pthread_mutex_unlock(task_mutex);

    return;
  }

  reset_note->note_offset = note_offset;
  
  note = reset_note->note;
  
  while(note != NULL){
    g_object_get(note,
		 "x1", &x1,
		 NULL);  

    g_object_set(note,
		 "x1", x1 + 1,
		 NULL);  

    note = note->next;
  }

  pthread_mutex_unlock(task_mutex);
}

/**
 * ags_reset_note_add:
 * @reset_note: the #AgsResetNote
 * @note: the #AgsNote
 *
 * Add @note.
 *
 * Since: 2.4.0
 */
void
ags_reset_note_add(AgsResetNote *reset_note,
		   AgsNote *note)
{
  pthread_mutex_t *task_mutex;

  if(!AGS_IS_RESET_NOTE(reset_note) ||
     !AGS_IS_NOTE(note)){
    return;
  }

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_note);

  pthread_mutex_lock(task_mutex);

  reset_note->note = g_list_prepend(reset_note->note,
				    note);
  g_object_ref(note);
  
  pthread_mutex_unlock(task_mutex);
}

/**
 * ags_reset_note_remove:
 * @reset_note: the #AgsResetNote
 * @note: the #AgsNote
 *
 * Remove @note.
 *
 * Since: 2.4.0
 */
void
ags_reset_note_remove(AgsResetNote *reset_note,
		      AgsNote *note)
{
  pthread_mutex_t *task_mutex;

  if(!AGS_IS_RESET_NOTE(reset_note) ||
     !AGS_IS_NOTE(note)){
    return;
  }

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_note);

  pthread_mutex_lock(task_mutex);

  if(g_list_find(reset_note->note,
		 note) != NULL){
    reset_note->note = g_list_remove(reset_note->note,
						note);
    g_object_unref(note);
  }
  
  pthread_mutex_unlock(task_mutex);
}

/**
 * ags_reset_note_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsResetNote
 *
 * Since: 2.4.0
 */
AgsResetNote*
ags_reset_note_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);

  if(ags_reset_note == NULL){
    AgsTaskThread *task_thread;
    
    AgsApplicationContext *application_context;

    /* reset note */
    ags_reset_note = ags_reset_note_new();

    pthread_mutex_unlock(&mutex);

    /* add cyclic task */
    application_context = ags_application_context_get_instance();

    task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));
    ags_task_thread_append_cyclic_task(task_thread,
				       (AgsTask *) ags_reset_note);

    /* unref */
    g_object_unref(task_thread);
  }else{
    pthread_mutex_unlock(&mutex);
  }

  return(ags_reset_note);
}

/**
 * ags_reset_note_new:
 *
 * Creates an #AgsResetNote.
 *
 * Returns: an new #AgsResetNote.
 *
 * Since: 2.4.0
 */
AgsResetNote*
ags_reset_note_new()
{
  AgsResetNote *reset_note;
  
  reset_note = (AgsResetNote *) g_object_new(AGS_TYPE_RESET_NOTE,
					     NULL);
  
  return(reset_note);
}
