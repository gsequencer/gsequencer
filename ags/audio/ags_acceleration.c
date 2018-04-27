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

#include <ags/audio/ags_acceleration.h>

#include <ags/libags.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_acceleration_class_init(AgsAccelerationClass *acceleration);
void ags_acceleration_init(AgsAcceleration *acceleration);
void ags_acceleration_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_acceleration_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_acceleration_finalize(GObject *gobject);

/**
 * SECTION:ags_acceleration
 * @short_description: Acceleration class
 * @title: AgsAcceleration
 * @section_id:
 * @include: ags/audio/ags_acceleration.h
 *
 * #AgsAcceleration represents a downhill-grade.
 */

enum{
  PROP_0,
  PROP_X,
  PROP_Y,
  PROP_ACCELERATION_NAME,
};

static gpointer ags_acceleration_parent_class = NULL;

static pthread_mutex_t ags_acceleration_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_acceleration_get_type()
{
  static GType ags_type_acceleration = 0;

  if(!ags_type_acceleration){
    static const GTypeInfo ags_acceleration_info = {
      sizeof(AgsAccelerationClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_acceleration_class_init,
      NULL,
      NULL,
      sizeof(AgsAcceleration),
      0,
      (GInstanceInitFunc) ags_acceleration_init,
    };

    ags_type_acceleration = g_type_register_static(G_TYPE_OBJECT,
						   "AgsAcceleration",
						   &ags_acceleration_info,
						   0);
  }

  return(ags_type_acceleration);
}

void 
ags_acceleration_class_init(AgsAccelerationClass *acceleration)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_acceleration_parent_class = g_type_class_peek_parent(acceleration);

  gobject = (GObjectClass *) acceleration;

  gobject->set_property = ags_acceleration_set_property;
  gobject->get_property = ags_acceleration_get_property;

  gobject->finalize = ags_acceleration_finalize;

  /* properties */
  /**
   * AgsAcceleration:x:
   *
   * Acceleration offset x.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("x",
				 i18n_pspec("offset x"),
				 i18n_pspec("The x offset"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X,
				  param_spec);

  /**
   * AgsAcceleration:y:
   *
   * Acceleration value y.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_double("y",
				   i18n_pspec("offset y"),
				   i18n_pspec("The y offset"),
				   -1.0 * G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y,
				  param_spec);

  /**
   * AgsAcceleration:acceleration-name:
   *
   * The acceleration's name.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("acceleration-name",
				   i18n_pspec("acceleration name"),
				   i18n_pspec("The acceleration's name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ACCELERATION_NAME,
				  param_spec);
}

void
ags_acceleration_init(AgsAcceleration *acceleration)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  acceleration->flags = 0;

  /* add acceleration mutex */
  acceleration->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  acceleration->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* fields */
  acceleration->x = 0;
  acceleration->y = 0.0;

  acceleration->acceleration_name = NULL;
}


