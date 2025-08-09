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

#include <ags/app/machine/ags_live_audio_unit_bridge_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>

#if defined(AGS_WITH_QUARTZ)
#import <CoreFoundation/CoreFoundation.h>
#import <Cocoa/Cocoa.h>
#endif

void
ags_live_audio_unit_bridge_show_audio_unit_ui_callback(GAction *action, GVariant *parameter,
						       AgsLiveAudioUnitBridge *live_audio_unit_bridge)
{ 
  //TODO:JK: implement me
}

void
ags_live_audio_unit_bridge_dial_changed_callback(GtkWidget *dial, AgsLiveAudioUnitBridge *live_audio_unit_bridge)
{
  AgsBulkMember *bulk_member;
  
  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(dial,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me
}

void
ags_live_audio_unit_bridge_scale_changed_callback(GtkWidget *scale, AgsLiveAudioUnitBridge *live_audio_unit_bridge)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(scale,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me
}

void
ags_live_audio_unit_bridge_spin_button_changed_callback(GtkWidget *spin_button, AgsLiveAudioUnitBridge *live_audio_unit_bridge)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(spin_button,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me
}

void
ags_live_audio_unit_bridge_check_button_clicked_callback(GtkWidget *check_button, AgsLiveAudioUnitBridge *live_audio_unit_bridge)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(check_button,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me
}

void
ags_live_audio_unit_bridge_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsLiveAudioUnitBridge *live_audio_unit_bridge)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(toggle_button,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me
}

void
ags_live_audio_unit_bridge_button_clicked_callback(GtkWidget *button, AgsLiveAudioUnitBridge *live_audio_unit_bridge)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(button,
							  AGS_TYPE_BULK_MEMBER);
  
  //TODO:JK: implement me

}
