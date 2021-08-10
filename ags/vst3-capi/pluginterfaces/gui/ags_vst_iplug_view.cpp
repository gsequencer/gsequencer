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

#include <ags/vst3-capi/pluginterfaces/gui/ags_vst_iplug_view.h>

#include <pluginterfaces/gui/iplugview.h>

extern "C" {
  
  gint32 ags_vst_view_rect_get_width(AgsVstViewRect *view_rect)
  {
    return(((Steinberg::ViewRect *) view_rect)->getWidth());
  }
  
  gint32 ags_vst_view_rect_get_height(AgsVstViewRect *view_rect)
  {
    return(((Steinberg::ViewRect *) view_rect)->getHeight());
  }

  AgsVstTUID*
  ags_vst_iplug_view_get_iid()
  {
    extern const Steinberg::TUID IPlugView__iid;

    return((AgsVstTUID *) (&IPlugView__iid));
  }
    
  AgsVstTResult ags_vst_iplug_view_attached(AgsVstIPlugView *iplug_view, void *parent, AgsVstFIDString type)
  {
    return(((Steinberg::IPlugView *) iplug_view)->attached(parent, (char *) type));
  }

  AgsVstTResult ags_vst_iplug_view_removed(AgsVstIPlugView *iplug_view)
  {
    return(((Steinberg::IPlugView *) iplug_view)->removed());
  }

  AgsVstTResult ags_vst_iplug_view_on_wheel(AgsVstIPlugView *iplug_view, gfloat distance)
  {
    return(((Steinberg::IPlugView *) iplug_view)->onWheel(distance));
  }

  AgsVstTResult ags_vst_iplug_view_on_key_down(AgsVstIPlugView *iplug_view, gunichar2 key, gint16 key_code, gint16 modifiers)
  {
    return(((Steinberg::IPlugView *) iplug_view)->onKeyDown((char16_t) key, key_code, modifiers));
  }

  AgsVstTResult ags_vst_iplug_view_on_key_up(AgsVstIPlugView *iplug_view, gunichar2 key, gint16 key_code, gint16 modifiers)
  {
    return(((Steinberg::IPlugView *) iplug_view)->onKeyUp((char16_t) key, key_code, modifiers));
  }
  
  AgsVstTResult ags_vst_iplug_view_get_size(AgsVstIPlugView *iplug_view, AgsVstViewRect *current_size)
  {
    return(((Steinberg::IPlugView *) iplug_view)->getSize((Steinberg::ViewRect *) current_size));
  }

  AgsVstTResult ags_vst_iplug_view_on_size(AgsVstIPlugView *iplug_view, AgsVstViewRect *new_size)
  {
    return(((Steinberg::IPlugView *) iplug_view)->onSize((Steinberg::ViewRect *) new_size));
  }

  AgsVstTResult ags_vst_iplug_view_on_focus(AgsVstIPlugView *iplug_view, gboolean state)
  {
    return(((Steinberg::IPlugView *) iplug_view)->onFocus(state));
  }

  AgsVstTResult ags_vst_iplug_view_set_frame(AgsVstIPlugView *iplug_view, AgsVstIPlugFrame *frame)
  {
    return(((Steinberg::IPlugView *) iplug_view)->setFrame((Steinberg::IPlugFrame *) frame));
  }

  AgsVstTResult ags_vst_iplug_view_can_resize(AgsVstIPlugView *iplug_view)
  {
    return(((Steinberg::IPlugView *) iplug_view)->canResize());
  }

  AgsVstTResult ags_vst_iplug_view_check_size_constraint(AgsVstIPlugView *iplug_view, AgsVstViewRect *rect)
  {
    return(((Steinberg::IPlugView *) iplug_view)->checkSizeConstraint((Steinberg::ViewRect *) rect));
  }

}
