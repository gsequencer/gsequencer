/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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
 * #AgsAcceleration represents an automated value of a port. You specify y the value written to a port
 * by offset x.
 */

enum{
  PROP_0,
  PROP_X,
  PROP_Y,
  PROP_ACCELERATION_NAME,
};

static gpointer ags_acceleration_parent_class = NULL;

GType
ags_acceleration_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_acceleration = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_acceleration);
  }

  return g_define_type_id__volatile;
}

GType
ags_acceleration_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_ACCELERATION_DEFAULT_START, "AGS_ACCELERATION_DEFAULT_START", "acceleration-default-start" },
      { AGS_ACCELERATION_DEFAULT_END, "AGS_ACCELERATION_DEFAULT_END", "acceleration-default-end" },
      { AGS_ACCELERATION_GUI, "AGS_ACCELERATION_GUI", "acceleration-gui" },
      { AGS_ACCELERATION_RUNTIME, "AGS_ACCELERATION_RUNTIME", "acceleration-runtime" },
      { AGS_ACCELERATION_HUMAN_READABLE, "AGS_ACCELERATION_HUMAN_READABLE", "acceleration-human-readable" },
      { AGS_ACCELERATION_DEFAULT_LENGTH, "AGS_ACCELERATION_DEFAULT_LENGTH", "acceleration-default-length" },
      { AGS_ACCELERATION_IS_SELECTED, "AGS_ACCELERATION_IS_SELECTED", "acceleration-is-selected" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsAccelerationFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
  acceleration->flags = 0;

  /* acceleration mutex */
  g_rec_mutex_init(&(acceleration->obj_mutex));
  
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

  GRecMutex *acceleration_mutex;

  acceleration = AGS_ACCELERATION(gobject);

  /* get acceleration mutex */
  acceleration_mutex = AGS_ACCELERATION_GET_OBJ_MUTEX(acceleration);

  switch(prop_id){
  case PROP_X:
    {
      g_rec_mutex_lock(acceleration_mutex);

      acceleration->x = g_value_get_uint(value);

      g_rec_mutex_unlock(acceleration_mutex);
    }
    break;
  case PROP_Y:
    {
      g_rec_mutex_lock(acceleration_mutex);

      acceleration->y = g_value_get_double(value);

      g_rec_mutex_unlock(acceleration_mutex);
    }
    break;
  case PROP_ACCELERATION_NAME:
    {
      gchar *acceleration_name;
      
      acceleration_name = g_value_get_string(value);
      
      g_rec_mutex_lock(acceleration_mutex);

      if(acceleration_name == acceleration->acceleration_name){
	g_rec_mutex_unlock(acceleration_mutex);

	return;
      }

      if(acceleration->acceleration_name != NULL){
	g_free(acceleration->acceleration_name);
      }

      acceleration->acceleration_name = g_strdup(acceleration_name);

      g_rec_mutex_unlock(acceleration_mutex);
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

  GRecMutex *acceleration_mutex;

  acceleration = AGS_ACCELERATION(gobject);

  /* get acceleration mutex */
  acceleration_mutex = AGS_ACCELERATION_GET_OBJ_MUTEX(acceleration);

  switch(prop_id){
  case PROP_X:
    {
      g_rec_mutex_lock(acceleration_mutex);

      g_value_set_uint(value, acceleration->x);

      g_rec_mutex_unlock(acceleration_mutex);
    }
    break;
  case PROP_Y:
    {
      g_rec_mutex_lock(acceleration_mutex);

      g_value_set_double(value, acceleration->y);

      g_rec_mutex_unlock(acceleration_mutex);
    }
    break;
  case PROP_ACCELERATION_NAME:
    {
      g_rec_mutex_lock(acceleration_mutex);

      g_value_set_string(value, acceleration->acceleration_name);

      g_rec_mutex_unlock(acceleration_mutex);
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
  
  if(acceleration->acceleration_name != NULL){
    free(acceleration->acceleration_name);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_acceleration_parent_class)->finalize(gobject);
}

/**
 * ags_acceleration_get_obj_mutex:
 * @acceleration: the #AgsAcceleration
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @acceleration
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_acceleration_get_obj_mutex(AgsAcceleration *acceleration)
{
  if(!AGS_IS_ACCELERATION(acceleration)){
    return(NULL);
  }

  return(AGS_ACCELERATION_GET_OBJ_MUTEX(acceleration));
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
 * Since: 3.0.0
 */
gboolean
ags_acceleration_test_flags(AgsAcceleration *acceleration, AgsAccelerationFlags flags)
{
  gboolean retval;
  
  GRecMutex *acceleration_mutex;

  if(!AGS_IS_ACCELERATION(acceleration)){
    return(FALSE);
  }
      
  /* get acceleration mutex */
  acceleration_mutex = AGS_ACCELERATION_GET_OBJ_MUTEX(acceleration);

  /* test */
  g_rec_mutex_lock(acceleration_mutex);

  retval = (flags & (acceleration->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(acceleration_mutex);

  return(retval);
}

/**
 * ags_acceleration_set_flags:
 * @acceleration: the #AgsAcceleration
 * @flags: the flags
 * 
 * Set @flags on @acceleration.
 * 
 * Since: 3.0.0
 */
void
ags_acceleration_set_flags(AgsAcceleration *acceleration, AgsAccelerationFlags flags)
{
  GRecMutex *acceleration_mutex;

  if(!AGS_IS_ACCELERATION(acceleration)){
    return;
  }
      
  /* get acceleration mutex */
  acceleration_mutex = AGS_ACCELERATION_GET_OBJ_MUTEX(acceleration);

  /* set */
  g_rec_mutex_lock(acceleration_mutex);

  acceleration->flags |= flags;
  
  g_rec_mutex_unlock(acceleration_mutex);
}

/**
 * ags_acceleration_unset_flags:
 * @acceleration: the #AgsAcceleration
 * @flags: the flags
 * 
 * Unset @flags on @acceleration.
 * 
 * Since: 3.0.0
 */
void
ags_acceleration_unset_flags(AgsAcceleration *acceleration, AgsAccelerationFlags flags)
{
  GRecMutex *acceleration_mutex;

  if(!AGS_IS_ACCELERATION(acceleration)){
    return;
  }
      
  /* get acceleration mutex */
  acceleration_mutex = AGS_ACCELERATION_GET_OBJ_MUTEX(acceleration);

  /* unset */
  g_rec_mutex_lock(acceleration_mutex);

  acceleration->flags &= (~flags);
  
  g_rec_mutex_unlock(acceleration_mutex);
}

/**
 * ags_acceleration_sort_func:
 * @a: an #AgsAcceleration
 * @b: an other #AgsAcceleration
 * 
 * Sort accelerations.
 * 
 * Returns: 0 if equal, -1 if smaller and 1 if bigger offset
 *
 * Since: 3.0.0
 */
gint
ags_acceleration_sort_func(gconstpointer a, gconstpointer b)
{
  guint a_x, b_x;

  if(a == NULL || b == NULL){
    return(0);
  }

  g_object_get(a,
	       "x", &a_x,
	       NULL);
    
  g_object_get(b,
	       "x", &b_x,
	       NULL);
    
  if(a_x == b_x){
    return(0);
  }

  if(a_x < b_x){
    return(-1);
  }else{
    return(1);
  }
}

/**
 * ags_acceleration_get_x:
 * @acceleration: the #AgsAcceleration
 *
 * Gets x.
 * 
 * Returns: the x
 * 
 * Since: 3.1.0
 */
guint
ags_acceleration_get_x(AgsAcceleration *acceleration)
{
  guint x;
  
  if(!AGS_IS_ACCELERATION(acceleration)){
    return(0);
  }

  g_object_get(acceleration,
	       "x", &x,
	       NULL);

  return(x);
}

/**
 * ags_acceleration_set_x:
 * @acceleration: the #AgsAcceleration
 * @x: the x
 *
 * Sets x.
 * 
 * Since: 3.1.0
 */
void
ags_acceleration_set_x(AgsAcceleration *acceleration, guint x)
{
  if(!AGS_IS_ACCELERATION(acceleration)){
    return;
  }

  g_object_set(acceleration,
	       "x", x,
	       NULL);
}

/**
 * ags_acceleration_get_y:
 * @acceleration: the #AgsAcceleration
 *
 * Gets y.
 * 
 * Returns: the y
 * 
 * Since: 3.1.0
 */
gdouble
ags_acceleration_get_y(AgsAcceleration *acceleration)
{
  gdouble y;
  
  if(!AGS_IS_ACCELERATION(acceleration)){
    return(0);
  }

  g_object_get(acceleration,
	       "y", &y,
	       NULL);

  return(y);
}

/**
 * ags_acceleration_set_y:
 * @acceleration: the #AgsAcceleration
 * @y: the y
 *
 * Sets y.
 * 
 * Since: 3.1.0
 */
void
ags_acceleration_set_y(AgsAcceleration *acceleration, gdouble y)
{
  if(!AGS_IS_ACCELERATION(acceleration)){
    return;
  }

  g_object_set(acceleration,
	       "y", y,
	       NULL);
}

/**
 * ags_acceleration_duplicate:
 * @acceleration: an #AgsAcceleration
 * 
 * Duplicate a acceleration.
 *
 * Returns: (transfer full): the duplicated #AgsAcceleration.
 *
 * Since: 3.0.0
 */
AgsAcceleration*
ags_acceleration_duplicate(AgsAcceleration *acceleration)
{
  AgsAcceleration *acceleration_copy;

  GRecMutex *acceleration_mutex;

  if(!AGS_IS_ACCELERATION(acceleration)){
    return(NULL);
  }
  
  /* get acceleration mutex */
  acceleration_mutex = AGS_ACCELERATION_GET_OBJ_MUTEX(acceleration);

  /* instantiate acceleration */
  acceleration_copy = ags_acceleration_new();

  acceleration_copy->flags = 0;

  g_rec_mutex_lock(acceleration_mutex);
  
  acceleration_copy->x = acceleration->x;
  acceleration_copy->y = acceleration->y;

  acceleration_copy->acceleration_name = g_strdup(acceleration->acceleration_name);

  g_rec_mutex_unlock(acceleration_mutex);
  
  return(acceleration_copy);
}

/**
 * ags_acceleration_new:
 *
 * Creates a new instance of #AgsAcceleration
 *
 * Returns: the new #AgsAcceleration
 *
 * Since: 3.0.0
 */
AgsAcceleration*
ags_acceleration_new()
{
  AgsAcceleration *acceleration;

  acceleration = (AgsAcceleration *) g_object_new(AGS_TYPE_ACCELERATION,
						  NULL);

  return(acceleration);
}
