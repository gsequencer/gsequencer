/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/plugin/ags_vst3_conversion.h>

#include <math.h>

#include <ags/i18n.h>

void ags_vst3_conversion_class_init(AgsVst3ConversionClass *conversion);
void ags_vst3_conversion_init (AgsVst3Conversion *conversion);
void ags_vst3_conversion_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_vst3_conversion_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_vst3_conversion_finalize(GObject *gobject);

gdouble ags_vst3_conversion_convert(AgsConversion *conversion,
				    gdouble value,
				    gboolean reverse);

/**
 * SECTION:ags_vst3_conversion
 * @short_description: Conversion of values
 * @title: AgsVst3Conversion
 * @section_id:
 * @include: ags/plugin/ags_vst3_conversion.h
 *
 * The #AgsVst3Conversion converts values.
 */

enum{
  PROP_0,
  PROP_LOWER,
  PROP_UPPER,
  PROP_STEP_COUNT,
};

static gpointer ags_vst3_conversion_parent_class = NULL;

GType
ags_vst3_conversion_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vst3_conversion = 0;

    static const GTypeInfo ags_vst3_conversion_info = {
      sizeof (AgsVst3ConversionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vst3_conversion_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVst3Conversion),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vst3_conversion_init,
    };

    ags_type_vst3_conversion = g_type_register_static(AGS_TYPE_CONVERSION,
						      "AgsVst3Conversion",
						      &ags_vst3_conversion_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vst3_conversion);
  }

  return g_define_type_id__volatile;
}

void
ags_vst3_conversion_class_init(AgsVst3ConversionClass *vst3_conversion)
{
  AgsConversionClass *conversion;
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_vst3_conversion_parent_class = g_type_class_peek_parent(vst3_conversion);

  /* GObjectClass */
  gobject = (GObjectClass *) vst3_conversion;
  
  gobject->set_property = ags_vst3_conversion_set_property;
  gobject->get_property = ags_vst3_conversion_get_property;

  gobject->finalize = ags_vst3_conversion_finalize;

  /* properties */
  /**
   * AgsVst3Conversion:lower:
   *
   * The lower to be used.
   * 
   * Since: 3.10.5
   */
  param_spec = g_param_spec_double("lower",
				   i18n_pspec("using lower"),
				   i18n_pspec("The lower to be used"),
				   -1.0 * G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_VST3_CONVERSION_DEFAULT_LOWER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOWER,
				  param_spec);

  /**
   * AgsVst3Conversion:upper:
   *
   * The upper to be used.
   * 
   * Since: 3.10.5
   */
  param_spec = g_param_spec_double("upper",
				   i18n_pspec("using upper"),
				   i18n_pspec("The upper to be used"),
				   -1.0 * G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_VST3_CONVERSION_DEFAULT_UPPER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UPPER,
				  param_spec);

  /**
   * AgsVst3Conversion:step-count:
   *
   * The step count to be used.
   * 
   * Since: 3.10.5
   */
  param_spec = g_param_spec_double("step-count",
				   i18n_pspec("using step count"),
				   i18n_pspec("The step count to be used"),
				   0.0,
				   G_MAXDOUBLE,
				   AGS_VST3_CONVERSION_DEFAULT_STEP_COUNT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STEP_COUNT,
				  param_spec);

  /* AgsConversionClass */
  conversion = (AgsConversionClass *) vst3_conversion;
    
  conversion->convert = ags_vst3_conversion_convert;
}

void
ags_vst3_conversion_init(AgsVst3Conversion *vst3_conversion)
{
  vst3_conversion->flags = 0;

  vst3_conversion->lower = AGS_VST3_CONVERSION_DEFAULT_LOWER;
  vst3_conversion->upper = AGS_VST3_CONVERSION_DEFAULT_UPPER;

  vst3_conversion->step_count = AGS_VST3_CONVERSION_DEFAULT_STEP_COUNT;
}

void
ags_vst3_conversion_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_vst3_conversion_parent_class)->finalize(gobject);
}


void
ags_vst3_conversion_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsVst3Conversion *vst3_conversion;

  GRecMutex *conversion_mutex;

  vst3_conversion = AGS_VST3_CONVERSION(gobject);

  /* get base plugin mutex */
  conversion_mutex = AGS_CONVERSION_GET_OBJ_MUTEX(vst3_conversion);

  switch(prop_id){
  case PROP_LOWER:
  {
    gdouble lower;

    lower = g_value_get_double(value);

    g_rec_mutex_lock(conversion_mutex);

    vst3_conversion->lower = lower;
      
    g_rec_mutex_unlock(conversion_mutex);
  }
  break;
  case PROP_UPPER:
  {
    gdouble upper;

    upper = g_value_get_double(value);

    g_rec_mutex_lock(conversion_mutex);

    vst3_conversion->upper = upper;
      
    g_rec_mutex_unlock(conversion_mutex);
  }
  break;
  case PROP_STEP_COUNT:
  {
    gdouble step_count;

    step_count = g_value_get_double(value);

    g_rec_mutex_lock(conversion_mutex);

    vst3_conversion->step_count = step_count;
      
    g_rec_mutex_unlock(conversion_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_vst3_conversion_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsVst3Conversion *vst3_conversion;

  GRecMutex *conversion_mutex;

  vst3_conversion = AGS_VST3_CONVERSION(gobject);

  /* get base plugin mutex */
  conversion_mutex = AGS_CONVERSION_GET_OBJ_MUTEX(vst3_conversion);

  switch(prop_id){
  case PROP_LOWER:
  {
    g_rec_mutex_lock(conversion_mutex);

    g_value_set_double(value, vst3_conversion->lower);

    g_rec_mutex_unlock(conversion_mutex);
  }
  break;
  case PROP_UPPER:
  {
    g_rec_mutex_lock(conversion_mutex);

    g_value_set_double(value, vst3_conversion->upper);

    g_rec_mutex_unlock(conversion_mutex);
  }
  break;
  case PROP_STEP_COUNT:
  {
    g_rec_mutex_lock(conversion_mutex);

    g_value_set_double(value, vst3_conversion->step_count);

    g_rec_mutex_unlock(conversion_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

gdouble
ags_vst3_conversion_convert(AgsConversion *conversion,
			    gdouble x,
			    gboolean reverse)
{
  AgsVst3Conversion *vst3_conversion;

  gdouble retval;

  vst3_conversion = AGS_VST3_CONVERSION(conversion);

  retval = x;
  
  if(reverse){
    //TODO:JK: implement me
  }else{
    //TODO:JK: implement me
  }
  
  return(retval);
}

/**
 * ags_vst3_conversion_new:
 *
 * Create a new instance of #AgsVst3Conversion.
 *
 * Returns: the new #AgsVst3Conversion
 *
 * Since: 3.10.5
 */
AgsVst3Conversion*
ags_vst3_conversion_new()
{
  AgsVst3Conversion *conversion;
  
  conversion = g_object_new(AGS_TYPE_VST3_CONVERSION,
			    NULL);

  return(conversion);
}
