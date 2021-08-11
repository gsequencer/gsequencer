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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_irepresentation.h>

#include <pluginterfaces/vst/ivstrepresentation.h>

extern "C" {

  const AgsVstTUID* ags_vst_ixml_representation_controller()
  {
    extern const Steinberg::TUID IXmlRepresentationController__iid;

    return((AgsVstTUID *) (&IXmlRepresentationController__iid));
  }

  AgsVstTResult ags_vst_ixml_representation_controller_get_xml_representation_stream(AgsVstIXmlRepresentationController *ixml_representation_controller,
										     AgsVstRepresentationInfo *info,
										     AgsVstIBStream *stream)
  {
    return(((Steinberg::Vst::IXmlRepresentationController *) ixml_representation_controller)->getXmlRepresentationStream(const_cast<Steinberg::Vst::RepresentationInfo&>(((Steinberg::Vst::RepresentationInfo *) info)[0]),
															 (Steinberg::IBStream *) stream));
  }

}
