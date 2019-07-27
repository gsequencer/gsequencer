/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/plugin/ags_ladspa_conversion.h>

#include <ags/libags.h>

#include <math.h>

#include <ags/i18n.h>

void ags_ladspa_conversion_class_init(AgsLadspaConversionClass *conversion);
void ags_ladspa_conversion_init (AgsLadspaConversion *conversion);
void ags_ladspa_conversion_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_ladspa_conversion_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_ladspa_conversion_finalize(GObject *gobject);

gdouble ags_ladspa_conversion_convert(AgsConversion *conversion,
				      gdouble value,
				      gboolean reverse);

/**
 * SECTION:ags_ladspa_conversion
 * @short_description: Conversion of values
 * @title: AgsLadspaConversion
 * @section_id:
 * @include: ags/plugin/ags_ladspa_conversion.h
 *
 * The #AgsLadspaConversion converts values.
 */

enum{
  PROP_0,
  PROP_SAMPLERATE,
  PROP_LOWER,
  PROP_UPPER,
  PROP_STEP_COUNT,
};

static gpointer ags_ladspa_conversion_parent_class = NULL;

GType
ags_ladspa_conversion_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ladspa_conversion = 0;

    static const GTypeInfo ags_ladspa_conversion_info = {
      sizeof (AgsLadspaConversionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ladspa_conversion_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLadspaConversion),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ladspa_conversion_init,
    };

    ags_type_ladspa_conversion = g_type_register_static(AGS_TYPE_CONVERSION,
							"AgsLadspaConversion",
							&ags_ladspa_conversion_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ladspa_conversion);
  }

  return g_define_type_id__volatile;
}

void
ags_ladspa_conversion_class_init(AgsLadspaConversionClass *ladspa_conversion)
{
  AgsConversionClass *conversion;
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_ladspa_conversion_parent_class = g_type_class_peek_parent(ladspa_conversion);

  /* GObjectClass */
  gobject = (GObjectClass *) ladspa_conversion;

  gobject->set_property = ags_ladspa_conversion_set_property;
  gobject->get_property = ags_ladspa_conversion_get_property;
  
  gobject->finalize = ags_ladspa_conversion_finalize;

  /* properties */
  /**
   * AgsLadspaConversion:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("using samplerate"),
				 i18n_pspec("The samplerate to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsLadspaConversion:lower:
   *
   * The lower to be used.
   * 
   * Since: 2.2.8
   */
  param_spec = g_param_spec_double("lower",
				   i18n_pspec("using lower"),
				   i18n_pspec("The lower to be used"),
				   -1.0 * G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOWER,
				  param_spec);

  /**
   * AgsLadspaConversion:upper:
   *
   * The upper to be used.
   * 
   * Since: 2.2.8
   */
  param_spec = g_param_spec_double("upper",
				   i18n_pspec("using upper"),
				   i18n_pspec("The upper to be used"),
				   -1.0 * G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UPPER,
				  param_spec);

  /**
   * AgsLadspaConversion:step-count:
   *
   * The step count to be used.
   * 
   * Since: 2.2.8
   */
  param_spec = g_param_spec_double("step-count",
				   i18n_pspec("using step count"),
				   i18n_pspec("The step count to be used"),
				   0.0,
				   G_MAXDOUBLE,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STEP_COUNT,
				  param_spec);
  
  /* AgsConversionClass */
  conversion = (AgsConversionClass *) ladspa_conversion;
    
  conversion->convert = ags_ladspa_conversion_convert;
}

void
ags_ladspa_conversion_init(AgsLadspaConversion *ladspa_conversion)
{
  AgsConfig *config;

  ladspa_conversion->flags = 0;

  config = ags_config_get_instance();
  
  ladspa_conversion->samplerate = ags_soundcard_helper_config_get_samplerate(config);

  ladspa_conversion->lower = AGS_LADSPA_CONVERSION_DEFAULT_LOWER;
  ladspa_conversion->upper = AGS_LADSPA_CONVERSION_DEFAULT_UPPER;

  ladspa_conversion->step_count = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT;
}

