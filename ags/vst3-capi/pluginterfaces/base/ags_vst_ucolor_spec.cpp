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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ucolor_spec.h>

#include <pluginterfaces/base/ucolorspec.h>

extern "C" {

  /**
   * Make color spec RGB.
   *
   * @param r red
   * @param g green
   * @param b blue
   * @return the Steinberg::ColorSpec as AgsVstColorSpec
   * 
   * @since 5.0.0
   */
  AgsVstColorSpec ags_vst_color_spec_make_color_spec_rgb(AgsVstColorComponent r, AgsVstColorComponent g, AgsVstColorComponent b)
  {
    return(Steinberg::MakeColorSpec(r, g, b));
  }

  /**
   * Make color spec RGBA.
   *
   * @param r red
   * @param g green
   * @param b blue
   * @param a alpha
   * @return the Steinberg::ColorSpec as AgsVstColorSpec
   * 
   * @since 5.0.0
   */
  AgsVstColorSpec ags_vst_color_spec_make_color_spec_rgba(AgsVstColorComponent r, AgsVstColorComponent g, AgsVstColorComponent b, AgsVstColorComponent a)
  {
    return(Steinberg::MakeColorSpec(r, g, b, a));
  }

  /**
   * Get blue.
   *
   * @param color_spec the color spec
   * @return the Steinberg::ColorComponent as AgsVstColorComponent
   * 
   * @since 5.0.0
   */
  AgsVstColorComponent ags_vst_color_spec_get_blue(AgsVstColorSpec color_spec)
  {
    return(Steinberg::GetBlue(color_spec));
  }

  /**
   * Get green.
   *
   * @param color_spec the color spec
   * @return the Steinberg::ColorComponent as AgsVstColorComponent
   * 
   * @since 5.0.0
   */
  AgsVstColorComponent ags_vst_color_spec_get_green(AgsVstColorSpec color_spec)
  {
    return(Steinberg::GetGreen(color_spec));
  }

  /**
   * Get red.
   *
   * @param color_spec the color spec
   * @return the Steinberg::ColorComponent as AgsVstColorComponent
   * 
   * @since 5.0.0
   */
  AgsVstColorComponent ags_vst_color_spec_get_red(AgsVstColorSpec color_spec)
  {
    return(Steinberg::GetRed(color_spec));
  }

  /**
   * Get alpha.
   *
   * @param color_spec the color spec
   * @return the Steinberg::ColorComponent as AgsVstColorComponent
   * 
   * @since 5.0.0
   */
  AgsVstColorComponent ags_vst_color_spec_get_alpha(AgsVstColorSpec color_spec)
  {
    return(Steinberg::GetAlpha(color_spec));
  }

  /**
   * Set blue.
   *
   * @param color_spec the color spec
   * @param b blue
   * 
   * @since 5.0.0
   */
  void ags_vst_color_spec_set_blue(AgsVstColorSpec *color_spec, AgsVstColorComponent b)
  {
    Steinberg::SetBlue(color_spec[0], b);
  }

  /**
   * Set green.
   *
   * @param color_spec the color spec
   * @param g green
   * 
   * @since 5.0.0
   */
  void ags_vst_color_spec_set_green(AgsVstColorSpec *color_spec, AgsVstColorComponent g)
  {
    Steinberg::SetGreen(color_spec[0], g);
  }

  /**
   * Set red.
   *
   * @param color_spec the color spec
   * @param r red
   * 
   * @since 5.0.0
   */
  void ags_vst_color_spec_set_red(AgsVstColorSpec *color_spec, AgsVstColorComponent r)
  {
    Steinberg::SetRed(color_spec[0], r);
  }

  /**
   * Set alpha.
   *
   * @param color_spec the color spec
   * @param a alpha
   * 
   * @since 5.0.0
   */
  void ags_vst_color_spec_set_alpha(AgsVstColorSpec *color_spec, AgsVstColorComponent a)
  {
    Steinberg::SetAlpha(color_spec[0], a);
  }

  /**
   * Normalize color component.
   *
   * @param c the color component
   * @return the normalized value
   * 
   * @since 5.0.0
   */
  gdouble ags_vst_color_spec_normalize_color_component(AgsVstColorComponent c)
  {
    return(Steinberg::NormalizeColorComponent(c));
  }

  /**
   * Denormalize color component.
   *
   * @param c the color
   * @return the color component
   * 
   * @since 5.0.0
   */
  AgsVstColorComponent ags_vst_color_spec_denormalize_color_component(gdouble c)
  {
    return(Steinberg::DenormalizeColorComponent(c));
  }

  /**
   * Set alpha norm.
   *
   * @param color_spec the color spec
   * @param a the alpha norm
   * 
   * @since 5.0.0
   */
  void ags_vst_color_spec_set_alpha_norm(AgsVstColorSpec *color_spec, gdouble a)
  {
    Steinberg::SetAlphaNorm(color_spec[0], a);
  }

  /**
   * Get alpha norm.
   *
   * @param color_spec the color spec
   * @return the alpha norm
   * 
   * @since 5.0.0
   */
  gdouble ags_vst_color_spec_get_alpha_norm(AgsVstColorSpec color_spec)
  {
    return(Steinberg::GetAlphaNorm(color_spec));
  }

  /**
   * Normalize alpha.
   *
   * @param a the alpha
   * @return the normalized alpha
   * 
   * @since 5.0.0
   */
  gdouble ags_vst_color_spec_normalize_alpha(guint8 alpha)
  {
    return(Steinberg::NormalizeAlpha(alpha));
  }

  /**
   * Denormalize alpha.
   *
   * @param alpha_norm the normalized alpha
   * @return the alpha
   * 
   * @since 5.0.0
   */
  AgsVstColorComponent ags_vst_color_spec_denormalize_alpha(gdouble alpha_norm)
  {
    return(Steinberg::DenormalizeAlpha(alpha_norm));
  }

  /**
   * Strip alpha.
   *
   * @param color_spec the color spec
   * @return the color spec
   * 
   * @since 5.0.0
   */
  AgsVstColorSpec ags_vst_color_spec_strip_alpha(AgsVstColorSpec color_spec)
  {
    return(Steinberg::StripAlpha(color_spec));
  }
  
  /**
   * Blend color.
   *
   * @param color_spec the color spec
   * @param opacity the opacity
   * @return the color spec
   * 
   * @since 5.0.0
   */
  AgsVstColorSpec ags_vst_color_spec_blend_color(AgsVstColorSpec color_spec, gdouble opacity)
  {
    return(Steinberg::BlendColor(color_spec, opacity));
  }

}
