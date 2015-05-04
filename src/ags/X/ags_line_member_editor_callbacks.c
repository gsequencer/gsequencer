/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/X/ags_line_member_editor_callbacks.h>

#include <ags/main.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/object/ags_plugin.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_output.h>

#include <ags/audio/task/ags_add_effect.h>

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

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

void
ags_line_member_editor_add_callback(GtkWidget *button,
				    AgsLineMemberEditor *line_member_editor)
{
  gtk_widget_show_all(line_member_editor->ladspa_browser);
}

void
ags_line_member_editor_ladspa_browser_response_callback(GtkDialog *dialog,
							gint response,
							AgsLineMemberEditor *line_member_editor)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;

  AgsThread *main_loop;
  AgsThread *task_thread;
  
  AgsApplicationContext *application_context;
  
  GList *pad, *pad_start;
  GList *list, *list_start;
  gchar *filename, *effect;
  gboolean has_bridge;
  gboolean is_output;
  
  auto void ags_line_member_editor_ladspa_browser_response_create_entry();
  
  void ags_line_member_editor_ladspa_browser_response_create_entry(){
    GtkHBox *hbox;
    GtkCheckButton *check_button;
    GtkLabel *label;

    /* create entry */
    hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(line_member_editor->line_member),
		       GTK_WIDGET(hbox),
		       FALSE, FALSE,
		       0);
      
    check_button = (GtkCheckButton *) gtk_check_button_new();
    gtk_box_pack_start(GTK_BOX(hbox),
		       GTK_WIDGET(check_button),
		       FALSE, FALSE,
		       0);

    //TODO:JK: ugly
    label = (GtkLabel *) gtk_label_new(g_strdup_printf("%s - %s\0",
						       filename,
						       effect));
    gtk_box_pack_start(GTK_BOX(hbox),
		       GTK_WIDGET(label),
		       FALSE, FALSE,
		       0);
    gtk_widget_show_all((GtkWidget *) hbox);
  }
  
  switch(response){
  case GTK_RESPONSE_ACCEPT:
    {
      machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(line_member_editor,
								    AGS_TYPE_MACHINE_EDITOR);
      line_editor = (AgsLineEditor *) gtk_widget_get_ancestor(line_member_editor,
							      AGS_TYPE_LINE_EDITOR);

      machine = machine_editor->machine;

      window = gtk_widget_get_toplevel(machine);
      g_object_get(window,
		   "application-context\0" , &application_context,
		   NULL);

      main_loop = application_context->main_loop;

      task_thread = ags_thread_find_type(main_loop,
					 AGS_TYPE_TASK_THREAD);
      
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
	
	/* find pad and line */
	line = NULL;
	
	if(is_output){
	  pad_start = 
	    pad = gtk_container_get_children(machine_editor->machine->output);
	}else{
	  pad_start = 
	    pad = gtk_container_get_children(machine_editor->machine->input);
	}

	pad = g_list_nth(pad,
			 line_editor->channel->pad);

	if(pad != NULL){
	  list_start =
	    list = gtk_container_get_children(AGS_PAD(pad->data)->expander_set);

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

	/* retrieve plugin */
	filename = ags_ladspa_browser_get_plugin_filename(line_member_editor->ladspa_browser);
	effect = ags_ladspa_browser_get_plugin_effect(line_member_editor->ladspa_browser);

	if(line != NULL){
	  AgsAddEffect *add_effect;
	  
	  ags_line_member_editor_ladspa_browser_response_create_entry();
	
	  /* add effect */
	  add_effect = ags_add_effect_new(line->channel,
					  filename,
					  effect);
	  ags_task_thread_append_task(task_thread,
				      add_effect);
	}
      }else{
	AgsEffectBridge *effect_bridge;
	AgsEffectLine *effect_line;
	
	effect_bridge = machine->bridge;
	
	if(is_output){
	  pad_start = 
	    pad = gtk_container_get_children(effect_bridge->output);
	}else{
	  pad_start = 
	    pad = gtk_container_get_children(effect_bridge->input);
	}

	pad = g_list_nth(pad,
			 line_editor->channel->pad);

	if(pad != NULL){
	  list_start =
	    list = gtk_container_get_children(AGS_EFFECT_PAD(pad->data)->table);

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

	/* retrieve plugin */
	filename = ags_ladspa_browser_get_plugin_filename(line_member_editor->ladspa_browser);
	effect = ags_ladspa_browser_get_plugin_effect(line_member_editor->ladspa_browser);

	if(effect_line != NULL){
	  AgsAddEffect *add_effect;
	  
	  ags_line_member_editor_ladspa_browser_response_create_entry();

	  /* add effect */
	  add_effect = ags_add_effect_new(effect_line->channel,
					  filename,
					  effect);
	  ags_task_thread_append_task(task_thread,
				      add_effect);
	}
      }
    }
    break;      
  }
}

void
ags_line_member_editor_remove_callback(GtkWidget *button,
				       AgsLineMemberEditor *line_member_editor)
{
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;

  GList *line_member;
  GList *list, *list_start, *pad, *pad_start;
  GList *children;
  guint nth;
  gboolean has_bridge;
  gboolean is_output;
  
  auto void ags_line_member_editor_ladspa_browser_response_destroy_entry();
  
  void ags_line_member_editor_ladspa_browser_response_destroy_entry(){
    /* destroy line member editor entry */
    gtk_widget_destroy(GTK_WIDGET(line_member->data));
  }
  
  if(button == NULL ||
     line_member_editor == NULL){
    return;
  }

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(line_member_editor,
								AGS_TYPE_MACHINE_EDITOR);
  line_editor = (AgsLineEditor *) gtk_widget_get_ancestor(line_member_editor,
							  AGS_TYPE_LINE_EDITOR);

  line_member = gtk_container_get_children(GTK_CONTAINER(line_member_editor->line_member));

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
	pad = gtk_container_get_children(machine->output);
    }else{
      pad_start = 
	pad = gtk_container_get_children(machine->input);
    }

    pad = g_list_nth(pad,
		     line_editor->channel->pad);

    if(pad != NULL){
      list_start =
	list = gtk_container_get_children(AGS_PAD(pad->data)->expander_set);

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
      for(nth = 0; line_member != NULL; nth++){

	children = gtk_container_get_children(GTK_CONTAINER(line_member->data));

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(children->data))){
	  ags_line_member_editor_ladspa_browser_response_destroy_entry();
	
	  /* remove effect */
	  ags_line_remove_effect(line,
				 nth);
	}
      
	line_member = line_member->next;
      }
    }
  }else{
    AgsEffectBridge *effect_bridge;
    AgsEffectLine *effect_line;
	
    effect_bridge = machine->bridge;
	
    if(is_output){
      pad_start = 
	pad = gtk_container_get_children(effect_bridge->output);
    }else{
      pad_start = 
	pad = gtk_container_get_children(effect_bridge->input);
    }

    pad = g_list_nth(pad,
		     line_editor->channel->pad);

    if(pad != NULL){
      list_start =
	list = gtk_container_get_children(AGS_EFFECT_PAD(pad->data)->table);

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
      for(nth = 0; line_member != NULL; nth++){

	children = gtk_container_get_children(GTK_CONTAINER(line_member->data));

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(children->data))){
	  ags_line_member_editor_ladspa_browser_response_destroy_entry();
	
	  /* remove effect */
	  ags_effect_line_remove_effect(effect_line,
					nth);
	}
      
	line_member = line_member->next;
      }
    }
  }
}
