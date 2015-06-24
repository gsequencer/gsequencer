/* This file is part of GSequencer.
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

#define AGS_CONFIG_DEFAULT_VERSION "0.4.2\0"
#define AGS_CONFIG_DEFAULT_BUILD_ID "CEST 02-10-2014 19:36\0"

#define AGS_CONFIG_GENERIC "generic\0"
#define AGS_CONFIG_THREAD "thread\0"
#define AGS_CONFIG_DEVOUT "device\0"
#define AGS_CONFIG_RECALL "recall\0"

typedef struct _AgsConfig AgsConfig;
typedef struct _AgsConfigClass AgsConfigClass;

struct _AgsConfig
{
  GObject object;

  gchar *version;
  gchar *build_id;
  
  GObject *ags_main;

  GKeyFile *key_file;
};

struct _AgsConfigClass
{
  GObjectClass object;
};

GType ags_config_get_type();

void ags_config_load_defaults(AgsConfig *config);
void ags_config_load_from_file(AgsConfig *config, gchar *filename);

void ags_config_save(AgsConfig *config);

void ags_config_set(AgsConfig *config, gchar *group, gchar *key, gchar *value);
gchar* ags_config_get(AgsConfig *config, gchar *group, gchar *key);

AgsConfig* ags_config_new();

#endif /*__AGS_CONFIG_H__*/
