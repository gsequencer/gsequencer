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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_icontext_menu.h>

#include <pluginterfaces/vst/ivstcontextmenu.h>

extern "C" {

  const AgsVstTUID* ags_vst_icomponent_handler3_get_iid()
  {
    extern const Steinberg::TUID IComponentHandler3__iid;

    return((AgsVstTUID *) (&IComponentHandler3__iid));
  }

  AgsVstIContextMenu* ags_vst_icomponent_handler3_create_context_menu(AgsVstIComponentHandler3 *icomponent_handler3,
								      AgsVstIPlugView* plug_view, AgsVstParamID *param_id)
  {
    return((AgsVstIContextMenu *) ((Steinberg::Vst::IComponentHandler3 *) icomponent_handler3)->createContextMenu((Steinberg::IPlugView *) plug_view, (Steinberg::Vst::ParamID *) param_id));
  }
  
  const AgsVstTUID* ags_vst_icontext_menu_target_get_iid()
  {
    extern const Steinberg::TUID IContextMenuTarget__iid;

    return((AgsVstTUID *) (&IContextMenuTarget__iid));
  }

  AgsVstTResult ags_vst_icontext_menu_target_execute_menu_item(AgsVstIContextMenuTarget *icontext_menu_target,
							       gint32 tag)
  {
    return(((Steinberg::Vst::IContextMenuTarget *) icontext_menu_target)->executeMenuItem(tag));
  }
  
  const AgsVstTUID* ags_vst_icontext_menu_get_iid()
  {
    extern const Steinberg::TUID IContextMenu__iid;

    return((AgsVstTUID *) (&IContextMenu__iid));
  }

  gint32 ags_vst_icontext_menu_get_item_count(AgsVstIContextMenu *icontext_menu)
  {
    return(((Steinberg::Vst::IContextMenu *) icontext_menu)->getItemCount());
  }

  AgsVstTResult ags_vst_icontext_menu_get_item(AgsVstIContextMenu *icontext_menu,
					       gint32 index, AgsVstItem *item, AgsVstIContextMenuTarget **target)
  {
    return(((Steinberg::Vst::IContextMenu *) icontext_menu)->getItem(index, const_cast<Steinberg::Vst::IContextMenu::Item&>(((Steinberg::Vst::IContextMenu::Item*) item)[0]), (Steinberg::Vst::IContextMenuTarget**) target));
  }

  AgsVstTResult ags_vst_icontext_menu_add_item(AgsVstIContextMenu *icontext_menu,
					       AgsVstItem *item, AgsVstIContextMenuTarget* target)
  {    
    return(((Steinberg::Vst::IContextMenu *) icontext_menu)->addItem(const_cast<Steinberg::Vst::IContextMenu::Item&>(((Steinberg::Vst::IContextMenu::Item*) item)[0]), (Steinberg::Vst::IContextMenuTarget *) target));
  }
  
  AgsVstTResult ags_vst_icontext_menu_remove_item(AgsVstIContextMenu *icontext_menu,
						  AgsVstItem *item, AgsVstIContextMenuTarget* target)
  {
    return(((Steinberg::Vst::IContextMenu *) icontext_menu)->removeItem(const_cast<Steinberg::Vst::IContextMenu::Item&>(((Steinberg::Vst::IContextMenu::Item*) item)[0]), (Steinberg::Vst::IContextMenuTarget *) target));
  }

  AgsVstTResult ags_vst_icontext_menu_popup(AgsVstIContextMenu *icontext_menu,
					    AgsVstUCoord x, AgsVstUCoord y)
  {
    return(((Steinberg::Vst::IContextMenu *) icontext_menu)->popup((Steinberg::UCoord) x, (Steinberg::UCoord) y));
  }

}
