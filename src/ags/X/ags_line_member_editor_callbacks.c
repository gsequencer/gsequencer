/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_channel_run_dummy.h>
#include <ags/audio/ags_recall_recycling_dummy.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/task/ags_add_recall_container.h>
#include <ags/audio/task/ags_add_recall.h>
#include <ags/audio/task/ags_add_line_member.h>
#include <ags/audio/task/ags_remove_recall.h>
#include <ags/audio/task/ags_remove_recall_container.h>

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
  gtk_widget_show_all((GtkWidget *) line_member_editor->ladspa_browser);
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
      GtkTable *table;
      
      AgsAddRecallContainer *add_recall_container;
      AgsAddRecall *add_recall;
      AgsAddLineMember *add_line_member;

      AgsRecallContainer *recall_container;
      AgsRecallChannelRunDummy *recall_channel_run_dummy;
      AgsRecallLadspa *recall_ladspa;

      GtkAdjustment *adjustment;

      AgsLadspaPlugin *ladspa_plugin;

      GList *plugin;
      GList *task;
      GList *port;
      GList *pad, *pad_start;
      GList *list, *list_start;
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

      machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
								    AGS_TYPE_MACHINE_EDITOR);
      line_editor = (AgsLineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
							      AGS_TYPE_LINE_EDITOR);
      
      line = NULL;

      if(AGS_IS_OUTPUT(line_editor->channel)){
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

      if(line == NULL){
	return;
      }
      
      audio_loop = (AgsAudioLoop *) AGS_MAIN(AGS_DEVOUT(AGS_MACHINE(machine_editor->machine)->audio->devout)->ags_main)->main_loop;
      task_thread = (AgsTaskThread *) audio_loop->task_thread;

      /* retrieve plugin */
      filename = ags_ladspa_browser_get_plugin_filename(line_member_editor->ladspa_browser);
      effect = ags_ladspa_browser_get_plugin_effect(line_member_editor->ladspa_browser);

      if(ags_recall_ladpsa_find(line->channel->recall,
				filename, effect) != NULL){
	/* return if duplicated */
	return;
      }

      plugin = gtk_container_get_children(GTK_CONTAINER(line_member_editor->ladspa_browser->plugin));
      index = gtk_combo_box_get_active(GTK_COMBO_BOX(plugin->next->next->next->data));

      g_list_free(plugin);

      /* load plugin */
      ags_ladspa_manager_load_file(filename);
      ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(filename);

      plugin_so = ladspa_plugin->plugin_so;

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
      recall_container = ags_recall_container_new();

      add_recall_container = ags_add_recall_container_new(line_editor->channel->audio,
							  recall_container);
      task = g_list_prepend(task,
			    add_recall_container);

      recall_ladspa = ags_recall_ladspa_new(line_editor->channel,
					    filename,
					    effect,
					    index);
      g_object_set(G_OBJECT(recall_ladspa),
		   "devout\0", AGS_AUDIO(line_editor->channel->audio)->devout,
		   "recall-container\0", recall_container,
		   NULL);
      AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
      ags_recall_ladspa_load(recall_ladspa);
      ags_recall_ladspa_load_ports(recall_ladspa);

      add_recall = ags_add_recall_new((GObject *) line_editor->channel,
				      (AgsRecall *) recall_ladspa,
				      TRUE);
      task = g_list_prepend(task,
			    add_recall);

      /* dummy */
      recall_channel_run_dummy = ags_recall_channel_run_dummy_new(line_editor->channel,
								  AGS_TYPE_RECALL_RECYCLING_DUMMY,
								  AGS_TYPE_RECALL_LADSPA_RUN);
      AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
      g_object_set(G_OBJECT(recall_channel_run_dummy),
		   "devout\0", AGS_AUDIO(line_editor->channel->audio)->devout,
		   "recall-container\0", recall_container,
		   "recall-channel\0", recall_ladspa,
		   NULL);

      add_recall = ags_add_recall_new((GObject *) line_editor->channel,
				      (AgsRecall *) recall_channel_run_dummy,
				      TRUE);
      task = g_list_prepend(task,
			    add_recall);

      /* ladspa recall */
      recall_container = ags_recall_container_new();

      add_recall_container = ags_add_recall_container_new(line_editor->channel->audio,
							  recall_container);
      task = g_list_prepend(task,
			    add_recall_container);

      recall_ladspa = ags_recall_ladspa_new(line_editor->channel,
					    filename,
					    effect,
					    index);
      g_object_set(G_OBJECT(recall_ladspa),
		   "devout\0", AGS_AUDIO(line_editor->channel->audio)->devout,
		   "recall-container\0", recall_container,
		   NULL);
      AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
      ags_recall_ladspa_load(recall_ladspa);
      port = ags_recall_ladspa_load_ports(recall_ladspa);

      add_recall = ags_add_recall_new((GObject *) line_editor->channel,
				      (AgsRecall *) recall_ladspa,
				      FALSE);
      task = g_list_prepend(task,
			    add_recall);

      /* dummy */
      recall_channel_run_dummy = ags_recall_channel_run_dummy_new(line_editor->channel,
								  AGS_TYPE_RECALL_RECYCLING_DUMMY,
								  AGS_TYPE_RECALL_LADSPA_RUN);
      AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
      g_object_set(G_OBJECT(recall_channel_run_dummy),
		   "devout\0", AGS_AUDIO(line_editor->channel->audio)->devout,
		   "recall-container\0", recall_container,
		   "recall-channel\0", recall_ladspa,
		   NULL);

      add_recall = ags_add_recall_new(line_editor->channel,
				      recall_channel_run_dummy,
				      FALSE);
      task = g_list_prepend(task,
			    add_recall);

      /* add controls of ports and apply range  */
      list_start = gtk_container_get_children(GTK_CONTAINER(line_member_editor->ladspa_browser->description));
      table = g_list_nth(list_start,
			 4)->data;
      g_list_free(list_start);

      list_start = 
	list = g_list_reverse(gtk_container_get_children(table));

      if(line != NULL){
	if(index != -1 &&
	   plugin_so){
	  ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
								 "ladspa_descriptor\0");

	  if(dlerror() == NULL && ladspa_descriptor){
	    plugin_descriptor = ladspa_descriptor(index);

	    port_descriptor = plugin_descriptor->PortDescriptors;   

	    while(port != NULL){
	      if((LADSPA_IS_PORT_CONTROL(port_descriptor[i]) && 
		   (LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
		    LADSPA_IS_PORT_OUTPUT(port_descriptor[i])))){
		GtkWidget *child_widget;
		GType child_type;
		GtkAdjustment *adjustment;

		gchar *str;
		
		if(x == 2){
		  x = 0;
		  y++;
		}

		list = list->next;
		str = gtk_combo_box_text_get_active_text(list->data);
		
		if(!g_ascii_strcasecmp(str,
				       "toggle button\0")){
		  child_type = GTK_TYPE_TOGGLE_BUTTON;
		}else if(!g_ascii_strcasecmp(str,
					     "check button\0")){
		  child_type = GTK_TYPE_CHECK_BUTTON;
		}else if(!g_ascii_strcasecmp(str,
					     "spin button\0")){
		  child_type = GTK_TYPE_SPIN_BUTTON;
		}else if(!g_ascii_strcasecmp(str,
					     "dial\0")){
		  child_type = AGS_TYPE_DIAL;
		}else if(!g_ascii_strcasecmp(str,
					     "vertical scale\0")){
		  child_type = GTK_TYPE_VSCALE;
		}
		
		
		line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
							     "widget-type\0", child_type,
							     "widget-label\0", plugin_descriptor->PortNames[i],
							     "plugin-name\0", AGS_PORT(port->data)->plugin_name,
							     "specifier\0", AGS_PORT(port->data)->specifier,
							     "control-port\0", AGS_PORT(port->data)->control_port,
							     NULL);

		child_widget = ags_line_member_get_widget(line_member);
		
		if(LADSPA_IS_HINT_TOGGLED(plugin_descriptor->PortRangeHints[i].HintDescriptor)){
		  //nothing
		}else{
		  lower_bound = plugin_descriptor->PortRangeHints[i].LowerBound;
		  upper_bound = plugin_descriptor->PortRangeHints[i].UpperBound;

		  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);

		  if(GTK_IS_SPIN_BUTTON(child_widget)){
		    g_object_set(child_widget,
				 "digits\0", 4,
				 "adjustment", adjustment,
				 NULL);
		    step = 0.0001;
		  }else if(AGS_IS_DIAL(child_widget)){
		    AgsDial *dial;
		    
		    dial = (AgsDial *) child_widget;
		    gtk_widget_set_size_request((GtkWidget *) dial,
						2 * dial->radius + 2 * dial->outline_strength + dial->button_width + 1,
						2 * dial->radius + 2 * dial->outline_strength + 1);
		    g_object_set(dial,
				 "adjustment", adjustment,
				 NULL);

		    if(upper_bound >= 0.0 && lower_bound >= 0.0){
		      step = (upper_bound - lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
		    }else if(upper_bound < 0.0 && lower_bound < 0.0){
		      step = -1.0 * (lower_bound - upper_bound) / AGS_DIAL_DEFAULT_PRECISION;
		    }else{
		      step = (upper_bound - lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
		    }
		  }else if(GTK_IS_VSCALE(child_widget)){
		    g_object_set(child_widget,
				 "adjustment", adjustment,
				 NULL);
		    step = 0.1;
		  }
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
		port = port->next;
		list = list->next;
	      }

	      i++;
	    }
	  }
	}
      }

      g_list_free(list_start);
      
      task = g_list_reverse(task);
      
      /* launch tasks */
      ags_task_thread_append_tasks(task_thread,
				   task);
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
  AgsLine *line;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsRemoveRecall *remove_recall;
  AgsRemoveRecallContainer *remove_recall_container;
  GList *control;
  GList *line_member;
  GList *children;
  GList *play_ladspa, *play_ladspa_start, *recall_ladspa, *recall_ladspa_start;
  GList *port;
  GList *task;
  GList *list, *list_start, *pad, *pad_start;
  guint index;

  if(button == NULL ||
     line_member_editor == NULL){
    return;
  }

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
								AGS_TYPE_MACHINE_EDITOR);
  line_editor = (AgsLineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
							  AGS_TYPE_LINE_EDITOR);

  audio_loop = (AgsAudioLoop *) AGS_MAIN(AGS_DEVOUT(AGS_MACHINE(machine_editor->machine)->audio->devout)->ags_main)->main_loop;
  task_thread = (AgsTaskThread *) audio_loop->task_thread;

  line_member = gtk_container_get_children(GTK_CONTAINER(line_member_editor->line_member));
  task = NULL;

  line = NULL;

  if(AGS_IS_OUTPUT(line_editor->channel)){
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

  play_ladspa = 
    play_ladspa_start = ags_recall_template_find_type(line_editor->channel->play,
						      AGS_TYPE_RECALL_LADSPA);
  recall_ladspa = 
    recall_ladspa_start = ags_recall_template_find_type(line_editor->channel->recall,
							AGS_TYPE_RECALL_LADSPA);

  for(index = 0; line_member != NULL; index++){

    children = gtk_container_get_children(GTK_CONTAINER(line_member->data));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(children->data))){
      task = NULL;

      /* play context */
      remove_recall = ags_remove_recall_new((GObject *) line_editor->channel,
					    g_list_nth(play_ladspa,
						       index)->data,
					    TRUE,
					    TRUE);
      task = g_list_prepend(task,
			    remove_recall);

      remove_recall = ags_remove_recall_new((GObject *) line_editor->channel,
					    ags_recall_find_template(AGS_RECALL_CONTAINER(AGS_RECALL(g_list_nth(play_ladspa,
														index)->data)->container)->recall_channel_run)->data,
					    TRUE,
					    TRUE);
      task = g_list_prepend(task,
			    remove_recall);

      remove_recall_container = ags_remove_recall_container_new(line_editor->channel->audio,
								(AgsRecallContainer *) AGS_RECALL(g_list_nth(play_ladspa,
													     index)->data)->container);
      task = g_list_prepend(task,
			    remove_recall_container);

      /* recall context */
      remove_recall = ags_remove_recall_new((GObject *) line_editor->channel,
					    g_list_nth(recall_ladspa,
						       index)->data,
					    FALSE,
					    TRUE);
      task = g_list_prepend(task,
			    remove_recall);

      remove_recall = ags_remove_recall_new((GObject *) line_editor->channel,
					    ags_recall_find_template(AGS_RECALL_CONTAINER(AGS_RECALL(g_list_nth(recall_ladspa,
														index)->data)->container)->recall_channel_run)->data,
					    FALSE,
					    TRUE);
      task = g_list_prepend(task,
			    remove_recall);

      remove_recall_container = ags_remove_recall_container_new(line_editor->channel->audio,
								(AgsRecallContainer *) AGS_RECALL(g_list_nth(recall_ladspa,
													     index)->data)->container);
      task = g_list_prepend(task,
			    remove_recall_container);

      /* destroy line member editor entry */
      gtk_widget_destroy(GTK_WIDGET(line_member->data));

      /* destroy controls */
      if(line != NULL){
	port = AGS_RECALL(g_list_nth(play_ladspa,
				     index)->data)->port;

	while(port != NULL){
	  control = gtk_container_get_children((GtkContainer *) line->expander->table);
	    
	    while(control != NULL){
	      if(AGS_IS_LINE_MEMBER(control->data) &&
		 AGS_LINE_MEMBER(control->data)->port == port->data){
		ags_expander_remove(line->expander,
				    control->data);
		break;
	      }
	      
	      control = control->next;
	    }
	  
	  port = port->next;
	}
      }
    }

    line_member = line_member->next;
  }

  ags_task_thread_append_tasks(task_thread,
			       task);
}
