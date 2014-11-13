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

#include <ags/X/editor/ags_pattern_edit_callbacks.h>

#include <ags/X/ags_editor.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_ffplayer.h>

gboolean
ags_pattern_edit_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsPatternEdit *pattern_edit)
{
  AgsEditor *editor;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(pattern_edit),
						 AGS_TYPE_EDITOR);

  if(editor->selected_machine != NULL){
    AgsMachine *machine;

    machine = editor->selected_machine;

    if(machine != NULL){
      cairo_t *cr;

      cr = gdk_cairo_create(widget->window);
      cairo_push_group(cr);

      if(AGS_IS_DRUM(machine)){
	ags_meter_paint(editor->meter);
	ags_pattern_edit_draw_segment(pattern_edit, cr);
	ags_pattern_edit_draw_pattern(pattern_edit, cr);
      }else if(AGS_IS_MATRIX(machine)){
	ags_meter_paint(editor->meter);
	ags_pattern_edit_draw_segment(pattern_edit, cr);
	ags_pattern_edit_draw_pattern(pattern_edit, cr);
      }

      if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
	ags_pattern_edit_draw_position(pattern_edit, cr);
      }

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
    }
  }

  return(TRUE);
}

gboolean
ags_pattern_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsPatternEdit *pattern_edit)
{
  pattern_edit->flags |= AGS_PATTERN_EDIT_RESETING_VERTICALLY;
  ags_pattern_edit_reset_vertically(pattern_edit, AGS_PATTERN_EDIT_RESET_VSCROLLBAR);
  pattern_edit->flags &= (~AGS_PATTERN_EDIT_RESETING_VERTICALLY);

  pattern_edit->flags |= AGS_PATTERN_EDIT_RESETING_HORIZONTALLY;
  ags_pattern_edit_reset_horizontally(pattern_edit, AGS_PATTERN_EDIT_RESET_HSCROLLBAR);
  pattern_edit->flags &= (~AGS_PATTERN_EDIT_RESETING_HORIZONTALLY);  

  return(FALSE);
}

gboolean
ags_pattern_edit_drawing_area_button_press_event (GtkWidget *widget, GdkEventButton *event, AgsPatternEdit *pattern_edit)
{

  return(TRUE);
}

gboolean
ags_pattern_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsPatternEdit *pattern_edit)
{

  return(FALSE);
}

gboolean
ags_pattern_edit_drawing_area_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, AgsPatternEdit *pattern_edit)
{

  return(FALSE);
}

void
ags_pattern_edit_vscrollbar_value_changed(GtkRange *range, AgsPatternEdit *pattern_edit)
{
  if((AGS_PATTERN_EDIT_RESETING_VERTICALLY & pattern_edit->flags) != 0){
    return;
  }

  pattern_edit->flags |= AGS_PATTERN_EDIT_RESETING_VERTICALLY;
  ags_pattern_edit_reset_vertically(pattern_edit, 0);
  pattern_edit->flags &= (~AGS_PATTERN_EDIT_RESETING_VERTICALLY);
}

void
ags_pattern_edit_hscrollbar_value_changed(GtkRange *range, AgsPatternEdit *pattern_edit)
{
  if((AGS_PATTERN_EDIT_RESETING_HORIZONTALLY & pattern_edit->flags) != 0){
    return;
  }

  /* reset ruler */
  gtk_adjustment_set_value(pattern_edit->ruler->adjustment,
			   GTK_RANGE(pattern_edit->hscrollbar)->adjustment->value / (double) pattern_edit->control_current.control_width);
  gtk_widget_queue_draw(pattern_edit->ruler);

  /* update pattern edit */
  pattern_edit->flags |= AGS_PATTERN_EDIT_RESETING_HORIZONTALLY;
  ags_pattern_edit_reset_horizontally(pattern_edit, 0);
  pattern_edit->flags &= (~AGS_PATTERN_EDIT_RESETING_HORIZONTALLY);
}
