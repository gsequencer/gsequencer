/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/audio/osc/controller/ags_osc_controller.h>

#include <ags/audio/osc/ags_osc_server.h>

#include <ags/i18n.h>

#include <stdlib.h>

void ags_osc_controller_class_init(AgsOscControllerClass *osc_controller);
void ags_osc_controller_init(AgsOscController *osc_controller);
void ags_osc_controller_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_osc_controller_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_osc_controller_dispose(GObject *gobject);
void ags_osc_controller_finalize(GObject *gobject);

/**
 * SECTION:ags_osc_controller
 * @short_description: base OSC controller
 * @title: AgsOscController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_controller.h
 *
 * The #AgsOscController is a base object to implement OSC controllers.
 */

enum{
  PROP_0,
  PROP_OSC_SERVER,
  PROP_CONTEXT_PATH,
};

static gpointer ags_osc_controller_parent_class = NULL;

static pthread_mutex_t ags_osc_controller_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_osc_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_controller = 0;

    static const GTypeInfo ags_osc_controller_info = {
      sizeof (AgsOscControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_controller_init,
    };
    
    ags_type_osc_controller = g_type_register_static(G_TYPE_OBJECT,
						     "AgsOscController",
						     &ags_osc_controller_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_controller_class_init(AgsOscControllerClass *osc_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_controller_parent_class = g_type_class_peek_parent(osc_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_controller;

  gobject->set_property = ags_osc_controller_set_property;
  gobject->get_property = ags_osc_controller_get_property;

  gobject->dispose = ags_osc_controller_dispose;
  gobject->finalize = ags_osc_controller_finalize;

  /* properties */
  /**
   * AgsOscController:osc-server:
   *
   * The assigned #AgsOscServer
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_object("osc-server",
				   i18n("assigned osc server"),
				   i18n("The assigned osc server"),
				   AGS_TYPE_OSC_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OSC_SERVER,
				  param_spec);

  /**
   * AgsOscController:context-path:
   *
   * The context path provided.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("context-path",
				   i18n_pspec("context path to provide"),
				   i18n_pspec("The context path provided by this controller"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTEXT_PATH,
				  param_spec);
}

void
ags_osc_controller_init(AgsOscController *osc_controller)
{
  /* osc controller mutex */
  osc_controller->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(osc_controller->obj_mutexattr);
  pthread_mutexattr_settype(osc_controller->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(osc_controller->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  osc_controller->obj_mutex =  (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(osc_controller->obj_mutex,
		     osc_controller->obj_mutexattr);

  osc_controller->osc_server = NULL;

  osc_controller->context_path = NULL;
}

void
ags_osc_controller_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsOscController *osc_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_controller = AGS_OSC_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = osc_controller->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  case PROP_OSC_SERVER:
    {
      AgsOscServer *osc_server;

      osc_server = (AgsOscServer *) g_value_get_object(value);

      pthread_mutex_lock(osc_controller_mutex);

      if(osc_controller->osc_server == (GObject *) osc_server){
	pthread_mutex_unlock(osc_controller_mutex);

	return;
      }

      if(osc_controller->osc_server != NULL){
	g_object_unref(G_OBJECT(osc_controller->osc_server));
      }

      if(osc_server != NULL){
	g_object_ref(G_OBJECT(osc_server));
      }
      
      osc_controller->osc_server = osc_server;

      pthread_mutex_unlock(osc_controller_mutex);
    }
    break;
  case PROP_CONTEXT_PATH:
    {
      char *context_path;

      context_path = (char *) g_value_get_string(value);

      pthread_mutex_lock(controller_mutex);

      osc_controller->context_path = g_strdup(context_path);

      pthread_mutex_unlock(controller_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_controller_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsOscController *osc_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_controller = AGS_OSC_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = osc_controller->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  case PROP_OSC_SERVER:
    {
      pthread_mutex_lock(osc_controller_mutex);

      g_value_set_object(value, osc_controller->osc_server);

      pthread_mutex_unlock(osc_controller_mutex);
    }
    break;
  case PROP_CONTEXT_PATH:
    {
      pthread_mutex_lock(controller_mutex);
      
      g_value_set_string(value, osc_controller->context_path);

      pthread_mutex_unlock(controller_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_controller_dispose(GObject *gobject)
{
  AgsOscController *osc_controller;

  osc_controller = AGS_OSC_CONTROLLER(gobject);

  if(osc_controller->osc_server != NULL){
    g_object_unref(osc_controller->osc_server);

    osc_controller->osc_server = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_controller_parent_class)->dispose(gobject);
}

void
ags_osc_controller_finalize(GObject *gobject)
{
  AgsOscController *osc_controller;

  osc_controller = AGS_OSC_CONTROLLER(gobject);

  pthread_mutex_destroy(osc_controller->obj_mutex);
  free(osc_controller->obj_mutex);

  pthread_mutexattr_destroy(osc_controller->obj_mutexattr);
  free(osc_controller->obj_mutexattr);

  if(osc_controller->osc_server != NULL){
    g_object_unref(osc_controller->osc_server);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_controller_parent_class)->finalize(gobject);
}

/**
 * ags_osc_controller_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.1.0
 */
pthread_mutex_t*
ags_osc_controller_get_class_mutex()
{
  return(&ags_osc_controller_class_mutex);
}

/**
 * ags_osc_controller_new:
 * 
 * Instantiate new #AgsOscController
 * 
 * Returns: the #AgsOscController
 * 
 * Since: 2.1.0
 */
AgsOscController*
ags_osc_controller_new()
{
  AgsOscController *osc_controller;

  osc_controller = (AgsOscController *) g_object_new(AGS_TYPE_OSC_CONTROLLER,
						     NULL);

  return(osc_controller);
}
