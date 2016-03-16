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

#include <ags/plugin/ags_lv2_conversion.h>

#include <ags/object/ags_soundcard.h>

#include <math.h>

/**
 * SECTION:ags_lv2_conversion
 * @short_description: Conversion of values
 * @title: AgsLv2Conversion
 * @section_id:
 * @include: ags/lib/ags_lv2_conversion.h
 *
 * The #AgsLv2Conversion converts values.
 */

void ags_lv2_conversion_class_init(AgsLv2ConversionClass *conversion);
void ags_lv2_conversion_init (AgsLv2Conversion *conversion);
void ags_lv2_conversion_finalize(GObject *gobject);

gdouble ags_lv2_conversion_convert(AgsConversion *conversion,
				   gdouble value,
				   gboolean reverse);

static gpointer ags_lv2_conversion_parent_class = NULL;

GType
ags_lv2_conversion_get_type(void)
{
  static GType ags_type_conversion = 0;

  if(!ags_type_conversion){
    static const GTypeInfo ags_lv2_conversion_info = {
      sizeof (AgsLv2ConversionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_conversion_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2Conversion),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_conversion_init,
    };

    ags_type_conversion = g_type_register_static(G_TYPE_OBJECT,
						 "AgsLv2Conversion\0",
						 &ags_lv2_conversion_info,
						 0);
  }

  return (ags_type_conversion);
}

void
ags_lv2_conversion_class_init(AgsLv2ConversionClass *lv2_conversion)
{
  AgsConversionClass *conversion;
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_lv2_conversion_parent_class = g_type_class_peek_parent(conversion);

  /* GObjectClass */
  gobject = (GObjectClass *) lv2_conversion;
  
  gobject->finalize = ags_lv2_conversion_finalize;

  /* AgsConversionClass */
  conversion = (AgsLv2Conversion *) lv2_conversion;
    
  conversion->convert = ags_lv2_conversion_convert;
}

void
ags_lv2_conversion_init(AgsLv2Conversion *lv2_conversion)
{
  AgsConfig *config;

  gchar *str;
  
  lv2_conversion->flags = 0;
}

void
ags_lv2_conversion_finalize(GObject *gobject)
{
  /* empty */
}

gdouble
ags_lv2_conversion_convert(AgsConversion *conversion,
			   gdouble value,
			   gboolean reverse)
{
  AgsLv2Conversion *lv2_conversion;

  lv2_conversion = AGS_LV2_CONVERSION(conversion);

  if(!reverse){
    if((AGS_LV2_CONVERSION_LOGARITHMIC & (lv2_conversion->flags)) != 0){
      value = exp(log(value));
    }
  }else{
    if((AGS_LV2_CONVERSION_LOGARITHMIC & (lv2_conversion->flags)) != 0){
      value = exp(log(value) * (1.0 / M_E));
    }
  }
  
  return(value);
}

/**
 * ags_lv2_conversion_new:
 *
 * Instantiate a new #AgsLv2Conversion.
 *
 * Returns: the new instance
 *
 * Since: 0.7.9
 */
AgsLv2Conversion*
ags_lv2_conversion_new()
{
  AgsLv2Conversion *conversion;
  
  conversion = g_object_new(AGS_TYPE_CONVERSION,
			    NULL);

  return(conversion);
}
