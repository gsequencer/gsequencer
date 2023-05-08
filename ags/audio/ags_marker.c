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

#include <ags/audio/ags_marker.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_marker_class_init(AgsMarkerClass *marker);
void ags_marker_init(AgsMarker *marker);
void ags_marker_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_marker_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_marker_finalize(GObject *gobject);

/**
 * SECTION:ags_marker
 * @short_description: Marker class
 * @title: AgsMarker
 * @section_id:
 * @include: ags/audio/ags_marker.h
 *
 * #AgsMarker represents an automated value of a port. You specify y the value written to a port
 * by offset x.
 */

enum{
  PROP_0,
  PROP_X,
  PROP_Y,
  PROP_MARKER_NAME,
};

static gpointer ags_marker_parent_class = NULL;

GType
ags_marker_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_marker = 0;

    static const GTypeInfo ags_marker_info = {
      sizeof(AgsMarkerClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_marker_class_init,
      NULL,
      NULL,
      sizeof(AgsMarker),
      0,
      (GInstanceInitFunc) ags_marker_init,
    };

    ags_type_marker = g_type_register_static(G_TYPE_OBJECT,
					     "AgsMarker",
					     &ags_marker_info,
					     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_marker);
  }

  return g_define_type_id__volatile;
}

GType
ags_marker_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_MARKER_DEFAULT_START, "AGS_MARKER_DEFAULT_START", "marker-default-start" },
      { AGS_MARKER_DEFAULT_END, "AGS_MARKER_DEFAULT_END", "marker-default-end" },
      { AGS_MARKER_GUI, "AGS_MARKER_GUI", "marker-gui" },
      { AGS_MARKER_RUNTIME, "AGS_MARKER_RUNTIME", "marker-runtime" },
      { AGS_MARKER_HUMAN_READABLE, "AGS_MARKER_HUMAN_READABLE", "marker-human-readable" },
      { AGS_MARKER_DEFAULT_LENGTH, "AGS_MARKER_DEFAULT_LENGTH", "marker-default-length" },
      { AGS_MARKER_IS_SELECTED, "AGS_MARKER_IS_SELECTED", "marker-is-selected" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsMarkerFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void 
ags_marker_class_init(AgsMarkerClass *marker)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_marker_parent_class = g_type_class_peek_parent(marker);

  gobject = (GObjectClass *) marker;

  gobject->set_property = ags_marker_set_property;
  gobject->get_property = ags_marker_get_property;

  gobject->finalize = ags_marker_finalize;

  /* properties */
  /**
   * AgsMarker:x:
   *
   * Marker offset x.
   * 
   * Since: 5.1.0
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
   * AgsMarker:y:
   *
   * Marker value y.
   * 
   * Since: 5.1.0
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
   * AgsMarker:marker-name:
   *
   * The marker's name.
   * 
   * Since: 5.1.0
   */
  param_spec = g_param_spec_string("marker-name",
				   i18n_pspec("marker name"),
				   i18n_pspec("The marker's name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARKER_NAME,
				  param_spec);
}

void
ags_marker_init(AgsMarker *marker)
{
  marker->flags = 0;

  /* marker mutex */
  g_rec_mutex_init(&(marker->obj_mutex));
  
  /* fields */
  marker->x = 0;
  marker->y = 120.0;

  marker->marker_name = NULL;
}


void
ags_marker_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsMarker *marker;

  GRecMutex *marker_mutex;

  marker = AGS_MARKER(gobject);

  /* get marker mutex */
  marker_mutex = AGS_MARKER_GET_OBJ_MUTEX(marker);

  switch(prop_id){
  case PROP_X:
    {
      g_rec_mutex_lock(marker_mutex);

      marker->x = g_value_get_uint(value);

      g_rec_mutex_unlock(marker_mutex);
    }
    break;
  case PROP_Y:
    {
      g_rec_mutex_lock(marker_mutex);

      marker->y = g_value_get_double(value);

      g_rec_mutex_unlock(marker_mutex);
    }
    break;
  case PROP_MARKER_NAME:
    {
      gchar *marker_name;
      
      marker_name = g_value_get_string(value);
      
      g_rec_mutex_lock(marker_mutex);

      if(marker_name == marker->marker_name){
	g_rec_mutex_unlock(marker_mutex);

	return;
      }

      if(marker->marker_name != NULL){
	g_free(marker->marker_name);
      }

      marker->marker_name = g_strdup(marker_name);

      g_rec_mutex_unlock(marker_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_marker_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsMarker *marker;

  GRecMutex *marker_mutex;

  marker = AGS_MARKER(gobject);

  /* get marker mutex */
  marker_mutex = AGS_MARKER_GET_OBJ_MUTEX(marker);

  switch(prop_id){
  case PROP_X:
    {
      g_rec_mutex_lock(marker_mutex);

      g_value_set_uint(value, marker->x);

      g_rec_mutex_unlock(marker_mutex);
    }
    break;
  case PROP_Y:
    {
      g_rec_mutex_lock(marker_mutex);

      g_value_set_double(value, marker->y);

      g_rec_mutex_unlock(marker_mutex);
    }
    break;
  case PROP_MARKER_NAME:
    {
      g_rec_mutex_lock(marker_mutex);

      g_value_set_string(value, marker->marker_name);

      g_rec_mutex_unlock(marker_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_marker_finalize(GObject *gobject)
{
  AgsMarker *marker;

  marker = AGS_MARKER(gobject);
  
  if(marker->marker_name != NULL){
    free(marker->marker_name);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_marker_parent_class)->finalize(gobject);
}

/**
 * ags_marker_get_obj_mutex:
 * @marker: the #AgsMarker
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @marker
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_marker_get_obj_mutex(AgsMarker *marker)
{
  if(!AGS_IS_MARKER(marker)){
    return(NULL);
  }

  return(AGS_MARKER_GET_OBJ_MUTEX(marker));
}

/**
 * ags_marker_test_flags:
 * @marker: the #AgsMarker
 * @flags: the flags
 * 
 * Test @flags to be set on @marker.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 5.1.0
 */
gboolean
ags_marker_test_flags(AgsMarker *marker, AgsMarkerFlags flags)
{
  gboolean retval;
  
  GRecMutex *marker_mutex;

  if(!AGS_IS_MARKER(marker)){
    return(FALSE);
  }
      
  /* get marker mutex */
  marker_mutex = AGS_MARKER_GET_OBJ_MUTEX(marker);

  /* test */
  g_rec_mutex_lock(marker_mutex);

  retval = (flags & (marker->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(marker_mutex);

  return(retval);
}

/**
 * ags_marker_set_flags:
 * @marker: the #AgsMarker
 * @flags: the flags
 * 
 * Set @flags on @marker.
 * 
 * Since: 5.1.0
 */
void
ags_marker_set_flags(AgsMarker *marker, AgsMarkerFlags flags)
{
  GRecMutex *marker_mutex;

  if(!AGS_IS_MARKER(marker)){
    return;
  }
      
  /* get marker mutex */
  marker_mutex = AGS_MARKER_GET_OBJ_MUTEX(marker);

  /* set */
  g_rec_mutex_lock(marker_mutex);

  marker->flags |= flags;
  
  g_rec_mutex_unlock(marker_mutex);
}

/**
 * ags_marker_unset_flags:
 * @marker: the #AgsMarker
 * @flags: the flags
 * 
 * Unset @flags on @marker.
 * 
 * Since: 5.1.0
 */
void
ags_marker_unset_flags(AgsMarker *marker, AgsMarkerFlags flags)
{
  GRecMutex *marker_mutex;

  if(!AGS_IS_MARKER(marker)){
    return;
  }
      
  /* get marker mutex */
  marker_mutex = AGS_MARKER_GET_OBJ_MUTEX(marker);

  /* unset */
  g_rec_mutex_lock(marker_mutex);

  marker->flags &= (~flags);
  
  g_rec_mutex_unlock(marker_mutex);
}

/**
 * ags_marker_sort_func:
 * @a: an #AgsMarker
 * @b: an other #AgsMarker
 * 
 * Sort markers.
 * 
 * Returns: 0 if equal, -1 if smaller and 1 if bigger offset
 *
 * Since: 5.1.0
 */
gint
ags_marker_sort_func(gconstpointer a, gconstpointer b)
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
 * ags_marker_get_x:
 * @marker: the #AgsMarker
 *
 * Gets x.
 * 
 * Returns: the x
 * 
 * Since: 3.1.0
 */
guint
ags_marker_get_x(AgsMarker *marker)
{
  guint x;
  
  if(!AGS_IS_MARKER(marker)){
    return(0);
  }

  g_object_get(marker,
	       "x", &x,
	       NULL);

  return(x);
}

/**
 * ags_marker_set_x:
 * @marker: the #AgsMarker
 * @x: the x
 *
 * Sets x.
 * 
 * Since: 3.1.0
 */
void
ags_marker_set_x(AgsMarker *marker, guint x)
{
  if(!AGS_IS_MARKER(marker)){
    return;
  }

  g_object_set(marker,
	       "x", x,
	       NULL);
}

/**
 * ags_marker_get_y:
 * @marker: the #AgsMarker
 *
 * Gets y.
 * 
 * Returns: the y
 * 
 * Since: 3.1.0
 */
gdouble
ags_marker_get_y(AgsMarker *marker)
{
  gdouble y;
  
  if(!AGS_IS_MARKER(marker)){
    return(0);
  }

  g_object_get(marker,
	       "y", &y,
	       NULL);

  return(y);
}

/**
 * ags_marker_set_y:
 * @marker: the #AgsMarker
 * @y: the y
 *
 * Sets y.
 * 
 * Since: 3.1.0
 */
void
ags_marker_set_y(AgsMarker *marker, gdouble y)
{
  if(!AGS_IS_MARKER(marker)){
    return;
  }

  g_object_set(marker,
	       "y", y,
	       NULL);
}

/**
 * ags_marker_duplicate:
 * @marker: an #AgsMarker
 * 
 * Duplicate a marker.
 *
 * Returns: (transfer full): the duplicated #AgsMarker.
 *
 * Since: 5.1.0
 */
AgsMarker*
ags_marker_duplicate(AgsMarker *marker)
{
  AgsMarker *marker_copy;

  GRecMutex *marker_mutex;

  if(!AGS_IS_MARKER(marker)){
    return(NULL);
  }
  
  /* get marker mutex */
  marker_mutex = AGS_MARKER_GET_OBJ_MUTEX(marker);

  /* instantiate marker */
  marker_copy = ags_marker_new();

  marker_copy->flags = 0;

  g_rec_mutex_lock(marker_mutex);
  
  marker_copy->x = marker->x;
  marker_copy->y = marker->y;

  marker_copy->marker_name = g_strdup(marker->marker_name);

  g_rec_mutex_unlock(marker_mutex);
  
  return(marker_copy);
}

/**
 * ags_marker_new:
 *
 * Creates a new instance of #AgsMarker
 *
 * Returns: the new #AgsMarker
 *
 * Since: 5.1.0
 */
AgsMarker*
ags_marker_new()
{
  AgsMarker *marker;

  marker = (AgsMarker *) g_object_new(AGS_TYPE_MARKER,
				      NULL);

  return(marker);
}
