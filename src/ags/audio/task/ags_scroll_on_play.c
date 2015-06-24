/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/task/ags_scroll_on_play.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>

#include <ags/X/ags_editor.h>
#include <ags/X/editor/ags_pattern_edit.h>
#include <ags/X/editor/ags_note_edit.h>

#include <ags/main.h>

void ags_scroll_on_play_class_init(AgsScrollOnPlayClass *scroll_on_play);
void ags_scroll_on_play_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_scroll_on_play_init(AgsScrollOnPlay *scroll_on_play);
void ags_scroll_on_play_connect(AgsConnectable *connectable);
void ags_scroll_on_play_disconnect(AgsConnectable *connectable);
void ags_scroll_on_play_finalize(GObject *gobject);

void ags_scroll_on_play_launch(AgsTask *task);

/**
 * SECTION:ags_scroll_on_play
 * @short_description: scrolls the editor
 * @title: AgsScrollOnPlay
 * @section_id:
 * @include: ags/audio/task/ags_scroll_on_play.h
 *
 * The #AgsScrollOnPlay task scrolls the specified #AgsEditor.
 */

static gpointer ags_scroll_on_play_parent_class = NULL;
static AgsConnectableInterface *ags_scroll_on_play_parent_connectable_interface;

GType
ags_scroll_on_play_get_type()
{
  static GType ags_type_scroll_on_play = 0;

  if(!ags_type_scroll_on_play){
    static const GTypeInfo ags_scroll_on_play_info = {
      sizeof (AgsScrollOnPlayClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scroll_on_play_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScrollOnPlay),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scroll_on_play_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_scroll_on_play_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_scroll_on_play = g_type_register_static(AGS_TYPE_TASK,
						     "AgsScrollOnPlay\0",
						     &ags_scroll_on_play_info,
						     0);

    g_type_add_interface_static(ags_type_scroll_on_play,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_scroll_on_play);
}

void
ags_scroll_on_play_class_init(AgsScrollOnPlayClass *scroll_on_play)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_scroll_on_play_parent_class = g_type_class_peek_parent(scroll_on_play);

  /* gobject */
  gobject = (GObjectClass *) scroll_on_play;

  gobject->finalize = ags_scroll_on_play_finalize;

  /* task */
  task = (AgsTaskClass *) scroll_on_play;

  task->launch = ags_scroll_on_play_launch;
}

void
ags_scroll_on_play_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_scroll_on_play_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_scroll_on_play_connect;
  connectable->disconnect = ags_scroll_on_play_disconnect;
}

void
ags_scroll_on_play_init(AgsScrollOnPlay *scroll_on_play)
{
  scroll_on_play->editor = NULL;
}

void
ags_scroll_on_play_connect(AgsConnectable *connectable)
{
  ags_scroll_on_play_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_scroll_on_play_disconnect(AgsConnectable *connectable)
{
  ags_scroll_on_play_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_scroll_on_play_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_scroll_on_play_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_scroll_on_play_launch(AgsTask *task)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsEditor *editor;
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *delay_audio_run;
  AgsScrollOnPlay *scroll_on_play;
  cairo_t *cr;
  GList *editor_child, *recall;
  gdouble position;
  gdouble delay;
  guint control_width, width;
  GValue value = {0,};

  scroll_on_play = AGS_SCROLL_ON_PLAY(task);

  editor = AGS_EDITOR(scroll_on_play->editor);

  window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(editor));


  editor_child = editor->editor_child;

  while(editor_child != NULL){
    machine = AGS_EDITOR_CHILD(editor_child->data)->machine;

    if(machine == NULL){
      editor_child = editor_child->next;
      
      continue;
    }

    recall = machine->audio->play;

    while((recall = ags_recall_find_type(recall,
					 AGS_TYPE_DELAY_AUDIO_RUN)) != NULL){
      if(AGS_RECALL(recall->data)->recall_id != NULL){
	break;
      }

      recall = recall->next;
    }

    if(recall == NULL){
      editor_child = editor_child->next;
      
      continue;
    }

    delay_audio_run = AGS_DELAY_AUDIO_RUN(recall->data);
    delay_audio = AGS_RECALL_AUDIO_RUN(delay_audio)->recall_audio;

    g_value_init(&value, G_TYPE_DOUBLE);
    ags_port_safe_read(delay_audio->notation_delay,
		       &value);
    delay = g_value_get_double(&value);
    
    position = ((gdouble) delay_audio_run->notation_counter) / delay * AGS_NOTE_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->control_unit.control_width;
    
    if(AGS_IS_NOTE_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)){
      width = GTK_WIDGET(AGS_NOTE_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->drawing_area)->allocation.width;
      control_width = AGS_NOTE_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->control_current.control_width;
      
      /* scroll */
      if(3 * control_width < width){
	if(((guint) position) % width >= width - control_width){
	  gtk_range_set_value(GTK_RANGE(AGS_NOTE_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->hscrollbar),
			      position - control_width);
	}
      }else{
	  gtk_range_set_value(GTK_RANGE(AGS_NOTE_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->hscrollbar),
			      position);
      }
      
      /* draw fader */
      cairo_push_group(cr);

      cr = gdk_cairo_create(GTK_WIDGET(AGS_NOTE_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->drawing_area)->window);
      ags_note_edit_draw_scroll(AGS_NOTE_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget), cr,
				position);

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);

      cairo_destroy(cr);
    }else if(AGS_IS_PATTERN_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)){
      width = GTK_WIDGET(AGS_PATTERN_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->drawing_area)->allocation.width;
      control_width = AGS_PATTERN_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->control_current.control_width;
      
      /* scroll */
      if(3 * control_width < width){
	if(((guint) position) % width >= width - control_width){
	  gtk_range_set_value(GTK_RANGE(AGS_PATTERN_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->hscrollbar),
			      position - control_width);
	}
      }else{
	  gtk_range_set_value(GTK_RANGE(AGS_PATTERN_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->hscrollbar),
			      position);
      }

      /* draw fader */
      cairo_push_group(cr);

      cr = gdk_cairo_create(GTK_WIDGET(AGS_PATTERN_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget)->drawing_area)->window);
      ags_pattern_edit_draw_scroll(AGS_PATTERN_EDIT(AGS_EDITOR_CHILD(editor_child->data)->edit_widget), cr,
				   position);

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);

      cairo_destroy(cr);
    }

    editor_child = editor_child->next;
  }
}

/**
 * ags_scroll_on_play_new:
 * @editor: the #AgsEditor to scroll
 *
 * Creates an #AgsScrollOnPlay.
 *
 * Returns: an new #AgsScrollOnPlay.
 *
 * Since: 0.4
 */
AgsScrollOnPlay*
ags_scroll_on_play_new(GtkWidget *editor)
{
  AgsScrollOnPlay *scroll_on_play;

  scroll_on_play = (AgsScrollOnPlay *) g_object_new(AGS_TYPE_SCROLL_ON_PLAY,
						    NULL);

  scroll_on_play->editor = editor;

  return(scroll_on_play);
}
