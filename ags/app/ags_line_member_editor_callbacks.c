/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_line_member_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_pad.h>
#include <ags/app/ags_effect_line.h>
#include <ags/app/ags_line_member.h>
#include <ags/app/ags_machine_editor.h>
#include <ags/app/ags_machine_editor_line.h>
#include <ags/app/ags_lv2_browser.h>
#if defined(AGS_WITH_VST3)
#include <ags/app/ags_vst3_browser.h>
#endif
#include <ags/app/ags_ladspa_browser.h>

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
    line_member_editor->plugin_browser = ags_plugin_browser_new((GtkWidget *) gtk_widget_get_ancestor(line_member_editor,
												      GTK_TYPE_WINDOW));

    ags_connectable_connect(AGS_CONNECTABLE(line_member_editor->plugin_browser));

    g_signal_connect(G_OBJECT(line_member_editor->plugin_browser), "response",
		     G_CALLBACK(ags_line_member_editor_plugin_browser_response_callback), line_member_editor);
  }
  
  gtk_widget_show((GtkWidget *) line_member_editor->plugin_browser);
}

void
ags_line_member_editor_plugin_browser_response_create_entry(AgsLineMemberEditor *line_member_editor,
							    gchar *filename, gchar *effect)
{
  AgsLineMemberEditorEntry *entry;

  gchar *str;
  
  /* create entry */
  entry = ags_line_member_editor_entry_new();

  str = g_strdup_printf("%s - %s",
			filename,
			effect);

  gtk_label_set_text(entry->label,
		     str);
  
  ags_line_member_editor_add_entry(line_member_editor,
				   entry);
  
  gtk_widget_show((GtkWidget *) entry);

  g_free(str);
}