void
ags_ladspa_conversion_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsLadspaConversion *ladspa_conversion;

  pthread_mutex_t *conversion_mutex;

  ladspa_conversion = AGS_LADSPA_CONVERSION(gobject);

  /* get base plugin mutex */
  conversion_mutex = AGS_CONVERSION_GET_OBJ_MUTEX(ladspa_conversion);

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      uint samplerate;

      samplerate = g_value_get_uint(value);

      pthread_mutex_lock(conversion_mutex);

      ladspa_conversion->samplerate = samplerate;
      
      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  case PROP_LOWER:
    {
      gdouble lower;

      lower = g_value_get_double(value);

      pthread_mutex_lock(conversion_mutex);

      ladspa_conversion->lower = lower;
      
      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  case PROP_UPPER:
    {
      gdouble upper;

      upper = g_value_get_double(value);

      pthread_mutex_lock(conversion_mutex);

      ladspa_conversion->upper = upper;
      
      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  case PROP_STEP_COUNT:
    {
      gdouble step_count;

      step_count = g_value_get_double(value);

      pthread_mutex_lock(conversion_mutex);

      ladspa_conversion->step_count = step_count;
      
      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ladspa_conversion_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsLadspaConversion *ladspa_conversion;

  pthread_mutex_t *conversion_mutex;

  ladspa_conversion = AGS_LADSPA_CONVERSION(gobject);

  /* get base plugin mutex */
  conversion_mutex = AGS_CONVERSION_GET_OBJ_MUTEX(ladspa_conversion);

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(conversion_mutex);

      g_value_set_uint(value, ladspa_conversion->samplerate);

      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  case PROP_LOWER:
    {
      pthread_mutex_lock(conversion_mutex);

      g_value_set_double(value, ladspa_conversion->lower);

      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  case PROP_UPPER:
    {
      pthread_mutex_lock(conversion_mutex);

      g_value_set_double(value, ladspa_conversion->upper);

      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  case PROP_STEP_COUNT:
    {
      pthread_mutex_lock(conversion_mutex);

      g_value_set_double(value, ladspa_conversion->step_count);

      pthread_mutex_unlock(conversion_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ladspa_conversion_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_ladspa_conversion_parent_class)->finalize(gobject);
}

/**
 * ags_ladspa_conversion_test_flags:
 * @ladspa_conversion: the #AgsLadspaConversion
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_ladspa_conversion_test_flags(AgsLadspaConversion *ladspa_conversion, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *conversion_mutex;

  if(!AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
    return(FALSE);
  }
  
  /* get base plugin mutex */
  conversion_mutex = AGS_CONVERSION_GET_OBJ_MUTEX(ladspa_conversion);

  /* test flags */
  pthread_mutex_lock(conversion_mutex);

  retval = ((flags & (ladspa_conversion->flags)) != 0) ? TRUE: FALSE;
  
  pthread_mutex_unlock(conversion_mutex);

  return(retval);
}

/**
 * ags_ladspa_conversion_set_flags:
 * @ladspa_conversion: the #AgsLadspaConversion
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.0.0
 */
void
ags_ladspa_conversion_set_flags(AgsLadspaConversion *ladspa_conversion, guint flags)
{
  pthread_mutex_t *conversion_mutex;

  if(!AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
    return;
  }
  
  /* get base plugin mutex */
  conversion_mutex = AGS_CONVERSION_GET_OBJ_MUTEX(ladspa_conversion);

  /* set flags */
  pthread_mutex_lock(conversion_mutex);

  ladspa_conversion->flags |= flags;
  
  pthread_mutex_unlock(conversion_mutex);
}

/**
 * ags_ladspa_conversion_unset_flags:
 * @ladspa_conversion: the #AgsLadspaConversion
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.0.0
 */
void
ags_ladspa_conversion_unset_flags(AgsLadspaConversion *ladspa_conversion, guint flags)
{
  pthread_mutex_t *conversion_mutex;

  if(!AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
    return;
  }
  
  /* get base plugin mutex */
  conversion_mutex = AGS_CONVERSION_GET_OBJ_MUTEX(ladspa_conversion);

  /* unset flags */
  pthread_mutex_lock(conversion_mutex);

  ladspa_conversion->flags &= (~flags);
  
  pthread_mutex_unlock(conversion_mutex);
}

gdouble
ags_ladspa_conversion_convert(AgsConversion *conversion,
			      gdouble x,
			      gboolean reverse)
{
  AgsLadspaConversion *ladspa_conversion;

  gdouble value, step;
  gdouble upper, lower, step_count;
  gdouble retval;
  
  ladspa_conversion = AGS_LADSPA_CONVERSION(conversion);

  if(reverse){
    if(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_SAMPLERATE)){
      guint samplerate;

      g_object_get(ladspa_conversion,
		   "samplerate", &samplerate,
		   NULL);
      
      retval = x / samplerate;
    }

    if(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_LOGARITHMIC)){
      g_object_get(ladspa_conversion,
		   "lower", &lower,
		   "upper", &upper,
		   "step_count", &step_count,
		   NULL);      

      value = x;
      step =
	retval = (step_count - 1) * log(value / lower) / log(upper / lower);
    }
  }else{
    if(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_SAMPLERATE)){
      guint samplerate;

      g_object_get(ladspa_conversion,
		   "samplerate", &samplerate,
		   NULL);

      value = x * samplerate;
    }

    if(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_LOGARITHMIC)){
      g_object_get(ladspa_conversion,
		   "lower", &lower,
		   "upper", &upper,
		   "step_count", &step_count,
		   NULL);      

      step = x;
      value = 
	retval = lower * pow(upper / lower, step / (step_count - 1));
    }
  }
  
  return(retval);
}

/**
 * ags_ladspa_conversion_new:
 *
 * Create a new instance of #AgsLadspaConversion.
 *
 * Returns: the new #AgsLadspaConversion.
 *
 * Since: 2.0.0
 */
AgsLadspaConversion*
ags_ladspa_conversion_new()
{
  AgsLadspaConversion *conversion;
  
  conversion = g_object_new(AGS_TYPE_LADSPA_CONVERSION,
			    NULL);

  return(conversion);
}
