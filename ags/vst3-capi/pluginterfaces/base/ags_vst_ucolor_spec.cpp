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

  AgsVstColorSpec ags_vst_color_spec_make_color_spec_rgb(AgsVstColorComponent r, AgsVstColorComponent g, AgsVstColorComponent b)
  {
    return(Steinberg::MakeColorSpec(r, g, b));
  }

  AgsVstColorSpec ags_vst_color_spec_make_color_spec_rgba(AgsVstColorComponent r, AgsVstColorComponent g, AgsVstColorComponent b, AgsVstColorComponent a)
  {
    return(Steinberg::MakeColorSpec(r, g, b, a));
  }

  AgsVstColorComponent ags_vst_color_spec_get_blue(AgsVstColorSpec color_spec)
  {
    return(Steinberg::GetBlue(color_spec));
  }

  AgsVstColorComponent ags_vst_color_spec_get_green(AgsVstColorSpec color_spec)
  {
    return(Steinberg::GetGreen(color_spec));
  }

  AgsVstColorComponent ags_vst_color_spec_get_red(AgsVstColorSpec color_spec)
  {
    return(Steinberg::GetRed(color_spec));
  }

  AgsVstColorComponent ags_vst_color_spec_get_alpha(AgsVstColorSpec color_spec)
  {
    return(Steinberg::GetAlpha(color_spec));
  }

  void ags_vst_color_spec_set_blue(AgsVstColorSpec *color_spec, AgsVstColorComponent b)
  {
    Steinberg::SetBlue(color_spec[0], b);
  }

  void ags_vst_color_spec_set_green(AgsVstColorSpec *color_spec, AgsVstColorComponent g)
  {
    Steinberg::SetGreen(color_spec[0], g);
  }

  void ags_vst_color_spec_set_red(AgsVstColorSpec *color_spec, AgsVstColorComponent r)
  {
    Steinberg::SetRed(color_spec[0], r);
  }

  void ags_vst_color_spec_set_alpha(AgsVstColorSpec *color_spec, AgsVstColorComponent a)
  {
    Steinberg::SetAlpha(color_spec[0], a);
  }

  gdouble ags_vst_color_spec_normalize_color_component(AgsVstColorComponent c)
  {
    return(Steinberg::NormalizeColorComponent(c));
  }

  AgsVstColorComponent ags_vst_color_spec_denormalize_color_component(gdouble c)
  {
    return(Steinberg::DenormalizeColorComponent(c));
  }

  void ags_vst_color_spec_set_alpha_norm(AgsVstColorSpec *color_spec, gdouble a)
  {
    Steinberg::SetAlphaNorm(color_spec[0], a);
  }

  gdouble ags_vst_color_spec_get_alpha_norm(AgsVstColorSpec color_spec)
  {
    return(Steinberg::GetAlphaNorm(color_spec));
  }

  gdouble ags_vst_color_spec_normalize_alpha(guint8 alpha)
  {
    return(Steinberg::NormalizeAlpha(alpha));
  }

  AgsVstColorComponent ags_vst_color_spec_denormalize_alpha(gdouble alpha_norm)
  {
    return(Steinberg::DenormalizeAlpha(alpha_norm));
  }

  AgsVstColorSpec ags_vst_color_spec_strip_alpha(AgsVstColorSpec color_spec)
  {
    return(Steinberg::StripAlpha(color_spec));
  }
  
  AgsVstColorSpec ags_vst_color_spec_blend_color(AgsVstColorSpec color_spec, gdouble opacity)
  {
    return(Steinberg::BlendColor(color_spec, opacity));
  }

}
