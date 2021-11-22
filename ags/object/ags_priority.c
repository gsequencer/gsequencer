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

#include <ags/object/ags_priority.h>

#include <ags/object/ags_marshal.h>

#include <gio/gio.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ags/config.h>

#if !defined(AGS_W32API)
#include <pwd.h>
#endif

#include <ags/config.h>
#include <ags/i18n.h>

void ags_priority_class_init(AgsPriorityClass *priority_class);
void ags_priority_init(AgsPriority *priority);
void ags_priority_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_priority_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_priority_dispose(GObject *gobject);
void ags_priority_finalize(GObject *gobject);

gchar* ags_priority_get_version(AgsPriority *priority);
void ags_priority_set_version(AgsPriority *priority, gchar *version);
gchar* ags_priority_get_build_id(AgsPriority *priority);
void ags_priority_set_build_id(AgsPriority *priority, gchar *build_id);

void ags_priority_real_load_defaults(AgsPriority *priority);
void ags_priority_real_set_value(AgsPriority *priority, gchar *group, gchar *key, gchar *value);
gchar* ags_priority_real_get_value(AgsPriority *priority, gchar *group, gchar *key);

/**
 * SECTION:ags_priority
 * @short_description: Priority Advanced Gtk+ Sequencer
 * @title: AgsPriority
 * @section_id:
 * @include: ags/object/ags_priority.h
 *
 * #AgsPriority provides priorities to Advanced Gtk+ Sequencer.
 */

enum{
  LOAD_DEFAULTS,
  SET_VALUE,
  GET_VALUE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
};

static gpointer ags_priority_parent_class = NULL;
static guint priority_signals[LAST_SIGNAL];

AgsPriority *ags_priority = NULL;

GType
ags_priority_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_priority = 0;

    static const GTypeInfo ags_priority_info = {
      sizeof (AgsPriorityClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_priority_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPriority),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_priority_init,
    };

    ags_type_priority = g_type_register_static(G_TYPE_OBJECT,
					       "AgsPriority",
					       &ags_priority_info,
					       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_priority);
  }

  return g_define_type_id__volatile;
}

GType
ags_priority_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_PRIORITY_CONNECTED, "AGS_PRIORITY_CONNECTED", "priority-connected" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsPriorityFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_priority_class_init(AgsPriorityClass *priority)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_priority_parent_class = g_type_class_peek_parent(priority);

  /* GObjectClass */
  gobject = (GObjectClass *) priority;

  gobject->set_property = ags_priority_set_property;
  gobject->get_property = ags_priority_get_property;

  gobject->dispose = ags_priority_dispose;
  gobject->finalize = ags_priority_finalize;

  /* properties */

  /* AgsPriorityClass */
  priority->load_defaults = ags_priority_real_load_defaults;
  priority->set_value = ags_priority_real_set_value;
  priority->get_value = ags_priority_real_get_value;

  /* signals */
  /**
   * AgsPriority::load-defaults:
   * @priority: the object to resolve
   *
   * The ::load-defaults signal notifies about loading defaults
   *
   * Since: 3.0.0
   */
  priority_signals[LOAD_DEFAULTS] =
    g_signal_new("load-defaults",
		 G_TYPE_FROM_CLASS (priority),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPriorityClass, load_defaults),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);


  /**
   * AgsPriority::set-value:
   * @priority: the object to resolve
   * @group: the group to apply to
   * @key: the key to set
   * @value: the value to apply
   *
   * The ::set-value signal notifies about value been setting.
   *
   * Since: 3.0.0
   */
  priority_signals[SET_VALUE] =
    g_signal_new("set-value",
		 G_TYPE_FROM_CLASS (priority),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPriorityClass, set_value),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__STRING_STRING_STRING,
		 G_TYPE_NONE, 3,
		 G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  /**
   * AgsPriority::get-value:
   * @priority: the object to resolve
   * @group: the group to retrieve from
   * @key: the key to get
   *
   * The ::get-value signal notifies about value been getting.
   *
   * Returns: the value
   *
   * Since: 3.0.0
   */
  priority_signals[GET_VALUE] =
    g_signal_new("get-value",
		 G_TYPE_FROM_CLASS (priority),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPriorityClass, get_value),
		 NULL, NULL,
		 ags_cclosure_marshal_STRING__STRING_STRING,
		 G_TYPE_STRING, 2,
		 G_TYPE_STRING, G_TYPE_STRING);
}

void
ags_priority_init(AgsPriority *priority)
{
  priority->flags = 0;
  
  g_rec_mutex_init(&(priority->obj_mutex));

  /* version and build id */
  priority->version = g_strdup(AGS_PRIORITY_DEFAULT_VERSION);
  priority->build_id = g_strdup(AGS_PRIORITY_DEFAULT_BUILD_ID);

  priority->key_file = g_key_file_new();
  g_key_file_ref(priority->key_file);
}

