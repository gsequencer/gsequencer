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

#include <ags/widget/ags_dial.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/task/ags_add_recall.h>
#include <ags/audio/task/ags_add_line_member.h>
#include <ags/audio/task/ags_remove_recall.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>
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
  switch(response){
  case GTK_RESPONSE_ACCEPT:
    {
      AgsAudioLoop *audio_loop;
      AgsTaskThread *task_thread;
      AgsLine *line;
      AgsLineMember *line_member;
      AgsMachineEditor *machine_editor;
      AgsLineEditor *line_editor;
      GtkHBox *hbox;
      GtkCheckButton *check_button;
      GtkLabel *label;
      AgsAddRecall *add_recall;
      AgsAddLineMember *add_line_member;
      AgsRecallLadspa *recall_ladspa;
      GtkAdjustment *adjustment;
      AgsLadspaPlugin *ladspa_plugin;
      GList *plugin;
      GList *task;
      GList *port;
      GList *list;
      gchar *filename, *effect;
      gdouble step;
      long index;
      guint x, y;

      void *plugin_so;
      LADSPA_Descriptor_Function ladspa_descriptor;
      LADSPA_Descriptor *plugin_descriptor;
      LADSPA_PortDescriptor *port_descriptor;
      LADSPA_Data lower_bound, upper_bound;
      long i;

      machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(line_member_editor,
								    AGS_TYPE_MACHINE_EDITOR);
      line_editor = (AgsLineEditor *) gtk_widget_get_ancestor(line_member_editor,
							      AGS_TYPE_LINE_EDITOR);
      
      line = NULL;

      if(AGS_IS_OUTPUT(line_editor->channel)){
	list = gtk_container_get_children(machine_editor->machine->output);
      }else{
	list = gtk_container_get_children(machine_editor->machine->input);
      }

      list = g_list_nth(list,
			line_editor->channel->pad);

      if(list != NULL){
	list = g_list_nth(gtk_container_get_children(AGS_PAD(list->data)->expander_set),
			  AGS_AUDIO(line_editor->channel->audio)->audio_channels - line_editor->channel->audio_channel - 1);

	if(list != NULL){
	  line = AGS_LINE(list->data);
	}
      }
      
      audio_loop = (AgsAudioLoop *) AGS_MAIN(AGS_DEVOUT(AGS_MACHINE(machine_editor->machine)->audio->devout)->ags_main)->main_loop;
      task_thread = (AgsTaskThread *) audio_loop->task_thread;

      filename = ags_ladspa_browser_get_plugin_filename(line_member_editor->ladspa_browser);
      effect = ags_ladspa_browser_get_plugin_effect(line_member_editor->ladspa_browser);

      plugin = gtk_container_get_children(GTK_CONTAINER(line_member_editor->ladspa_browser->plugin));
      index = gtk_combo_box_get_active(GTK_COMBO_BOX(plugin->next->next->next->data));

      ags_ladspa_manager_load_file(filename);
      ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(filename);
  
      plugin_so = ladspa_plugin->plugin_so;

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

      label = (GtkLabel *) gtk_label_new(g_strdup_printf("%s - %s\0",
							 filename,
							 effect));
      gtk_box_pack_start(GTK_BOX(hbox),
			 GTK_WIDGET(label),
			 FALSE, FALSE,
			 0);
      gtk_widget_show_all((GtkWidget *) hbox);

      if(line != NULL){
	x = 0;
	y = 0;
	i = 0;

	list = line->expander->children;

	while(list != NULL){
	  if(y <= AGS_EXPANDER_CHILD(list->data)->y){
	    y = AGS_EXPANDER_CHILD(list->data)->y + 1;
	  }

	  list = list->next;
	}
      }

      /* tasks */
      task = NULL;

      /* ladspa play */
      recall_ladspa = ags_recall_ladspa_new(line_editor->channel,
					    filename,
					    effect,
					    index);
      AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
      port = ags_recall_ladspa_load_ports(recall_ladspa);

      add_recall = ags_add_recall_new(line_editor->channel,
				      recall_ladspa,
				      TRUE);
      task = g_list_prepend(task,
			    add_recall);

      /* ladspa recall */
      recall_ladspa = ags_recall_ladspa_new(line_editor->channel,
					    filename,
					    effect,
					    index);
      AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
      port = ags_recall_ladspa_load_ports(recall_ladspa);

      add_recall = ags_add_recall_new(line_editor->channel,
				      recall_ladspa,
				      FALSE);
      task = g_list_prepend(task,
			    add_recall);

      if(line != NULL){
	if(index != -1 &&
	   plugin_so){
	  ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
								 "ladspa_descriptor\0");

	  if(dlerror() == NULL && ladspa_descriptor){
	    plugin_descriptor = ladspa_descriptor(index);

	    port_descriptor = plugin_descriptor->PortDescriptors;   

	    while(port != NULL){
	      if(LADSPA_IS_PORT_AUDIO(port_descriptor[i])){
		i++;
		continue;
	      }

	      if(x == 2){
		x = 0;
		y++;
	      }

	      line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
							   "widget-type\0", AGS_TYPE_DIAL,
							   "plugin-name\0", AGS_PORT(port->data)->plugin_name,
							   "specifier\0", AGS_PORT(port->data)->specifier,
							   "control-port\0", AGS_PORT(port->data)->control_port,
							   NULL);

	      lower_bound = plugin_descriptor->PortRangeHints[i].LowerBound;
	      upper_bound = plugin_descriptor->PortRangeHints[i].UpperBound;
	      g_object_get(ags_line_member_get_widget(line_member),
			   "adjustment", &adjustment,
			   NULL);

	      if(upper_bound >= 0.0 && lower_bound >= 0.0){
		step = (upper_bound - lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
	      }else if(upper_bound < 0.0 && lower_bound < 0.0){
		step = -1.0 * (upper_bound + lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
	      }else{
		step = (upper_bound - lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
	      }

	      gtk_adjustment_set_step_increment(adjustment,
						step);
	      gtk_adjustment_set_lower(adjustment,
				       lower_bound);
	      gtk_adjustment_set_upper(adjustment,
				       upper_bound);
	      gtk_adjustment_set_value(adjustment,
				       lower_bound);

	      add_line_member = ags_add_line_member_new(line,
							line_member,
							x, y,
							1, 1);
	      task = g_list_prepend(task,
				    add_line_member);
	  
	      x++;
	      i++;
	      port = port->next;
	    }
	  }

	  task = g_list_reverse(task);

	  /* launch tasks */
	  ags_task_thread_append_tasks(task_thread,
				       task);
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
  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsRemoveRecall *remove_recall;
  GList *line_member;
  GList *children;
  GList *play_ladspa, *recall_ladspa;
  GList *task;
  guint index;
  gboolean found;

  if(button == NULL ||
     line_member_editor == NULL){
    return;
  }

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(line_member_editor,
								AGS_TYPE_MACHINE_EDITOR);
  line_editor = (AgsLineEditor *) gtk_widget_get_ancestor(line_member_editor,
							  AGS_TYPE_LINE_EDITOR);

  audio_loop = (AgsAudioLoop *) AGS_MAIN(AGS_DEVOUT(AGS_MACHINE(machine_editor->machine)->audio->devout)->ags_main)->main_loop;
  task_thread = (AgsTaskThread *) audio_loop->task_thread;

  line_member = gtk_container_get_children(GTK_CONTAINER(line_member_editor->line_member));
  task = NULL;

  play_ladspa = ags_recall_template_find_type(line_editor->channel->play,
					      AGS_TYPE_RECALL_LADSPA);
  recall_ladspa = ags_recall_template_find_type(line_editor->channel->recall,
						AGS_TYPE_RECALL_LADSPA);

  found = FALSE;

  for(index = 0; line_member != NULL; index++){
    if(found){
      index -= 1;
      found = FALSE;
    }

    children = gtk_container_get_children(GTK_CONTAINER(line_member->data));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(children->data))){
      task = NULL;

      remove_recall = ags_remove_recall_new(line_editor->channel,
					    g_list_nth(play_ladspa,
						       index)->data,
					    TRUE);
      task = g_list_prepend(task,
			    remove_recall);

      remove_recall = ags_remove_recall_new(line_editor->channel,
					    g_list_nth(recall_ladspa,
						       index)->data,
					    FALSE);
      task = g_list_prepend(task,
			    remove_recall);

      gtk_widget_destroy(GTK_WIDGET(line_member->data));

      found = TRUE;
    }

    line_member = line_member->next;
  }

  ags_task_thread_append_tasks(task_thread,
			       task);
}
