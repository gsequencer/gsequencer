/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/ags_line_member_editor_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_pad.h>
#include <ags/X/ags_effect_line.h>
#include <ags/X/ags_line_member.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>
#include <ags/X/ags_lv2_browser.h>
#if defined(AGS_WITH_VST3)
#include <ags/X/ags_vst3_browser.h>
#endif
#include <ags/X/ags_ladspa_browser.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

void ags_line_member_editor_plugin_browser_response_create_entry(AgsLineMemberEditor *line_member_editor,
								 gchar *filename, gchar *effect);
  
void
ags_line_member_editor_add_callback(GtkWidget *button,
				    AgsLineMemberEditor *line_member_editor)
{
  if(line_member_editor->plugin_browser == NULL){
    line_member_editor->plugin_browser = ags_plugin_browser_new((GtkWidget *) line_member_editor);

    ags_connectable_connect(AGS_CONNECTABLE(line_member_editor->plugin_browser));

    g_signal_connect(G_OBJECT(line_member_editor->plugin_browser), "response",
		     G_CALLBACK(ags_line_member_editor_plugin_browser_response_callback), line_member_editor);
  }
  
  gtk_widget_show_all((GtkWidget *) line_member_editor->plugin_browser);
}

void
ags_line_member_editor_plugin_browser_response_create_entry(AgsLineMemberEditor *line_member_editor,
							    gchar *filename, gchar *effect)
{
  GtkBox *hbox;
  GtkCheckButton *check_button;
  GtkLabel *label;

  gchar *str;
  
  /* create entry */
  hbox = (GtkBox *) gtk_hbox_new(GTK_ORIENTATION_HORIZONTAL,
				 0);
  gtk_box_pack_start(line_member_editor->line_member,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);
      
  check_button = (GtkCheckButton *) gtk_check_button_new();
  gtk_box_pack_start(hbox,
		     (GtkWidget *) check_button,
		     FALSE, FALSE,
		     0);

  str = g_strdup_printf("%s - %s",
			filename,
			effect);
    
  label = (GtkLabel *) gtk_label_new(str);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);
  gtk_widget_show_all((GtkWidget *) hbox);

  g_free(str);
}

