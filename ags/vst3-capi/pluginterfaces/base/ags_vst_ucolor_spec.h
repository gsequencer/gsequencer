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

#ifndef __AGS_VST_UCOLOR_SPEC_H__
#define __AGS_VST_UCOLOR_SPEC_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef guint32 AgsVstColorSpec;
  typedef guint8 AgsVstColorComponent;
  
  typedef guint32 AgsVstUColorSpec;
  typedef guint8 AgsVstUColorComponent;

  typedef enum{
    AGS_VST_KBLACK = 0,
    AGS_VST_KWHITE,
    AGS_VST_KGRAY5,
    AGS_VST_KGRAY10,
    AGS_VST_KGRAY20,
    AGS_VST_KGRAY30,
    AGS_VST_KGRAY40,
    AGS_VST_KGRAY50,
    AGS_VST_KGRAY60,
    AGS_VST_KGRAY70,
    AGS_VST_KGRAY80,
    AGS_VST_KGRAY90,
    AGS_VST_KRED,
    AGS_VST_KLTRED,
    AGS_VST_KDKRED,
    AGS_VST_KGREEN,
    AGS_VST_KLTGREEN,
    AGS_VST_KDKGREEN,
    AGS_VST_KBLUE,
    AGS_VST_KLTBLUE,
    AGS_VST_KDKBLUE,
    AGS_VST_KMAGENTA,
    AGS_VST_KLTMAGENTA,
    AGS_VST_KDKMAGENTA,
    AGS_VST_KYELLOW,
    AGS_VST_KLTYELLOW,
    AGS_VST_KDKYELLOW,
    AGS_VST_KORANGE,
    AGS_VST_KLTORANGE,
    AGS_VST_KDKORANGE,
    AGS_VST_KGOLD,
    AGS_VST_KBLACK50,
    AGS_VST_KBLACK70,
    AGS_VST_KNUM_STANDARD_COLORS,
    AGS_VST_KLTGRAY = AGS_VST_KGRAY20,
    AGS_VST_KGRAY = AGS_VST_KGRAY50,
    AGS_VST_KDKGRAY = AGS_VST_KGRAY70
  }AgsVstStandardColor;
  
  AgsVstColorSpec ags_vst_color_spec_make_color_spec_rgb(AgsVstColorComponent r, AgsVstColorComponent g, AgsVstColorComponent b);
  AgsVstColorSpec ags_vst_color_spec_make_color_spec_rgba(AgsVstColorComponent r, AgsVstColorComponent g, AgsVstColorComponent b, AgsVstColorComponent a);

  AgsVstColorComponent ags_vst_color_spec_get_blue(AgsVstColorSpec color_spec);
  AgsVstColorComponent ags_vst_color_spec_get_green(AgsVstColorSpec color_spec);
  AgsVstColorComponent ags_vst_color_spec_get_red(AgsVstColorSpec color_spec);
  AgsVstColorComponent ags_vst_color_spec_get_alpha(AgsVstColorSpec color_spec);

  void ags_vst_color_spec_set_blue(AgsVstColorSpec *color_spec, AgsVstColorComponent b);
  void ags_vst_color_spec_set_green(AgsVstColorSpec *color_spec, AgsVstColorComponent g);
  void ags_vst_color_spec_set_red(AgsVstColorSpec *color_spec, AgsVstColorComponent r);
  void ags_vst_color_spec_set_alpha(AgsVstColorSpec *color_spec, AgsVstColorComponent a);

  gdouble ags_vst_color_spec_normalize_color_component(AgsVstColorComponent c);
  AgsVstColorComponent ags_vst_color_spec_denormalize_color_component(gdouble c);

  void ags_vst_color_spec_set_alpha_norm(AgsVstColorSpec *color_spec, gdouble a);
  gdouble ags_vst_color_spec_get_alpha_norm(AgsVstColorSpec color_spec);
  gdouble ags_vst_color_spec_normalize_alpha(guint8 alpha);
  AgsVstColorComponent ags_vst_color_spec_denormalize_alpha(gdouble alpha_norm);

  AgsVstColorSpec ags_vst_color_spec_strip_alpha(AgsVstColorSpec color_spec);
  
  AgsVstColorSpec ags_vst_color_spec_blend_color(AgsVstColorSpec color_spec, gdouble opacity);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_UCOLOR_SPEC_H__*/
