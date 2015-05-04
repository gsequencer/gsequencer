/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/audio/task/ags_add_effect.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_effect_audio.h>
#include <ags/audio/ags_effect_channel.h>

void ags_add_effect_class_init(AgsAddEffectClass *add_effect);
void ags_add_effect_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_effect_init(AgsAddEffect *add_effect);
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
 * The #AgsAddEffect task adds #AgsEffect to context.
 */

static gpointer ags_add_effect_parent_class = NULL;
static AgsConnectableInterface *ags_add_effect_parent_connectable_interface;

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

  ags_add_effect_parent_class = g_type_class_peek_parent(add_effect);

  /* gobject */
  gobject = (GObjectClass *) add_effect;

  gobject->finalize = ags_add_effect_finalize;

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
ags_add_effect_new(GObject *context,
		   AgsChannel *channel,
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
