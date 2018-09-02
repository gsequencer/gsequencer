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

#include <ags/plugin/ags_ladspa_conversion.h>

#include <ags/object/ags_soundcard.h>

#include <math.h>

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
};

static gpointer ags_ladspa_conversion_parent_class = NULL;

GType
ags_ladspa_conversion_get_type(void)
{
  static GType ags_type_ladspa_conversion = 0;

  if(!ags_type_ladspa_conversion){
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
  }

  return(ags_type_ladspa_conversion);
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

  /* AgsConversionClass */
  conversion = (AgsConversionClass *) ladspa_conversion;
    
  conversion->convert = ags_ladspa_conversion_convert;
}

void
ags_ladspa_conversion_init(AgsLadspaConversion *ladspa_conversion)
{
  AgsConfig *config;

  gchar *str;
  
  ladspa_conversion->flags = 0;

  config = ags_config_get_instance();

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate");
  }
  
  if(str != NULL){
    ladspa_conversion->samplerate = g_ascii_strtoull(str,
						     NULL,
						     10);
    
    free(str);
  }else{
    ladspa_conversion->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }
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
  pthread_mutex_lock(ags_conversion_get_class_mutex());
  
  conversion_mutex = AGS_CONVERSION(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_conversion_get_class_mutex());

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
  pthread_mutex_lock(ags_conversion_get_class_mutex());
  
  conversion_mutex = AGS_CONVERSION(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_conversion_get_class_mutex());

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(conversion_mutex);

      g_value_set_uint(value, ladspa_conversion->samplerate);

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
  /* empty */

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
  pthread_mutex_lock(ags_conversion_get_class_mutex());
  
  conversion_mutex = AGS_CONVERSION(ladspa_conversion)->obj_mutex;
  
  pthread_mutex_unlock(ags_conversion_get_class_mutex());

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
  pthread_mutex_lock(ags_conversion_get_class_mutex());
  
  conversion_mutex = AGS_CONVERSION(ladspa_conversion)->obj_mutex;
  
  pthread_mutex_unlock(ags_conversion_get_class_mutex());

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
  pthread_mutex_lock(ags_conversion_get_class_mutex());
  
  conversion_mutex = AGS_CONVERSION(ladspa_conversion)->obj_mutex;
  
  pthread_mutex_unlock(ags_conversion_get_class_mutex());

  /* unset flags */
  pthread_mutex_lock(conversion_mutex);

  ladspa_conversion->flags &= (~flags);
  
  pthread_mutex_unlock(conversion_mutex);
}

gdouble
ags_ladspa_conversion_convert(AgsConversion *conversion,
			      gdouble value,
			      gboolean reverse)
{
  AgsLadspaConversion *ladspa_conversion;

  ladspa_conversion = AGS_LADSPA_CONVERSION(conversion);

  if(reverse){
    if(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_SAMPLERATE)){
      guint samplerate;

      g_object_get(ladspa_conversion,
		   "samplerate", &samplerate,
		   NULL);
      
      value /= samplerate;
    }

    if(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_LOGARITHMIC)){
      value = log(value);
    }
  }else{
    if(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_SAMPLERATE)){
      guint samplerate;

      g_object_get(ladspa_conversion,
		   "samplerate", &samplerate,
		   NULL);

      value *= samplerate;
    }

    if(ags_ladspa_conversion_test_flags(ladspa_conversion, AGS_LADSPA_CONVERSION_LOGARITHMIC)){
      value = exp(value);
    }
  }
  
  return(value);
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
