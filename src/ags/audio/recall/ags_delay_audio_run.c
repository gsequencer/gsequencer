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

#include <ags/audio/recall/ags_delay_audio_run.h>

#include <ags/object/ags_marshal.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_delay_audio.h>

#include <stdlib.h>
#include <math.h>

void ags_delay_audio_run_class_init(AgsDelayAudioRunClass *delay);
void ags_delay_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_delay_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_delay_audio_run_init(AgsDelayAudioRun *delay);
void ags_delay_audio_run_connect(AgsConnectable *connectable);
void ags_delay_audio_run_disconnect(AgsConnectable *connectable);
void ags_delay_audio_run_run_connect(AgsRunConnectable *run_connectable);
void ags_delay_audio_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_delay_audio_run_finalize(GObject *gobject);

void ags_delay_audio_run_run_init_pre(AgsRecall *recall);
void ags_delay_audio_run_run_pre(AgsRecall *recall);
void ags_delay_audio_run_done(AgsRecall *recall);
void ags_delay_audio_run_cancel(AgsRecall *recall);
void ags_delay_audio_run_remove(AgsRecall *recall);
AgsRecall* ags_delay_audio_run_duplicate(AgsRecall *recall,
					 AgsRecallID *recall_id,
					 guint *n_params, GParameter *parameter);
void ags_delay_audio_run_notify_dependency(AgsRecall *recall, guint notify_mode, gint count);

enum{
  NOTATION_ALLOC_OUTPUT,
  NOTATION_ALLOC_INPUT,
  NOTATION_COUNT,
  SEQUENCER_ALLOC_OUTPUT,
  SEQUENCER_ALLOC_INPUT,
  SEQUENCER_COUNT,
  LAST_SIGNAL,
};

static gpointer ags_delay_audio_run_parent_class = NULL;
static AgsConnectableInterface *ags_delay_audio_run_parent_connectable_interface;
static AgsRunConnectableInterface *ags_delay_audio_run_parent_run_connectable_interface;

static guint delay_audio_run_signals[LAST_SIGNAL];

