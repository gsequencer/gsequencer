/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/X/ags_editor_callbacks.h>

#include <ags/main.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/task/ags_scroll_on_play.h>

#include <ags/X/ags_window.h>

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_meter.h>
#include <ags/X/editor/ags_note_edit.h>

#include <math.h>
#include <string.h>
#include <cairo.h>

void ags_editor_link_index_callback(GtkRadioButton *radio_button, GtkDialog *dialog);
void ags_editor_link_index_response_callback(GtkDialog *dialog, int response, AgsEditor *editor);

#define AGS_EDITOR_LINK_INDEX_VBOX "AgsEditorLinkIndexVBox"

void
ags_editor_parent_set_callback(GtkWidget  *widget, GtkObject *old_parent, AgsEditor *editor)
{
  if(old_parent != NULL)
    return;
  
  editor->note_edit->flags |= AGS_NOTE_EDIT_RESETING_HORIZONTALLY;
  ags_note_edit_reset_horizontally(editor->note_edit, AGS_NOTE_EDIT_RESET_HSCROLLBAR);
  editor->note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_HORIZONTALLY);
}

gboolean
ags_editor_destroy_callback(GtkObject *object, AgsEditor *editor)
{
  ags_editor_destroy(object);

  return(TRUE);
}

void
ags_editor_show_callback(GtkWidget *widget, AgsEditor *editor)
{
  ags_editor_show(widget);
}

gboolean
ags_editor_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsEditor *editor)
{
  if(event->button == 3)
    gtk_menu_popup (GTK_MENU (editor->popup),
                    NULL, NULL, NULL, NULL,
                    event->button, event->time);

  return(TRUE);
}

void
ags_editor_index_callback(GtkRadioButton *radio_button, AgsEditor *editor)
{
  AgsMachine *machine;

  editor->selected = radio_button;

  if(editor->selected != NULL)
    machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE));
  else
    machine = NULL;

  ags_editor_change_machine(editor, machine);

  if(machine != NULL){
    guint pads;

    if((AGS_AUDIO_NOTATION_DEFAULT & (machine->audio->flags)) != 0){
      pads = machine->audio->input_pads;
    }else{
      pads = machine->audio->output_pads;
    }

    editor->note_edit->map_height = pads * editor->note_edit->control_height;
  }

  editor->note_edit->flags |= AGS_NOTE_EDIT_RESETING_VERTICALLY;
  ags_note_edit_reset_vertically(editor->note_edit, AGS_NOTE_EDIT_RESET_VSCROLLBAR);
  editor->note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_VERTICALLY);
}

void
ags_editor_popup_add_tab_callback(GtkWidget *widget, GtkMenu *popup)
{
}

void
ags_editor_popup_remove_tab_callback(GtkWidget *widget, GtkMenu *popup)
{
}

void
ags_editor_popup_add_index_callback(GtkWidget *widget, GtkMenu *popup)
{
  AgsEditor *editor;
  
  editor = AGS_EDITOR(g_object_get_data((GObject *) popup, g_type_name(AGS_TYPE_EDITOR)));

  ags_editor_add_index(editor);
}

void
ags_editor_popup_remove_index_callback(GtkWidget *widget, GtkMenu *popup)
{
}

void
ags_editor_popup_link_index_callback(GtkWidget *widget, GtkMenu *popup)
{
  AgsWindow *window;
  AgsEditor *editor;
  AgsMachine *machine;
  GtkDialog *dialog;
  GtkScrolledWindow *scrolled_window;
  GtkVBox *vbox_radio;
  GtkRadioButton *radio_button0, *radio_button;
  GList *list;

  editor = AGS_EDITOR(g_object_get_data((GObject *) popup, (char *) g_type_name(AGS_TYPE_EDITOR)));
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) editor);

  dialog = (GtkDialog *) gtk_dialog_new_with_buttons(g_strdup("select machine\0"),
						     (GtkWindow *) window,
						     GTK_DIALOG_DESTROY_WITH_PARENT,
						     GTK_STOCK_OK,
						     GTK_RESPONSE_ACCEPT,
						     GTK_STOCK_CANCEL,
						     GTK_RESPONSE_REJECT,
						     NULL);
  g_object_set_data((GObject *) dialog, g_type_name(AGS_TYPE_EDITOR), editor);
  g_signal_connect_after((GObject *) dialog, "response\0",
			 G_CALLBACK(ags_editor_link_index_response_callback), editor);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_box_pack_start((GtkBox *) dialog->vbox, (GtkWidget *) scrolled_window, TRUE, TRUE, 0);

  vbox_radio = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  g_object_set_data((GObject *) dialog, AGS_EDITOR_LINK_INDEX_VBOX, (gpointer) vbox_radio);
  gtk_scrolled_window_add_with_viewport(scrolled_window, (GtkWidget *) vbox_radio);

  list = gtk_container_get_children((GtkContainer *) (window->machines));

  if(list != NULL){
    radio_button =
      radio_button0 = (GtkRadioButton *) gtk_radio_button_new_from_widget(NULL);
    goto ags_editor_popup_link_index_callback0;
  }

  while(list != NULL){
    radio_button = (GtkRadioButton *) gtk_radio_button_new_from_widget(radio_button0);
  ags_editor_popup_link_index_callback0:
    machine = AGS_MACHINE(list->data);
    gtk_button_set_label((GtkButton *) radio_button, g_strconcat(G_OBJECT_TYPE_NAME((GObject *) machine), ": \0", machine->name, NULL));
    g_object_set_data((GObject *) radio_button, (char *) g_type_name(AGS_TYPE_MACHINE), list->data);
    gtk_box_pack_start((GtkBox *) vbox_radio, (GtkWidget *) radio_button, FALSE, FALSE, 0);

    list = list->next;
  }

  gtk_widget_show_all((GtkWidget *) dialog);
}

