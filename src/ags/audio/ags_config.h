/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_CONFIG_H__
#define __AGS_CONFIG_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_CONFIG                (ags_config_get_type ())
#define AGS_CONFIG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONFIG, AgsConfig))
#define AGS_CONFIG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONFIG, AgsConfigClass))
#define AGS_IS_CONFIG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CONFIG))
#define AGS_IS_CONFIG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CONFIG))
#define AGS_CONFIG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CONFIG, AgsConfigClass))

#define AGS_CONFIG_DEFAULT_VERSION "0.4.0\0"
#define AGS_CONFIG_DEFAULT_BUILD_ID "CEST 22-06-2014 03:07\0"

typedef struct _AgsConfig AgsConfig;
typedef struct _AgsConfigClass AgsConfigClass;

typedef enum{
  AGS_CONFIG_IN_MEMORY    = 1,
  AGS_CONFIG_LIB          = 1 << 1,
  AGS_CONFIG_GUI          = 1 << 2,
  AGS_CONFIG_SERVER       = 1 << 3,
}AgsConfigFlags;

typedef enum{
  AGS_FRONTEND_NONINTERACTIVE  = 1,
  AGS_FRONTEND_INTERACTIVE     = 1 << 1,
  AGS_FRONTED_FILE             = 1 << 2,
  AGS_FRONTEND_GTK             = 1 << 3,
}AgsFlags;

struct _AgsConfig
{
  GObject object;

  guint flags;

  guint version;
  gchar *build_id;

  guint ags_flags;

  GHashTable *hash;
};

struct _AgsConfigClass
{
  GObjectClass object;
};

GType ags_config_get_type();

void ags_config_load_defaults(AgsConfig *config);

void ags_config_set(AgsConfig *config, gchar *key, GValue *value);
void ags_config_get(AgsConfig *config, gchar *key, GValue *value);

AgsConfig* ags_config_new();

#endif /*__AGS_CONFIG_H__*/
