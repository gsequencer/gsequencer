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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iphysical_ui.h>

#include <pluginterfaces/vst/ivstphysicalui.h>

extern "C" {

  const AgsVstTUID* ags_vst_inote_expression_physical_ui_mapping_get_iid()
  {
    extern const Steinberg::TUID INoteExpressionPhysicalUIMapping__iid;

    return((AgsVstTUID *) (&INoteExpressionPhysicalUIMapping__iid));
  }

  AgsVstTResult ags_vst_inote_expression_physical_ui_mapping_get_physical_ui_mapping(AgsVstINoteExpressionPhysicalUIMapping *inote_expression_physical_ui_mapping,
										     gint32 bus_index, gint16 channel,
										     AgsVstPhysicalUIMapList *list)
  {
    return(((Steinberg::Vst::INoteExpressionPhysicalUIMapping *) inote_expression_physical_ui_mapping)->getPhysicalUIMapping(bus_index, channel,
															     const_cast<Steinberg::Vst::PhysicalUIMapList&>(((Steinberg::Vst::PhysicalUIMapList *) list)[0])));
  }

}