void
ags_priority_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsPriority *priority;

  GRecMutex *priority_mutex;

  priority = AGS_PRIORITY(gobject);

  /* get priority mutex */
  priority_mutex = AGS_PRIORITY_GET_OBJ_MUTEX(priority);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_priority_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsPriority *priority;

  GRecMutex *priority_mutex;

  priority = AGS_PRIORITY(gobject);

  /* get priority mutex */
  priority_mutex = AGS_PRIORITY_GET_OBJ_MUTEX(priority);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}
void
ags_priority_dispose(GObject *gobject)
{
  AgsPriority *priority;

  priority = (AgsPriority *) gobject;

  /* call parent */
  G_OBJECT_CLASS(ags_priority_parent_class)->dispose(gobject);
}

void
ags_priority_finalize(GObject *gobject)
{
  AgsPriority *priority;

  priority = (AgsPriority *) gobject;

  /* key file */
  if(priority->key_file != NULL){
    g_key_file_unref(priority->key_file);
  }

  /* global variable */
  if(ags_priority == priority){
    ags_priority = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_priority_parent_class)->finalize(gobject);
}

gchar*
ags_priority_get_version(AgsPriority *priority)
{
  gchar *version;
  
  GRecMutex *priority_mutex;

  if(!AGS_IS_PRIORITY(priority)){
    return(NULL);
  }
  
  priority_mutex = AGS_PRIORITY_GET_OBJ_MUTEX(priority);

  /* get version */
  g_rec_mutex_lock(priority_mutex);

  version = priority->version;
  
  g_rec_mutex_unlock(priority_mutex);

  return(version);
}

void
ags_priority_set_version(AgsPriority *priority, gchar *version)
{
  GRecMutex *priority_mutex;

  if(!AGS_IS_PRIORITY(priority)){
    return;
  }
  
  priority_mutex = AGS_PRIORITY_GET_OBJ_MUTEX(priority);

  /* set version */
  g_rec_mutex_lock(priority_mutex);

  priority->version = g_strdup(version);
  
  g_rec_mutex_unlock(priority_mutex);
}

gchar*
ags_priority_get_build_id(AgsPriority *priority)
{
  gchar *build_id;
  
  GRecMutex *priority_mutex;

  if(!AGS_IS_PRIORITY(priority)){
    return(NULL);
  }
  
  priority_mutex = AGS_PRIORITY_GET_OBJ_MUTEX(priority);

  /* get build id */
  g_rec_mutex_lock(priority_mutex);

  build_id = priority->build_id;
  
  g_rec_mutex_unlock(priority_mutex);

  return(build_id);
}

void
ags_priority_set_build_id(AgsPriority *priority, gchar *build_id)
{
  GRecMutex *priority_mutex;

  if(!AGS_IS_PRIORITY(priority)){
    return;
  }
  
  priority_mutex = AGS_PRIORITY_GET_OBJ_MUTEX(priority);

  /* set version */
  g_rec_mutex_lock(priority_mutex);

  priority->build_id = g_strdup(build_id);
  
  g_rec_mutex_unlock(priority_mutex);
}

void
ags_priority_real_load_defaults(AgsPriority *priority)
{

  GRecMutex *priority_mutex;
  
  priority_mutex = AGS_PRIORITY_GET_OBJ_MUTEX(priority);

  /* load defaults */
  g_rec_mutex_lock(priority_mutex);

  ags_priority_set_value(priority, AGS_PRIORITY_RT_THREAD, AGS_PRIORITY_KEY_LIBAGS, "0"); // 45

  ags_priority_set_value(priority, AGS_PRIORITY_RT_THREAD, AGS_PRIORITY_KEY_SERVER_MAIN_LOOP, "0"); // 15

  ags_priority_set_value(priority, AGS_PRIORITY_RT_THREAD, AGS_PRIORITY_KEY_AUDIO_MAIN_LOOP, "0"); // 15
  ags_priority_set_value(priority, AGS_PRIORITY_RT_THREAD, AGS_PRIORITY_KEY_AUDIO, "0"); // 45
  ags_priority_set_value(priority, AGS_PRIORITY_RT_THREAD, AGS_PRIORITY_KEY_OSC_SERVER_MAIN_LOOP, "0"); // 15

  ags_priority_set_value(priority, AGS_PRIORITY_RT_THREAD, AGS_PRIORITY_KEY_GUI_MAIN_LOOP, "0"); // 15

  g_rec_mutex_unlock(priority_mutex);
}

/**
 * ags_priority_load_defaults:
 * @priority: the #AgsPriority
 *
 * Load priorities from default values.
 *
 * Since: 3.0.0
 */
void
ags_priority_load_defaults(AgsPriority *priority)
{
  g_return_if_fail(AGS_IS_PRIORITY(priority));

  g_object_ref(G_OBJECT(priority));
  g_signal_emit(G_OBJECT(priority),
		priority_signals[LOAD_DEFAULTS], 0);
  g_object_unref(G_OBJECT(priority));
}

/**
 * ags_priority_load_from_file:
 * @priority: the #AgsPriority
 * @filename: the priorityuration file
 *
 * Load priorities from @filename.
 *
 * Since: 3.0.0
 */