GType
ags_delay_audio_run_get_type()
{
  static GType ags_type_delay_audio_run = 0;

  if(!ags_type_delay_audio_run){
    static const GTypeInfo ags_delay_audio_run_info = {
      sizeof (AgsDelayAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_delay_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDelayAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_delay_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_delay_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_delay_audio_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_delay_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
						      "AgsDelayAudioRun\0",
						      &ags_delay_audio_run_info,
						      0);

    g_type_add_interface_static(ags_type_delay_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_delay_audio_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_delay_audio_run);
}

void
ags_delay_audio_run_class_init(AgsDelayAudioRunClass *delay_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_delay_audio_run_parent_class = g_type_class_peek_parent(delay_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) delay_audio_run;

  gobject->finalize = ags_delay_audio_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) delay_audio_run;

  recall->run_init_pre = ags_delay_audio_run_run_init_pre;
  recall->run_pre = ags_delay_audio_run_run_pre;
  recall->done = ags_delay_audio_run_done;
  recall->cancel = ags_delay_audio_run_cancel;
  recall->remove = ags_delay_audio_run_remove;
  recall->duplicate = ags_delay_audio_run_duplicate;
  recall->notify_dependency = ags_delay_audio_run_notify_dependency;

  /* AgsDelayAudioRun */
  delay_audio_run->notation_alloc_output = NULL;
  delay_audio_run->notation_alloc_input = NULL;
  delay_audio_run->notation_count = NULL;

  delay_audio_run->sequencer_alloc_output = NULL;
  delay_audio_run->sequencer_alloc_input = NULL;
  delay_audio_run->sequencer_count = NULL;

  /* signals */
  delay_audio_run_signals[NOTATION_ALLOC_OUTPUT] =
    g_signal_new("notation_alloc_output\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, notation_alloc_output),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  delay_audio_run_signals[NOTATION_ALLOC_INPUT] =
    g_signal_new("notation_alloc_input\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, notation_alloc_input),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  delay_audio_run_signals[NOTATION_COUNT] =
    g_signal_new("notation_count\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, notation_count),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  delay_audio_run_signals[SEQUENCER_ALLOC_OUTPUT] =
    g_signal_new("sequencer_alloc_output\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, sequencer_alloc_output),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  delay_audio_run_signals[SEQUENCER_ALLOC_INPUT] =
    g_signal_new("sequencer_alloc_input\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, sequencer_alloc_input),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  delay_audio_run_signals[SEQUENCER_COUNT] =
    g_signal_new("sequencer_count\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, sequencer_count),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

}

void
ags_delay_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_delay_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_delay_audio_run_connect;
  connectable->disconnect = ags_delay_audio_run_disconnect;
}

void
ags_delay_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_delay_audio_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_delay_audio_run_run_connect;
  run_connectable->disconnect = ags_delay_audio_run_run_disconnect;
}

void
ags_delay_audio_run_init(AgsDelayAudioRun *delay_audio_run)
{
  delay_audio_run->attack = NULL;

  delay_audio_run->dependency_ref = 0;

  delay_audio_run->hide_ref = 0;
  delay_audio_run->hide_ref_counter = 0;

  delay_audio_run->notation_counter = 0;
  delay_audio_run->sequencer_counter = 0;
}

void
ags_delay_audio_run_connect(AgsConnectable *connectable)
{
  AgsDelayAudioRun *delay_audio_run;

  ags_delay_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_delay_audio_run_disconnect(AgsConnectable *connectable)
{
  ags_delay_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_delay_audio_run_run_connect(AgsRunConnectable *run_connectable)
{
  ags_delay_audio_run_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_delay_audio_run_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_delay_audio_run_parent_run_connectable_interface->disconnect(run_connectable);
}

void
ags_delay_audio_run_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_delay_audio_run_parent_class)->finalize(gobject);
}

void
ags_delay_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsDevout *devout;
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *delay_audio_run;

  /* call parent class */
  AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->run_init_pre(recall);

  /* AgsRecall */
  recall->flags |= AGS_RECALL_PERSISTENT;

  /* AgsDelayAudioRun */
  delay_audio = AGS_DELAY_AUDIO(AGS_RECALL_AUDIO_RUN(recall)->recall_audio);
  delay_audio_run = AGS_DELAY_AUDIO_RUN(recall);

  devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);

  delay_audio_run->attack = ags_attack_duplicate_from_devout((GObject *) devout);
}

void
ags_delay_audio_run_run_pre(AgsRecall *recall)
{
  AgsDelayAudioRun *delay_audio_run;

  AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->run_pre(recall);

  delay_audio_run = AGS_DELAY_AUDIO_RUN(recall);

  if((AGS_RECALL_PERSISTENT & (recall->flags)) == 0 &&
     delay_audio_run->dependency_ref == 0){
    delay_audio_run->notation_counter = 0;
    delay_audio_run->sequencer_counter = 0;
    ags_recall_done(recall);
  }else{
    AgsDelayAudio *delay_audio;

    delay_audio = AGS_DELAY_AUDIO(delay_audio_run->recall_audio_run.recall_audio);

    if(delay_audio_run->hide_ref != 0)
      delay_audio_run->hide_ref_counter += 1;

    if(delay_audio_run->notation_counter == 0){
      AgsDevout *devout;
      guint run_order;
      guint attack;
      guint frames;

      devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);

      if(delay_audio_run->hide_ref != 0){
	run_order = delay_audio_run->hide_ref_counter;
      }else{
	run_order = 0;
      }

      //TODO:JK: optimize attack calculation
      frames = (guint) ceil(delay_audio->notation_delay * (gdouble) devout->buffer_size);
      attack = (delay_audio_run->attack->first_start +
		delay_audio_run->notation_counter * frames) % devout->buffer_size;

      /* notation speed */
      ags_delay_audio_run_notation_alloc_output(delay_audio_run, run_order,
						attack);
      ags_delay_audio_run_notation_alloc_input(delay_audio_run, run_order,
					       attack);
      ags_delay_audio_run_notation_count(delay_audio_run, run_order,
					 attack);
    }

    if(delay_audio_run->sequencer_counter == 0){
      AgsDevout *devout;
      guint run_order;
      guint attack;
      guint frames;

      devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);

      //TODO:JK: optimize attack calculation
      frames = (guint) ceil(delay_audio->sequencer_delay * (gdouble) devout->buffer_size);
      attack = (delay_audio_run->attack->first_start +
		delay_audio_run->sequencer_counter * frames) % devout->buffer_size;

      if(delay_audio_run->hide_ref != 0){
	run_order = delay_audio_run->hide_ref_counter;
      }else{
	run_order = 0;
      }

      g_message("ags_delay_audio_run_run_pre: alloc sequencer[%d]\0", run_order);


      /* sequencer speed */
      //      g_message("ags_delay_audio_run_run_pre[%d]\0", run_order);
      ags_delay_audio_run_sequencer_alloc_output(delay_audio_run, run_order,
						 attack);

      ags_delay_audio_run_sequencer_alloc_input(delay_audio_run, run_order,
						attack);
      ags_delay_audio_run_sequencer_count(delay_audio_run, run_order,
					  attack);
    }

    if(delay_audio_run->hide_ref_counter == delay_audio_run->hide_ref){
      delay_audio_run->hide_ref_counter = 0;

      if(delay_audio_run->notation_counter == (guint) ceil(delay_audio->notation_delay) - 1){
	delay_audio_run->notation_counter = 0;
      }else{
	delay_audio_run->notation_counter += 1;
      }

      if(delay_audio_run->sequencer_counter == (guint) ceil(delay_audio->sequencer_delay) - 1){
	  delay_audio_run->sequencer_counter = 0;
      }else{
	delay_audio_run->sequencer_counter += 1;
      }
    }
  }
}

void
ags_delay_audio_run_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->done(recall);

  /* empty */
}

