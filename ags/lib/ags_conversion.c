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

/**
 * SECTION:ags_conversion
 * @short_description: Conversion of values
 * @title: AgsConversion
 * @section_id:
 * @include: ags/lib/ags_conversion.h
 *
 * The #AgsConversion converts values.
 */

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

GType
ags_conversion_get_type(void)
{
  static GType ags_type_conversion = 0;

  if(!ags_type_conversion){
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
						 "AgsConversion\0",
						 &ags_conversion_info,
						 0);
  }

  return (ags_type_conversion);
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
   * Since: 0.7.8
   */
  param_spec = g_param_spec_string("name\0",
				   "name of conversion\0",
				   "The name of the conversion\0",
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
   * Since: 0.7.8
   */
  param_spec = g_param_spec_string("description\0",
				   "description of conversion\0",
				   "The description of the conversion\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESCRIPTION,
				  param_spec);


  /* AgsConversionClass */
  conversion->convert = NULL;
  
  /* signals */
  /**
   * AgsConversion::convert:
   * @conversion: the #AgsConversion to modify
   * @value: the value to convert
   *
   * The ::convert signal notifies about converted value.
   */
  conversion_signals[CONVERT] =
    g_signal_new("convert\0",
		 G_TYPE_FROM_CLASS(conversion),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsConversionClass, convert),
		 NULL, NULL,
		 g_cclosure_user_marshal_DOUBLE__DOUBLE,
		 G_TYPE_DOUBLE, 1,
		 G_TYPE_DOUBLE);
}

void
ags_conversion_init(AgsConversion *conversion)
{
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

  conversion = AGS_CONVERSION(gobject);

  switch(prop_id){
  case PROP_NAME:
    {
      gchar *name;

      name = (gchar *) g_value_get_string(value);

      if(conversion->name == name){
	return;
      }
      
      if(conversion->name != NULL){
	g_free(conversion->name);
      }

      conversion->name = g_strdup(name);
    }
    break;
  case PROP_DESCRIPTION:
    {
      gchar *description;

      description = (gchar *) g_value_get_string(value);

      if(conversion->description == description){
	return;
      }
      
      if(conversion->description != NULL){
	g_free(conversion->description);
      }

      conversion->description = g_strdup(description);
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

  conversion = AGS_CONVERSION(gobject);

  switch(prop_id){
  case PROP_NAME:
    g_value_set_string(value, conversion->name);
    break;
  case PROP_DESCRIPTION:
    g_value_set_string(value, conversion->description);
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
}

gdouble
ags_conversion_convert(AgsConversion *conversion,
		       gdouble value)
{
  gdouble retval;
  
  g_return_if_fail(AGS_IS_CONVERSION(conversion));

  g_object_ref((GObject *) conversion);
  g_signal_emit(G_OBJECT(conversion),
		conversion_signals[CONVERT], 0,
		value,
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
 * Since: 0.7.2
 */
AgsConversion*
ags_conversion_new()
{
  AgsConversion *conversion;
  
  conversion = g_object_new(AGS_TYPE_CONVERSION,
			    NULL);

  return(conversion);
}
