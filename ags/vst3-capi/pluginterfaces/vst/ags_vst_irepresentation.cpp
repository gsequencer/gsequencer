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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_irepresentation.h>

#include <pluginterfaces/vst/ivstrepresentation.h>

extern "C" {

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_ixml_representation_controller_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IXmlRepresentationController::iid.toTUID()));
  }

  /**
   * Get XML representation stream.
   *
   * @param ixml_representation_controller the XML representation controller
   * @param info the representation info
   * @param stream the stream
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_ixml_representation_controller_get_xml_representation_stream(AgsVstIXmlRepresentationController *ixml_representation_controller,
										     AgsVstRepresentationInfo *info,
										     AgsVstIBStream *stream)
  {
    return(((Steinberg::Vst::IXmlRepresentationController *) ixml_representation_controller)->getXmlRepresentationStream(const_cast<Steinberg::Vst::RepresentationInfo&>(((Steinberg::Vst::RepresentationInfo *) info)[0]),
															 (Steinberg::IBStream *) stream));
  }

}