void
ags_delay_audio_run_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->cancel(recall);

  /* empty */
}

void
ags_delay_audio_run_remove(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->remove(recall);

  /* empty */
}

AgsRecall*
ags_delay_audio_run_duplicate(AgsRecall *recall,
			      AgsRecallID *recall_id,
			      guint *n_params, GParameter *parameter)
{
  AgsDelayAudioRun *delay_audio_run, *copy;
  
  delay_audio_run = (AgsDelayAudioRun *) recall;
  copy = (AgsDelayAudioRun *) AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->duplicate(recall,
											    recall_id,
											    n_params, parameter);

  copy->dependency_ref = delay_audio_run->dependency_ref;

  copy->hide_ref = delay_audio_run->hide_ref;
  copy->hide_ref_counter = delay_audio_run->hide_ref_counter;

  //TODO:JK: may be you want to make a AgsRecallDependency, but a AgsCountable isn't a AgsRecall at all
  copy->notation_counter = delay_audio_run->notation_counter;
  copy->sequencer_counter = delay_audio_run->sequencer_counter;

  return((AgsRecall *) copy);
}

void
ags_delay_audio_run_notify_dependency(AgsRecall *recall, guint notify_mode, gint count)
{
  AgsDelayAudioRun *delay_audio_run;

  delay_audio_run = AGS_DELAY_AUDIO_RUN(recall);

  switch(notify_mode){
  case AGS_RECALL_NOTIFY_RUN:
    delay_audio_run->hide_ref += count;
    g_message("delay_audio_run->hide_ref: %u\n\0", delay_audio_run->hide_ref);
    break;
  case AGS_RECALL_NOTIFY_AUDIO:
    break;
  case AGS_RECALL_NOTIFY_AUDIO_RUN:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL_RUN:
    delay_audio_run->dependency_ref += count;

    break;
  default:
    g_message("ags_delay_audio_run.c - ags_delay_audio_run_notify: unknown notify\0");
  }
}

void
ags_delay_audio_run_notation_alloc_output(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					  guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[NOTATION_ALLOC_OUTPUT], 0,
		nth_run, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_delay_audio_run_notation_alloc_input(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					 guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[NOTATION_ALLOC_INPUT], 0,
		nth_run, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_delay_audio_run_notation_count(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				   guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[NOTATION_COUNT], 0,
		nth_run, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_delay_audio_run_sequencer_alloc_output(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					   guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[SEQUENCER_ALLOC_OUTPUT], 0,
		nth_run, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_delay_audio_run_sequencer_alloc_input(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					  guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[SEQUENCER_ALLOC_INPUT], 0,
		nth_run, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_delay_audio_run_sequencer_count(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				    guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[SEQUENCER_COUNT], 0,
		nth_run, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

AgsDelayAudioRun*
ags_delay_audio_run_new()
{
  AgsDelayAudioRun *delay_audio_run;

  delay_audio_run = (AgsDelayAudioRun *) g_object_new(AGS_TYPE_DELAY_AUDIO_RUN,
						      NULL);

  return(delay_audio_run);
}
