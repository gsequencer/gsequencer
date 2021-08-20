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

#ifndef __AGS_VST_ICOMPONENT_H__
#define __AGS_VST_ICOMPONENT_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_iplugin_base.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ibstream.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct IComponent AgsVstIComponent;

  typedef struct BusInfo AgsVstBusInfo;

  typedef struct RoutingInfo AgsVstRoutingInfo;

  typedef enum{
    AGS_VST_KAUDIO = 0,
    AGS_VST_KEVENT,
    AGS_VST_KNUM_MEDIA_TYPES
  }AgsVstMediaTypes;

  typedef enum
  {
    AGS_VST_KINPUT = 0,
    AGS_VST_KOUTPUT
  }AgsVstBusDirections;

  typedef enum
  {
    AGS_VST_KMAIN = 0,
    AGS_VST_KAUX
  }AgsVstBusTypes;

  typedef enum
  {
    AGS_VST_KDEFAULT_ACTIVE     = 1,
    AGS_VST_KIS_CONTROL_VOLTAGE = 1 << 1
  }AgsVstBusFlags;

  typedef enum
  {
    AGS_VST_KSIMPLE = 0,
    AGS_VST_KADVANCED,
    AGS_vST_KOFFLINE_PROCESSING
  }AgsVstIoModes;
  
  const gint32 ags_vst_kdefault_factory_flags = AGS_VST_KUNICODE;

  const AgsVstTUID* ags_vst_icomponent_get_iid();

  AgsVstTResult ags_vst_icomponent_get_controller_class_id(AgsVstIComponent *icomponent, AgsVstTUID *class_id);

  AgsVstTResult ags_vst_icomponent_set_io_mode(AgsVstIComponent *icomponent, guint io_mode);

  gint32 ags_vst_icomponent_get_bus_count(AgsVstIComponent *icomponent,
					  guint type, guint dir);
  
  AgsVstTResult ags_vst_icomponent_get_bus_info(AgsVstIComponent *icomponent,
						guint type, guint dir,
						gint32 index,
						AgsVstBusInfo *bus);

  AgsVstTResult ags_vst_icomponent_get_routing_info(AgsVstIComponent *icomponent,
						    AgsVstRoutingInfo *in_info, AgsVstRoutingInfo *out_info);
  
  AgsVstTResult ags_vst_icomponent_activate_bus(AgsVstIComponent *icomponent,
						guint type, guint dir,
						gint32 index,
						gboolean state);

  AgsVstTResult ags_vst_icomponent_set_active(AgsVstIComponent *icomponent,
					      gboolean state);

  AgsVstTResult ags_vst_icomponent_set_state(AgsVstIComponent *icomponent,
					     AgsVstIBStream *state);
  AgsVstTResult ags_vst_icomponent_get_state(AgsVstIComponent *icomponent,
					     AgsVstIBStream *state);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_ICOMPONENT_H__*/
