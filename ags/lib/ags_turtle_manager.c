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

#include <ags/lib/ags_turtle_manager.h>

#include <ags/lib/ags_turtle.h>

void ags_turtle_manager_class_init(AgsTurtleManagerClass *turtle_manager);
void ags_turtle_manager_init (AgsTurtleManager *turtle_manager);
void ags_turtle_manager_dispose(GObject *gobject);
void ags_turtle_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_turtle_manager
 * @short_description: Singleton pattern to organize turtles
 * @title: AgsTurtleManager
 * @section_id:
 * @include: ags/lib/ags_turtle_manager.h
 *
 * The #AgsTurtleManager keeps track of your turtles.
 */

static gpointer ags_turtle_manager_parent_class = NULL;

AgsTurtleManager *ags_turtle_manager = NULL;

GType
ags_turtle_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_turtle_manager = 0;

    static const GTypeInfo ags_turtle_manager_info = {
      sizeof (AgsTurtleManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_turtle_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTurtleManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_turtle_manager_init,
    };

    ags_type_turtle_manager = g_type_register_static(G_TYPE_OBJECT,
						     "AgsTurtleManager",
						     &ags_turtle_manager_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_turtle_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_turtle_manager_class_init(AgsTurtleManagerClass *turtle_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_turtle_manager_parent_class = g_type_class_peek_parent(turtle_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) turtle_manager;

  gobject->dispose = ags_turtle_manager_dispose;
  gobject->finalize = ags_turtle_manager_finalize;
}

void
ags_turtle_manager_init(AgsTurtleManager *turtle_manager)
{
  g_rec_mutex_init(&(turtle_manager->obj_mutex));

  turtle_manager->turtle = NULL;
}

void
ags_turtle_manager_dispose(GObject *gobject)
{
  AgsTurtleManager *turtle_manager;

  GList *turtle;

  turtle_manager = AGS_TURTLE_MANAGER(gobject);

  turtle = turtle_manager->turtle;
  turtle_manager->turtle = NULL;
  
  g_list_free_full(turtle,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_turtle_manager_parent_class)->dispose(gobject);
}

void
ags_turtle_manager_finalize(GObject *gobject)
{
  AgsTurtleManager *turtle_manager;

  GList *turtle;

  turtle_manager = AGS_TURTLE_MANAGER(gobject);

  /* turtle */
  turtle = turtle_manager->turtle;

  if(turtle != NULL){
    g_list_free_full(turtle,
		     g_object_unref);
  }

  if(turtle_manager == ags_turtle_manager){
    ags_turtle_manager = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_turtle_manager_parent_class)->finalize(gobject);
}

/**
 * ags_turtle_manager_find:
 * @turtle_manager: the #AgsTurtleManager
 * @filename: the filename as string
 * 
 * Find @filename in @turtle_manager.
 *
 * Returns: (transfer full): the matching #AgsTurtle, or %NULL
 * 
 * Since: 3.0.0
 */
GObject*
ags_turtle_manager_find(AgsTurtleManager *turtle_manager,
			gchar *filename)
{
  GObject *retval;
  
  GList *start_turtle, *turtle;

  gboolean success;
  
  GRecMutex *turtle_manager_mutex;
  GRecMutex *turtle_mutex;

  if(!AGS_IS_TURTLE_MANAGER(turtle_manager) ||
     filename == NULL){
    return(NULL);
  }

  turtle_manager_mutex = AGS_TURTLE_MANAGER_GET_OBJ_MUTEX(turtle_manager);

  g_rec_mutex_lock(turtle_manager_mutex);
  
  turtle =
    start_turtle = g_list_copy_deep(turtle_manager->turtle,
				    (GCopyFunc) g_object_ref,
				    NULL);

  g_rec_mutex_unlock(turtle_manager_mutex);

  retval = NULL;
  
  while(turtle != NULL){
    turtle_mutex = AGS_TURTLE_GET_OBJ_MUTEX(turtle->data);

    g_rec_mutex_lock(turtle_mutex);
  
    success = (!g_ascii_strcasecmp(AGS_TURTLE(turtle->data)->filename,
				   filename)) ? TRUE: FALSE;

    g_rec_mutex_unlock(turtle_mutex);    
    
    if(success){
      retval = turtle->data;

      g_object_ref(retval);
      
      break;
    }
    
    turtle = turtle->next;
  }

  g_list_free_full(start_turtle,
		   g_object_unref);
  
  return(retval);
}

/**
 * ags_turtle_manager_add:
 * @turtle_manager: the #AgsTurtleManager
 * @turtle: (transfer full): the #AgsTurtle
 * 
 * Adds @turtle to @turtle_manager.
 *
 * Since: 3.0.0
 */
void
ags_turtle_manager_add(AgsTurtleManager *turtle_manager,
		       GObject *turtle)
{
  if(!AGS_IS_TURTLE_MANAGER(turtle_manager) ||
     !AGS_IS_TURTLE(turtle)){
    return;
  }

  if(g_list_find(turtle_manager->turtle,
		 turtle) == NULL){
    turtle_manager->turtle = g_list_prepend(turtle_manager->turtle,
					    turtle);
    g_object_ref(turtle);
  }
}

/**
 * ags_turtle_manager_get_instance:
 *
 * Get instance of #AgsTurtleManager.
 *
 * Returns: (transfer none): the #AgsTurtleManager
 *
 * Since: 3.0.0
 */
AgsTurtleManager*
ags_turtle_manager_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&(mutex));

  if(ags_turtle_manager == NULL){
    ags_turtle_manager = ags_turtle_manager_new();
  }

  g_mutex_unlock(&(mutex));
  
  return(ags_turtle_manager);
}

/**
 * ags_turtle_manager_new:
 *
 * Creates an #AgsTurtleManager
 *
 * Returns: a new #AgsTurtleManager
 *
 * Since: 3.0.0
 */
AgsTurtleManager*
ags_turtle_manager_new()
{
  AgsTurtleManager *turtle_manager;

  turtle_manager = (AgsTurtleManager *) g_object_new(AGS_TYPE_TURTLE_MANAGER,
						     NULL);

  return(turtle_manager);
}
