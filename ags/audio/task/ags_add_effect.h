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

#ifndef __AGS_ADD_EFFECT_H__
#define __AGS_ADD_EFFECT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_ADD_EFFECT                (ags_add_effect_get_type())
#define AGS_ADD_EFFECT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ADD_EFFECT, AgsAddEffect))
#define AGS_ADD_EFFECT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ADD_EFFECT, AgsAddEffectClass))
#define AGS_IS_ADD_EFFECT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ADD_EFFECT))
#define AGS_IS_ADD_EFFECT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ADD_EFFECT))
#define AGS_ADD_EFFECT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ADD_EFFECT, AgsAddEffectClass))

typedef struct _AgsAddEffect AgsAddEffect;
typedef struct _AgsAddEffectClass AgsAddEffectClass;

struct _AgsAddEffect
{
  AgsTask task;

  AgsChannel *channel;

  gchar *filename;
  gchar *effect;
};

struct _AgsAddEffectClass
{
  AgsTaskClass task;
};

GType ags_add_effect_get_type();

AgsAddEffect* ags_add_effect_new(AgsChannel *channel,
				 gchar *filename,
				 gchar *effect);

#endif /*__AGS_ADD_EFFECT_H__*/
