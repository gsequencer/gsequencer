/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/editor/ags_wave_toolbar_callbacks.h>

#include <ags/X/ags_wave_editor.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <math.h>
#include <gdk/gdkkeysyms.h>

void
ags_wave_toolbar_tool_popup_select_audio_data_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar)
{
  //TODO:JK: implement me
}

void
ags_wave_toolbar_tool_popup_position_cursor_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar)
{
  //TODO:JK: implement me
}

void
ags_wave_toolbar_zoom_callback(GtkComboBox *combo_box, AgsWaveToolbar *wave_toolbar)
{
  AgsWaveEditor *wave_editor;
  GtkWidget *widget;
  
  GtkAdjustment *adjustment;

  GList *list_start;
  
  double zoom_factor, zoom;

  auto void ags_wave_toolbar_zoom_callback_apply(GList *list);
  
  void ags_wave_toolbar_zoom_callback_apply(GList *list){
    AgsWaveEdit *wave_edit;
    
    while(list != NULL){
      wave_edit = list->data;
      
      gtk_widget_queue_draw(wave_edit);
      
      /* reset ruler */
      wave_edit->ruler->factor = zoom_factor;
      wave_edit->ruler->precision = zoom;
      wave_edit->ruler->scale_precision = 1.0 / zoom;
  
      gtk_widget_queue_draw((GtkWidget *) wave_edit->ruler);

      list = list->next;
    }
  }
  
  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_toolbar,
								      AGS_TYPE_WAVE_EDITOR);
  
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom) - 2.0);

  /* edit */
  wave_editor->ruler->factor = zoom_factor;
  wave_editor->ruler->precision = zoom;
  wave_editor->ruler->scale_precision = 1.0 / zoom;
  
  gtk_widget_queue_draw((GtkWidget *) wave_editor->ruler);

  list_start = gtk_container_get_children(wave_editor->scrolled_wave_edit_box->wave_edit_box);
  ags_wave_toolbar_zoom_callback_apply(list_start);

  g_list_free(list_start);

  ags_wave_editor_reset_scrollbar(wave_editor);
}
