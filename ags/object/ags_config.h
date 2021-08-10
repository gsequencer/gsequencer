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

#ifndef __AGS_CONFIG_H__
#define __AGS_CONFIG_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_CONFIG                (ags_config_get_type ())
#define AGS_CONFIG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONFIG, AgsConfig))
#define AGS_CONFIG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONFIG, AgsConfigClass))
#define AGS_IS_CONFIG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CONFIG))
#define AGS_IS_CONFIG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CONFIG))
#define AGS_CONFIG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CONFIG, AgsConfigClass))

#define AGS_CONFIG_GET_OBJ_MUTEX(obj) (&(((AgsConfig *) obj)->obj_mutex))

#define AGS_CONFIG_DEFAULT_VERSION "0.7.0"
#define AGS_CONFIG_DEFAULT_BUILD_ID "CEST 13-10-2015 01:19"

#define AGS_CONFIG_GENERIC "generic"
#define AGS_CONFIG_THREAD "thread"
#define AGS_CONFIG_SERVER "server"
#define AGS_CONFIG_SOUNDCARD "soundcard"
#define AGS_CONFIG_SOUNDCARD_0 "soundcard-0"
#define AGS_CONFIG_SEQUENCER "sequencer"
#define AGS_CONFIG_SEQUENCER_0 "sequencer-0"
#define AGS_CONFIG_RECALL "recall"
#define AGS_CONFIG_OSC_SERVER "osc-server"
#define AGS_CONFIG_OSC_SERVER_0 "osc-server-0"

typedef struct _AgsConfig AgsConfig;
typedef struct _AgsConfigClass AgsConfigClass;

struct _AgsConfig
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;
  
  gchar *version;
  gchar *build_id;
  
  GKeyFile *key_file;
};

struct _AgsConfigClass
{
  GObjectClass gobject;

  void (*load_defaults)(AgsConfig *config);

  void (*set_value)(AgsConfig *config, gchar *group, gchar *key, gchar *value);
  gchar* (*get_value)(AgsConfig *config, gchar *group, gchar *key);
};

GType ags_config_get_type();

void ags_config_load_defaults(AgsConfig *config);
void ags_config_load_from_file(AgsConfig *config, gchar *filename);
void ags_config_load_from_data(AgsConfig *config,
			       char *buffer, gsize buffer_length);

void ags_config_set_value(AgsConfig *config, gchar *group, gchar *key, gchar *value);
gchar* ags_config_get_value(AgsConfig *config, gchar *group, gchar *key);

void ags_config_to_data(AgsConfig *config,
			char **buffer, gsize *buffer_length);
void ags_config_save(AgsConfig *config);

void ags_config_clear(AgsConfig *config);

AgsConfig* ags_config_get_instance();
AgsConfig* ags_config_new();

G_END_DECLS

#endif /*__AGS_CONFIG_H__*/
