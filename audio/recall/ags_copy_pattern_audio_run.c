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

#include <ags/audio/recall/ags_copy_pattern_audio_run.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

void ags_copy_pattern_audio_run_class_init(AgsCopyPatternAudioRunClass *copy_pattern_audio_run);
void ags_copy_pattern_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_pattern_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_copy_pattern_audio_run_init(AgsCopyPatternAudioRun *copy_pattern_audio_run);
void ags_copy_pattern_audio_run_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_copy_pattern_audio_run_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_copy_pattern_audio_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_pattern_audio_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_pattern_audio_run_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_pattern_audio_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_pattern_audio_run_finalize(GObject *gobject);

AgsRecall* ags_copy_pattern_audio_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id);
void ags_copy_pattern_audio_run_notify_dependency(AgsRecall *recall, guint notify_mode, gint count);

void ags_copy_pattern_audio_run_tic_count_callback(AgsDelayAudioRun *delay_audio_run,
						   AgsCopyPatternAudioRun *copy_pattern_audio_run);

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_BIT,
};

static gpointer ags_copy_pattern_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_copy_pattern_audio_run_parent_connectable_interface;
static AgsRunConnectableInterface *ags_copy_pattern_audio_run_parent_run_connectable_interface;

GType
ags_copy_pattern_audio_run_get_type()
{
  static GType ags_type_copy_pattern_audio_run = 0;

  if(!ags_type_copy_pattern_audio_run){
    static const GTypeInfo ags_copy_pattern_audio_run_info = {
      sizeof (AgsCopyPatternAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_audio_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_pattern_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							     "AgsCopyPatternAudioRun\0",
							     &ags_copy_pattern_audio_run_info,
							     0);
    
    g_type_add_interface_static(ags_type_copy_pattern_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_pattern_audio_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_copy_pattern_audio_run);
}

void
ags_copy_pattern_audio_run_class_init(AgsCopyPatternAudioRunClass *copy_pattern_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_copy_pattern_audio_run_parent_class = g_type_class_peek_parent(copy_pattern_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_pattern_audio_run;

  gobject->set_property = ags_copy_pattern_audio_run_set_property;
  gobject->get_property = ags_copy_pattern_audio_run_get_property;

  gobject->finalize = ags_copy_pattern_audio_run_finalize;

  /* properties */
  param_spec = g_param_spec_object("delay_audio_run\0",
				   "assigned AgsDelayAudioRun\0",
				   "The AgsDelayAudioRun which emits tic_count signal\0",
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  param_spec = g_param_spec_uint("bit\0",
				 "playing bit\0",
				 "The bit that is currently played\0",
				 0,
				 128,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BIT,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_pattern_audio_run;

  recall->duplicate = ags_copy_pattern_audio_run_duplicate;
  recall->notify_dependency = ags_copy_pattern_audio_run_notify_dependency;
}

void
ags_copy_pattern_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_pattern_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_pattern_audio_run_connect;
  connectable->disconnect = ags_copy_pattern_audio_run_disconnect;
}

void
ags_copy_pattern_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_copy_pattern_audio_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_copy_pattern_audio_run_run_connect;
  run_connectable->disconnect = ags_copy_pattern_audio_run_run_disconnect;
}

void
ags_copy_pattern_audio_run_init(AgsCopyPatternAudioRun *copy_pattern_audio_run)
{
  copy_pattern_audio_run->flags = 0;

  copy_pattern_audio_run->recall_ref = 0;

  copy_pattern_audio_run->hide_ref = 0;
  copy_pattern_audio_run->hide_ref_counter = 0;

  copy_pattern_audio_run->delay_audio_run = NULL;
  copy_pattern_audio_run->bit = 0;
}

void
ags_copy_pattern_audio_run_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run;

      delay_audio_run = (AgsDelayAudioRun *) g_value_get_object(value);

      if(copy_pattern_audio_run->delay_audio_run == delay_audio_run)
	return;

      if(copy_pattern_audio_run->delay_audio_run != NULL){
	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(copy_pattern_audio_run)->flags)) != 0)
	  g_signal_handler_disconnect(G_OBJECT(copy_pattern_audio_run),
				      copy_pattern_audio_run->tic_count_handler);

	g_object_unref(G_OBJECT(copy_pattern_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(G_OBJECT(delay_audio_run));

	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(copy_pattern_audio_run)->flags)) != 0)
	  copy_pattern_audio_run->tic_count_handler =
	    g_signal_connect(G_OBJECT(delay_audio_run), "tic_count\0",
			     G_CALLBACK(ags_copy_pattern_audio_run_tic_count_callback), copy_pattern_audio_run);
      }

      copy_pattern_audio_run->delay_audio_run = delay_audio_run;
    }
    break;
  case PROP_BIT:
    {
      guint bit;

      bit = g_value_get_uint(value);

      copy_pattern_audio_run->bit = bit;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_audio_run_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      g_value_set_object(value, copy_pattern_audio_run->delay_audio_run);
    }
    break;
  case PROP_BIT:
    {
      g_value_set_uint(value, copy_pattern_audio_run->bit);
    }
    break;
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_audio_run_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_copy_pattern_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_copy_pattern_audio_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_copy_pattern_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_copy_pattern_audio_run_run_connect(AgsRunConnectable *run_connectable)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  ags_copy_pattern_audio_run_parent_run_connectable_interface->connect(run_connectable);

  /* AgsCopyPattern */
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(run_connectable);

  copy_pattern_audio_run->flags |= AGS_COPY_PATTERN_AUDIO_RUN_RUN_CONNECTED;

  copy_pattern_audio_run->tic_count_handler =
    g_signal_connect(G_OBJECT(copy_pattern_audio_run->delay_audio_run), "tic_count\0",
		     G_CALLBACK(ags_copy_pattern_audio_run_tic_count_callback), copy_pattern_audio_run);
}

