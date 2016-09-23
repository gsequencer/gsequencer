/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/X/ags_effect_line_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recycling_context.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/recall/ags_peak_channel.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/widget/ags_indicator.h>
#include <ags/widget/ags_vindicator.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_listing_editor.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line_member.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_pad_editor.h>
#include <ags/X/ags_line_editor.h>
#include <ags/X/ags_line_member_editor.h>
#include <ags/X/ags_plugin_browser.h>
#include <ags/X/ags_ladspa_browser.h>
#include <ags/X/ags_dssi_browser.h>
#include <ags/X/ags_lv2_browser.h>

#include <ags/X/task/ags_change_indicator.h>

void
ags_effect_line_remove_recall_callback(AgsRecall *recall, AgsEffectLine *effect_line)
{
  if(recall->recall_id != NULL && recall->recall_id->recycling_context->parent != NULL){
    if(AGS_IS_RECALL_AUDIO(recall) || AGS_RECALL_AUDIO_RUN(recall)){
      ags_audio_remove_recall(AGS_AUDIO(effect_line->channel->audio), (GObject *) recall, FALSE);
    }else{
      ags_channel_remove_recall(AGS_CHANNEL(effect_line->channel), (GObject *) recall, FALSE);
    }
  }else{
    if(AGS_IS_RECALL_AUDIO(recall) || AGS_RECALL_AUDIO_RUN(recall)){
      ags_audio_remove_recall(AGS_AUDIO(effect_line->channel->audio), (GObject *) recall, TRUE);
    }else{
      ags_channel_remove_recall(AGS_CHANNEL(effect_line->channel), (GObject *) recall, TRUE);
    }
  }
}

void
ags_effect_line_add_effect_callback(AgsChannel *channel,
				    gchar *filename,
				    gchar *effect,
				    AgsEffectLine *effect_line)
{
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineMemberEditor *line_member_editor;
  AgsPluginBrowser *plugin_browser;
  
  GList *pad_editor, *pad_editor_start;
  GList *line_editor, *line_editor_start;
  GList *control_type_name;
  
  gdk_threads_enter();

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) effect_line,
						   AGS_TYPE_MACHINE);
  machine_editor = (AgsMachineEditor *) machine->properties;

  control_type_name = NULL;  

  if(machine_editor != NULL){
    pad_editor_start = 
      pad_editor = gtk_container_get_children((GtkContainer *) machine_editor->input_editor->child);
    pad_editor = g_list_nth(pad_editor,
			    channel->pad);
    
    if(pad_editor != NULL){
      line_editor_start =
	line_editor = gtk_container_get_children((GtkContainer *) AGS_PAD_EDITOR(pad_editor->data)->line_editor);
      line_editor = g_list_nth(line_editor,
			       channel->audio_channel);
  
      g_list_free(pad_editor);
    }else{
      line_editor = NULL;
    }

    if(line_editor != NULL){
      line_member_editor = AGS_LINE_EDITOR(line_editor->data)->member_editor;

      plugin_browser = line_member_editor->plugin_browser;

      if(plugin_browser != NULL &&
	 plugin_browser->active_browser != NULL){
	GList *description, *description_start;
	GList *port_control, *port_control_start;

	gchar *controls;
	
	if(AGS_IS_LADSPA_BROWSER(plugin_browser->active_browser)){
	  description_start = 
	    description = gtk_container_get_children((GtkContainer *) AGS_LADSPA_BROWSER(plugin_browser->active_browser)->description);
	}else if(AGS_IS_DSSI_BROWSER(plugin_browser->active_browser)){
	  description_start = 
	    description = gtk_container_get_children((GtkContainer *) AGS_DSSI_BROWSER(plugin_browser->active_browser)->description);
	}else if(AGS_IS_LV2_BROWSER(plugin_browser->active_browser)){
	  description_start = 
	    description = gtk_container_get_children((GtkContainer *) AGS_LV2_BROWSER(plugin_browser->active_browser)->description);
	}else{
	  g_message("ags_effect_line_callbacks.c unsupported plugin browser\0");
	}

	if(description != NULL){
	  description = g_list_last(description);
	  
	  port_control_start =
	    port_control = gtk_container_get_children(GTK_CONTAINER(description->data));
	  g_list_free(description_start);
	  
	  if(port_control != NULL){
	    while(port_control != NULL){
	      controls = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(port_control->data));

	      if(!g_ascii_strncasecmp(controls,
				      "spin button\0",
				      12)){
		control_type_name = g_list_prepend(control_type_name,
						   "GtkSpinButton\0");
	      }else if(!g_ascii_strncasecmp(controls,
					    "dial\0",
					    5)){
		control_type_name = g_list_prepend(control_type_name,
						   "AgsDial\0");
	      }else if(!g_ascii_strncasecmp(controls,
					    "vertical scale\0",
					    15)){
		control_type_name = g_list_prepend(control_type_name,
						   "GtkVScale\0");
	      }else if(!g_ascii_strncasecmp(controls,
					    "horizontal scale\0",
					    17)){
		control_type_name = g_list_prepend(control_type_name,
						   "GtkHScale\0");
	      }else if(!g_ascii_strncasecmp(controls,
					    "check-button\0",
					    13)){
		control_type_name = g_list_prepend(control_type_name,
						   "GtkCheckButton\0");
	      }else if(!g_ascii_strncasecmp(controls,
					    "toggle button\0",
					    14)){
		control_type_name = g_list_prepend(control_type_name,
						   "GtkToggleButton\0");
	      }
	      
	      port_control = port_control->next;
	      port_control = port_control->next;
	    }
	  }
	}
      }
      
      line_member_editor->plugin_browser;
    }
  }else{
    control_type_name = NULL;
  }
  
  ags_effect_line_add_effect(effect_line,
			     control_type_name,
			     filename,
			     effect);

  gdk_threads_leave();
}

void
ags_effect_line_remove_effect_callback(AgsChannel *channel,
				       guint nth,
				       AgsEffectLine *effect_line)
{
  gdk_threads_enter();

  ags_effect_line_remove_effect(effect_line,
				nth);

  gdk_threads_leave();
}
