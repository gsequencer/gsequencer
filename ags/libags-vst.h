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

#ifndef __LIBAGS_VST_H__
#define __LIBAGS_VST_H__

#include <glib.h>

#include <ags/vst3-capi/host/ags_vst_host_context.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_const_string_table.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fplatform.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fstrdefs.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_futils.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fvariant.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_geo_constants.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ibstream.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_icloneable.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ierror_context.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ipersistent.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_iplugin_base.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_istring_result.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_iupdate_handler.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_keycodes.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_smart_pointer.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ucolor_spec.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ustring.h>

#include <ags/vst3-capi/pluginterfaces/gui/ags_vst_iplug_view.h>
#include <ags/vst3-capi/pluginterfaces/gui/ags_vst_iplug_view_content_scale_support.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iattributes.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iaudio_processor.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iautomation_state.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ichannel_context_info.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_icomponent.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_icontext_menu.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iedit_controller.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ievents.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ihost_application.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iinter_app_audio.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_imessage.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_imidi_controllers.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_imidi_learn.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_inote_expression.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iparameter_changes.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iparameter_function_name.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iphysical_ui.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iplug_interface_support.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iplug_view.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iprefetchable_support.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iprocess_context.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_irepresentation.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iunits.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_preset_keys.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_speaker.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#include <ags/vst3-capi/public.sdk/source/vst/utility/ags_vst_uid.h>

#include <ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_event_list.h>
#include <ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_module.h>

#endif /*__LIBAGS_VST_H__*/
