/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/editor/ags_wave_edit_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_composite_edit.h>

#include <math.h>

void
ags_wave_edit_update_ui_callback(GObject *ui_provider,
				 AgsWaveEdit *wave_edit)
{
  AgsCompositeEditor *composite_editor;

  GtkAdjustment *hscrollbar_adjustment;

  AgsAudio *audio;
  
  AgsApplicationContext *application_context;

  GObject *output_soundcard;
    
  double zoom_factor;
  double width;
  double x;

  if((AGS_WAVE_EDIT_AUTO_SCROLL & (wave_edit->flags)) == 0){
    return;
  }
    
  application_context = ags_application_context_get_instance();
    
  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
    
  if(composite_editor->selected_machine == NULL){
    return;
  }

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(composite_editor->toolbar)->zoom));

  audio = composite_editor->selected_machine->audio;      

  output_soundcard = ags_audio_get_output_soundcard(audio);

  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(wave_edit->hscrollbar);
    
  /* reset offset */
  wave_edit->note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
  wave_edit->note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(output_soundcard));

  /* 256th */
  wave_edit->note_offset_256th = 16 * wave_edit->note_offset;
  wave_edit->note_offset_256th_absolute = 16 * wave_edit->note_offset_absolute;

  /* reset scrollbar */
  x = (double) wave_edit->note_offset * (double) wave_edit->control_width / zoom_factor;
    
  width = (double) gtk_widget_get_width(wave_edit->drawing_area);
  
  if(x < gtk_adjustment_get_value(hscrollbar_adjustment) ||
     x > gtk_adjustment_get_value(hscrollbar_adjustment) + (zoom_factor * width * (3.0 / 4.0))){
    gtk_adjustment_set_value(hscrollbar_adjustment,
			     x);

    gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(composite_editor->wave_edit->hscrollbar),
			     x);
  }else{
    gtk_widget_queue_draw(wave_edit->drawing_area);
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }
}

void
ags_wave_edit_draw_callback(GtkWidget *drawing_area,
			    cairo_t *cr,
			    gint width, gint height,
			    AgsWaveEdit *wave_edit)
{
  ags_wave_edit_draw(wave_edit, cr);
}

void
ags_wave_edit_drawing_area_resize_callback(GtkWidget *drawing_area,
					   gint width, gint height,
					   AgsWaveEdit *wave_edit)
{
  ags_wave_edit_reset_vscrollbar(wave_edit);
  ags_wave_edit_reset_hscrollbar(wave_edit);
  
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

void
ags_wave_edit_vscrollbar_value_changed(GtkAdjustment *adjustment, AgsWaveEdit *wave_edit)
{
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
}

void
ags_wave_edit_hscrollbar_value_changed(GtkAdjustment *adjustment, AgsWaveEdit *wave_edit)
{
  AgsCompositeEditor *composite_editor;
  
  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;
  gdouble value;

  application_context = ags_application_context_get_instance();

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  value = gtk_adjustment_get_value(adjustment) / (gui_scale_factor * 64.0);
  gtk_adjustment_set_value(wave_edit->ruler->adjustment,
			   gui_scale_factor * value);
  gtk_widget_queue_draw((GtkWidget *) wave_edit->ruler);
    
  gtk_adjustment_set_value(composite_editor->wave_edit->ruler->adjustment,
			   gui_scale_factor * value);
  gtk_widget_queue_draw((GtkWidget *) composite_editor->wave_edit->ruler);

  gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(composite_editor->tempo_edit->hscrollbar),
			   gtk_adjustment_get_value(adjustment));
  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
}

