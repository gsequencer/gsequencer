/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/task/ags_start_sequencer.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_sequencer_thread.h>

#include <ags/i18n.h>

void ags_start_sequencer_class_init(AgsStartSequencerClass *start_sequencer);
void ags_start_sequencer_init(AgsStartSequencer *start_sequencer);
void ags_start_sequencer_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_start_sequencer_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_start_sequencer_dispose(GObject *gobject);
void ags_start_sequencer_finalize(GObject *gobject);

void ags_start_sequencer_launch(AgsTask *task);

/**
 * SECTION:ags_start_sequencer
 * @short_description: start sequencer object
 * @title: AgsStartSequencer
 * @section_id:
 * @include: ags/audio/task/ags_start_sequencer.h
 *
 * The #AgsStartSequencer task starts sequencer.
 */

static gpointer ags_start_sequencer_parent_class = NULL;

enum{
  PROP_0,
};

GType
ags_start_sequencer_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_start_sequencer = 0;

    static const GTypeInfo ags_start_sequencer_info = {
      sizeof(AgsStartSequencerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_start_sequencer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsStartSequencer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_start_sequencer_init,
    };

    ags_type_start_sequencer = g_type_register_static(AGS_TYPE_TASK,
						      "AgsStartSequencer",
						      &ags_start_sequencer_info,
						      0);

    g_once_init_leave(&g_define_type_id__static, ags_type_start_sequencer);
  }

  return(g_define_type_id__static);
}

void
ags_start_sequencer_class_init(AgsStartSequencerClass *start_sequencer)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_start_sequencer_parent_class = g_type_class_peek_parent(start_sequencer);

  /* gobject */
  gobject = (GObjectClass *) start_sequencer;

  gobject->set_property = ags_start_sequencer_set_property;
  gobject->get_property = ags_start_sequencer_get_property;

  gobject->dispose = ags_start_sequencer_dispose;
  gobject->finalize = ags_start_sequencer_finalize;

  /* properties */
  
  /* task */
  task = (AgsTaskClass *) start_sequencer;

  task->launch = ags_start_sequencer_launch;
}

void
ags_start_sequencer_init(AgsStartSequencer *start_sequencer)
{
  //empty
}

void
ags_start_sequencer_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsStartSequencer *start_sequencer;

  start_sequencer = AGS_START_SEQUENCER(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_sequencer_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsStartSequencer *start_sequencer;

  start_sequencer = AGS_START_SEQUENCER(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_sequencer_dispose(GObject *gobject)
{
  AgsStartSequencer *start_sequencer;

  start_sequencer = AGS_START_SEQUENCER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_start_sequencer_parent_class)->dispose(gobject);
}

void
ags_start_sequencer_finalize(GObject *gobject)
{
  AgsStartSequencer *start_sequencer;

  start_sequencer = AGS_START_SEQUENCER(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_start_sequencer_parent_class)->finalize(gobject);
}

void
ags_start_sequencer_launch(AgsTask *task)
{
  AgsStartSequencer *start_sequencer;

  AgsThread *audio_loop;
  AgsThread *sequencer_thread;

  AgsApplicationContext *application_context;

  start_sequencer = AGS_START_SEQUENCER(task);

  application_context = ags_application_context_get_instance();

  g_return_if_fail(AGS_IS_CONCURRENCY_PROVIDER(application_context));
  
  /* get main loop */
  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  sequencer_thread = ags_thread_find_type(audio_loop,
					  AGS_TYPE_SEQUENCER_THREAD);

  while(sequencer_thread != NULL){
    if(AGS_IS_SEQUENCER_THREAD(sequencer_thread)){
      /* append to AgsSequencer */
      AGS_SEQUENCER_THREAD(sequencer_thread)->error = NULL;
      
#ifdef AGS_DEBUG
      g_message("start sequencer");
#endif
      
      ags_thread_add_start_queue(audio_loop,
				 sequencer_thread);
    }
    
    sequencer_thread = ags_atomic_pointer_get(&(sequencer_thread->next));
  }

  /* unref */
  g_object_unref(audio_loop);
}

/**
 * ags_start_sequencer_new:
 *
 * Creates an #AgsStartSequencer.
 *
 * Returns: an new #AgsStartSequencer.
 *
 * Since: 3.0.0
 */
AgsStartSequencer*
ags_start_sequencer_new()
{
  AgsStartSequencer *start_sequencer;

  start_sequencer = (AgsStartSequencer *) g_object_new(AGS_TYPE_START_SEQUENCER,
						       NULL);

  return(start_sequencer);
}