void
ags_acceleration_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsAcceleration *acceleration;

  pthread_mutex_t *acceleration_mutex;

  acceleration = AGS_ACCELERATION(gobject);

  /* get acceleration mutex */
  pthread_mutex_lock(ags_acceleration_get_class_mutex());
  
  acceleration_mutex = acceleration->obj_mutex;
  
  pthread_mutex_unlock(ags_acceleration_get_class_mutex());

  switch(prop_id){
  case PROP_X:
    {
      pthread_mutex_lock(acceleration_mutex);

      acceleration->x = g_value_get_uint(value);

      pthread_mutex_unlock(acceleration_mutex);
    }
    break;
  case PROP_Y:
    {
      pthread_mutex_lock(acceleration_mutex);

      acceleration->y = g_value_get_double(value);

      pthread_mutex_unlock(acceleration_mutex);
    }
    break;
  case PROP_ACCELERATION_NAME:
    {
      gchar *acceleration_name;
      
      acceleration_name = g_value_get_string(value);
      
      pthread_mutex_lock(acceleration_mutex);

      if(acceleration_name == acceleration->acceleration_name){
	pthread_mutex_unlock(acceleration_mutex);

	return;
      }

      if(acceleration->acceleration_name != NULL){
	g_free(acceleration->acceleration_name);
      }

      acceleration->acceleration_name = g_strdup(acceleration_name);

      pthread_mutex_unlock(acceleration_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_acceleration_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsAcceleration *acceleration;

  pthread_mutex_t *acceleration_mutex;

  acceleration = AGS_ACCELERATION(gobject);

  /* get acceleration mutex */
  pthread_mutex_lock(ags_acceleration_get_class_mutex());
  
  acceleration_mutex = acceleration->obj_mutex;
  
  pthread_mutex_unlock(ags_acceleration_get_class_mutex());

  switch(prop_id){
  case PROP_X:
    {
      pthread_mutex_lock(acceleration_mutex);

      g_value_set_uint(value, acceleration->x);

      pthread_mutex_unlock(acceleration_mutex);
    }
    break;
  case PROP_Y:
    {
      pthread_mutex_lock(acceleration_mutex);

      g_value_set_double(value, acceleration->y);

      pthread_mutex_unlock(acceleration_mutex);
    }
    break;
  case PROP_ACCELERATION_NAME:
    {
      pthread_mutex_lock(acceleration_mutex);

      g_value_set_string(value, acceleration->acceleration_name);

      pthread_mutex_unlock(acceleration_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_acceleration_finalize(GObject *gobject)
{
  AgsAcceleration *acceleration;

  acceleration = AGS_ACCELERATION(gobject);

  pthread_mutex_destroy(acceleration->obj_mutex);
  free(acceleration->obj_mutex);

  pthread_mutexattr_destroy(acceleration->obj_mutexattr);
  free(acceleration->obj_mutexattr);
  
  if(acceleration->acceleration_name != NULL){
    free(acceleration->acceleration_name);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_acceleration_parent_class)->finalize(gobject);
}

/**
 * ags_acceleration_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_acceleration_get_class_mutex()
{
  return(&ags_acceleration_class_mutex);
}

/**
 * ags_acceleration_test_flags:
 * @acceleration: the #AgsAcceleration
 * @flags: the flags
 * 
 * Test @flags to be set on @acceleration.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_acceleration_test_flags(AgsAcceleration *acceleration, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *acceleration_mutex;

  if(!AGS_IS_ACCELERATION(acceleration)){
    return(FALSE);
  }
      
  /* get acceleration mutex */
  pthread_mutex_lock(ags_acceleration_get_class_mutex());
  
  acceleration_mutex = current_acceleration->obj_mutex;
  
  pthread_mutex_unlock(ags_acceleration_get_class_mutex());

  /* test */
  pthread_mutex_lock(acceleration_mutex);

  retval = (flags & (acceleration->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(acceleration_mutex);

  return(retval);
}

/**
 * ags_acceleration_set_flags:
 * @acceleration: the #AgsAcceleration
 * @flags: the flags
 * 
 * Set @flags on @acceleration.
 * 
 * Since: 2.0.0
 */
void
ags_acceleration_set_flags(AgsAcceleration *acceleration, guint flags)
{
  pthread_mutex_t *acceleration_mutex;

  if(!AGS_IS_ACCELERATION(acceleration)){
    return;
  }
      
  /* get acceleration mutex */
  pthread_mutex_lock(ags_acceleration_get_class_mutex());
  
  acceleration_mutex = current_acceleration->obj_mutex;
  
  pthread_mutex_unlock(ags_acceleration_get_class_mutex());

  /* set */
  pthread_mutex_lock(acceleration_mutex);

  acceleration->flags |= flags;
  
  pthread_mutex_unlock(acceleration_mutex);
}

/**
 * ags_acceleration_unset_flags:
 * @acceleration: the #AgsAcceleration
 * @flags: the flags
 * 
 * Unset @flags on @acceleration.
 * 
 * Since: 2.0.0
 */
void
ags_acceleration_unset_flags(AgsAcceleration *acceleration, guint flags)
{
  pthread_mutex_t *acceleration_mutex;

  if(!AGS_IS_ACCELERATION(acceleration)){
    return;
  }
      
  /* get acceleration mutex */
  pthread_mutex_lock(ags_acceleration_get_class_mutex());
  
  acceleration_mutex = current_acceleration->obj_mutex;
  
  pthread_mutex_unlock(ags_acceleration_get_class_mutex());

  /* unset */
  pthread_mutex_lock(acceleration_mutex);

  acceleration->flags &= (~flags);
  
  pthread_mutex_unlock(acceleration_mutex);
}

/**
 * ags_acceleration_duplicate:
 * @acceleration: an #AgsAcceleration
 * 
 * Duplicate a acceleration.
 *
 * Returns: the duplicated #AgsAcceleration.
 *
 * Since: 2.0.0
 */
AgsAcceleration*
ags_acceleration_duplicate(AgsAcceleration *acceleration)
{
  AgsAcceleration *acceleration_copy;

  pthread_mutex_t *acceleration_mutex;

  if(!AGS_IS_ACCELERATION(acceleration)){
    return(NULL);
  }
  
  /* get acceleration mutex */
  pthread_mutex_lock(ags_acceleration_get_class_mutex());
  
  acceleration_mutex = acceleration->obj_mutex;
  
  pthread_mutex_unlock(ags_acceleration_get_class_mutex());

  /**/
  acceleration_copy = ags_acceleration_new();

  acceleration_copy->flags = 0;

  pthread_mutex_lock(acceleration_mutex);
  
  acceleration_copy->x = acceleration->x;
  acceleration_copy->y = acceleration->y;

  acceleration_copy->acceleration_name = g_strdup(acceleration->acceleration_name);

  pthread_mutex_unlock(acceleration_mutex);
  
  return(acceleration_copy);
}

/**
 * ags_acceleration_new:
 *
 * Creates an #AgsAcceleration
 *
 * Returns: a new #AgsAcceleration
 *
 * Since: 2.0.0
 */
AgsAcceleration*
ags_acceleration_new()
{
  AgsAcceleration *acceleration;

  acceleration = (AgsAcceleration *) g_object_new(AGS_TYPE_ACCELERATION,
						  NULL);

  return(acceleration);
}
