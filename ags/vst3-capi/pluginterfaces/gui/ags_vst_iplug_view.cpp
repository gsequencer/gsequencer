/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

  const AgsVstFIDString ags_vst_kplatform_type_hwnd = "HWND";
  const AgsVstFIDString ags_vst_kplatform_type_hi_view = "HIView";
  const AgsVstFIDString ags_vst_kplatform_type_ns_view = "NSView";
  const AgsVstFIDString ags_vst_kplatform_type_ui_view = "UIView";
  const AgsVstFIDString ags_vst_kplatform_type_x11_embed_window_id = "X11EmbedWindowID";

  /**
   * Allocate rect.
   *
   * @return the new instance of Steinberg::ViewRect as AgsVstViewRect pointer
   *
   * @since 5.0.0
   */
  AgsVstViewRect* ags_vst_view_rect_alloc()
  {
    return((AgsVstViewRect *) (new Steinberg::ViewRect));
  }
    
  /**
   * Get width.
   *
   * @param view_rect the rect
   * @return the width
   *
   * @since 5.0.0
   */
  gint32 ags_vst_view_rect_get_width(AgsVstViewRect *view_rect)
  {
    return(((Steinberg::ViewRect *) view_rect)->getWidth());
  }
  
  /**
   * Get height.
   *
   * @param view_rect the rect
   * @return the height
   *
   * @since 5.0.0
   */
  gint32 ags_vst_view_rect_get_height(AgsVstViewRect *view_rect)
  {
    return(((Steinberg::ViewRect *) view_rect)->getHeight());
  }

  /**
   * Get IID.
   * 
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  AgsVstTUID*
  ags_vst_iplug_view_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IPlugView::iid.toTUID()));
  }

  /**
   * Attached.
   *
   * @param iplug_view the plug view
   * @param parent the parent
   * @type the type
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_attached(AgsVstIPlugView *iplug_view, void *parent, AgsVstFIDString type)
  {
    return(((Steinberg::IPlugView *) iplug_view)->attached(parent, (char *) type));
  }

  /**
   * Removed.
   *
   * @param iplug_view the plug view
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_removed(AgsVstIPlugView *iplug_view)
  {
    return(((Steinberg::IPlugView *) iplug_view)->removed());
  }

  /**
   * On-wheel.
   *
   * @param iplug_view the plug view
   * @param distance the distance
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_on_wheel(AgsVstIPlugView *iplug_view, gfloat distance)
  {
    return(((Steinberg::IPlugView *) iplug_view)->onWheel(distance));
  }

  /**
   * On-key-down.
   *
   * @param iplug_view the plug view
   * @param key the key
   * @param key_code the key code
   * @param modifiers the modifiers
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_on_key_down(AgsVstIPlugView *iplug_view, gunichar2 key, gint16 key_code, gint16 modifiers)
  {
    return(((Steinberg::IPlugView *) iplug_view)->onKeyDown((char16_t) key, key_code, modifiers));
  }

  /**
   * On-key-up.
   *
   * @param iplug_view the plug view
   * @param key the key
   * @param key_code the key code
   * @param modifiers the modifiers
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_on_key_up(AgsVstIPlugView *iplug_view, gunichar2 key, gint16 key_code, gint16 modifiers)
  {
    return(((Steinberg::IPlugView *) iplug_view)->onKeyUp((char16_t) key, key_code, modifiers));
  }
  
  /**
   * Get size.
   *
   * @param iplug_view the plug view
   * @param current_size the current size
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_get_size(AgsVstIPlugView *iplug_view, AgsVstViewRect *current_size)
  {
    return(((Steinberg::IPlugView *) iplug_view)->getSize((Steinberg::ViewRect *) current_size));
  }

  /**
   * On-size.
   *
   * @param iplug_view the plug view
   * @param new_size the new size
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_on_size(AgsVstIPlugView *iplug_view, AgsVstViewRect *new_size)
  {
    return(((Steinberg::IPlugView *) iplug_view)->onSize((Steinberg::ViewRect *) new_size));
  }

  /**
   * On-focus.
   *
   * @param iplug_view the plug view
   * @param state the state
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_on_focus(AgsVstIPlugView *iplug_view, gboolean state)
  {
    return(((Steinberg::IPlugView *) iplug_view)->onFocus(state));
  }

  /**
   * Set frame.
   *
   * @param iplug_view the plug view
   * @param frame the frame
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_set_frame(AgsVstIPlugView *iplug_view, AgsVstIPlugFrame *frame)
  {
    return(((Steinberg::IPlugView *) iplug_view)->setFrame((Steinberg::IPlugFrame *) frame));
  }

  /**
   * Can resize.
   *
   * @param iplug_view the plug view
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_can_resize(AgsVstIPlugView *iplug_view)
  {
    return(((Steinberg::IPlugView *) iplug_view)->canResize());
  }

  /**
   * Check size constraint.
   *
   * @param iplug_view the plug view
   * @param rect the rect
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_check_size_constraint(AgsVstIPlugView *iplug_view, AgsVstViewRect *rect)
  {
    return(((Steinberg::IPlugView *) iplug_view)->checkSizeConstraint((Steinberg::ViewRect *) rect));
  }

}
