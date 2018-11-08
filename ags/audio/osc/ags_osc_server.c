/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/osc/ags_osc_server.h>

void ags_osc_server_class_init(AgsOscServerClass *osc_server);
void ags_osc_server_init(AgsOscServer *osc_server);
void ags_osc_server_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_osc_server_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_osc_server_finalize(GObject *gobject);

/**
 * SECTION:ags_osc_server
 * @short_description: the OSC server
 * @title: AgsOscServer
 * @section_id:
 * @include: ags/audio/osc/ags_osc_server.h
 *
 * #AgsOscServer your osc server.
 */

enum{
  PROP_0,
};

enum{
  START,
  LAST_SIGNAL,
};

static gpointer ags_osc_server_parent_class = NULL;
static guint osc_server_signals[LAST_SIGNAL];

static pthread_mutex_t ags_osc_server_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_osc_server_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_server = 0;

    static const GTypeInfo ags_osc_server_info = {
      sizeof (AgsOscServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_server_init,
    };

    ags_type_osc_server = g_type_register_static(G_TYPE_OBJECT,
						 "AgsOscServer", &ags_osc_server_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_server);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_server_class_init(AgsOscServerClass *osc_server)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_server_parent_class = g_type_class_peek_parent(osc_server);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_server;

  gobject->set_property = ags_osc_server_set_property;
  gobject->get_property = ags_osc_server_get_property;
  
  gobject->finalize = ags_osc_server_finalize;
}

void
ags_osc_server_init(AgsOscServer *osc_server)
{
  osc_server->flags = 0;
  
  /* osc server mutex */
  osc_server->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(osc_server->obj_mutexattr);
  pthread_mutexattr_settype(osc_server->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(osc_server->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  osc_server->obj_mutex =  (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(osc_server->obj_mutex,
		     osc_server->obj_mutexattr);
}

void
ags_osc_server_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsOscServer *osc_server;

  pthread_mutex_t *osc_server_mutex;

  osc_server = AGS_OSC_SERVER(gobject);

  /* get osc server mutex */
  pthread_mutex_lock(ags_osc_server_get_class_mutex());
  
  osc_server_mutex = osc_server->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_server_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_server_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsOscServer *osc_server;

  pthread_mutex_t *osc_server_mutex;

  osc_server = AGS_OSC_SERVER(gobject);

  /* get osc server mutex */
  pthread_mutex_lock(ags_osc_server_get_class_mutex());
  
  osc_server_mutex = osc_server->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_server_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_server_finalize(GObject *gobject)
{
  AgsOscServer *osc_server;
    
  osc_server = (AgsOscServer *) gobject;

  pthread_mutex_destroy(osc_server->obj_mutex);
  free(osc_server->obj_mutex);

  pthread_mutexattr_destroy(osc_server->obj_mutexattr);
  free(osc_server->obj_mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_osc_server_parent_class)->finalize(gobject);
}

/**
 * ags_osc_server_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.1.0
 */
pthread_mutex_t*
ags_osc_server_get_class_mutex()
{
  return(&ags_osc_server_class_mutex);
}

/**
 * ags_osc_server_start:
 * @osc_server: the #AgsOscServer
 * 
 * Start OSC server.
 * 
 * Since: 2.1.0
 */
void
ags_osc_server_start(AgsOscServer *osc_server)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_server_new:
 * 
 * Creates a new instance of #AgsOscServer
 *
 * Returns: the new #AgsOscServer
 * 
 * Since: 2.1.0
 */
AgsOscServer*
ags_osc_server_new()
{
  AgsOscServer *osc_server;
   
  osc_server = (AgsOscServer *) g_object_new(AGS_TYPE_OSC_SERVER,
					     NULL);
  
  return(osc_server);
}