void
ags_line_member_editor_plugin_browser_response_callback(GtkDialog *dialog,
							gint response,
							AgsLineMemberEditor *line_member_editor)
{
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsPluginBrowser *plugin_browser;
    
  GList *pad, *pad_start;
  GList *list, *list_start;
  GList *control_type_name;

  gchar *plugin_name;
  gchar *filename, *effect;

  gint active_plugin_type;
  gint position;
  gboolean has_bridge;
  gboolean is_output;  

  plugin_browser = AGS_PLUGIN_BROWSER(line_member_editor->plugin_browser);
  
  switch(response){
  case GTK_RESPONSE_ACCEPT:
    {
      machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
								    AGS_TYPE_MACHINE_EDITOR);
      line_editor = (AgsLineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
							      AGS_TYPE_LINE_EDITOR);

      machine = machine_editor->machine;

      position = 0;

      //NOTE:JK: related to ags-fx-buffer
      if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
	 (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0 ||
	 (AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
	position = 1;
      }
      
      if(AGS_IS_OUTPUT(line_editor->channel)){
	is_output = TRUE;
      }else{
	is_output = FALSE;
      }

      if(machine->bridge != NULL){
	has_bridge = TRUE;
      }else{
	has_bridge = FALSE;
      }

      active_plugin_type = gtk_combo_box_get_active(plugin_browser->plugin_type);
      plugin_name = NULL;

      if(AGS_IS_LADSPA_BROWSER(plugin_browser->active_browser)){
	plugin_name = "ags-fx-ladspa";
      }else if(AGS_IS_LV2_BROWSER(plugin_browser->active_browser)){
	plugin_name = "ags-fx-lv2";
#if defined(AGS_WITH_VST3)
      }else if(AGS_IS_VST3_BROWSER(plugin_browser->active_browser)){
	plugin_name = "ags-fx-vst3";
#endif
      }

      /* get control type */
      control_type_name = NULL;  

      if(plugin_browser != NULL &&
	 plugin_browser->active_browser != NULL){
	GList *description, *description_start;
	GList *port_control, *port_control_start;

	gchar *controls;

	/* get plugin browser */
	description = 
	  description_start = NULL;
	port_control_start = NULL;
		
	if(AGS_IS_LADSPA_BROWSER(plugin_browser->active_browser)){
	  description_start = 
	    description = gtk_container_get_children((GtkContainer *) AGS_LADSPA_BROWSER(plugin_browser->active_browser)->description);
	}else if(AGS_IS_LV2_BROWSER(plugin_browser->active_browser)){
	  description_start = 
	    description = gtk_container_get_children((GtkContainer *) AGS_LV2_BROWSER(plugin_browser->active_browser)->description);
#if defined(AGS_WITH_VST3)
	}else if(AGS_IS_VST3_BROWSER(plugin_browser->active_browser)){
	  description_start = 
	    description = gtk_container_get_children((GtkContainer *) AGS_VST3_BROWSER(plugin_browser->active_browser)->description);
#endif
	}else{
	  g_message("ags_line_member_editor_callbacks.c unsupported plugin browser");
	}

	/* get port description */
	if(description != NULL){
	  description = g_list_last(description);
	  
	  port_control_start =
	    port_control = gtk_container_get_children(GTK_CONTAINER(description->data));
	  
	  if(port_control != NULL){
	    while(port_control != NULL){
	      controls = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(port_control->data));

	      if(!g_ascii_strncasecmp(controls,
				      "led",
				      4)){
		control_type_name = g_list_prepend(control_type_name,
						   "AgsLed");
	      }else if(!g_ascii_strncasecmp(controls,
					    "vertical indicator",
					    19)){
		control_type_name = g_list_prepend(control_type_name,
						   "AgsVIndicator");
	      }else if(!g_ascii_strncasecmp(controls,
					    "horizontal indicator",
					    19)){
		control_type_name = g_list_prepend(control_type_name,
						   "AgsHIndicator");
	      }else if(!g_ascii_strncasecmp(controls,
					    "spin button",
					    12)){
		control_type_name = g_list_prepend(control_type_name,
						   "GtkSpinButton");
	      }else if(!g_ascii_strncasecmp(controls,
					    "dial",
					    5)){
		control_type_name = g_list_prepend(control_type_name,
						   "AgsDial");
	      }else if(!g_ascii_strncasecmp(controls,
					    "vertical scale",
					    15)){
		control_type_name = g_list_prepend(control_type_name,
						   "GtkVScale");
	      }else if(!g_ascii_strncasecmp(controls,
					    "horizontal scale",
					    17)){
		control_type_name = g_list_prepend(control_type_name,
						   "GtkHScale");
	      }else if(!g_ascii_strncasecmp(controls,
					    "check-button",
					    13)){
		control_type_name = g_list_prepend(control_type_name,
						   "GtkCheckButton");
	      }else if(!g_ascii_strncasecmp(controls,
					    "toggle button",
					    14)){
		control_type_name = g_list_prepend(control_type_name,
						   "GtkToggleButton");
	      }
	      
	      port_control = port_control->next;
	      port_control = port_control->next;
	    }
	  }

	  /* free lists */
	  g_list_free(description_start);
	  g_list_free(port_control_start);
	}
      }

      /* retrieve plugin */
      filename = ags_plugin_browser_get_plugin_filename(line_member_editor->plugin_browser);
      effect = ags_plugin_browser_get_plugin_effect(line_member_editor->plugin_browser);
      
      if(!has_bridge){	
	AgsLine *line;
	
	/* find pad and line */
	line = NULL;
	
	if(is_output){
	  pad_start = 
	    pad = gtk_container_get_children((GtkContainer *) machine_editor->machine->output);
	}else{
	  pad_start = 
	    pad = gtk_container_get_children((GtkContainer *) machine_editor->machine->input);
	}

	pad = g_list_nth(pad,
			 line_editor->channel->pad);

	if(pad != NULL){
	  list_start =
	    list = gtk_container_get_children((GtkContainer *) AGS_PAD(pad->data)->expander_set);

	  while(list != NULL){
	    if(AGS_LINE(list->data)->channel == line_editor->channel){
	      break;
	    }

	    list = list->next;
	  }

	  if(list != NULL){
	    line = AGS_LINE(list->data);
	    g_list_free(list_start);
	  }
	}

	g_list_free(pad_start);

	if(filename != NULL &&
	   effect != NULL &&
	   line != NULL){
	  guint audio_channel;
	  guint pad;

	  g_object_get(line->channel,
		       "audio-channel", &audio_channel,
		       "pad", &pad,
		       NULL);
	  
	  ags_line_add_plugin(line,
			      control_type_name,
			      ags_recall_container_new(), ags_recall_container_new(),
			      plugin_name,
			      filename,
			      effect,
			      audio_channel, audio_channel + 1,
			      pad, pad + 1,
			      position,
			      (AGS_FX_FACTORY_ADD | (is_output ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT)), 0);
	}
	
	g_free(filename);
	g_free(effect);
      }else{
	AgsEffectBridge *effect_bridge;
	AgsEffectLine *effect_line;
	
	effect_bridge = (AgsEffectBridge *) machine->bridge;
	effect_line = NULL;
	
	/* find effect pad and effect line */
	if(is_output){
	  pad_start = 
	    pad = gtk_container_get_children((GtkContainer *) effect_bridge->output);
	}else{
	  pad_start = 
	    pad = gtk_container_get_children((GtkContainer *) effect_bridge->input);
	}

	pad = g_list_nth(pad,
			 line_editor->channel->pad);

	if(pad != NULL){
	  list_start =
	    list = gtk_container_get_children((GtkContainer *) AGS_EFFECT_PAD(pad->data)->grid);

	  while(list != NULL){
	    if(AGS_EFFECT_LINE(list->data)->channel == line_editor->channel){
	      break;
	    }

	    list = list->next;
	  }

	  if(list != NULL){
	    effect_line = AGS_EFFECT_LINE(list->data);
	    g_list_free(list_start);
	  }
	}

	g_list_free(pad_start);

	if(filename != NULL &&
	   effect != NULL &&
	   effect_line != NULL){
	  guint audio_channel;
	  guint pad;

	  g_object_get(effect_line->channel,
		       "audio-channel", &audio_channel,
		       "pad", &pad,
		       NULL);

	  ags_effect_line_add_plugin(effect_line,
				     control_type_name,
				     ags_recall_container_new(), ags_recall_container_new(),
				     plugin_name,
				     filename,
				     effect,
				     audio_channel, audio_channel + 1,
				     pad, pad + 1,
				     position,
				     (AGS_FX_FACTORY_ADD | (is_output ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT)), 0);
	}
	
	g_free(filename);
	g_free(effect);
      }

      ags_applicable_reset(AGS_APPLICABLE(line_member_editor));
    }
    break;      
  }
		
  if(AGS_IS_LADSPA_BROWSER(plugin_browser->active_browser)){
    gtk_combo_box_set_active(AGS_LADSPA_BROWSER(plugin_browser->active_browser)->filename,
			     -1);
  }else if(AGS_IS_LV2_BROWSER(plugin_browser->active_browser)){
    gtk_combo_box_set_active(AGS_LV2_BROWSER(plugin_browser->active_browser)->filename,
			     -1);
#if defined(AGS_WITH_VST3)
  }else if(AGS_IS_VST3_BROWSER(plugin_browser->active_browser)){
    gtk_combo_box_set_active(AGS_VST3_BROWSER(plugin_browser->active_browser)->filename,
			     -1);
#endif
  }

  gtk_combo_box_set_active(plugin_browser->plugin_type,
			   -1);
}

