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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ihost_application.h>

#include <pluginterfaces/vst/ivsthostapplication.h>

extern "C" {
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_ihost_application_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IHostApplication::iid.toTUID()));
  }

  /**
   * Get name.
   *
   * @param ihost_application the host application
   * @param name the name
   * @return the return value
   *
   * @since 5.0.0
   */
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
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_ivst3_to_vst2_wrapper_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IVst3ToVst2Wrapper::iid.toTUID()));
  }
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_ivst3_to_au_wrapper_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IVst3ToAUWrapper::iid.toTUID()));
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_ivst3_to_aax_wrapper_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IVst3ToAAXWrapper::iid.toTUID()));
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_ivst3_wrapper_mpe_support_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IVst3WrapperMPESupport::iid.toTUID()));
  }

  /**
   * Enable MPE input processing.
   *
   * @param ivst3_wrapper_mpe_support the MPE support
   * @param state the state
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_ivst3_wrapper_mpe_support_enable_mpe_input_processing(AgsVstIVst3WrapperMPESupport *ivst3_wrapper_mpe_support,
									      gboolean state)
  {
    return(((Steinberg::Vst::IVst3WrapperMPESupport *) ivst3_wrapper_mpe_support)->enableMPEInputProcessing(state));
  }
								       
  /**
   * Set MPE input device settings.
   *
   * @param ivst3_wrapper_mpe_support the MPE support
   * @param master_channel the master channel
   * @param member_begin_channel the member begin channel
   * @param member_end_channel the member end channel
   * @return the return value
   * 
   * @since 5.0.0
   */
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
