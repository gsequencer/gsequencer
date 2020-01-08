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

#include <ags/audio/task/ags_start_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <ags/i18n.h>

void ags_start_channel_class_init(AgsStartChannelClass *start_channel);
void ags_start_channel_init(AgsStartChannel *start_channel);
void ags_start_channel_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_start_channel_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_start_channel_dispose(GObject *gobject);
void ags_start_channel_finalize(GObject *gobject);

void ags_start_channel_launch(AgsTask *task);

/**
 * SECTION:ags_start_channel
 * @short_description: start channel object to audio loop
 * @title: AgsStartChannel
 * @section_id:
 * @include: ags/audio/task/ags_start_channel.h
 *
 * The #AgsStartChannel task starts #AgsChannel to #AgsAudioLoop.
 */

static gpointer ags_start_channel_parent_class = NULL;

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_SOUND_SCOPE,
};

GType
ags_start_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_start_channel = 0;

    static const GTypeInfo ags_start_channel_info = {
      sizeof (AgsStartChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_start_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStartChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_start_channel_init,
    };

    ags_type_start_channel = g_type_register_static(AGS_TYPE_TASK,
						    "AgsStartChannel",
						    &ags_start_channel_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_start_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_start_channel_class_init(AgsStartChannelClass *start_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_start_channel_parent_class = g_type_class_peek_parent(start_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) start_channel;

  gobject->set_property = ags_start_channel_set_property;
  gobject->get_property = ags_start_channel_get_property;

  gobject->dispose = ags_start_channel_dispose;
  gobject->finalize = ags_start_channel_finalize;

  /* properties */
  /**
   * AgsStartChannel:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel of start channel"),
				   i18n_pspec("The channel of start channel task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsStartChannel:sound-scope:
   *
   * The effects sound-scope.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_int("sound-scope",
				 i18n_pspec("sound scope"),
				 i18n_pspec("The sound scope"),
				 -1,
				 AGS_SOUND_SCOPE_LAST,
				 -1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUND_SCOPE,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) start_channel;

  task->launch = ags_start_channel_launch;
}

void
ags_start_channel_init(AgsStartChannel *start_channel)
{
  start_channel->channel = NULL;

  start_channel->sound_scope = -1;
}

void
ags_start_channel_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsStartChannel *start_channel;

  start_channel = AGS_START_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(start_channel->channel == channel){
	return;
      }

      if(start_channel->channel != NULL){
	g_object_unref(start_channel->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      start_channel->channel = channel;
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      start_channel->sound_scope = g_value_get_int(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_channel_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsStartChannel *start_channel;

  start_channel = AGS_START_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, start_channel->channel);
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      g_value_set_int(value, start_channel->sound_scope);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_channel_dispose(GObject *gobject)
{
  AgsStartChannel *start_channel;

  start_channel = AGS_START_CHANNEL(gobject);

  if(start_channel->channel != NULL){
    g_object_unref(start_channel->channel);

    start_channel->channel = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_start_channel_parent_class)->dispose(gobject);
}

void
ags_start_channel_finalize(GObject *gobject)
{
  AgsStartChannel *start_channel;

  start_channel = AGS_START_CHANNEL(gobject);

  if(start_channel->channel != NULL){
    g_object_unref(start_channel->channel);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_start_channel_parent_class)->finalize(gobject);
}

void
ags_start_channel_launch(AgsTask *task)
{
  AgsChannel *channel;
  
  AgsStartChannel *start_channel;

  GList *recall_id;
  
  gint sound_scope;

  start_channel = AGS_START_CHANNEL(task);

  g_object_get(start_channel,
	       "channel", &channel,
	       "sound-scope", &sound_scope,
	       NULL);

  recall_id = ags_channel_start(channel,
				sound_scope);

  g_object_unref(channel);
  
  g_list_free_full(recall_id,
		   g_object_unref);
}

/**
 * ags_start_channel_new:
 * @channel: the #AgsChannel to start
 *
 * Create a new instance of #AgsStartChannel.
 *
 * Returns: the new #AgsStartChannel.
 *
 * Since: 3.0.0
 */
AgsStartChannel*
ags_start_channel_new(AgsChannel *channel,
		      gint sound_scope)
{
  AgsStartChannel *start_channel;

  start_channel = (AgsStartChannel *) g_object_new(AGS_TYPE_START_CHANNEL,
						   "channel", channel,
						   "sound-scope", sound_scope,
						   NULL);

  return(start_channel);
}
