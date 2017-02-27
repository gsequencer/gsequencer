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

#include <ags/object/ags_connection_manager.h>

void ags_connection_manager_class_init(AgsConnectionManagerClass *connection_manager);
void ags_connection_manager_init (AgsConnectionManager *connection_manager);
void ags_connection_manager_dispose(GObject *gobject);
void ags_connection_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_connection_manager
 * @short_description: Singleton pattern to organize connections
 * @title: AgsConnectionManager
 * @section_id:
 * @include: ags/object/ags_connection_manager.h
 *
 * The #AgsConnectionManager manages your connections.
 */

static gpointer ags_connection_manager_parent_class = NULL;

AgsConnectionManager *ags_connection_manager = NULL;

GType
ags_connection_manager_get_type (void)
{
  static GType ags_type_connection_manager = 0;

  if(!ags_type_connection_manager){
    static const GTypeInfo ags_connection_manager_info = {
      sizeof (AgsConnectionManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_connection_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConnectionManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_connection_manager_init,
    };

    ags_type_connection_manager = g_type_register_static(G_TYPE_OBJECT,
						  "AgsConnectionManager\0",
						  &ags_connection_manager_info,
						  0);
  }

  return (ags_type_connection_manager);
}

void
ags_connection_manager_class_init(AgsConnectionManagerClass *connection_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_connection_manager_parent_class = g_type_class_peek_parent(connection_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) connection_manager;

  gobject->dispose = ags_connection_manager_dispose;
  gobject->finalize = ags_connection_manager_finalize;
}

void
ags_connection_manager_init(AgsConnectionManager *connection_manager)
{
  connection_manager->connection = NULL;
}

void
ags_connection_manager_dispose(GObject *gobject)
{
  AgsConnectionManager *connection_manager;

  connection_manager = AGS_CONNECTION_MANAGER(gobject);

  if(connection_manager->connection != NULL){
    g_list_free_full(connection_manager->connection,
		     g_object_unref);

    connection_manager->connection = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_connection_manager_parent_class)->dispose(gobject);
}

void
ags_connection_manager_finalize(GObject *gobject)
{
  AgsConnectionManager *connection_manager;

  connection_manager = AGS_CONNECTION_MANAGER(gobject);

  if(connection_manager->connection != NULL){
    g_list_free_full(connection_manager->connection,
		     g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_connection_manager_parent_class)->finalize(gobject);
}

GList*
ags_connection_manager_get_connection(AgsConnectionManager *connection_manager)
{
  if(connection_manager == NULL){
    connection_manager = ags_connection_manager_get_instance();
  }
  
  return(connection_manager->connection);
}

/**
 * ags_connection_manager_add_connection:
 * @connection_manager: the #AgsConnectionManager
 * @connection: the #AgsConnection
 * 
 * Add an #AgsConnection to @connection_manager.
 * 
 * Since: 0.7.65
 */
void
ags_connection_manager_add_connection(AgsConnectionManager *connection_manager,
				      AgsConnection *connection)
{
  if(connection_manager == NULL){
    connection_manager = ags_connection_manager_get_instance();
  }

  connection_manager->connection = g_list_prepend(connection_manager->connection,
						  connection);
}

/**
 * ags_connection_manager_remove_connection:
 * @connection_manager: the #AgsConnectionManager
 * @connection: the #AgsConnection
 * 
 * Remove an #AgsConnection from @connection_manager.
 * 
 * Since: 0.7.65
 */
void
ags_connection_manager_remove_connection(AgsConnectionManager *connection_manager,
					 AgsConnection *connection)
{
  if(connection_manager == NULL){
    connection_manager = ags_connection_manager_get_instance();
  }

  connection_manager->connection = g_list_remove(connection_manager->connection,
						 connection);
}

/**
 * ags_connection_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsConnectionManager
 *
 * Since: 0.7.65
 */
AgsConnectionManager*
ags_connection_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_connection_manager == NULL){
    ags_connection_manager = ags_connection_manager_new();

    pthread_mutex_unlock(&(mutex));
  }else{
    pthread_mutex_unlock(&(mutex));
  }

  return(ags_connection_manager);
}

/**
 * ags_connection_manager_new:
 *
 * Creates an #AgsConnectionManager
 *
 * Returns: a new #AgsConnectionManager
 *
 * Since: 0.7.65
 */
AgsConnectionManager*
ags_connection_manager_new()
{
  AgsConnectionManager *connection_manager;

  connection_manager = (AgsConnectionManager *) g_object_new(AGS_TYPE_CONNECTION_MANAGER,
							     NULL);

  return(connection_manager);
}
