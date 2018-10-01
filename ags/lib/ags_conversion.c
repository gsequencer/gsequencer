/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2016 Joël Krähemann
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

#include <ags/lib/ags_conversion.h>

#include <ags/object/ags_marshal.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_conversion_class_init(AgsConversionClass *conversion);
void ags_conversion_init (AgsConversion *conversion);
void ags_conversion_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_conversion_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_conversion_finalize(GObject *gobject);

gdouble ags_conversion_real_convert(AgsConversion *conversion,
				    gdouble value,
				    gboolean reverse);

/**
 * SECTION:ags_conversion
 * @short_description: abstract conversion
 * @title: AgsConversion
 * @section_id:
 * @include: ags/lib/ags_conversion.h
 *
 * AgsConversion does a zero conversion it returns
 * during ::convert() the very same value.
 */

enum{
  CONVERT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_NAME,
  PROP_DESCRIPTION,
};

static gpointer ags_conversion_parent_class = NULL;
static guint conversion_signals[LAST_SIGNAL];

static pthread_mutex_t ags_conversion_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_conversion_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_conversion = 0;

    static const GTypeInfo ags_conversion_info = {
      sizeof (AgsConversionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_conversion_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConversion),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_conversion_init,
    };

    ags_type_conversion = g_type_register_static(G_TYPE_OBJECT,
						 "AgsConversion",
						 &ags_conversion_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_conversion);
  }

  return g_define_type_id__volatile;
}

void
ags_conversion_class_init(AgsConversionClass *conversion)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_conversion_parent_class = g_type_class_peek_parent(conversion);

  /* GObjectClass */
  gobject = (GObjectClass *) conversion;
  
  gobject->set_property = ags_conversion_set_property;
  gobject->get_property = ags_conversion_get_property;

  gobject->finalize = ags_conversion_finalize;
  
  /* properties */
  /**
   * AgsConversion:name:
   *
   * The name of the conversion.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("name",
				   i18n_pspec("name of conversion"),
				   i18n_pspec("The name of the conversion"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NAME,
				  param_spec);

  /**
   * AgsConversion:description:
   *
   * The description of the conversion.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("description",
				   i18n_pspec("description of conversion"),
				   i18n_pspec("The description of the conversion"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESCRIPTION,
				  param_spec);


  /* AgsConversionClass */
  conversion->convert = ags_conversion_real_convert;
  
  /* signals */
  /**
   * AgsConversion::convert:
   * @conversion: the #AgsConversion to modify
   * @value: the value to convert
   * @reverse: the direction to translate
   *
   * The ::convert signal notifies about converted value.
   *
   * Returns: the converted value
   * 
   * Since: 2.0.0
   */
  conversion_signals[CONVERT] =
    g_signal_new("convert",
		 G_TYPE_FROM_CLASS(conversion),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsConversionClass, convert),
		 NULL, NULL,
		 ags_cclosure_marshal_DOUBLE__DOUBLE_BOOLEAN,
		 G_TYPE_DOUBLE, 2,
		 G_TYPE_DOUBLE,
		 G_TYPE_BOOLEAN);
}

void
ags_conversion_init(AgsConversion *conversion)
{
  conversion->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(conversion->obj_mutexattr);
  pthread_mutexattr_settype(conversion->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(conversion->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  
  conversion->obj_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(conversion->obj_mutex, conversion->obj_mutexattr);

  conversion->name = NULL;
  conversion->description = NULL;
}

void
ags_conversion_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsConversion *conversion;

  pthread_mutex_t *conversion_mutex;
  
  conversion = AGS_CONVERSION(gobject);

  /* get conversion mutex */
  pthread_mutex_lock(ags_conversion_get_class_mutex());
  
  conversion_mutex = conversion->obj_mutex;

  pthread_mutex_unlock(ags_conversion_get_class_mutex());
  
  switch(prop_id){
  case PROP_NAME:
    {
      gchar *name;

      name = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(conversion_mutex);
      
      if(conversion->name == name){
	pthread_mutex_unlock(conversion_mutex);
	
	return;
      }
      
      if(conversion->name != NULL){
	g_free(conversion->name);
      }

      conversion->name = g_strdup(name);

      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  case PROP_DESCRIPTION:
    {
      gchar *description;

      description = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(conversion_mutex);

      if(conversion->description == description){
	pthread_mutex_unlock(conversion_mutex);

	return;
      }
      
      if(conversion->description != NULL){
	g_free(conversion->description);
      }

      conversion->description = g_strdup(description);

      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_conversion_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsConversion *conversion;

  pthread_mutex_t *conversion_mutex;
  
  conversion = AGS_CONVERSION(gobject);

  /* get conversion mutex */
  pthread_mutex_lock(ags_conversion_get_class_mutex());
  
  conversion_mutex = conversion->obj_mutex;

  pthread_mutex_unlock(ags_conversion_get_class_mutex());

  switch(prop_id){
  case PROP_NAME:
    {
      pthread_mutex_lock(conversion_mutex);

      g_value_set_string(value, conversion->name);

      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  case PROP_DESCRIPTION:
    {
      pthread_mutex_lock(conversion_mutex);

      g_value_set_string(value, conversion->description);

      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_conversion_finalize(GObject *gobject)
{
  AgsConversion *conversion;
  
  conversion = AGS_CONVERSION(gobject);

  if(conversion->name != NULL){
    g_free(conversion->name);
  }

  if(conversion->description != NULL){
    g_free(conversion->description);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_conversion_parent_class)->finalize(gobject);
}

/**
 * ags_conversion_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_conversion_get_class_mutex()
{
  return(&ags_conversion_class_mutex);
}

gdouble
ags_conversion_real_convert(AgsConversion *conversion,
			    gdouble value,
			    gboolean reverse)
{
  return(value);
}

/**
 * ags_conversion_convert:
 * @conversion: the #AgsConversion
 * @value: the value to convert
 * @reverse: the direction to convert
 *
 * Convert a value if @reverse is %FALSE then use the target format
 * otherwise if %TRUE convert back to original format.
 *
 * Returns: the converted value as gdouble
 *
 * Since: 2.0.0
 */
gdouble
ags_conversion_convert(AgsConversion *conversion,
		       gdouble value,
		       gboolean reverse)
{
  gdouble retval;
  
  g_return_val_if_fail(AGS_IS_CONVERSION(conversion),
		       value);

  g_object_ref((GObject *) conversion);
  g_signal_emit(G_OBJECT(conversion),
		conversion_signals[CONVERT], 0,
		value,
		reverse,
		&retval);
  g_object_unref((GObject *) conversion);

  return(retval);
}

/**
 * ags_conversion_new:
 *
 * Instantiate a new #AgsConversion.
 *
 * Returns: the new instance
 *
 * Since: 2.0.0
 */
AgsConversion*
ags_conversion_new()
{
  AgsConversion *conversion;
  
  conversion = g_object_new(AGS_TYPE_CONVERSION,
			    NULL);

  return(conversion);
}
