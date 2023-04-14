/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_geo_constants.h>

#include <pluginterfaces/base/geoconstants.h>

extern "C" {

  /**
   * To oposite.
   *
   * @param dir the direction
   * @return the return value
   *
   * @since 5.0.0
   */
  guint ags_vst_geo_constants_to_oposite(guint dir)
  {
    return((AgsVstDirection) Steinberg::GeoConstants::toOpposite((Steinberg::Direction) dir));
  }
  
  /**
   * To orientation.
   *
   * @param dir the direction
   * @return the return value
   *
   * @since 5.0.0
   */
  guint ags_vst_geo_constants_to_orientation(guint dir)
  {
    return((AgsVstOrientation) Steinberg::GeoConstants::toOrientation((Steinberg::Direction) dir));
  }
  
  /**
   * To orthogonal orientation.
   *
   * @param dir the direction
   * @return the return value
   *
   * @since 5.0.0
   */
  guint ags_vst_geo_constants_to_orthogonal_orientation(guint dir)
  {
    return((AgsVstOrientation) Steinberg::GeoConstants::toOrthogonalOrientation((Steinberg::Orientation) dir));
  }
  
}
