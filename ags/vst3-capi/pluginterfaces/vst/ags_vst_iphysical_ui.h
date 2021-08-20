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

#ifndef __AGS_VST_IPHYSICAL_UI_H__
#define __AGS_VST_IPHYSICAL_UI_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_inote_expression.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef guint32 AgsVstPhysicalUITypeID;

  typedef struct AgsVstPhysicalUIMap PhysicalUIMap;

  typedef struct AgsVstPhysicalUIMapList PhysicalUIMapList;

  typedef struct AgsVstINoteExpressionPhysicalUIMapping INoteExpressionPhysicalUIMapping;
  
  typedef enum
  {
    AGS_VST_KPUI_X_MOVEMENT = 0,
    AGS_VST_KPUI_Y_MOVEMENT,
    AGS_VST_KPUI_PRESSURE,
    AGS_VST_KPUI_TYPE_COUNT,
    AGS_VST_KINVALID_PUI_TYPE_ID = 0xFFFFFFFF
  }AgsVstPhysicalUITypeIDs;

  const AgsVstTUID* ags_vst_inote_expression_physical_ui_mapping_get_iid();

  AgsVstTResult ags_vst_inote_expression_physical_ui_mapping_get_physical_ui_mapping(AgsVstINoteExpressionPhysicalUIMapping *inote_expression_physical_ui_mapping,
										     gint32 bus_index, gint16 channel,
										     AgsVstPhysicalUIMapList *list);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IPHYSICAL_UI_H__*/

