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

#include <ags/app/machine/ags_spectrometer_callbacks.h>

#include <ags/app/ags_window.h>

void
ags_spectrometer_update_ui_callback(GObject *ui_provider,
				    AgsSpectrometer *spectrometer)
{
  AgsPort *port;
    
  GList *fg_plot;
  GList *start_recall, *recall;

  guint samplerate;
  guint buffer_size;
  guint audio_buffer_size;
  gdouble correction;
  gdouble frequency;
  gdouble gfrequency;
  double magnitude;
  guint copy_mode;
  guint i;
  guint j;
  guint k;
    
  GValue value = {0,};

  samplerate = AGS_MACHINE(spectrometer)->samplerate;
  buffer_size = AGS_MACHINE(spectrometer)->buffer_size;

  audio_buffer_size = 0;
    
  g_object_get(AGS_MACHINE(spectrometer)->audio,
	       "buffer-size", &audio_buffer_size,
	       NULL);

  if(buffer_size != audio_buffer_size){
    return;
  }
    
  ags_audio_buffer_util_clear_double(spectrometer->audio_buffer_util,
				     spectrometer->magnitude, 1,
				     buffer_size);

  copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE;

  /* play context */
  recall = ags_recall_container_get_recall_channel(spectrometer->analyse_play_container);

  start_recall = g_list_copy(recall);
    
  recall = start_recall;
    
  while(recall != NULL){
    /* get magnitude */
    port = NULL;

    g_object_get(recall->data,
		 "magnitude", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_POINTER);
	
      g_value_set_pointer(&value, spectrometer->magnitude_cache);
	
      ags_port_safe_read(port, &value);
	
      g_value_unset(&value);

      g_object_unref(port);
    }

    /* copy cache */
    ags_audio_buffer_util_copy_buffer_to_buffer(spectrometer->audio_buffer_util,
						spectrometer->magnitude, 1, 0,
						spectrometer->magnitude_cache, 1, 0,
						buffer_size, copy_mode);
      
    recall = recall->next;
  }

  /* recall context */
  recall = ags_recall_container_get_recall_channel(spectrometer->analyse_recall_container);

  start_recall = g_list_copy(recall);

  while(recall != NULL){
    /* get magnitude */
    port = NULL;

    g_object_get(recall->data,
		 "magnitude", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_POINTER);
	
      g_value_set_pointer(&value, spectrometer->magnitude_cache);
	
      ags_port_safe_read(port, &value);
	
      g_value_unset(&value);

      g_object_unref(port);
    }

    /* copy cache */
    ags_audio_buffer_util_copy_buffer_to_buffer(spectrometer->audio_buffer_util,
						spectrometer->magnitude, 1, 0,
						spectrometer->magnitude_cache, 1, 0,
						buffer_size, copy_mode);
      
    recall = recall->next;
  }
    
  /* plot */
  fg_plot = spectrometer->fg_plot;
    
  correction = (44100.0 - AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE) / (double) AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  while(fg_plot != NULL){
    magnitude = 0.0;

    frequency = 0.0;
    gfrequency = 0.0;
      
    for(i = 0, j = 1; i < AGS_SPECTROMETER_PLOT_DEFAULT_POINT_COUNT && j < buffer_size / 2; i++){
      frequency = (double) j * correction;

      if(AGS_SPECTROMETER_DEFAULT_X_END >= 0.0 &&
	 AGS_SPECTROMETER_DEFAULT_X_START < 0.0){
	gfrequency = (correction / 2.0) * (exp((((double) i) / (gdouble) AGS_SPECTROMETER_PLOT_DEFAULT_POINT_COUNT * ((AGS_SPECTROMETER_DEFAULT_X_END + AGS_SPECTROMETER_DEFAULT_X_START) / AGS_CARTESIAN_DEFAULT_X_STEP_WIDTH)) / 12.0) - 1.0);
      }else if(AGS_SPECTROMETER_DEFAULT_X_END >= 0.0 &&
	       AGS_SPECTROMETER_DEFAULT_X_START >= 0.0){
	gfrequency = (correction / 2.0) * (exp((((double) i) / (gdouble) AGS_SPECTROMETER_PLOT_DEFAULT_POINT_COUNT * ((AGS_SPECTROMETER_DEFAULT_X_END - AGS_SPECTROMETER_DEFAULT_X_START) / AGS_CARTESIAN_DEFAULT_X_STEP_WIDTH)) / 12.0) - 1.0);
      }else{
	g_message("only positive frequencies allowed");
      }

      magnitude = 0.0;
	
      for(k = 0; j < buffer_size / 2 && frequency < gfrequency; j++, k++){
	frequency = (double) j * correction;

	magnitude += spectrometer->magnitude[j];
      }

      if(magnitude < 0.0){
	magnitude *= -1.0;
      }
	
      if(k != 0){
	AGS_PLOT(fg_plot->data)->point[i][1] = 20.0 * log10(((double) magnitude / (double) k) + 1.0) * AGS_SPECTROMETER_EXTRA_SCALE;
      }else{
	AGS_PLOT(fg_plot->data)->point[i][1] = 0.0;
      }

      if(frequency > samplerate ||
	 gfrequency > samplerate){
	break;
      }
    }

    /* iterate */
    fg_plot = fg_plot->next;
  }
        
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) spectrometer->cartesian);
}
