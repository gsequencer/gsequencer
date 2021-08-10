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

#ifndef __AGS_VST_IPLUG_VIEW_CONTENT_SCALE_SUPPORT_H__
#define __AGS_VST_IPLUG_VIEW_CONTENT_SCALE_SUPPORT_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstIPlugViewContentScaleSupport IPlugViewContentScaleSupport;  

  typedef gfloat AgsVstScaleFactor;
  
  AgsVstTUID* ags_vst_iplug_view_content_scale_support_get_iid();

  AgsVstTResult ags_vst_iplug_view_content_scale_support_set_content_scale_factor(AgsVstIPlugViewContentScaleSupport *iplug_view_content_scale_support,
										  AgsVstScaleFactor factor);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IPLUG_VIEW_CONTENT_SCALE_SUPPORT_H__*/