void
ags_priority_load_from_file(AgsPriority *priority, gchar *filename)
{
  GFile *file;

  GRecMutex *priority_mutex;

  if(!AGS_IS_PRIORITY(priority)){
    return;
  }
  
  priority_mutex = AGS_PRIORITY_GET_OBJ_MUTEX(priority);

  file = g_file_new_for_path(filename);

  g_message("loading priorities for: %s", filename);

  if(!g_file_query_exists(file,
			  NULL)){
    ags_priority_load_defaults(priority);
  }else{
    GKeyFile *key_file;

    gchar **groups, **groups_start;
    gchar **keys, **keys_start;
    gchar *value;

    GError *error;

    g_rec_mutex_lock(priority_mutex);

    error = NULL;
    
    key_file = g_key_file_new();
    g_key_file_load_from_file(key_file,
			      filename,
			      G_KEY_FILE_NONE,
			      &error);

    if(error != NULL){
      g_warning("%s", error->message);

      g_error_free(error);
    }

    groups =
      groups_start = g_key_file_get_groups(key_file,
					   NULL);

    while(*groups != NULL){
      keys =
	keys_start = g_key_file_get_keys(key_file,
					 *groups,
					 NULL,
					 NULL);

      while(*keys != NULL){
	value = g_key_file_get_value(key_file,
				     *groups,
				     *keys,
				     NULL);
	ags_priority_set_value(priority,
			       *groups,
			       *keys,
			       value);
	
	keys++;
      }

      g_strfreev(keys_start);

      groups++;
    }

    g_strfreev(groups_start);
    g_key_file_unref(key_file);

    g_rec_mutex_unlock(priority_mutex);
  }

  g_object_unref(file);
}


void
ags_priority_real_set_value(AgsPriority *priority, gchar *group, gchar *key, gchar *value)
{
  GRecMutex *priority_mutex;
  
  priority_mutex = AGS_PRIORITY_GET_OBJ_MUTEX(priority);

  /* set value */
  g_rec_mutex_lock(priority_mutex);
  
  g_key_file_set_value(priority->key_file, group, key, value);

  g_rec_mutex_unlock(priority_mutex);
}

/**
 * ags_priority_set_value:
 * @priority: the #AgsPriority
 * @group: the priority group identifier
 * @key: the key of the property
 * @value: the value to set
 *
 * Set priority by @group and @key, applying @value.
 *
 * Since: 3.0.0
 */
void
ags_priority_set_value(AgsPriority *priority, gchar *group, gchar *key, gchar *value)
{
  g_return_if_fail(AGS_IS_PRIORITY(priority));

  g_object_ref(G_OBJECT(priority));
  g_signal_emit(G_OBJECT(priority),
		priority_signals[SET_VALUE], 0,
		group, key, value);
  g_object_unref(G_OBJECT(priority));
}

gchar*
ags_priority_real_get_value(AgsPriority *priority, gchar *group, gchar *key)
{
  gchar *str;
  GError *error;
  
  GRecMutex *priority_mutex;
  
  priority_mutex = AGS_PRIORITY_GET_OBJ_MUTEX(priority);

  /* get value */
  g_rec_mutex_lock(priority_mutex);
  
  error = NULL;

  str = g_key_file_get_value(priority->key_file, group, key, &error);

  if(error != NULL){
//    g_warning("%s", error->message);
    
    g_error_free(error);
  }
  
  g_rec_mutex_unlock(priority_mutex);

  return(str);
}

/**
 * ags_priority_get_value:
 * @priority: the #AgsPriority
 * @group: the priority group identifier
 * @key: the key of the property
 *
 * Retrieve priority by @group and @key.
 *
 * Returns: (transfer full): the property's value
 *
 * Since: 3.0.0
 */
gchar*
ags_priority_get_value(AgsPriority *priority, gchar *group, gchar *key)
{
  gchar *value;

  g_return_val_if_fail(AGS_IS_PRIORITY(priority), NULL);

  g_object_ref(G_OBJECT(priority));
  g_signal_emit(G_OBJECT(priority),
		priority_signals[GET_VALUE], 0,
		group, key,
		&value);
  g_object_unref(G_OBJECT(priority));

  return(value);
}

/**
 * ags_priority_get_instance:
 *
 * Get priority instance.
 *
 * Returns: (transfer none): the priority instance
 *
 * Since: 3.0.0
 */
AgsPriority*
ags_priority_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_priority == NULL){
    ags_priority = ags_priority_new(NULL);
  }

  g_mutex_unlock(&mutex);

  return(ags_priority);
}

/**
 * ags_priority_new:
 *
 * Create a new instance of #AgsPriority.
 *
 * Returns: the new #AgsPriority.
 *
 * Since: 3.0.0
 */
AgsPriority*
ags_priority_new()
{
  AgsPriority *priority;

  priority = (AgsPriority *) g_object_new(AGS_TYPE_PRIORITY,
					  NULL);

  return(priority);
}
