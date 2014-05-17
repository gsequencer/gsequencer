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
#include <ags/audio/recall/ags_delay_audio.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_id.h>

#include <stdlib.h>
#include <math.h>

void ags_delay_audio_run_class_init(AgsDelayAudioRunClass *delay_audio_run);
void ags_delay_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_delay_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_delay_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_delay_audio_run_init(AgsDelayAudioRun *delay_audio_run);
void ags_delay_audio_run_connect(AgsConnectable *connectable);
void ags_delay_audio_run_disconnect(AgsConnectable *connectable);
void ags_delay_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_delay_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
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
static AgsDynamicConnectableInterface *ags_delay_audio_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_delay_audio_run_parent_plugin_interface;

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

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_delay_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_delay_audio_run_plugin_interface_init,
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
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_delay_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
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
    g_signal_new("notation-alloc-output\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, notation_alloc_output),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_UINT, G_TYPE_UINT);

  delay_audio_run_signals[NOTATION_ALLOC_INPUT] =
    g_signal_new("notation-alloc-input\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, notation_alloc_input),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_UINT, G_TYPE_UINT);

  delay_audio_run_signals[NOTATION_COUNT] =
    g_signal_new("notation-count\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, notation_count),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_UINT, G_TYPE_UINT);

  delay_audio_run_signals[SEQUENCER_ALLOC_OUTPUT] =
    g_signal_new("sequencer-alloc-output\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, sequencer_alloc_output),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_UINT, G_TYPE_UINT);

  delay_audio_run_signals[SEQUENCER_ALLOC_INPUT] =
    g_signal_new("sequencer-alloc-input\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, sequencer_alloc_input),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_UINT, G_TYPE_UINT);

  delay_audio_run_signals[SEQUENCER_COUNT] =
    g_signal_new("sequencer-count\0",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, sequencer_count),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
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
ags_delay_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_delay_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_delay_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_delay_audio_run_disconnect_dynamic;
}

void
ags_delay_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_delay_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
}

void
ags_delay_audio_run_init(AgsDelayAudioRun *delay_audio_run)
{
  AGS_RECALL(delay_audio_run)->name = "ags-delay\0";
  AGS_RECALL(delay_audio_run)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(delay_audio_run)->build_id = AGS_BUILD_ID;
  AGS_RECALL(delay_audio_run)->xml_type = "ags-delay-audio-run\0";
  AGS_RECALL(delay_audio_run)->port = NULL;

  delay_audio_run->dependency_ref = 0;

  delay_audio_run->hide_ref = 0;
  delay_audio_run->hide_ref_counter = 0;

  delay_audio_run->notation_counter = 0;
  delay_audio_run->sequencer_counter = 0;
}

void
ags_delay_audio_run_connect(AgsConnectable *connectable)
{
  ags_delay_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_delay_audio_run_disconnect(AgsConnectable *connectable)
{
  ags_delay_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_delay_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_delay_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_delay_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_delay_audio_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
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

  /* AgsDelayAudioRun */
  delay_audio = AGS_DELAY_AUDIO(AGS_RECALL_AUDIO_RUN(recall)->recall_audio);
  delay_audio_run = AGS_DELAY_AUDIO_RUN(recall);

  devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);

  /* run order */
  delay_audio_run->hide_ref_counter = 0;
}

void
ags_delay_audio_run_run_pre(AgsRecall *recall)
{
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *delay_audio_run;
  guint notation_delay, sequencer_delay;
  GValue value = { 0, };

  AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->run_pre(recall);

  delay_audio_run = AGS_DELAY_AUDIO_RUN(recall);

  if((AGS_RECALL_PERSISTENT & (recall->flags)) == 0 &&
     delay_audio_run->dependency_ref == 0){
    delay_audio_run->notation_counter = 0;
    delay_audio_run->sequencer_counter = 0;

    ags_recall_done(recall);

    return;
  }

  delay_audio = AGS_DELAY_AUDIO(AGS_RECALL_AUDIO_RUN(delay_audio_run)->recall_audio);

  /* read notation-delay port */
  g_value_init(&value, G_TYPE_DOUBLE);

  ags_port_safe_read(delay_audio->notation_delay, &value);

  notation_delay = (guint) ceil(g_value_get_double(&value));

  /* read sequencer-delay port */
  g_value_reset(&value);

  ags_port_safe_read(delay_audio->sequencer_delay, &value);

  sequencer_delay = (guint) ceil(g_value_get_double(&value));

  if(delay_audio_run->notation_counter == 0){
    AgsDevout *devout;
    guint run_order;
    guint delay, attack;

    devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);

    run_order = delay_audio_run->hide_ref_counter;

    /* delay and attack */
    attack = devout->attack[((devout->tic_counter + 1 == AGS_NOTATION_TICS_PER_BEAT) ?
			     0:
			     devout->tic_counter + 1)];
      
    delay = devout->delay[((devout->tic_counter + 1 == AGS_NOTATION_TICS_PER_BEAT) ?
			   0:
			   devout->tic_counter + 1)];

    g_message("ags_delay_audio_run_run_pre@%llu: alloc notation[%u]\0",
	      delay_audio_run,
	      run_order);
      
    /* notation speed */
    ags_delay_audio_run_notation_alloc_output(delay_audio_run,
					      run_order,
					      delay, attack);
    ags_delay_audio_run_notation_alloc_input(delay_audio_run,
					     run_order,
					     delay, attack);
    ags_delay_audio_run_notation_count(delay_audio_run,
				       run_order,
				       delay, attack);
  }

  if(delay_audio_run->sequencer_counter == 0){
    AgsDevout *devout;
    guint run_order;
    guint delay, attack;

    devout = AGS_DEVOUT(AGS_RECALL_AUDIO(delay_audio)->audio->devout);

    /* delay and attack */
    attack = devout->attack[((devout->tic_counter + 1 == AGS_NOTATION_TICS_PER_BEAT) ?
			     0:
			     devout->tic_counter + 1)];
    delay = devout->delay[((devout->tic_counter + 1 == AGS_NOTATION_TICS_PER_BEAT) ?
			   0:
			   devout->tic_counter + 1)];

    run_order = delay_audio_run->hide_ref_counter;

    g_message("ags_delay_audio_run_run_pre@%llu: alloc sequencer[%u]\0",
	      delay_audio_run,
	      run_order);

    /* sequencer speed */
    ags_delay_audio_run_sequencer_alloc_output(delay_audio_run,
					       run_order,
					       delay, attack);

    ags_delay_audio_run_sequencer_alloc_input(delay_audio_run,
					      run_order,
					      delay, attack);
    ags_delay_audio_run_sequencer_count(delay_audio_run,
					run_order,
					delay, attack);
  }

  if(delay_audio_run->hide_ref != 0){
    delay_audio_run->hide_ref_counter += 1;

    if(delay_audio_run->hide_ref_counter == delay_audio_run->hide_ref){
      delay_audio_run->hide_ref_counter = 0;

      if(delay_audio_run->notation_counter == notation_delay){
	delay_audio_run->notation_counter = 0;
      }else{
	delay_audio_run->notation_counter += 1;
      }

      if(delay_audio_run->sequencer_counter >= sequencer_delay){
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
ags_delay_audio_run_notation_alloc_output(AgsDelayAudioRun *delay_audio_run, guint run_order,
					  guint delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[NOTATION_ALLOC_OUTPUT], 0,
		run_order, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_delay_audio_run_notation_alloc_input(AgsDelayAudioRun *delay_audio_run, guint run_order,
					 guint delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[NOTATION_ALLOC_INPUT], 0,
		run_order, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_delay_audio_run_notation_count(AgsDelayAudioRun *delay_audio_run, guint run_order,
				   guint delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[NOTATION_COUNT], 0,
		run_order, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_delay_audio_run_sequencer_alloc_output(AgsDelayAudioRun *delay_audio_run, guint run_order,
					   guint delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[SEQUENCER_ALLOC_OUTPUT], 0,
		run_order, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_delay_audio_run_sequencer_alloc_input(AgsDelayAudioRun *delay_audio_run, guint run_order,
					  guint delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[SEQUENCER_ALLOC_INPUT], 0,
		run_order, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

void
ags_delay_audio_run_sequencer_count(AgsDelayAudioRun *delay_audio_run, guint run_order,
				    guint delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[SEQUENCER_COUNT], 0,
		run_order, attack);
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
