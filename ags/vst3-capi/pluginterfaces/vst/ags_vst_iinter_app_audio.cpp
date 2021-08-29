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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iinter_app_audio.h>

#include <pluginterfaces/vst/ivstinterappaudio.h>
#include <pluginterfaces/vst/ivstevents.h>

extern "C" {

  const AgsVstTUID* ags_vst_iinter_app_audio_host_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IInterAppAudioHost::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iinter_app_audio_host_get_screen_size(AgsVstIInterAppAudioHost *iinter_app_audio_host,
							      AgsVstViewRect *view_rect, gfloat *scale)
  {
    return(((Steinberg::Vst::IInterAppAudioHost *) iinter_app_audio_host)->getScreenSize((Steinberg::ViewRect *) view_rect, scale));
  }

  AgsVstTResult ags_vst_iinter_app_audio_host_connected_to_host(AgsVstIInterAppAudioHost *iinter_app_audio_host)
  {
    return(((Steinberg::Vst::IInterAppAudioHost *) iinter_app_audio_host)->connectedToHost());
  }

  AgsVstTResult ags_vst_iinter_app_audio_host_switch_to_host(AgsVstIInterAppAudioHost *iinter_app_audio_host)
  {
    return(((Steinberg::Vst::IInterAppAudioHost *) iinter_app_audio_host)->switchToHost());
  }

  AgsVstTResult ags_vst_iinter_app_audio_host_send_remote_control_event(AgsVstIInterAppAudioHost *iinter_app_audio_host,
									guint32 event)
  {
    return(((Steinberg::Vst::IInterAppAudioHost *) iinter_app_audio_host)->sendRemoteControlEvent(event));
  }
  
  AgsVstTResult ags_vst_iinter_app_audio_host_get_host_icon(AgsVstIInterAppAudioHost *iinter_app_audio_host,
							    void **icon)
  {
    return(((Steinberg::Vst::IInterAppAudioHost *) iinter_app_audio_host)->getHostIcon(icon));
  }
  
  AgsVstTResult ags_vst_iinter_app_audio_host_schedule_event_from_ui(AgsVstIInterAppAudioHost *iinter_app_audio_host,
								     AgsVstEvent *event)
  {
    Steinberg::Vst::Event *tmp_event_0 = (Steinberg::Vst::Event *) event;
    const Steinberg::Vst::Event& tmp_event_1 = const_cast<Steinberg::Vst::Event&>(tmp_event_0[0]);
    
    return(((Steinberg::Vst::IInterAppAudioHost *) iinter_app_audio_host)->scheduleEventFromUI(const_cast<Steinberg::Vst::Event&>(tmp_event_1)));
  }
  
  AgsVstIInterAppAudioPresetManager* ags_vst_iinter_app_audio_host_create_preset_manager(AgsVstIInterAppAudioHost *iinter_app_audio_host,
											 AgsVstTUID *cid)
  {
    Steinberg::TUID *tmp_cid = (Steinberg::TUID *) cid;

    return((AgsVstIInterAppAudioPresetManager *) ((Steinberg::Vst::IInterAppAudioHost *) iinter_app_audio_host)->createPresetManager(tmp_cid[0]));
  }
  
  AgsVstTResult ags_vst_iinter_app_audio_host_show_settings_view(AgsVstIInterAppAudioHost *iinter_app_audio_host)
  {
    return(((Steinberg::Vst::IInterAppAudioHost *) iinter_app_audio_host)->showSettingsView());
  }

  const AgsVstTUID* ags_vst_iinter_app_audio_connection_notification_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IInterAppAudioConnectionNotification::iid.toTUID()));
  }

  void ags_vst_iinter_app_audio_connection_notification_on_inter_app_audio_connection_notification_state_change(AgsVstIInterAppAudioConnectionNotification *iinter_app_audio_connection_notification,
														gboolean new_state)
  {
    ((Steinberg::Vst::IInterAppAudioConnectionNotification *) iinter_app_audio_connection_notification)->onInterAppAudioConnectionStateChange(new_state);
  }

  const AgsVstTUID* ags_vst_iinter_app_audio_preset_manager_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IInterAppAudioPresetManager::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iinter_app_audio_preset_manager_run_load_preset_browser(AgsVstIInterAppAudioPresetManager *iinter_app_audio_manager)
  {
    return(((Steinberg::Vst::IInterAppAudioPresetManager *) iinter_app_audio_manager)->runLoadPresetBrowser());
  }

  AgsVstTResult ags_vst_iinter_app_audio_preset_manager_run_save_preset_browser(AgsVstIInterAppAudioPresetManager *iinter_app_audio_manager)
  {
    return(((Steinberg::Vst::IInterAppAudioPresetManager *) iinter_app_audio_manager)->runSavePresetBrowser());
  }

  AgsVstTResult ags_vst_iinter_app_audio_preset_manager_load_next_preset(AgsVstIInterAppAudioPresetManager *iinter_app_audio_manager)
  {
    return(((Steinberg::Vst::IInterAppAudioPresetManager *) iinter_app_audio_manager)->loadNextPreset());
  }

  AgsVstTResult ags_vst_iinter_app_audio_preset_manager_load_previous_preset(AgsVstIInterAppAudioPresetManager *iinter_app_audio_manager)
  {
    return(((Steinberg::Vst::IInterAppAudioPresetManager *) iinter_app_audio_manager)->loadPreviousPreset());
  }

}

