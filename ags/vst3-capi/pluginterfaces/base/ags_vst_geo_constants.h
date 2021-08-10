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

#ifndef __AGS_VST_GEO_CONSTANTS_H__
#define __AGS_VST_GEO_CONSTANTS_H__
  
enum AgsVstDirection 
{
  AGS_VST_KNORTH,
  AGS_VST_KNORTH_EAST,
  AGS_VST_KEAST,
  AGS_VST_KSOUTH_EAST,
  AGS_VST_KSOUTH,
  AGS_VST_KSOUTH_WEST,
  AGS_VST_KWEST,
  AGS_VST_KNORTH_WEST,
  AGS_VST_KNO_DIRECTION,
  AGS_VST_KNUMBER_OF_DIRECTIONS,
};

enum AgsVstOrientation 
{
  AGS_VST_KHORIZONTAL,
  AGS_VST_KVERTICAL,
  AGS_VST_KNUMBER_OF_ORIENTATIONS,
};

#ifdef __cplusplus
extern "C" {
#endif

  AgsVstDirection ags_vst_geo_constants_to_oposite(AgsVstDirection dir);
  AgsVstOrientation ags_vst_geo_constants_to_orientation(AgsVstDirection dir);
  AgsVstOrientation ags_vst_geo_constants_to_orthogonal_orientation(AgsVstOrientation dir);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_GEO_CONSTANTS_H__*/
  
