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

#include <ags/object/ags_plugin.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_recall_ladspa.h>

#include <ags/audio/task/ags_add_recall.h>
#include <ags/audio/task/ags_remove_recall.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>

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
      AgsMachineEditor *machine_editor;
      AgsLineEditor *line_editor;
      GtkHBox *hbox;
      GtkCheckButton *check_button;
      GtkLabel *label;
      AgsAddRecall *add_recall;
      AgsRecallLadspa *recall_ladspa;
      GList *list;
      gchar *filename, *effect;
      guint index;

      machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(line_member_editor,
								    AGS_TYPE_MACHINE_EDITOR);
      line_editor = (AgsLineEditor *) gtk_widget_get_ancestor(line_member_editor,
							      AGS_TYPE_LINE_EDITOR);

      audio_loop = (AgsAudioLoop *) AGS_MAIN(AGS_DEVOUT(AGS_MACHINE(machine_editor->machine)->audio->devout)->ags_main)->main_loop;
      task_thread = (AgsTaskThread *) audio_loop->task_thread;

      filename = ags_ladspa_browser_get_plugin_filename(line_member_editor->ladspa_browser);
      effect = ags_ladspa_browser_get_plugin_effect(line_member_editor->ladspa_browser);

      index = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_container_get_children(GTK_CONTAINER(line_member_editor->ladspa_browser->plugin))->next->next->next->data));

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

      /* ladspa play */
      recall_ladspa = ags_recall_ladspa_new(filename,
					    effect,
					    index);
      AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
      ags_plugin_set_ports(AGS_PLUGIN(recall_ladspa),
			   NULL);

      list = NULL;

      add_recall = ags_add_recall_new(line_editor->channel,
				      recall_ladspa,
				      TRUE);
      list = g_list_prepend(list,
			    add_recall);

      /* ladspa recall */
      recall_ladspa = ags_recall_ladspa_new(filename,
					    effect,
					    index);
      AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
      ags_plugin_set_ports(AGS_PLUGIN(recall_ladspa),
			   NULL);

      add_recall = ags_add_recall_new(line_editor->channel,
				      recall_ladspa,
				      FALSE);
      list = g_list_prepend(list,
			    add_recall);

      /* launch task */
      ags_task_thread_append_tasks(task_thread,
				   list);
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
