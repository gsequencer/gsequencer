/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_automation_editor_callbacks.h>

#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

void
ags_automation_editor_set_audio_channels_callback(AgsAudio *audio,
						  guint audio_channels, guint audio_channels_old,
						  AgsAutomationEditor *automation_editor)
{
  guint i, j;

  if(audio_channels_old < audio_channels){
    /* output */
    for(i = 0; i < audio->output_pads; i++){
      for(j = audio_channels_old; j < audio_channels; j++){
	ags_notebook_insert_tab(automation_editor->output_notebook,
				i * audio_channels + j);
      }
    }

    /* input */
    for(i = 0; i < audio->input_pads; i++){
      for(j = audio_channels_old; j < audio_channels; j++){
	ags_notebook_insert_tab(automation_editor->input_notebook,
				i * audio_channels + j);
      }
    }
  }else{
    /* output */
    for(i = 0; i < audio->output_pads; i++){
      for(j = audio_channels; j < audio_channels_old; j++){
	ags_notebook_remove_tab(automation_editor->output_notebook,
				i * audio_channels + j);
      }
    }

    /* input */
    for(i = 0; i < audio->input_pads; i++){
      for(j = audio_channels; j < audio_channels_old; j++){
	ags_notebook_remove_tab(automation_editor->input_notebook,
				i * audio_channels + j);
      }
    }
  }
}

void
ags_automation_editor_set_pads_callback(AgsAudio *audio,
					GType channel_type,
					guint pads, guint pads_old,
					AgsAutomationEditor *automation_editor)
{
  guint i, j;
  
  if(g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
    /* output */
    if(pads > pads_old){
      /* grow */
      for(i = pads_old; i < pads; i++){
	for(j = 0; j < audio->audio_channels; j++){
	  ags_notebook_add_tab(automation_editor->output_notebook);
	}
      }
    }else{
      /* shrink */
      for(i = pads_old; i < pads; i++){
	for(j = 0; j < audio->audio_channels; j++){
	  ags_notebook_remove_tab(automation_editor->output_notebook,
				  i * audio->audio_channels + j);
	}
      }
    }
  }else if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    /* input */
    if(pads > pads_old){
      /* grow */
      for(i = pads_old; i < pads; i++){
	for(j = 0; j < audio->audio_channels; j++){
	  ags_notebook_add_tab(automation_editor->input_notebook);
	}
      }
    }else{
      /* shrink */
      for(i = pads_old; i < pads; i++){
	for(j = 0; j < audio->audio_channels; j++){
	  ags_notebook_remove_tab(automation_editor->input_notebook,
				  i * audio->audio_channels + j);
	}
      }
    }
  }
}

void
ags_automation_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
					       AgsAutomationEditor *automation_editor)
{
  ags_automation_editor_machine_changed(automation_editor,
					machine);
}

void
ags_automation_editor_change_position_callback(AgsNavigation *navigation, gdouble tact,
					       AgsAutomationEditor *automation_editor)
{
  //TODO:JK: implement me
}

void
ags_automation_editor_edit_vscrollbar_value_changed_callback(GtkWidget *automation_edit,
							     AgsAutomationEditor *automation_editor)
{
  //TODO:JK: implement me
}
