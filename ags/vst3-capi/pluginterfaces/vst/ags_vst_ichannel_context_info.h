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

#ifndef __AGS_VST_ICHANNEL_CONTEXT_INFO_H__
#define __AGS_VST_ICHANNEL_CONTEXT_INFO_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iattributes.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct IInfoListener AgsVstIInfoListener;
  
  typedef enum
  {
    AGS_VST_KPRE_VOLUME_FADER = 0,
    AGS_VST_KPOST_VOLUME_FADER,
    AGS_VST_KUSED_AS_PANNER
  }AgsVstChannelPluginLocation;

  typedef guint32 AgsVstColorSpec;
  typedef guint8 AgsVstColorComponent;

  extern const AgsVstCString AGS_VST_KCHANNEL_UID_KEY;
  extern const AgsVstCString AGS_VST_KCHANNEL_UID_LENGTH_KEY;
  extern const AgsVstCString AGS_VST_KCHANNEL_NAME_KEY;
  extern const AgsVstCString AGS_VST_KCHANNEL_NAME_LENGTH_KEY;
  extern const AgsVstCString AGS_VST_KCHANNEL_COLOR_KEY;
  extern const AgsVstCString AGS_VST_KCHANNEL_INDEX_KEY;
  extern const AgsVstCString AGS_VST_KCHANNEL_INDEX_NAMESPACE_ORDER_KEY;
  extern const AgsVstCString AGS_VST_KCHANNEL_INDEX_NAMESPACE_KEY;
  extern const AgsVstCString AGS_VST_KCHANNEL_INDEX_NAMESPACE_LENGTH_KEY;
  extern const AgsVstCString AGS_VST_KCHANNEL_IMAGE_KEY;
  extern const AgsVstCString AGS_VST_KCHANNEL_PLUGIN_LOCATION_KEY;
  
  const AgsVstTUID* ags_vst_iinfo_listener_get_iid();

  AgsVstTResult ags_vst_iinfo_listener_set_channel_context_info(AgsVstIInfoListener *iinfo_listener,
								AgsVstIAttributeList *list);

  AgsVstColorComponent ags_vst_ichannel_context_get_blue(AgsVstColorSpec cs);
  
  AgsVstColorComponent ags_vst_ichannel_context_get_green(AgsVstColorSpec cs);

  AgsVstColorComponent ags_vst_ichannel_context_get_red(AgsVstColorSpec cs);

  AgsVstColorComponent ags_vst_ichannel_context_get_alpha(AgsVstColorSpec cs);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_ICHANNEL_CONTEXT_INFO_H__*/
