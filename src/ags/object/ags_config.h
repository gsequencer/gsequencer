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

#define AGS_TYPE_CONFIG                (ags_config_get_type ())
#define AGS_CONFIG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONFIG, AgsConfig))
#define AGS_CONFIG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONFIG, AgsConfigClass))
#define AGS_IS_CONFIG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CONFIG))
#define AGS_IS_CONFIG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CONFIG))
#define AGS_CONFIG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CONFIG, AgsConfigClass))

#define AGS_CONFIG_DEFAULT_VERSION "0.4.3\0"
#define AGS_CONFIG_DEFAULT_BUILD_ID "Sat Apr  4 02:44:55 GMT 2015\0"

#define AGS_INIT_CONTEXT_TSD_CONFIG "ags-config\0"

#define AGS_CONFIG_GENERIC "generic\0"
#define AGS_CONFIG_THREAD "thread\0"
#define AGS_CONFIG_DEVOUT "device\0"
#define AGS_CONFIG_RECALL "recall\0"

typedef struct _AgsConfig AgsConfig;
typedef struct _AgsConfigClass AgsConfigClass;

struct _AgsConfig
{
  GObject object;

  guint version;
  gchar *build_id;
  
  GObject *application_context;

  GKeyFile *key_file;
};

struct _AgsConfigClass
{
  GObjectClass object;

  void (*load_defaults)(AgsConfig *config);

  void (*set_value)(AgsConfig *config, gchar *group, gchar *key, gchar *value);
  gchar* (*get_value)(AgsConfig *config, gchar *group, gchar *key);
};

GType ags_config_get_type();

void ags_config_load_defaults(AgsConfig *config);
void ags_config_load_from_file(AgsConfig *config, gchar *filename);

void ags_config_set_value(AgsConfig *config, gchar *group, gchar *key, gchar *value);
gchar* ags_config_get_value(AgsConfig *config, gchar *group, gchar *key);

void ags_config_save(AgsConfig *config);

AgsConfig* ags_config_new(GObject *application_context);

#endif /*__AGS_CONFIG_H__*/
