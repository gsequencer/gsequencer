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

#ifndef __AGS_VST_HOST_CONTEXT_H__
#define __AGS_VST_HOST_CONTEXT_H__

#include <glib.h>

#include <ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_plug_interface_support.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct HostContext AgsVstHostContext;
  
  AgsVstHostContext* ags_vst_host_context_get_instance();
  AgsVstHostContext* ags_vst_host_context_new();

  AgsVstPlugInterfaceSupport* ags_vst_host_context_get_plug_interface_support(AgsVstHostContext *host_context);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_HOST_CONTEXT_H__*/
