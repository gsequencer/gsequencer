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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ichannel_context_info.h>

#include <pluginterfaces/vst/ivstchannelcontextinfo.h>

extern "C" {

  const AgsVstTUID* ags_vst_iinfo_listener_get_iid()
  {
    extern const Steinberg::TUID IInfoListener__iid;

    return((AgsVstTUID *) (&IInfoListener__iid));
  }

  AgsVstTResult ags_vst_iinfo_listener_set_channel_context_info(AgsVstIInfoListener *iinfo_listener,
								AgsVstIAttributeList *list)
  {
    return(((Steinberg::Vst::ChannelContext::IInfoListener *) iinfo_listener)->setChannelContextInfos((Steinberg::Vst::IAttributeList *) list));
  }

  AgsVstColorComponent ags_vst_ichannel_context_get_blue(AgsVstColorSpec cs)
  {
    return(Steinberg::Vst::ChannelContext::GetBlue(cs));
  }

  AgsVstColorComponent ags_vst_ichannel_context_get_green(AgsVstColorSpec cs)
  {
    return(Steinberg::Vst::ChannelContext::GetGreen(cs));
  }

  AgsVstColorComponent ags_vst_ichannel_context_get_red(AgsVstColorSpec cs)
  {
    return(Steinberg::Vst::ChannelContext::GetRed(cs));
  }

  AgsVstColorComponent ags_vst_ichannel_context_get_alpha(AgsVstColorSpec cs)
  {
    return(Steinberg::Vst::ChannelContext::GetAlpha(cs));
  }

}
