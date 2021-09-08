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

#ifndef __AGS_VST_IPLUG_VIEW_H__
#define __AGS_VST_IPLUG_VIEW_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>

#define AGS_VST_IPLUG_VIEW_IID (ags_vst_iplug_view_get_iid())

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct ViewRect AgsVstViewRect;
  typedef struct IPlugView AgsVstIPlugView;
  typedef struct IPlugFrame AgsVstIPlugFrame;
  typedef struct IEventHandler AgsVstIEventHandler;
  typedef struct ITimerHandler AgsVstITimerHandler;
  typedef struct IRunLoop AgsVstIRunLoop;

  extern const AgsVstFIDString ags_vst_kplatform_type_hwnd;
  extern const AgsVstFIDString ags_vst_kplatform_type_hi_view;
  extern const AgsVstFIDString ags_vst_kplatform_type_ns_view;
  extern const AgsVstFIDString ags_vst_kplatform_type_ui_view;
  extern const AgsVstFIDString ags_vst_kplatform_type_x11_embed_window_id;

  AgsVstViewRect* ags_vst_view_rect_alloc();
  
  gint32 ags_vst_view_rect_get_width(AgsVstViewRect *view_rect);
  gint32 ags_vst_view_rect_get_height(AgsVstViewRect *view_rect);

  AgsVstTUID* ags_vst_iplug_view_get_iid();
  
  AgsVstTResult ags_vst_iplug_view_attached(AgsVstIPlugView *iplug_view, void *parent, AgsVstFIDString type);
  AgsVstTResult ags_vst_iplug_view_removed(AgsVstIPlugView *iplug_view);

  AgsVstTResult ags_vst_iplug_view_on_wheel(AgsVstIPlugView *iplug_view, gfloat distance);

  AgsVstTResult ags_vst_iplug_view_on_key_down(AgsVstIPlugView *iplug_view, gunichar2 key, gint16 key_code, gint16 modifiers);
  AgsVstTResult ags_vst_iplug_view_on_key_up(AgsVstIPlugView *iplug_view, gunichar2 key, gint16 key_code, gint16 modifiers);
  
  AgsVstTResult ags_vst_iplug_view_get_size(AgsVstIPlugView *iplug_view, AgsVstViewRect *current_size);

  AgsVstTResult ags_vst_iplug_view_on_size(AgsVstIPlugView *iplug_view, AgsVstViewRect *new_size);

  AgsVstTResult ags_vst_iplug_view_on_focus(AgsVstIPlugView *iplug_view, gboolean state);

  AgsVstTResult ags_vst_iplug_view_set_frame(AgsVstIPlugView *iplug_view, AgsVstIPlugFrame *frame);

  AgsVstTResult ags_vst_iplug_view_can_resize(AgsVstIPlugView *iplug_view);

  AgsVstTResult ags_vst_iplug_view_check_size_constraint(AgsVstIPlugView *iplug_view, AgsVstViewRect *rect);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IPLUG_VIEW_H__*/
