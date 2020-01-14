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

#include <ags/lib/ags_log.h>

#include <stdlib.h>

void ags_log_class_init(AgsLogClass *log);
void ags_log_init (AgsLog *log);
void ags_log_finalize(GObject *gobject);

/**
 * SECTION:ags_log
 * @short_description: Log
 * @title: AgsLog
 * @section_id:
 * @include: ags/lib/ags_log.h
 *
 * The #AgsLog logging class.
 */

static gpointer ags_log_parent_class = NULL;
AgsLog *ags_log = NULL;

GType
ags_log_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_log = 0;

    static const GTypeInfo ags_log_info = {
      sizeof (AgsLogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_log_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_log_init,
    };

    ags_type_log = g_type_register_static(G_TYPE_OBJECT,
					  "AgsLog",
					  &ags_log_info,
					  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_log);
  }

  return g_define_type_id__volatile;
}

void
ags_log_class_init(AgsLogClass *log)
{
  GObjectClass *gobject;
  
  ags_log_parent_class = g_type_class_peek_parent(log);

  /* GObjectClass */
  gobject = (GObjectClass *) log;
  
  gobject->finalize = ags_log_finalize;
}

void
ags_log_init(AgsLog *log)
{
  log->flags = 0;

  /* create mutex */
  g_rec_mutex_init(&(log->obj_mutex));
  
  log->messages = NULL;
}

void
ags_log_finalize(GObject *gobject)
{
  AgsLog *log;

  log = AGS_LOG(gobject);

  /* free messages and list */
  g_list_free_full(g_atomic_pointer_get(&(log->messages)),
		   g_free);

  if(log == ags_log){
    ags_log = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_log_parent_class)->finalize(gobject);
}

/**
 * ags_log_get_instance:
 * 
 * Get your logging instance.
 *
 * Returns: (transfer none): the #AgsLog instance
 *
 * Since: 3.0.0
 */
AgsLog*
ags_log_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(ags_log == NULL){
    ags_log = ags_log_new();
  }

  g_mutex_unlock(&mutex);

  return(ags_log);
}

/**
 * ags_log_add_message:
 * @log: the #AgsLog
 * @str: (transfer full): the message
 * 
 * Add a message to @log.
 *
 * Since: 3.0.0
 */
void
ags_log_add_message(AgsLog *log,
		    gchar *str)
{
  g_rec_mutex_lock(&(log->obj_mutex));
  
  g_atomic_pointer_set(&(log->messages),
		       g_list_prepend(g_atomic_pointer_get(&(log->messages)),
				      g_strdup(str)));

  g_rec_mutex_unlock(&(log->obj_mutex));
}

/**
 * ags_log_get_messages:
 * @log: the #AgsLog
 * 
 * Get log messages as #GList-struct containing strings.
 *
 * Returns: (element-type utf8) (transfer none): the #GList-struct containing log messages
 *
 * Since: 3.0.0
 */
GList*
ags_log_get_messages(AgsLog *log)
{
  return(g_atomic_pointer_get(&(log->messages)));
}

/**
 * ags_log_new:
 *
 * Instantiate a new #AgsLog.
 *
 * Returns: the new instance
 *
 * Since: 3.0.0
 */
AgsLog*
ags_log_new()
{
  AgsLog *log;
  
  log = g_object_new(AGS_TYPE_LOG,
		     NULL);

  return(log);
}
