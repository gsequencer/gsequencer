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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ihost_application.h>

#include <pluginterfaces/vst/ivsthostapplication.h>

extern "C" {
  
  const AgsVstTUID* ags_vst_ihost_application_get_iid()
  {
    extern const Steinberg::TUID IHostApplication__iid;

    return((AgsVstTUID *) (&IHostApplication__iid));
  }

  AgsVstTResult ags_vst_ihost_application_get_name(AgsVstIHostApplication *ihost_application,
						   AgsVstString128 name)
  {
    return(((Steinberg::Vst::IHostApplication *) ihost_application)->getName((Steinberg::Vst::TChar *) name));
  }

  AgsVstTResult ags_vst_ihost_application_create_instance(AgsVstIHostApplication *ihost_application,
							  AgsVstTUID *cid, AgsVstTUID *iid, void **obj)
  {
    return(((Steinberg::Vst::IHostApplication *) ihost_application)->createInstance(((Steinberg::TUID *) cid)[0], ((Steinberg::TUID *) iid)[0], obj));
  }
  
  const AgsVstTUID* ags_vst_ivst3_to_vst2_wrapper_get_iid()
  {
    extern const Steinberg::TUID IVst3ToVst2Wrapper__iid;

    return((AgsVstTUID *) (&IVst3ToVst2Wrapper__iid));
  }
  
  const AgsVstTUID* ags_vst_ivst3_to_au_wrapper_get_iid()
  {
    extern const Steinberg::TUID IVst3ToAUWrapper__iid;

    return((AgsVstTUID *) (&IVst3ToAUWrapper__iid));
  }

  const AgsVstTUID* ags_vst_ivst3_to_aax_wrapper_get_iid()
  {
    extern const Steinberg::TUID IVst3ToAAXWrapper__iid;

    return((AgsVstTUID *) (&IVst3ToAAXWrapper__iid));
  }

  const AgsVstTUID* ags_vst_ivst3_wrapper_mpe_support_get_iid()
  {
    extern const Steinberg::TUID IVst3WrapperMPESupport__iid;

    return((AgsVstTUID *) (&IVst3WrapperMPESupport__iid));
  }

  AgsVstTResult ags_vst_ivst3_wrapper_mpe_support_enable_mpe_input_processing(AgsVstIVst3WrapperMPESupport *ivst3_wrapper_mpe_support,
									     gboolean state)
  {
    return(((Steinberg::Vst::IVst3WrapperMPESupport *) ivst3_wrapper_mpe_support)->enableMPEInputProcessing(state));
  }
								       
  AgsVstTResult ags_vst_ivst3_wrapper_mpe_suppor_set_mpe_input_device_settings(AgsVstIVst3WrapperMPESupport *ivst3_wrapper_mpe_support,
									       gint32 master_channel,
									       gint32 member_begin_channel,
									       gint32 member_end_channel)
  {
    return(((Steinberg::Vst::IVst3WrapperMPESupport *) ivst3_wrapper_mpe_support)->setMPEInputDeviceSettings(master_channel,
													     member_begin_channel,
													     member_end_channel));
  }

}
