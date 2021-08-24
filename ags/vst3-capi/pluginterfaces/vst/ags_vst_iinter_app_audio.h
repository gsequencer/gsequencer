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

#ifndef __AGS_VST_IINTER_APP_AUDIO_H__
#define __AGS_VST_IINTER_APP_AUDIO_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>

#ifdef __cplusplus
extern "C" {
#endif

  //FIXME:JK: add missing include
  typedef struct ViewRect AgsVstViewRect;
  typedef struct Event AgsVstEvent;
  
  typedef struct IInterAppAudioHost AgsVstIInterAppAudioHost;
  typedef struct IInterAppAudioConnectionNotification AgsVstIInterAppAudioConnectionNotification;
  typedef struct IInterAppAudioPresetManager AgsVstIInterAppAudioPresetManager;

  const AgsVstTUID* ags_vst_iinter_app_audio_host_get_iid();

  AgsVstTResult ags_vst_iinter_app_audio_host_get_screen_size(AgsVstIInterAppAudioHost *iinter_app_audio_host,
							      AgsVstViewRect *view_rect, gfloat *scale);

  AgsVstTResult ags_vst_iinter_app_audio_host_connected_to_host(AgsVstIInterAppAudioHost *iinter_app_audio_host);

  AgsVstTResult ags_vst_iinter_app_audio_host_switch_to_host(AgsVstIInterAppAudioHost *iinter_app_audio_host);

  AgsVstTResult ags_vst_iinter_app_audio_host_send_remote_control_event(AgsVstIInterAppAudioHost *iinter_app_audio_host,
									guint32 event);
  
  AgsVstTResult ags_vst_iinter_app_audio_host_get_host_icon(AgsVstIInterAppAudioHost *iinter_app_audio_host,
							    void **icon);
  
  AgsVstTResult ags_vst_iinter_app_audio_host_schedule_event_from_ui(AgsVstIInterAppAudioHost *iinter_app_audio_host,
								     AgsVstEvent *event);
  
  AgsVstIInterAppAudioPresetManager* ags_vst_iinter_app_audio_host_create_preset_manager(AgsVstIInterAppAudioHost *iinter_app_audio_host,
											 AgsVstTUID *cid);
  
  AgsVstTResult ags_vst_iinter_app_audio_host_show_settings_view(AgsVstIInterAppAudioHost *iinter_app_audio_host);

  const AgsVstTUID* ags_vst_iinter_app_audio_connection_notification_get_iid();

  void ags_vst_iinter_app_audio_connection_notification_on_inter_app_audio_connection_notification_state_change(AgsVstIInterAppAudioConnectionNotification *iinter_app_audio_connection_notification,
														gboolean new_state);

  const AgsVstTUID* ags_vst_iinter_app_audio_preset_manager_get_iid();

  AgsVstTResult ags_vst_iinter_app_audio_preset_manager_run_load_preset_browser(AgsVstIInterAppAudioPresetManager *iinter_app_audio_manager);

  AgsVstTResult ags_vst_iinter_app_audio_preset_manager_run_save_preset_browser(AgsVstIInterAppAudioPresetManager *iinter_app_audio_manager);

  AgsVstTResult ags_vst_iinter_app_audio_preset_manager_load_next_preset(AgsVstIInterAppAudioPresetManager *iinter_app_audio_manager);

  AgsVstTResult ags_vst_iinter_app_audio_preset_manager_load_previous_preset(AgsVstIInterAppAudioPresetManager *iinter_app_audio_manager);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IINTER_APP_AUDIO_H__*/
