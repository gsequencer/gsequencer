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

#include <ags/audio/task/ags_set_muted.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

#include <math.h>

#include <ags/i18n.h>

void ags_set_muted_class_init(AgsSetMutedClass *set_muted);
void ags_set_muted_init(AgsSetMuted *set_muted);
void ags_set_muted_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_set_muted_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_set_muted_dispose(GObject *gobject);
void ags_set_muted_finalize(GObject *gobject);

void ags_set_muted_launch(AgsTask *task);

void ags_set_muted_recall(AgsSetMuted *set_muted, AgsRecall *recall);
void ags_set_muted_channel(AgsSetMuted *set_muted, AgsChannel *channel);
void ags_set_muted_audio(AgsSetMuted *set_muted, AgsAudio *audio);

/**
 * SECTION:ags_set_muted
 * @short_description: set muted
 * @title: AgsSetMuted
 * @section_id:
 * @include: ags/audio/task/recall/ags_set_muted.h
 *
 * The #AgsSetMuted task sets muted to #AgsMutable.
 */

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_MUTED,
};

static gpointer ags_set_muted_parent_class = NULL;
static AgsConnectableInterface *ags_set_muted_parent_connectable_interface;

GType
ags_set_muted_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_set_muted = 0;

    static const GTypeInfo ags_set_muted_info = {
      sizeof(AgsSetMutedClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_muted_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSetMuted),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_muted_init,
    };

    ags_type_set_muted = g_type_register_static(AGS_TYPE_TASK,
						"AgsSetMuted",
						&ags_set_muted_info,
						0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_set_muted);
  }

  return g_define_type_id__volatile;
}

void
ags_set_muted_class_init(AgsSetMutedClass *set_muted)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_set_muted_parent_class = g_type_class_peek_parent(set_muted);

  /* GObjectClass */
  gobject = (GObjectClass *) set_muted;

  gobject->set_property = ags_set_muted_set_property;
  gobject->get_property = ags_set_muted_get_property;

  gobject->dispose = ags_set_muted_dispose;
  gobject->finalize = ags_set_muted_finalize;

  /* properties */
  /**
   * AgsSetMuted:scope:
   *
   * The assigned #GObject as scope.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("scope",
				   i18n_pspec("scope of set buffer size"),
				   i18n_pspec("The scope of set buffer size"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCOPE,
				  param_spec);

  /**
   * AgsSetMuted:muted:
   *
   * The muted to set to scope.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_boolean("muted",
				    i18n_pspec("muted"),
				    i18n_pspec("The muted to set"),
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MUTED,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) set_muted;
  
  task->launch = ags_set_muted_launch;
}

void
ags_set_muted_init(AgsSetMuted *set_muted)
{
  set_muted->scope = NULL;
  set_muted->muted = FALSE;
}

void
ags_set_muted_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsSetMuted *set_muted;

  set_muted = AGS_SET_MUTED(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(set_muted->scope == (GObject *) scope){
	return;
      }

      if(set_muted->scope != NULL){
	g_object_unref(set_muted->scope);
      }

      if(scope != NULL){
	g_object_ref(scope);
      }

      set_muted->scope = (GObject *) scope;
    }
    break;
  case PROP_MUTED:
    {
      set_muted->muted = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_muted_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsSetMuted *set_muted;

  set_muted = AGS_SET_MUTED(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_object(value, set_muted->scope);
    }
    break;
  case PROP_MUTED:
    {
      g_value_set_boolean(value, set_muted->muted);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_muted_dispose(GObject *gobject)
{
  AgsSetMuted *set_muted;

  set_muted = AGS_SET_MUTED(gobject);

  if(set_muted->scope != NULL){
    g_object_unref(set_muted->scope);

    set_muted->scope = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_set_muted_parent_class)->dispose(gobject);
}

void
ags_set_muted_finalize(GObject *gobject)
{
  AgsSetMuted *set_muted;

  set_muted = AGS_SET_MUTED(gobject);

  if(set_muted->scope != NULL){
    g_object_unref(set_muted->scope);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_set_muted_parent_class)->finalize(gobject);
}

void
ags_set_muted_launch(AgsTask *task)
{
  AgsSetMuted *set_muted;

  set_muted = AGS_SET_MUTED(task);

  if(AGS_IS_AUDIO(set_muted->scope)){
    AgsAudio *audio;

    audio = AGS_AUDIO(set_muted->scope);

    ags_set_muted_audio(set_muted, audio);
  }else if(AGS_IS_CHANNEL(set_muted->scope)){
    AgsChannel *channel;

    channel = AGS_CHANNEL(set_muted->scope);

    ags_set_muted_channel(set_muted, channel);
  }else if(AGS_IS_RECALL(set_muted->scope)){
    AgsRecall *recall;

    recall = AGS_RECALL(set_muted->scope);

    ags_set_muted_recall(set_muted, recall);
  }else{
    g_warning("AgsSetMuted: Not supported scope");
  }
}

void
ags_set_muted_recall(AgsSetMuted *set_muted, AgsRecall *recall)
{
  if(AGS_IS_MUTABLE(recall)){
    ags_mutable_set_muted(AGS_MUTABLE(recall), set_muted->muted);
  }
}

void
ags_set_muted_channel(AgsSetMuted *set_muted, AgsChannel *channel)
{
  GList *list_start, *list;

  /* set muted - play */
  g_object_get(channel,
	       "play", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_set_muted_recall(set_muted, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* set muted - recall */
  g_object_get(channel,
	       "recall", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_set_muted_recall(set_muted, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

void
ags_set_muted_audio(AgsSetMuted *set_muted, AgsAudio *audio)
{
  AgsChannel *input, *output;
  AgsChannel *channel, *next_channel;

  GList *list_start, *list;

  /* get some fields */
  g_object_get(audio,
	       "output", &output,
	       "input", &input,
	       NULL);

  /* set muted - play */
  g_object_get(audio,
	       "play", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_set_muted_recall(set_muted, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* set muted - recall */
  g_object_get(audio,
	       "recall", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_set_muted_recall(set_muted, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* AgsChannel - output */
  if(output != NULL){
    channel = output;
    g_object_ref(channel);
    
    while(channel != NULL){
      /* set muted */
      ags_set_muted_channel(set_muted, channel);

      /* iterate */
      next_channel = ags_channel_next(channel);
    
      g_object_unref(channel);

      channel = next_channel;
    }
  }
  
  /* AgsChannel - input */
  if(input != NULL){
    channel = input;
    g_object_ref(channel);

    while(channel != NULL){
      /* set muted */
      ags_set_muted_channel(set_muted, channel);

      /* iterate */
      next_channel = ags_channel_next(channel);
    
      g_object_unref(channel);

      channel = next_channel;
    }
  }
}

/**
 * ags_set_muted_new:
 * @scope: the #GObject
 * @muted: muted to set
 *
 * Creates an #AgsSetMuted.
 *
 * Returns: an new #AgsSetMuted.
 *
 * Since: 2.0.0
 */
AgsSetMuted*
ags_set_muted_new(GObject *scope,
		  gboolean muted)
{
  AgsSetMuted *set_muted;
  
  set_muted = (AgsSetMuted *) g_object_new(AGS_TYPE_SET_MUTED,
					   "scope", scope,
					   "muted", muted,
					   NULL);
  
  return(set_muted);
}
