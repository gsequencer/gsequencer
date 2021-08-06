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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_keycodes.h>

#include <pluginterfaces/base/keycodes.h>

extern "C" {

  gunichar2 ags_vst_keycodes_virtual_key_code_to_char(guint8 vkey)
  {
    return(Steinberg::VirtualKeyCodeToChar(vkey));
  }

  guint8 ags_vst_keycodes_char_to_virtual_key_code(gunichar2 character)
  {
    return(Steinberg::CharToVirtualKeyCode(character));
  }

#if 0
  gboolean ags_vst_keycodes_is_modifier_only_key(const AgsVstKey *key)
  {
    //TODO:JK: implement me
  }
#endif
  
}
