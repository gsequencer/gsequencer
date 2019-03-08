/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/ags_sheet_editor_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

void
ags_sheet_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
					  AgsSheetEditor *sheet_editor)
{
  ags_sheet_editor_machine_changed(sheet_editor, machine);
}

void
ags_sheet_editor_resize_audio_channels_callback(AgsMachine *machine, 
						guint audio_channels, guint audio_channels_old,
						AgsSheetEditor *sheet_editor)
{
  guint i;
  
  if(audio_channels > audio_channels_old){
    GList *tab;
    
    for(i = audio_channels_old; i < audio_channels; i++){
      ags_notebook_insert_tab(sheet_editor->notebook,
			      i);

      tab = sheet_editor->notebook->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }
  }else{
    /* shrink notebook */
    for(i = audio_channels; i < audio_channels_old; i++){
      ags_notebook_remove_tab(sheet_editor->notebook,
			      audio_channels);
    }
  }
}

void
ags_sheet_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
				      guint pads, guint pads_old,
				      AgsSheetEditor *sheet_editor)
{
  AgsAudio *audio;

  audio = machine->audio;

  /* verify pads */
  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT)){
    if(!g_type_is_a(channel_type,
		    AGS_TYPE_INPUT)){
      return;
    }    
  }else{
    if(!g_type_is_a(channel_type,
		    AGS_TYPE_OUTPUT)){
      return;
    }
  }

  //TODO:JK: implement me
}
