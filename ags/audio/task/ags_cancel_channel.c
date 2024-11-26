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

#include <ags/audio/task/ags_cancel_channel.h>

#include <ags/audio/ags_channel.h>

#include <ags/i18n.h>

void ags_cancel_channel_class_init(AgsCancelChannelClass *cancel_channel);
void ags_cancel_channel_init(AgsCancelChannel *cancel_channel);
void ags_cancel_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_cancel_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_cancel_channel_dispose(GObject *gobject);
void ags_cancel_channel_finalize(GObject *gobject);

void ags_cancel_channel_launch(AgsTask *task);

/**
 * SECTION:ags_cancel_channel
 * @short_description: cancel channel task
 * @title: AgsCancelChannel
 * @section_id:
 * @include: ags/audio/task/ags_cancel_channel.h
 *
 * The #AgsCancelChannel task cancels #AgsChannel playback.
 */

static gpointer ags_cancel_channel_parent_class = NULL;

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_SOUND_SCOPE,
};

GType
ags_cancel_channel_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_cancel_channel = 0;

    static const GTypeInfo ags_cancel_channel_info = {
      sizeof(AgsCancelChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cancel_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCancelChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cancel_channel_init,
    };

    ags_type_cancel_channel = g_type_register_static(AGS_TYPE_TASK,
						     "AgsCancelChannel",
						     &ags_cancel_channel_info,
						     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_cancel_channel);
  }

  return(g_define_type_id__static);
}

void
ags_cancel_channel_class_init(AgsCancelChannelClass *cancel_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_cancel_channel_parent_class = g_type_class_peek_parent(cancel_channel);

  /* gobject */
  gobject = (GObjectClass *) cancel_channel;

  gobject->set_property = ags_cancel_channel_set_property;
  gobject->get_property = ags_cancel_channel_get_property;

  gobject->dispose = ags_cancel_channel_dispose;
  gobject->finalize = ags_cancel_channel_finalize;

  /* properties */
  /**
   * AgsCancelChannel:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel of cancel channel"),
				   i18n_pspec("The channel of cancel channel task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsCancelChannel:sound-scope:
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
  
  /* task */
  task = (AgsTaskClass *) cancel_channel;

  task->launch = ags_cancel_channel_launch;
}

void
ags_cancel_channel_init(AgsCancelChannel *cancel_channel)
{
  cancel_channel->channel = NULL;

  cancel_channel->sound_scope = -1;
}

void
ags_cancel_channel_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = AGS_CANCEL_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(cancel_channel->channel == channel){
	return;
      }

      if(cancel_channel->channel != NULL){
	g_object_unref(cancel_channel->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      cancel_channel->channel = channel;
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      cancel_channel->sound_scope = g_value_get_int(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_cancel_channel_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = AGS_CANCEL_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, cancel_channel->channel);
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      g_value_set_int(value, cancel_channel->sound_scope);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_cancel_channel_dispose(GObject *gobject)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = AGS_CANCEL_CHANNEL(gobject);

  if(cancel_channel->channel != NULL){
    g_object_unref(cancel_channel->channel);

    cancel_channel->channel = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_cancel_channel_parent_class)->dispose(gobject);
}

void
ags_cancel_channel_finalize(GObject *gobject)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = AGS_CANCEL_CHANNEL(gobject);

  if(cancel_channel->channel != NULL){
    g_object_unref(cancel_channel->channel);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_cancel_channel_parent_class)->finalize(gobject);
}

void
ags_cancel_channel_launch(AgsTask *task)
{
  AgsChannel *channel;
  
  AgsCancelChannel *cancel_channel;

  GList *start_recall_id;

  gint sound_scope;

  cancel_channel = AGS_CANCEL_CHANNEL(task);

  g_return_if_fail(AGS_IS_CHANNEL(cancel_channel->channel));

  g_object_get(cancel_channel,
	       "channel", &channel,
	       "sound-scope", &sound_scope,
	       NULL);

  start_recall_id = ags_channel_check_scope(channel, sound_scope);
  
  ags_channel_stop(channel,
		   start_recall_id, sound_scope);
  
  g_object_unref(channel);

  g_list_free_full(start_recall_id,
		   g_object_unref);
}

/**
 * ags_cancel_channel_new:
 * @channel: the #AgsChannel to cancel
 * @sound_scope: the #AgsSoundScope-enum or -1 for all
 *
 * Create a new instance of  #AgsCancelChannel.
 *
 * Returns: the new #AgsCancelChannel.
 *
 * Since: 3.0.0
 */
AgsCancelChannel*
ags_cancel_channel_new(AgsChannel *channel,
		       gint sound_scope)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = (AgsCancelChannel *) g_object_new(AGS_TYPE_CANCEL_CHANNEL,
						     "channel", channel,
						     "sound-scope", sound_scope,
						     NULL);

  return(cancel_channel);
}
