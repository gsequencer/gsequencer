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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ichannel_context_info.h>

#include <pluginterfaces/vst/ivstchannelcontextinfo.h>

extern "C" {

  const AgsVstCString AGS_VST_KCHANNEL_UID_KEY = "channel uid";
  const AgsVstCString AGS_VST_KCHANNEL_UID_LENGTH_KEY = "channel uid length";
  const AgsVstCString AGS_VST_KCHANNEL_NAME_KEY = "channel name";
  const AgsVstCString AGS_VST_KCHANNEL_NAME_LENGTH_KEY = "channel name length";
  const AgsVstCString AGS_VST_KCHANNEL_COLOR_KEY = "channel color";
  const AgsVstCString AGS_VST_KCHANNEL_INDEX_KEY = "channel index";
  const AgsVstCString AGS_VST_KCHANNEL_INDEX_NAMESPACE_ORDER_KEY = "channel index namespace order";
  const AgsVstCString AGS_VST_KCHANNEL_INDEX_NAMESPACE_KEY = "channel index namespace";
  const AgsVstCString AGS_VST_KCHANNEL_INDEX_NAMESPACE_LENGTH_KEY = "channel index namespace length";
  const AgsVstCString AGS_VST_KCHANNEL_IMAGE_KEY = "channel image";
  const AgsVstCString AGS_VST_KCHANNEL_PLUGIN_LOCATION_KEY = "channel plugin location";
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iinfo_listener_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::ChannelContext::IInfoListener::iid.toTUID()));
  }

  /**
   * Set channel context info.
   *
   * @param iinfo_listener the info listener
   * @param list the attribute list
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iinfo_listener_set_channel_context_info(AgsVstIInfoListener *iinfo_listener,
								AgsVstIAttributeList *list)
  {
    return(((Steinberg::Vst::ChannelContext::IInfoListener *) iinfo_listener)->setChannelContextInfos((Steinberg::Vst::IAttributeList *) list));
  }

  /**
   * Get blue.
   *
   * @param cs the color spec
   * @return the color component
   *
   * @since 5.0.0
   */
  AgsVstColorComponent ags_vst_ichannel_context_get_blue(AgsVstColorSpec cs)
  {
    return(Steinberg::Vst::ChannelContext::GetBlue(cs));
  }

  /**
   * Get green.
   *
   * @param cs the color spec
   * @return the color component
   *
   * @since 5.0.0
   */
  AgsVstColorComponent ags_vst_ichannel_context_get_green(AgsVstColorSpec cs)
  {
    return(Steinberg::Vst::ChannelContext::GetGreen(cs));
  }

  /**
   * Get red.
   *
   * @param cs the color spec
   * @return the color component
   *
   * @since 5.0.0
   */
  AgsVstColorComponent ags_vst_ichannel_context_get_red(AgsVstColorSpec cs)
  {
    return(Steinberg::Vst::ChannelContext::GetRed(cs));
  }

  /**
   * Get alpha.
   *
   * @param cs the color spec
   * @return the color component
   *
   * @since 5.0.0
   */
  AgsVstColorComponent ags_vst_ichannel_context_get_alpha(AgsVstColorSpec cs)
  {
    return(Steinberg::Vst::ChannelContext::GetAlpha(cs));
  }

}
