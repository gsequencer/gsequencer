/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/task/ags_add_effect.h>

#include <ags/object/ags_connectable.h>

void ags_add_effect_class_init(AgsAddEffectClass *add_effect);
void ags_add_effect_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_effect_init(AgsAddEffect *add_effect);
void ags_add_effect_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_add_effect_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_add_effect_connect(AgsConnectable *connectable);
void ags_add_effect_disconnect(AgsConnectable *connectable);
void ags_add_effect_finalize(GObject *gobject);

void ags_add_effect_launch(AgsTask *task);

/**
 * SECTION:ags_add_effect
 * @short_description: add effect object to context
 * @title: AgsAddEffect
 * @section_id:
 * @include: ags/audio/task/ags_add_effect.h
 *
 * The #AgsAddEffect task adds the specified effect to #AgsChannel.
 */

static gpointer ags_add_effect_parent_class = NULL;
static AgsConnectableInterface *ags_add_effect_parent_connectable_interface;

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_FILENAME,
  PROP_EFFECT,
};

GType
ags_add_effect_get_type()
{
  static GType ags_type_add_effect = 0;

  if(!ags_type_add_effect){
    static const GTypeInfo ags_add_effect_info = {
      sizeof (AgsAddEffectClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_effect_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddEffect),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_effect_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_effect_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_effect = g_type_register_static(AGS_TYPE_TASK,
						 "AgsAddEffect\0",
						 &ags_add_effect_info,
						 0);

    g_type_add_interface_static(ags_type_add_effect,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_effect);
}

void
ags_add_effect_class_init(AgsAddEffectClass *add_effect)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_add_effect_parent_class = g_type_class_peek_parent(add_effect);

  /* gobject */
  gobject = (GObjectClass *) add_effect;

  gobject->set_property = ags_add_effect_set_property;
  gobject->get_property = ags_add_effect_get_property;

  gobject->finalize = ags_add_effect_finalize;

  /* properties */
  /**
   * AgsAddEffect:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("channel\0",
				   "channel of add effect\0",
				   "The channel of add effect task\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsAddEffect:filename:
   *
   * The assigned filename.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("filename\0",
				   "the filename\0",
				   "The filename containing the effect\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsAddEffect:effect:
   *
   * The assigned effect.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("effect\0",
				   "the effect\0",
				   "The effect\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) add_effect;

  task->launch = ags_add_effect_launch;
}

void
ags_add_effect_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_effect_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_effect_connect;
  connectable->disconnect = ags_add_effect_disconnect;
}

void
ags_add_effect_init(AgsAddEffect *add_effect)
{
  add_effect->channel = NULL;
  add_effect->filename = NULL;
  add_effect->effect = NULL;
}

void
ags_add_effect_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsAddEffect *add_effect;

  add_effect = AGS_ADD_EFFECT(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(add_effect->channel == (GObject *) channel){
	return;
      }

      if(add_effect->channel != NULL){
	g_object_unref(add_effect->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      add_effect->channel = (GObject *) channel;
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(add_effect->filename == filename){
	return;
      }

      if(add_effect->filename != NULL){
        g_free(add_effect->filename);
      }

      add_effect->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;

      effect = g_value_get_string(value);

      if(add_effect->effect == effect){
	return;
      }

      if(add_effect->effect != NULL){
        g_free(add_effect->effect);
      }

      add_effect->effect = g_strdup(effect);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_effect_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsAddEffect *add_effect;

  add_effect = AGS_ADD_EFFECT(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, add_effect->channel);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, add_effect->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, add_effect->effect);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_effect_connect(AgsConnectable *connectable)
{
  ags_add_effect_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_effect_disconnect(AgsConnectable *connectable)
{
  ags_add_effect_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_effect_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_effect_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_effect_launch(AgsTask *task)
{
  AgsAddEffect *add_effect;
  
  add_effect = AGS_ADD_EFFECT(task);

  ags_channel_add_effect(add_effect->channel,
			 add_effect->filename,
			 add_effect->effect);
}

/**
 * ags_add_effect_new:
 * @channel: the #AgsChannel to set up the effect
 * @filename: the filename including @effect
 * @effect: the effect's name
 *
 * Creates an #AgsAddEffect.
 *
 * Returns: an new #AgsAddEffect.
 *
 * Since: 0.4.3
 */
AgsAddEffect*
ags_add_effect_new(AgsChannel *channel,
		   gchar *filename,
		   gchar *effect)
{
  AgsAddEffect *add_effect;

  add_effect = (AgsAddEffect *) g_object_new(AGS_TYPE_ADD_EFFECT,
					     NULL);

  add_effect->channel = channel;
  add_effect->filename = filename;
  add_effect->effect = effect;
  
  return(add_effect);
}
