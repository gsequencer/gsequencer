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
void ags_copy_pattern_audio_run_connect(AgsConnectable *connectable);
void ags_copy_pattern_audio_run_disconnect(AgsConnectable *connectable);
void ags_copy_pattern_audio_run_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_pattern_audio_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_pattern_audio_run_finalize(GObject *gobject);

void ags_copy_pattern_audio_run_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_copy_pattern_audio_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

enum{
  PROP_0,
  PROP_COUNT_BEATS_AUDIO_RUN,
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
  param_spec = g_param_spec_object("count_beats_audio_run\0",
				   "assigned AgsCountBeatsAudioRun\0",
				   "The AgsCountBeatsAudioRun which emits beat signal\0",
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_pattern_audio_run;

  recall->resolve_dependencies = ags_copy_pattern_audio_run_resolve_dependencies;
  recall->duplicate = ags_copy_pattern_audio_run_duplicate;
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
  copy_pattern_audio_run->count_beats_audio_run = NULL;
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
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run;

      count_beats_audio_run = (AgsCountBeatsAudioRun *) g_value_get_object(value);

      if(copy_pattern_audio_run->count_beats_audio_run == count_beats_audio_run)
	return;

      if(copy_pattern_audio_run->count_beats_audio_run != NULL){
	g_object_unref(G_OBJECT(copy_pattern_audio_run->count_beats_audio_run));
      }

      copy_pattern_audio_run->count_beats_audio_run = count_beats_audio_run;
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
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_value_set_object(value, copy_pattern_audio_run->count_beats_audio_run);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_audio_run_finalize(GObject *gobject)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  if(copy_pattern_audio_run->count_beats_audio_run != NULL)
    g_object_unref(copy_pattern_audio_run->count_beats_audio_run);

  /* call parent */
  G_OBJECT_CLASS(ags_copy_pattern_audio_run_parent_class)->finalize(gobject);
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
  /* call parent */
  ags_copy_pattern_audio_run_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_copy_pattern_audio_run_run_disconnect(AgsRunConnectable *run_connectable)
{
  /* call parent */
  ags_copy_pattern_audio_run_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_copy_pattern_audio_run_resolve_dependencies(AgsRecall *recall)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  AgsRecallDependency *recall_dependency;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  GList *list;
  guint group_id;
  guint i, i_stop;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(recall);

  list = recall->dependencies;
  group_id = recall->recall_id->group_id;

  count_beats_audio_run = NULL;
  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_COUNT_BEATS_AUDIO_RUN(recall_dependency->recall_template)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_find(recall_dependency, group_id);

      i++;
    }

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "count_beats_audio_run\0", count_beats_audio_run,
	       NULL);
}

AgsRecall*
ags_copy_pattern_audio_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsAudio *audio;
  AgsRecallAudio *recall_audio;
  AgsCopyPatternAudio *copy_pattern_audio;
  AgsCopyPatternAudioRun *copy_pattern_audio_run, *copy;
  GList *list, *list_start;
  guint group_id;
  GValue delay_audio_run_value = {0,};

  printf("ags_copy_pattern_audio_run_duplicate\n\0");

  copy = AGS_COPY_PATTERN_AUDIO_RUN(AGS_RECALL_CLASS(ags_copy_pattern_audio_run_parent_class)->duplicate(recall, recall_id));

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(recall);

  return((AgsRecall *) copy);
}

AgsCopyPatternAudioRun*
ags_copy_pattern_audio_run_new(AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = (AgsCopyPatternAudioRun *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
								   "count_beats_audio_run\0", count_beats_audio_run,
								   NULL);

  return(copy_pattern_audio_run);
}