void
ags_line_member_editor_remove_callback(GtkWidget *button,
				       AgsLineMemberEditor *line_member_editor)
{
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;

  GList *start_line_member, *line_member;
  GList *list, *list_start, *pad, *pad_start;
  GList *children;

  guint nth;
  gboolean has_bridge;
  gboolean is_output;
  
  if(button == NULL ||
     line_member_editor == NULL){
    return;
  }

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
								AGS_TYPE_MACHINE_EDITOR);
  line_editor = (AgsLineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
							  AGS_TYPE_LINE_EDITOR);

  line_member =
    start_line_member = gtk_container_get_children(GTK_CONTAINER(line_member_editor->line_member));

  machine = machine_editor->machine;

  if(AGS_IS_OUTPUT(line_editor->channel)){
    is_output = TRUE;
  }else{
    is_output = FALSE;
  }

  if(machine->bridge != NULL){
    has_bridge = TRUE;
  }else{
    has_bridge = FALSE;
  }

  if(!has_bridge){	
    AgsLine *line;
    
    /* retrieve line and pad */
    line = NULL;

    if(AGS_IS_OUTPUT(line_editor->channel)){
      pad_start = 
	pad = gtk_container_get_children((GtkContainer *) machine->output);
    }else{
      pad_start = 
	pad = gtk_container_get_children((GtkContainer *) machine->input);
    }

    pad = g_list_nth(pad,
		     line_editor->channel->pad);

    if(pad != NULL){
      list_start =
	list = gtk_container_get_children((GtkContainer *) AGS_PAD(pad->data)->expander_set);

      while(list != NULL){
	if(AGS_LINE(list->data)->channel == line_editor->channel){
	  break;
	}

	list = list->next;
      }

      if(list != NULL){
	line = AGS_LINE(list->data);
	g_list_free(list_start);
      }
    }

    g_list_free(pad_start);

    /* iterate line member */
    if(line != NULL){
      for(nth = 0; line_member != NULL;){
	children = gtk_container_get_children((GtkContainer *) GTK_CONTAINER(line_member->data));

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(children->data))){
	  /* remove effect */
	  ags_line_remove_plugin(line,
				 nth);
	}

	g_list_free(children);

	nth++;
	
	line_member = line_member->next;
      }
    }
  }else{
    AgsEffectBridge *effect_bridge;
    AgsEffectLine *effect_line;
	
    effect_bridge = AGS_EFFECT_BRIDGE(machine->bridge);

    effect_line = NULL;
    
    /* retrieve effect line and effect pad */
    if(is_output){
      pad_start = 
	pad = gtk_container_get_children((GtkContainer *) effect_bridge->output);
    }else{
      pad_start = 
	pad = gtk_container_get_children((GtkContainer *) effect_bridge->input);
    }

    pad = g_list_nth(pad,
		     line_editor->channel->pad);

    if(pad != NULL){
      list_start =
	list = gtk_container_get_children((GtkContainer *) AGS_EFFECT_PAD(pad->data)->grid);

      while(list != NULL){
	if(AGS_EFFECT_LINE(list->data)->channel == line_editor->channel){
	  break;
	}

	list = list->next;
      }

      if(list != NULL){
	effect_line = AGS_EFFECT_LINE(list->data);
	g_list_free(list_start);
      }
    }

    g_list_free(pad_start);

    /* iterate line member */
    if(effect_line != NULL){
      for(nth = 0; line_member != NULL;){
	children = gtk_container_get_children((GtkContainer *) GTK_CONTAINER(line_member->data));

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(children->data))){
	  /* remove effect */
	  ags_effect_line_remove_plugin(effect_line,
					nth);
	}

	g_list_free(children);

	line_member = line_member->next;
	
	nth++;
      }
    }
  }

  ags_applicable_reset(AGS_APPLICABLE(line_member_editor));

  g_list_free(start_line_member);
}