void
ags_line_member_editor_plugin_browser_response_callback(GtkDialog *dialog,
							gint response,
							AgsLineMemberEditor *line_member_editor)
{
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorLine *machine_editor_line;
  AgsPluginBrowser *plugin_browser;
    
  GList *start_pad, *pad;
  GList *start_line, *line;
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
      machine_editor_line = (AgsMachineEditorLine *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
									     AGS_TYPE_MACHINE_EDITOR_LINE);

      machine = machine_editor->machine;

      position = 0;

      //NOTE:JK: related to ags-fx-buffer
      if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
	 (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0 ||
	 (AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
	position = 1;
      }
      
      if(AGS_IS_OUTPUT(machine_editor_line->channel)){
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
	GList *start_port_editor, *port_editor;

	/* get port editor */
	start_port_editor = NULL;
		
	if(AGS_IS_LADSPA_BROWSER(plugin_browser->active_browser)){
	  port_editor =
	    start_port_editor = ags_ladspa_browser_get_port_editor(plugin_browser->active_browser);
	}else if(AGS_IS_LV2_BROWSER(plugin_browser->active_browser)){
	  port_editor =
	    start_port_editor = ags_lv2_browser_get_port_editor(plugin_browser->active_browser);
#if defined(AGS_WITH_VST3)
	}else if(AGS_IS_VST3_BROWSER(plugin_browser->active_browser)){
	  port_editor =
	    start_port_editor = ags_vst3_browser_get_port_editor(plugin_browser->active_browser);
#endif
	}else{
	  g_message("ags_line_member_editor_callbacks.c unsupported plugin browser");
	}
	  
	while(port_editor != NULL){
	  GtkTreeModel *model;
	  
	  GtkTreeIter iter;

	  gchar *control;

	  model = gtk_combo_box_get_model(AGS_PORT_EDITOR(port_editor->data)->port_control);

	  gtk_combo_box_get_active_iter(AGS_PORT_EDITOR(port_editor->data)->port_control,
					&iter);

	  control = NULL;

	  gtk_tree_model_get(model,
			     &iter,
			     0, &control,
			     -1);
	  
	  control_type_name = g_list_prepend(control_type_name,
					     control);
	      
	  port_editor = port_editor->next;
	}

	/* free lists */
	g_list_free(start_port_editor);
      }

      /* retrieve plugin */
      filename = ags_plugin_browser_get_plugin_filename(line_member_editor->plugin_browser);
      effect = ags_plugin_browser_get_plugin_effect(line_member_editor->plugin_browser);
      
      if(!has_bridge){	
	AgsLine *current_line;
	
	/* find pad and line */
	current_line = NULL;
	
	if(is_output){
	    pad =
	      start_pad = ags_machine_get_output_pad(machine_editor->machine);
	}else{
	    pad =
	      start_pad = ags_machine_get_input_pad(machine_editor->machine);
	}

	pad = g_list_nth(pad,
			 machine_editor_line->channel->pad);

	if(pad != NULL){
	  line =
	    start_line = ags_pad_get_line(AGS_PAD(pad->data));

	  while(line != NULL){
	    if(AGS_LINE(line->data)->channel == machine_editor_line->channel){
	      break;
	    }

	    line = line->next;
	  }

	  if(line != NULL){
	    current_line = AGS_LINE(line->data);
	    g_list_free(start_line);
	  }
	}

	g_list_free(start_pad);

	if(filename != NULL &&
	   effect != NULL &&
	   current_line != NULL){
	  guint audio_channel;
	  guint pad;

	  g_object_get(current_line->channel,
		       "audio-channel", &audio_channel,
		       "pad", &pad,
		       NULL);
	  
	  ags_line_add_plugin(current_line,
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
	AgsEffectLine *current_effect_line;
	
	effect_bridge = (AgsEffectBridge *) machine->bridge;
	current_effect_line = NULL;
	
	/* find effect pad and effect line */
	if(is_output){
	  pad =
	    start_pad = ags_effect_bridge_get_output_effect_pad(effect_bridge);
	}else{
	  pad =
	    start_pad = ags_effect_bridge_get_input_effect_pad(effect_bridge);
	}

	pad = g_list_nth(pad,
			 machine_editor_line->channel->pad);

	if(pad != NULL){
	  line =
	    start_line = ags_effect_pad_get_effect_line(AGS_EFFECT_PAD(pad->data));

	  while(line != NULL){
	    if(AGS_EFFECT_LINE(line->data)->channel == machine_editor_line->channel){
	      break;
	    }

	    line = line->next;
	  }

	  if(line != NULL){
	    current_effect_line = AGS_EFFECT_LINE(line->data);
	    g_list_free(start_line);
	  }
	}

	g_list_free(start_pad);

	if(filename != NULL &&
	   effect != NULL &&
	   current_effect_line != NULL){
	  guint audio_channel;
	  guint pad;

	  g_object_get(current_effect_line->channel,
		       "audio-channel", &audio_channel,
		       "pad", &pad,
		       NULL);

	  ags_effect_line_add_plugin(current_effect_line,
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
  AgsMachineEditorLine *machine_editor_line;

  GList *start_entry, *entry;
  GList *start_pad, *pad;
  GList *start_line, *line;

  guint nth;
  gboolean has_bridge;
  gboolean is_output;
  
  if(button == NULL ||
     line_member_editor == NULL){
    return;
  }

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
								AGS_TYPE_MACHINE_EDITOR);
  machine_editor_line = (AgsMachineEditorLine *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
									 AGS_TYPE_MACHINE_EDITOR_LINE);

  entry =
    start_entry = ags_line_member_editor_get_entry(line_member_editor);

  machine = machine_editor->machine;

  if(AGS_IS_OUTPUT(machine_editor_line->channel)){
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
    AgsLine *current_line;
    
    /* retrieve line and pad */
    current_line = NULL;

    if(AGS_IS_OUTPUT(machine_editor_line->channel)){
      pad =
	start_pad = ags_machine_get_output_pad(machine);
    }else{
      pad =
	start_pad = ags_machine_get_input_pad(machine);
    }

    pad = g_list_nth(pad,
		     machine_editor_line->channel->pad);

    if(pad != NULL){
      line =
	start_line = ags_pad_get_line(AGS_PAD(pad->data));

      while(line != NULL){
	if(AGS_LINE(line->data)->channel == machine_editor_line->channel){
	  break;
	}

	line = line->next;
      }

      if(line != NULL){
	current_line = AGS_LINE(line->data);
	g_list_free(start_line);
      }
    }

    g_list_free(start_pad);

    /* iterate line member */
    if(current_line != NULL){
      for(nth = 0; entry != NULL;){
	if(gtk_check_button_get_active(AGS_LINE_MEMBER_EDITOR_ENTRY(entry->data))){
	  /* remove effect */
	  ags_line_remove_plugin(current_line,
				 nth);
	}

	nth++;
	
	entry = entry->next;
      }
    }
  }else{
    AgsEffectBridge *effect_bridge;
    AgsEffectLine *current_effect_line;
	
    effect_bridge = AGS_EFFECT_BRIDGE(machine->bridge);

    current_effect_line = NULL;
    
    /* retrieve effect line and effect pad */
    if(is_output){
      pad =
	start_pad = ags_effect_bridge_get_output_effect_pad(effect_bridge);
    }else{
      pad =
	start_pad = ags_effect_bridge_get_input_effect_pad(effect_bridge);
    }

    pad = g_list_nth(pad,
		     machine_editor_line->channel->pad);

    if(pad != NULL){
      line =
	start_line = ags_effect_pad_get_effect_line(AGS_EFFECT_PAD(pad->data));

      while(line != NULL){
	if(AGS_EFFECT_LINE(line->data)->channel == machine_editor_line->channel){
	  break;
	}

	line = line->next;
      }

      if(line != NULL){
	current_effect_line = AGS_EFFECT_LINE(line->data);
	g_list_free(start_line);
      }
    }

    g_list_free(start_pad);

    /* iterate line member */
    if(current_effect_line != NULL){
      for(nth = 0; entry != NULL;){
	if(gtk_check_button_get_active(AGS_LINE_MEMBER_EDITOR_ENTRY(entry->data))){
	  /* remove effect */
	  ags_effect_line_remove_plugin(current_effect_line,
					nth);
	}

	entry = entry->next;
	
	nth++;
      }
    }
  }

  ags_applicable_reset(AGS_APPLICABLE(line_member_editor));

  g_list_free(start_entry);
}
