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

#ifndef __AGS_VST_ICONTEXT_MENU_H__
#define __AGS_VST_ICONTEXT_MENU_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/gui/ags_vst_iplug_view.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct IComponentHandler3 AgsVstIComponentHandler3;

  typedef struct IContextMenuTarget AgsVstIContextMenuTarget;

  typedef struct IContextMenuItem AgsVstIContextMenuItem;

  typedef struct IContextMenu AgsVstIContextMenu;
  
  typedef struct Item AgsVstItem;
  
  const AgsVstTUID* ags_vst_icomponent_handler3_get_iid();

  AgsVstIContextMenu* ags_vst_icomponent_handler3_create_context_menu(AgsVstIComponentHandler3 *icomponent_handler3,
								      AgsVstIPlugView* plug_view, AgsVstParamID *param_id);
  
  const AgsVstTUID* ags_vst_icontext_menu_target_get_iid();

  AgsVstTResult ags_vst_icontext_menu_target_execute_menu_item(AgsVstIContextMenuTarget *icontext_menu_target,
							       gint32 tag);
  
  const AgsVstTUID* ags_vst_icontext_menu_get_iid();

  gint32 ags_vst_icontext_menu_get_item_count(AgsVstIContextMenu *icontext_menu);

  AgsVstTResult ags_vst_icontext_menu_get_item(AgsVstIContextMenu *icontext_menu,
					       gint32 index, AgsVstItem *item, AgsVstIContextMenuTarget **target);

  AgsVstTResult ags_vst_icontext_menu_add_item(AgsVstIContextMenu *icontext_menu,
					       AgsVstItem *item, AgsVstIContextMenuTarget* target);
  
  AgsVstTResult ags_vst_icontext_menu_remove_item(AgsVstIContextMenu *icontext_menu,
						  AgsVstItem *item, AgsVstIContextMenuTarget* target);

  AgsVstTResult ags_vst_icontext_menu_popup(AgsVstIContextMenu *icontext_menu,
					    AgsVstUCoord x, AgsVstUCoord y);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_ICONTEXT_MENU_H__*/
