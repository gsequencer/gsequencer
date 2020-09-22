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

#ifndef __AGS_PRIORITY_H__
#define __AGS_PRIORITY_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_PRIORITY                (ags_priority_get_type ())
#define AGS_TYPE_PRIORITY_FLAGS          (ags_priority_flags_get_type())
#define AGS_PRIORITY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PRIORITY, AgsPriority))
#define AGS_PRIORITY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PRIORITY, AgsPriorityClass))
#define AGS_IS_PRIORITY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PRIORITY))
#define AGS_IS_PRIORITY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PRIORITY))
#define AGS_PRIORITY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PRIORITY, AgsPriorityClass))

#define AGS_PRIORITY_GET_OBJ_MUTEX(obj) (&(((AgsPriority *) obj)->obj_mutex))

#define AGS_PRIORITY_DEFAULT_VERSION "2.4.2"
#define AGS_PRIORITY_DEFAULT_BUILD_ID "Mon Dec  2 08:15:02 UTC 2019"

#define AGS_PRIORITY_RT_THREAD "rt-thread"

#define AGS_PRIORITY_KEY_LIBAGS "libags"

#define AGS_PRIORITY_KEY_SERVER_MAIN_LOOP "server-main-loop"

#define AGS_PRIORITY_KEY_AUDIO_MAIN_LOOP "audio-main-loop"
#define AGS_PRIORITY_KEY_AUDIO "audio"
#define AGS_PRIORITY_KEY_OSC_SERVER_MAIN_LOOP "osc-server-main-loop"

#define AGS_PRIORITY_KEY_GUI_MAIN_LOOP "gui-main-loop"

typedef struct _AgsPriority AgsPriority;
typedef struct _AgsPriorityClass AgsPriorityClass;

/**
 * AgsPriorityFlags:
 * @AGS_PRIORITY_CONNECTED: the priority was connected by calling #AgsConnectable::connect()
 * 
 * Enum values to control the behavior or indicate internal state of #AgsPriority by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PRIORITY_CONNECTED    = 1,
}AgsPriorityFlags;

struct _AgsPriority
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;
  
  gchar *version;
  gchar *build_id;

  GKeyFile *key_file;
};

struct _AgsPriorityClass
{
  GObjectClass gobject;

  void (*load_defaults)(AgsPriority *priority);

  void (*set_value)(AgsPriority *priority, gchar *group, gchar *key, gchar *value);
  gchar* (*get_value)(AgsPriority *priority, gchar *group, gchar *key);
};

GType ags_priority_get_type();
GType ags_priority_flags_get_type();

void ags_priority_load_defaults(AgsPriority *priority);
void ags_priority_load_from_file(AgsPriority *priority, gchar *filename);

void ags_priority_set_value(AgsPriority *priority, gchar *group, gchar *key, gchar *value);
gchar* ags_priority_get_value(AgsPriority *priority, gchar *group, gchar *key);

AgsPriority* ags_priority_get_instance();
AgsPriority* ags_priority_new();

G_END_DECLS

#endif /*__AGS_PRIORITY_H__*/
