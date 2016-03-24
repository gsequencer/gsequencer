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

#include <ags/plugin/ags_ladspa_conversion.h>

#include <ags/object/ags_soundcard.h>

#include <math.h>

/**
 * SECTION:ags_ladspa_conversion
 * @short_description: Conversion of values
 * @title: AgsLadspaConversion
 * @section_id:
 * @include: ags/lib/ags_ladspa_conversion.h
 *
 * The #AgsLadspaConversion converts values.
 */

void ags_ladspa_conversion_class_init(AgsLadspaConversionClass *conversion);
void ags_ladspa_conversion_init (AgsLadspaConversion *conversion);
void ags_ladspa_conversion_finalize(GObject *gobject);

gdouble ags_ladspa_conversion_convert(AgsConversion *conversion,
				      gdouble value,
				      gboolean reverse);

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
							"AgsLadspaConversion\0",
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
  
  ags_ladspa_conversion_parent_class = g_type_class_peek_parent(conversion);

  /* GObjectClass */
  gobject = (GObjectClass *) ladspa_conversion;
  
  gobject->finalize = ags_ladspa_conversion_finalize;

  /* AgsConversionClass */
  conversion = (AgsLadspaConversion *) ladspa_conversion;
    
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
			     "samplerate\0");

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
ags_ladspa_conversion_finalize(GObject *gobject)
{
  /* empty */
}

gdouble
ags_ladspa_conversion_convert(AgsConversion *conversion,
			      gdouble value,
			      gboolean reverse)
{
  AgsLadspaConversion *ladspa_conversion;

  ladspa_conversion = AGS_LADSPA_CONVERSION(conversion);

  if(!reverse){
    if((AGS_LADSPA_CONVERSION_LOGARITHMIC & (ladspa_conversion->flags)) != 0){
      value = exp(log(value));
    }

    if((AGS_LADSPA_CONVERSION_SAMPLERATE & (ladspa_conversion->flags)) != 0){
      if((AGS_LADSPA_CONVERSION_BOUNDED_BELOW & (ladspa_conversion->flags)) != 0){
	if(value < 0.0){
	  value *= ladspa_conversion->samplerate;
	}
      }

      if((AGS_LADSPA_CONVERSION_BOUNDED_ABOVE & (ladspa_conversion->flags)) != 0){
	if(value >= 0.0){
	  value *= ladspa_conversion->samplerate;
	}
      }
    }
  }else{
    if((AGS_LADSPA_CONVERSION_LOGARITHMIC & (ladspa_conversion->flags)) != 0){
      value = exp(log(value) * (1.0 / M_E));
    }

    if((AGS_LADSPA_CONVERSION_SAMPLERATE & (ladspa_conversion->flags)) != 0){
      if((AGS_LADSPA_CONVERSION_BOUNDED_BELOW & (ladspa_conversion->flags)) != 0){
	if(value < 0.0){
	  value /= ladspa_conversion->samplerate;
	}
      }

      if((AGS_LADSPA_CONVERSION_BOUNDED_ABOVE & (ladspa_conversion->flags)) != 0){
	if(value >= 0.0){
	  value /= ladspa_conversion->samplerate;
	}
      }
    }
  }
  
  return(value);
}

/**
 * ags_ladspa_conversion_new:
 *
 * Instantiate a new #AgsLadspaConversion.
 *
 * Returns: the new instance
 *
 * Since: 0.7.9
 */
AgsLadspaConversion*
ags_ladspa_conversion_new()
{
  AgsLadspaConversion *conversion;
  
  conversion = g_object_new(AGS_TYPE_CONVERSION,
			    NULL);

  return(conversion);
}