void
ags_editor_link_index_callback(GtkRadioButton *radio_button, GtkDialog *dialog)
{
}

void
ags_editor_link_index_response_callback(GtkDialog *dialog, gint response, AgsEditor *editor)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsMachine *machine0, *machine1;
    GList *list;
    
    list = gtk_container_get_children((GtkContainer *) GTK_VBOX(g_object_get_data((GObject *) dialog, AGS_EDITOR_LINK_INDEX_VBOX)));
    
    if(list == NULL)
      return;

    while(list != NULL && !(GTK_TOGGLE_BUTTON(list->data)->active))
      list = list->next;
    
    machine0 = (AgsMachine *) g_object_get_data(G_OBJECT(list->data), (char *) g_type_name(AGS_TYPE_MACHINE));
    machine1 = (AgsMachine *) g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE));
    
    ags_editor_change_machine(editor, machine0);
  }

  gtk_widget_destroy((GtkWidget *) dialog);
}

void
ags_editor_change_position_callback(AgsNavigation *navigation, gdouble tact,
				    AgsEditor *editor)
{
  cairo_t *cr;
  gdouble loop_start, loop_end;
  gdouble position;

  loop_start = gtk_spin_button_get_value(navigation->loop_left_tact);
  loop_end = gtk_spin_button_get_value(navigation->loop_right_tact);

  if(!gtk_toggle_button_get_active(navigation->loop) || tact <= loop_end){
    position = tact * editor->note_edit->control_current.control_width;
  }else{
    position = loop_start * editor->note_edit->control_current.control_width;
  }

  /* scroll */
  if(position - (0.125 * editor->note_edit->control_current.control_width) > 0.0){
    gtk_range_set_value(GTK_RANGE(editor->note_edit->hscrollbar),
			position - (0.125 * editor->note_edit->control_current.control_width));
  }

  /* draw fader */
  cr = gdk_cairo_create(GTK_WIDGET(editor->note_edit->drawing_area)->window);

  cairo_push_group(cr);

  ags_note_edit_draw_scroll(editor->note_edit, cr,
			    position);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
}

void
ags_editor_set_audio_channels_callback(AgsAudio *audio,
				       guint audio_channels, guint audio_channels_old,
				       AgsEditor *editor)
{
  GList *tabs;
  GList *notation;
  guint i;

  if(audio_channels_old < audio_channels){
    tabs = g_list_nth(editor->notebook->tabs,
		      audio_channels_old);
    notation = g_list_nth(audio->notation,
			  audio_channels_old);

    for(i = audio_channels_old; i < audio_channels; i++){
      ags_notebook_insert_tab(editor->notebook,
			      i);
      AGS_NOTEBOOK_TAB(tabs->data)->notation = notation->data;

      tabs = tabs->next;
      notation = notation->next;
    }
  }else{
    for(i = audio_channels; i < audio_channels_old; i++){
      ags_notebook_remove_tab(editor->notebook,
			      i);
    }
  }
}

void
ags_editor_set_pads_callback(AgsAudio *audio,
			     GType channel_type,
			     guint pads, guint pads_old,
			     AgsEditor *editor)
{
  if((AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) != 0){
    if(!g_type_is_a(channel_type, AGS_TYPE_INPUT)){
      return;
    }

    pads = audio->input_pads;
  }else{
    if(!g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
      return;
    }

    pads = audio->output_pads;
  }

  editor->note_edit->map_height = pads * editor->note_edit->control_height;
  
  editor->note_edit->flags |= AGS_NOTE_EDIT_RESETING_VERTICALLY;
  ags_note_edit_reset_vertically(editor->note_edit, AGS_NOTE_EDIT_RESET_VSCROLLBAR);
  editor->note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_VERTICALLY);
  
  editor->note_edit->flags |= AGS_NOTE_EDIT_RESETING_HORIZONTALLY;
  ags_note_edit_reset_horizontally(editor->note_edit, AGS_NOTE_EDIT_RESET_HSCROLLBAR);
  editor->note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_HORIZONTALLY);  
}
