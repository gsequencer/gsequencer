/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/machine/ags_audio_unit_bridge_callbacks.h>

#include <ags/ags_api_config.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>

#if defined(AGS_WITH_AUDIO_UNIT_PLUGINS)
#include <CoreFoundation/CoreFoundation.h>
#include <AVFoundation/AVFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/AUComponent.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <CoreAudio/CoreAudio.h>
#include <AppKit/AppKit.h>
#include <CoreAudioKit/CoreAudioKit.h>
#endif

void
ags_audio_unit_bridge_show_audio_unit_ui_callback(GAction *action, GVariant *parameter,
						  AgsAudioUnitBridge *audio_unit_bridge)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  AUAudioUnit *au_audio_unit;
  
  if(!AGS_IS_AUDIO_UNIT_BRIDGE(audio_unit_bridge)){
    return;
  }
  
  /* load plugin */
  audio_unit_plugin = ags_audio_unit_manager_find_audio_unit_plugin(ags_audio_unit_manager_get_instance(),
								    audio_unit_bridge->filename,
								    audio_unit_bridge->effect);

  if(audio_unit_plugin == NULL){
    return;
  }

#if defined(AGS_WITH_AUDIO_UNIT_PLUGINS)  
  au_audio_unit = [(AVAudioNode *) audio_unit_bridge->av_audio_unit AUAudioUnit];

  [au_audio_unit requestViewControllerWithCompletionHandler:^(AUViewControllerBase *viewController){
      [(AUViewController *) viewController loadView];
      [(AUViewController *) viewController viewDidLoad];
    }];
#endif
}

void
ags_audio_unit_bridge_dial_changed_callback(GtkWidget *dial, AgsAudioUnitBridge *audio_unit_bridge)
{
  AgsBulkMember *bulk_member;
  
  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(dial,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me
}

void
ags_audio_unit_bridge_scale_changed_callback(GtkWidget *scale, AgsAudioUnitBridge *audio_unit_bridge)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(scale,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me
}

void
ags_audio_unit_bridge_spin_button_changed_callback(GtkWidget *spin_button, AgsAudioUnitBridge *audio_unit_bridge)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(spin_button,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me
}

void
ags_audio_unit_bridge_check_button_clicked_callback(GtkWidget *check_button, AgsAudioUnitBridge *audio_unit_bridge)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(check_button,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me
}

void
ags_audio_unit_bridge_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsAudioUnitBridge *audio_unit_bridge)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(toggle_button,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me
}

void
ags_audio_unit_bridge_button_clicked_callback(GtkWidget *button, AgsAudioUnitBridge *audio_unit_bridge)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(button,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me

}