void
ags_copy_pattern_audio_run_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  ags_copy_pattern_audio_run_parent_run_connectable_interface->connect(run_connectable);

  /* AgsCopyPattern */
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(run_connectable);

  copy_pattern_audio_run->flags &= (~AGS_COPY_PATTERN_AUDIO_RUN_RUN_CONNECTED);

  g_signal_handler_disconnect(G_OBJECT(copy_pattern_audio_run), copy_pattern_audio_run->tic_count_handler);
}

void
ags_copy_pattern_audio_run_finalize(GObject *gobject)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  if(copy_pattern_audio_run->delay_audio_run != NULL)
    g_object_unref(copy_pattern_audio_run->delay_audio_run);

  /* call parent */
  G_OBJECT_CLASS(ags_copy_pattern_audio_run_parent_class)->finalize(gobject);
}

AgsRecall*
ags_copy_pattern_audio_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsAudio *audio;
  AgsRecallContainer *container;
  AgsRecallAudio *recall_audio;
  AgsCopyPatternAudio *copy_pattern_audio;
  AgsCopyPatternAudioRun *copy_pattern_audio_run, *copy;
  GList *list, *list_start;
  guint group_id;
  GValue container_value = {0,};
  GValue recall_audio_value = {0,};
  GValue audio_value = {0,};
  GValue delay_audio_run_value = {0,};

  copy = AGS_COPY_PATTERN_AUDIO_RUN(AGS_RECALL_CLASS(ags_copy_pattern_audio_run_parent_class)->duplicate(recall, recall_id));

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(recall);

  g_value_init(&container_value, G_TYPE_OBJECT);
  g_object_get_property(G_OBJECT(recall),
			"container\0",
			&container_value);
  container = AGS_RECALL_CONTAINER(g_value_get_object(&container_value));
  g_value_unset(&container_value);

  g_value_init(&recall_audio_value, G_TYPE_OBJECT);
  g_object_get_property(G_OBJECT(container),
			"recall_audio\0",
			&recall_audio_value);
  recall_audio = AGS_RECALL_AUDIO(g_value_get_object(&recall_audio_value));
  g_value_unset(&recall_audio_value);

  g_value_init(&audio_value, G_TYPE_OBJECT);
  g_object_get_property(G_OBJECT(recall_audio),
			"audio\0",
			&audio_value);
  audio = AGS_AUDIO(g_value_get_object(&audio_value));
  g_value_unset(&audio_value);

  if(audio != NULL){
    if((AGS_RECALL_ID_HIGHER_LEVEL_IS_RECALL & (recall_id->flags)) == 0)
      list_start = audio->play;
    else
      list_start = audio->recall;

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0)
      group_id = recall_id->group_id;
    else
      group_id = recall_id->parent_group_id;

    list = ags_recall_find_type_with_group_id(list_start,
					      AGS_TYPE_DELAY_AUDIO_RUN,
					      group_id);

    if(list != NULL){
      g_value_init(&delay_audio_run_value, G_TYPE_OBJECT);
      g_value_set_object(&delay_audio_run_value, list->data);
      g_object_set_property(G_OBJECT(copy),
			    "delay_audio_run_run\0",
			    &delay_audio_run_value);
      g_value_unset(&delay_audio_run_value);
    }
  }

  return((AgsRecall *) copy);
}

void
ags_copy_pattern_audio_run_notify_dependency(AgsRecall *recall, guint notify_mode, gint count)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(recall);

  switch(notify_mode){
  case AGS_RECALL_NOTIFY_RUN:
    copy_pattern_audio_run->hide_ref += count;

    break;
  case AGS_RECALL_NOTIFY_SHARED_AUDIO:
    break;
  case AGS_RECALL_NOTIFY_SHARED_AUDIO_RUN:
    break;
  case AGS_RECALL_NOTIFY_SHARED_CHANNEL:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL_RUN:
    copy_pattern_audio_run->recall_ref += count;

    break;
  default:
    printf("ags_copy_pattern_audio_run.c - ags_copy_pattern_audio_run_notify: unknown notify");
  }
}

void
ags_copy_pattern_audio_run_tic_count_callback(AgsDelayAudioRun *delay_audio_run,
					      AgsCopyPatternAudioRun *copy_pattern_audio_run)
{
  AgsCopyPatternAudio *copy_pattern_audio;
  GValue value = {0,};

  g_value_init(&value, G_TYPE_OBJECT);
  g_object_get_property(G_OBJECT(copy_pattern_audio_run),
			"recall_audio\0",
			&value);
  copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(g_value_get_object(&value));
  g_value_unset(&value);

  if(copy_pattern_audio_run->hide_ref != 0)
    copy_pattern_audio_run->hide_ref_counter++;

  if(copy_pattern_audio_run->hide_ref_counter == copy_pattern_audio_run->hide_ref){
    copy_pattern_audio_run->hide_ref_counter = 0;

    if(copy_pattern_audio_run->bit == copy_pattern_audio->length - 1){
      if(copy_pattern_audio->loop ||
	 copy_pattern_audio_run->recall_ref == 0)
	copy_pattern_audio_run->bit = 0;
    }else
      copy_pattern_audio_run->bit++;
  }
}

AgsCopyPatternAudioRun*
ags_copy_pattern_audio_run_new(AgsDelayAudioRun *delay_audio_run,
			       guint bit)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = (AgsCopyPatternAudioRun *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
								   "delay_audio_run\0", delay_audio_run,
								   "bit\0", bit,
								   NULL);

  return(copy_pattern_audio_run);
}
