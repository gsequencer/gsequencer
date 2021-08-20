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

#ifndef __AGS_VST_IREPRESENTATION_H__
#define __AGS_VST_IREPRESENTATION_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ibstream.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef struct RepresentationInfo AgsVstRepresentationInfo;

  typedef struct IXmlRepresentationController AgsVstIXmlRepresentationController;

  typedef enum
  {
    AGS_VST_KKNOB = 0,
    AGS_VST_KPRESSET_KNOB,
    AGS_VST_KSWITCH_KNOB,
    AGS_VST_KSWITCH,
    AGS_VST_KLED,
    AGS_VST_KLINK,
    AGS_VST_KDISPLAY,
    AGS_VST_KFADER,
    AGS_VST_KEEND_OF_LAYER_TYPE
  }AgsVstLayerType;
  
  const AgsVstTUID* ags_vst_ixml_representation_controller_get_iid();

  AgsVstTResult ags_vst_ixml_representation_controller_get_xml_representation_stream(AgsVstIXmlRepresentationController *ixml_representation_controller,
										     AgsVstRepresentationInfo *info,
										     AgsVstIBStream *stream);  
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IREPRESENTATION_H__*/
