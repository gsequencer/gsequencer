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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_icontext_menu.h>

#include <pluginterfaces/vst/ivstcontextmenu.h>

extern "C" {

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_icomponent_handler3_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IComponentHandler3::iid.toTUID()));
  }

  /**
   * Create context menu.
   *
   * @param icomponent_handler3 the icomponent handler
   * @param plug_view the plug-view
   * @param param_id the parameter identifier
   * @return the AgsVstIContextMenu
   * 
   * @since 5.0.0
   */
  AgsVstIContextMenu* ags_vst_icomponent_handler3_create_context_menu(AgsVstIComponentHandler3 *icomponent_handler3,
								      AgsVstIPlugView* plug_view, AgsVstParamID *param_id)
  {
    return((AgsVstIContextMenu *) ((Steinberg::Vst::IComponentHandler3 *) icomponent_handler3)->createContextMenu((Steinberg::IPlugView *) plug_view, (Steinberg::Vst::ParamID *) param_id));
  }
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_icontext_menu_target_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IContextMenuTarget::iid.toTUID()));
  }

  AgsVstTResult ags_vst_icontext_menu_target_execute_menu_item(AgsVstIContextMenuTarget *icontext_menu_target,
							       gint32 tag)
  {
    return(((Steinberg::Vst::IContextMenuTarget *) icontext_menu_target)->executeMenuItem(tag));
  }
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_icontext_menu_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IContextMenu::iid.toTUID()));
  }

  /**
   * Get item count.
   *
   * @param icontext_menu the icontext menu
   * @return the item count
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_icontext_menu_get_item_count(AgsVstIContextMenu *icontext_menu)
  {
    return(((Steinberg::Vst::IContextMenu *) icontext_menu)->getItemCount());
  }

  /**
   * Get item count.
   *
   * @param icontext_menu the icontext menu
   * @param index the index
   * @param item the item
   * @param target the context menu target
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icontext_menu_get_item(AgsVstIContextMenu *icontext_menu,
					       gint32 index, AgsVstItem *item, AgsVstIContextMenuTarget **target)
  {
    return(((Steinberg::Vst::IContextMenu *) icontext_menu)->getItem(index, const_cast<Steinberg::Vst::IContextMenu::Item&>(((Steinberg::Vst::IContextMenu::Item*) item)[0]), (Steinberg::Vst::IContextMenuTarget**) target));
  }

  /**
   * Add item.
   *
   * @param icontext_menu the icontext menu
   * @param item the item
   * @param target the context menu target
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icontext_menu_add_item(AgsVstIContextMenu *icontext_menu,
					       AgsVstItem *item, AgsVstIContextMenuTarget* target)
  {    
    return(((Steinberg::Vst::IContextMenu *) icontext_menu)->addItem(const_cast<Steinberg::Vst::IContextMenu::Item&>(((Steinberg::Vst::IContextMenu::Item*) item)[0]), (Steinberg::Vst::IContextMenuTarget *) target));
  }
  
  /**
   * Remove item.
   *
   * @param icontext_menu the icontext menu
   * @param item the item
   * @param target the context menu target
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icontext_menu_remove_item(AgsVstIContextMenu *icontext_menu,
						  AgsVstItem *item, AgsVstIContextMenuTarget* target)
  {
    return(((Steinberg::Vst::IContextMenu *) icontext_menu)->removeItem(const_cast<Steinberg::Vst::IContextMenu::Item&>(((Steinberg::Vst::IContextMenu::Item*) item)[0]), (Steinberg::Vst::IContextMenuTarget *) target));
  }

  /**
   * Popup.
   *
   * @param icontext_menu the icontext menu
   * @param x the x-coordinate
   * @param y the y-coordinate
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icontext_menu_popup(AgsVstIContextMenu *icontext_menu,
					    AgsVstUCoord x, AgsVstUCoord y)
  {
    return(((Steinberg::Vst::IContextMenu *) icontext_menu)->popup((Steinberg::UCoord) x, (Steinberg::UCoord) y));
  }

}
