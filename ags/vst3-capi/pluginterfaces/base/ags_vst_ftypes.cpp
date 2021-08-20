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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.h>

#include <pluginterfaces/base/ftypes.h>

using namespace Steinberg;

extern "C" {

  gboolean ags_vst_str_empty(ags_vst_tchar *str)
  {
    return(static_cast<gboolean>(strEmpty(reinterpret_cast<tchar *>(str))));
  }
  
  gboolean ags_vst_str8_empty(gchar *str)
  {
    return(static_cast<gboolean>(str8Empty(reinterpret_cast<char8 *>(str))));
  }
  
  gboolean ags_vst_str16_empty(gunichar2 *str)
  {
    return(static_cast<gboolean>(str16Empty(reinterpret_cast<char16 *>(str))));
  }
  
}
