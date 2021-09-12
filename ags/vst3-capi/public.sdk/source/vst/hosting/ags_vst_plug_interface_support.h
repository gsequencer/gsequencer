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

#ifndef __AGS_VST_PLUG_INTERFACE_SUPPORT_H__
#define __AGS_VST_PLUG_INTERFACE_SUPPORT_H__

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iplug_interface_support.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct PlugInterfaceSupport AgsVstPlugInterfaceSupport;

  AgsVstPlugInterfaceSupport* ags_vst_plug_interface_support_new();

  void ags_vst_plug_interface_support_add_plug_interface_supported(AgsVstPlugInterfaceSupport *plug_interface_support,
								   AgsVstTUID *tuid);

  gboolean ags_vst_plug_interface_support_remove_plug_interface_supported(AgsVstPlugInterfaceSupport *plug_interface_support,
									  AgsVstTUID *tuid);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_PLUG_INTERFACE_SUPPORT_H__*/
