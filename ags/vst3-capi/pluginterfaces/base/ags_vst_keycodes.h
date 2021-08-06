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

#ifndef __AGS_VST_KEYCODES_H__
#define __AGS_VST_KEYCODES_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef enum
  {
    AGS_VST_KEY_BACK = 1,
    AGS_VST_KEY_TAB,
    AGS_VST_KEY_CLEAR,
    AGS_VST_KEY_RETURN,
    AGS_VST_KEY_PAUSE,
    AGS_VST_KEY_ESCAPE,
    AGS_VST_KEY_SPACE,
    AGS_VST_KEY_NEXT,
    AGS_VST_KEY_END,
    AGS_VST_KEY_HOME,

    AGS_VST_KEY_LEFT,
    AGS_VST_KEY_UP,
    AGS_VST_KEY_RIGHT,
    AGS_VST_KEY_DOWN,
    AGS_VST_KEY_PAGEUP,
    AGS_VST_KEY_PAGEDOWN,

    AGS_VST_KEY_SELECT,
    AGS_VST_KEY_PRINT,
    AGS_VST_KEY_ENTER,
    AGS_VST_KEY_SNAPSHOT,
    AGS_VST_KEY_INSERT,
    AGS_VST_KEY_DELETE,
    AGS_VST_KEY_HELP,
    AGS_VST_KEY_NUMPAD0,
    AGS_VST_KEY_NUMPAD1,
    AGS_VST_KEY_NUMPAD2,
    AGS_VST_KEY_NUMPAD3,
    AGS_VST_KEY_NUMPAD4,
    AGS_VST_KEY_NUMPAD5,
    AGS_VST_KEY_NUMPAD6,
    AGS_VST_KEY_NUMPAD7,
    AGS_VST_KEY_NUMPAD8,
    AGS_VST_KEY_NUMPAD9,
    AGS_VST_KEY_MULTIPLY,
    AGS_VST_KEY_ADD,
    AGS_VST_KEY_SEPARATOR,
    AGS_VST_KEY_SUBTRACT,
    AGS_VST_KEY_DECIMAL,
    AGS_VST_KEY_DIVIDE,
    AGS_VST_KEY_F1,
    AGS_VST_KEY_F2,
    AGS_VST_KEY_F3,
    AGS_VST_KEY_F4,
    AGS_VST_KEY_F5,
    AGS_VST_KEY_F6,
    AGS_VST_KEY_F7,
    AGS_VST_KEY_F8,
    AGS_VST_KEY_F9,
    AGS_VST_KEY_F10,
    AGS_VST_KEY_F11,
    AGS_VST_KEY_F12,
    AGS_VST_KEY_NUMLOCK,
    AGS_VST_KEY_SCROLL,

    AGS_VST_KEY_SHIFT,
    AGS_VST_KEY_CONTROL,
    AGS_VST_KEY_ALT,

    AGS_VST_KEY_EQUALS,                             // only occurs on a Mac
    AGS_VST_KEY_CONTEXTMENU,                // Windows only

    // multimedia keys
    AGS_VST_KEY_MEDIA_PLAY,
    AGS_VST_KEY_MEDIA_STOP,
    AGS_VST_KEY_MEDIA_PREV,
    AGS_VST_KEY_MEDIA_NEXT,
    AGS_VST_KEY_VOLUME_UP,
    AGS_VST_KEY_VOLUME_DOWN,

    AGS_VST_KEY_F13,
    AGS_VST_KEY_F14,
    AGS_VST_KEY_F15,
    AGS_VST_KEY_F16,
    AGS_VST_KEY_F17,
    AGS_VST_KEY_F18,
    AGS_VST_KEY_F19,

    AGS_VST_VKEY_FIRST_CODE = AGS_VST_KEY_BACK,
    AGS_VST_VKEY_LAST_CODE = AGS_VST_KEY_F19,

    AGS_VST_VKEY_FIRST_ASCII = 128
    /*
      AGS_VST_KEY_0 - AGS_VST_KEY_9 are the same as ASCII '0' - '9' (0x30 - 0x39) + AGS_VST_FIRST_ASCII
      AGS_VST_KEY_A - AGS_VST_KEY_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A) + AGS_VST_FIRST_ASCII
    */
  }AgsVstVirtualKeyCodes;

  typedef enum
  {
    AGS_VST_KSHIFT_KEY     = 1 << 0, ///< same on both PC and Mac
    AGS_VST_KALTERNATE_KEY = 1 << 1, ///< same on both PC and Mac
    AGS_VST_KCOMMAND_KEY   = 1 << 2, ///< windows ctrl key; mac cmd key (apple button)
    AGS_VST_KCONTROL_KEY   = 1 << 3  ///< windows: not assigned, mac: ctrl key
  }AgsVstKeyModifier;

  typedef struct AgsVstKeyCode KeyCode;
  
  gunichar2 ags_vst_keycodes_virtual_key_code_to_char(guint8 vkey);
  guint8 ags_vst_keycodes_char_to_virtual_key_code(gunichar2 character);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_KEYCODES_H__*/
