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

#ifndef __AGS_EFFECT_CONTAINER_H__
#define __AGS_EFFECT_CONTAINER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_EFFECT_CONTAINER                (ags_effect_container_get_type())
#define AGS_EFFECT_CONTAINER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_CONTAINER, AgsEffectContainer))
#define AGS_EFFECT_CONTAINER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_CONTAINER, AgsEffectContainerClass))
#define AGS_IS_EFFECT_CONTAINER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT_CONTAINER))
#define AGS_IS_EFFECT_CONTAINER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EFFECT_CONTAINER))
#define AGS_EFFECT_CONTAINER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EFFECT_CONTAINER, AgsEffectContainerClass))

#define AGS_EFFECT_CONTAINER_DEFAULT_VERSION "0.4.3\0"
#define AGS_EFFECT_CONTAINER_DEFAULT_BUILD_ID "CEST 15-03-2015 13:40\0"

typedef struct _AgsEffectContainer AgsEffectContainer;
typedef struct _AgsEffectContainerClass AgsEffectContainerClass;

typedef enum{
  AGS_EFFECT_CONTAINER_CONNECTED        = 1,
}AgsEffectContainerFlags;

struct _AgsEffectContainer
{
  GtkVBox vbox;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;
  
  AgsChannel *channel;
};

struct _AgsEffectContainerClass
{
  GtkVBoxClass vbox;

  void (*add_effect)(AgsEffectContainer *effect_container,
		     gchar *effect);
  void (*remove_effect)(AgsEffectContainer *effect_container,
			guint nth);
};

GType ags_effect_container_get_type(void);

AgsEffectContainer* ags_effect_container_new(AgsChannel *channel);

#endif /*__AGS_EFFECT_CONTAINER_H__*/
