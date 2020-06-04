/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/machine/ags_spectrometer.h>
#include <ags/X/machine/ags_spectrometer_callbacks.h>

#include <ags/X/ags_ui_provider.h>

void ags_spectrometer_class_init(AgsSpectrometerClass *spectrometer);
void ags_spectrometer_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_spectrometer_init(AgsSpectrometer *spectrometer);
void ags_spectrometer_finalize(GObject *gobject);

void ags_spectrometer_map_recall(AgsMachine *machine);

void ags_spectrometer_connect(AgsConnectable *connectable);
void ags_spectrometer_disconnect(AgsConnectable *connectable);

void ags_spectrometer_resize_audio_channels_callback(AgsMachine *machine,
						     guint audio_channels, guint audio_channels_old,
						     gpointer data);
void ags_spectrometer_resize_pads_callback(AgsMachine *machine,
					   GType channel_type,
					   guint pads, guint pads_old,
					   gpointer data);

void ags_spectrometer_buffer_size_changed_callback(AgsMachine *machine,
						   guint buffer_size, guint old_buffer_size,
						   gpointer data);

gdouble ags_spectrometer_x_small_scale_func(gdouble value,
					    gpointer data);
gdouble ags_spectrometer_x_big_scale_func(gdouble value,
					  gpointer data);

gchar* ags_spectrometer_x_label_func(gdouble value,
				     gpointer data);
gchar* ags_spectrometer_y_label_func(gdouble value,
				     gpointer data);

/**
 * SECTION:ags_spectrometer
 * @short_description: spectrometer sequencer
 * @title: AgsSpectrometer
 * @section_id:
 * @include: ags/X/machine/ags_spectrometer.h
 *
 * The #AgsSpectrometer is a composite widget to act as spectrometer sequencer.
 */

static gpointer ags_spectrometer_parent_class = NULL;

static AgsConnectableInterface *ags_spectrometer_parent_connectable_interface;

GHashTable *ags_spectrometer_cartesian_queue_draw = NULL;

GType
ags_spectrometer_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_spectrometer = 0;

    static const GTypeInfo ags_spectrometer_info = {
      sizeof(AgsSpectrometerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_spectrometer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSpectrometer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_spectrometer_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_spectrometer_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_spectrometer = g_type_register_static(AGS_TYPE_MACHINE,
						   "AgsSpectrometer", &ags_spectrometer_info,
						   0);
    
    g_type_add_interface_static(ags_type_spectrometer,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_spectrometer);
  }

  return g_define_type_id__volatile;
}

void
ags_spectrometer_class_init(AgsSpectrometerClass *spectrometer)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_spectrometer_parent_class = g_type_class_peek_parent(spectrometer);

  /* GObjectClass */
  gobject = (GObjectClass *) spectrometer;

  gobject->finalize = ags_spectrometer_finalize;

  /*  */
  machine = (AgsMachineClass *) spectrometer;

  machine->map_recall = ags_spectrometer_map_recall;
}

void
ags_spectrometer_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_spectrometer_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_spectrometer_connect;
  connectable->disconnect = ags_spectrometer_disconnect;
}

void
ags_spectrometer_init(AgsSpectrometer *spectrometer)
{
  GtkVBox *vbox;
  AgsCartesian *cartesian;

  AgsPlot *fg_plot;

  AgsConfig *config;

  gchar *str;

  guint buffer_size;
  gdouble width, height;
  gdouble default_width, default_height;
  gdouble offset;
  
  config = ags_config_get_instance();
  
  g_signal_connect_after((GObject *) spectrometer, "parent_set",
			 G_CALLBACK(ags_spectrometer_parent_set_callback), (gpointer) spectrometer);

  ags_audio_set_flags(AGS_MACHINE(spectrometer)->audio, (AGS_AUDIO_SYNC));
  g_object_set(AGS_MACHINE(spectrometer)->audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       NULL);

  g_signal_connect_after(spectrometer, "resize-audio-channels",
			 G_CALLBACK(ags_spectrometer_resize_audio_channels_callback), NULL);
  
  g_signal_connect_after(spectrometer, "resize-pads",
			 G_CALLBACK(ags_spectrometer_resize_pads_callback), NULL);

  g_signal_connect_after(spectrometer, "buffer-size-changed",
			 G_CALLBACK(ags_spectrometer_buffer_size_changed_callback), NULL);

  if(ags_spectrometer_cartesian_queue_draw == NULL){
    ags_spectrometer_cartesian_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								  NULL,
								  NULL);
  }

  spectrometer->name = NULL;
  spectrometer->xml_type = "ags-spectrometer";

  spectrometer->mapped_output_pad = 0;
  spectrometer->mapped_input_pad = 0;

  spectrometer->analyse_play_container = ags_recall_container_new();
  spectrometer->analyse_recall_container = ags_recall_container_new();

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) gtk_bin_get_child((GtkBin *) spectrometer), (GtkWidget *) vbox);

  /* cartesian */
  cartesian = 
    spectrometer->cartesian = ags_cartesian_new();

  cartesian->x_start = AGS_SPECTROMETER_DEFAULT_X_START;
  cartesian->x_end = AGS_SPECTROMETER_DEFAULT_X_END;

  cartesian->y_start = AGS_SPECTROMETER_DEFAULT_Y_START;
  cartesian->y_end = AGS_SPECTROMETER_DEFAULT_Y_END;
  
  cartesian->surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
						  cartesian->x_end - cartesian->x_start, cartesian->y_end - cartesian->y_start);

  cartesian->x_small_scale_func = ags_spectrometer_x_small_scale_func;
  cartesian->x_big_scale_func = ags_spectrometer_x_big_scale_func;

  cartesian->x_label_func = ags_spectrometer_x_label_func;
  cartesian->y_label_func = ags_spectrometer_y_label_func;

  /* label */
  cartesian->x_label_precision = 1.0;
  
  ags_cartesian_reallocate_label(cartesian,
				 TRUE);
  ags_cartesian_reallocate_label(cartesian,
				 FALSE);

  ags_cartesian_fill_label(cartesian,
			   TRUE);
  ags_cartesian_fill_label(cartesian,
			   FALSE);  
  
  width = cartesian->x_end - cartesian->x_start;
  height = cartesian->y_end - cartesian->y_start;

  spectrometer->fg_plot = NULL;

  /* alloc fg plot */
  fg_plot = ags_spectrometer_fg_plot_alloc(spectrometer,
					   0.125, 0.5, 1.0);
  ags_cartesian_add_plot(cartesian,
			 fg_plot);
    
  spectrometer->fg_plot = g_list_prepend(spectrometer->fg_plot,
					 fg_plot);

  /* cartesian - size, pack and redraw */
  gtk_widget_set_size_request((GtkWidget *) cartesian,
			      (gint) (width + 2.0 * cartesian->x_margin), (gint) (height + 2.0 * cartesian->y_margin));
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(cartesian),
		     FALSE, FALSE,
		     0);

  gtk_widget_queue_draw((GtkWidget *) cartesian);

  /* buffer */
  buffer_size = AGS_MACHINE(spectrometer)->buffer_size;
  
  spectrometer->magnitude_cache = (double *) g_malloc(buffer_size * sizeof(double));
  ags_audio_buffer_util_clear_double(spectrometer->magnitude_cache, 1,
				     buffer_size);

  spectrometer->magnitude = (double *) g_malloc(buffer_size * sizeof(double));
  ags_audio_buffer_util_clear_double(spectrometer->magnitude, 1,
				     buffer_size);
  
  /* queue draw */
  g_hash_table_insert(ags_spectrometer_cartesian_queue_draw,
		      cartesian, ags_spectrometer_cartesian_queue_draw_timeout);
  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0, (GSourceFunc) ags_spectrometer_cartesian_queue_draw_timeout, (gpointer) cartesian);
}

void
ags_spectrometer_finalize(GObject *gobject)
{
  AgsSpectrometer *spectrometer;

  spectrometer = (AgsSpectrometer *) gobject;

  g_object_disconnect(spectrometer,
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_spectrometer_resize_audio_channels_callback),
		      NULL,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_spectrometer_resize_pads_callback),
		      NULL,
		      "any_signal::buffer-size-changed",
		      G_CALLBACK(ags_spectrometer_buffer_size_changed_callback),
		      NULL,
		      NULL);

  g_hash_table_remove(ags_spectrometer_cartesian_queue_draw,
		      spectrometer->cartesian);
  
  g_free(spectrometer->magnitude_cache);
  
  /* call parent */
  G_OBJECT_CLASS(ags_spectrometer_parent_class)->finalize(gobject);
}

void
ags_spectrometer_connect(AgsConnectable *connectable)
{
  AgsSpectrometer *spectrometer;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  spectrometer = AGS_SPECTROMETER(connectable);

  /* call parent */
  ags_spectrometer_parent_connectable_interface->connect(connectable);
}

void
ags_spectrometer_disconnect(AgsConnectable *connectable)
{
  AgsSpectrometer *spectrometer;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  spectrometer = AGS_SPECTROMETER(connectable);

  /* call parent */
  ags_spectrometer_parent_connectable_interface->disconnect(connectable);
}

void
ags_spectrometer_resize_audio_channels_callback(AgsMachine *machine,
						guint audio_channels, guint audio_channels_old,
						gpointer data)
{
  AgsSpectrometer *spectrometer;
  AgsCartesian *cartesian;

  AgsAudio *audio;
  
  spectrometer = (AgsSpectrometer *) machine;  

  cartesian = spectrometer->cartesian;

  audio = machine->audio;
  
  if(audio_channels > audio_channels_old){
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_spectrometer_input_map_recall(spectrometer,
					audio_channels_old,
					0);

      ags_spectrometer_output_map_recall(spectrometer,
					 audio_channels_old,
					 0);
    }
  }
}

void
ags_spectrometer_resize_pads_callback(AgsMachine *machine,
				      GType channel_type,
				      guint pads, guint pads_old,
				      gpointer data)
{
  AgsSpectrometer *spectrometer;

  AgsCartesian *cartesian;
    
  AgsAudio *audio;
  
  guint audio_channels;
  gboolean grow;

  spectrometer = (AgsSpectrometer *) machine;

  cartesian = spectrometer->cartesian;
  
  audio = machine->audio;

  /* get some fields */
  audio_channels = machine->audio_channels;
  
  /* check available */
  if(pads == pads_old ||
     audio_channels == 0){
    return;
  }

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(grow){      
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_spectrometer_input_map_recall(spectrometer,
					  0,
					  pads_old);
      }
    }else{
      spectrometer->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_spectrometer_output_map_recall(spectrometer,
					   0,
					   pads_old);
      }
    }else{
      spectrometer->mapped_output_pad = pads;
    }
  }
}

void
ags_spectrometer_buffer_size_changed_callback(AgsMachine *machine,
					      guint buffer_size, guint old_buffer_size,
					      gpointer data)
{
  AgsSpectrometer *spectrometer;

  if(buffer_size == old_buffer_size){
    return;
  }

  spectrometer = (AgsSpectrometer *) machine;

  if(buffer_size > 0){
    if(spectrometer->magnitude_cache == NULL){
      spectrometer->magnitude_cache = (double *) g_malloc(buffer_size * sizeof(double));
      ags_audio_buffer_util_clear_double(spectrometer->magnitude_cache, 1,
					 buffer_size);

      spectrometer->magnitude = (double *) g_malloc(buffer_size * sizeof(double));
      ags_audio_buffer_util_clear_double(spectrometer->magnitude, 1,
					 buffer_size);
    }else{
      spectrometer->magnitude_cache = (double *) g_realloc(spectrometer->magnitude_cache,
							   buffer_size * sizeof(double));
      ags_audio_buffer_util_clear_double(spectrometer->magnitude_cache, 1,
					 buffer_size);

      spectrometer->magnitude = (double *) g_realloc(spectrometer->magnitude,
						     buffer_size * sizeof(double));
      ags_audio_buffer_util_clear_double(spectrometer->magnitude, 1,
					 buffer_size);
    }
  }else{
    g_free(spectrometer->magnitude_cache);
    g_free(spectrometer->magnitude);

    spectrometer->magnitude_cache = NULL;
    spectrometer->magnitude = NULL;
  }
}

void
ags_spectrometer_map_recall(AgsMachine *machine)
{
  AgsSpectrometer *spectrometer;
  
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  spectrometer = (AgsSpectrometer *) machine;

  audio = machine->audio;

  position = 0;
  
  /* add new controls */
  ags_fx_factory_create(machine->audio,
			spectrometer->analyse_play_container, spectrometer->analyse_recall_container,
			"ags-fx-analyse",
			NULL,
			NULL,
			0, 0,
			0, 0,
			position,
			(AGS_FX_FACTORY_ADD), 0);

  /* depending on destination */
  ags_spectrometer_input_map_recall(spectrometer,
				    0,
				    0);

  /* depending on destination */
  ags_spectrometer_output_map_recall(spectrometer,
				     0,
				     0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_spectrometer_parent_class)->map_recall(machine);
}

void
ags_spectrometer_input_map_recall(AgsSpectrometer *spectrometer,
				  guint audio_channel_start,
				  guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(spectrometer->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(spectrometer)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* add to machine */
  ags_fx_factory_create(AGS_MACHINE(spectrometer)->audio,
			spectrometer->analyse_play_container, spectrometer->analyse_recall_container,
			"ags-fx-analyse",
			NULL,
			NULL,
			audio_channel_start, audio_channels,
			input_pad_start, input_pads,
			position,
			(AGS_FX_FACTORY_REMAP), 0);
  
  spectrometer->mapped_input_pad = input_pads;
}

void
ags_spectrometer_output_map_recall(AgsSpectrometer *spectrometer,
				   guint audio_channel_start,
				   guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(spectrometer->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(spectrometer)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);

  spectrometer->mapped_output_pad = output_pads;
}

gdouble
ags_spectrometer_x_small_scale_func(gdouble value, gpointer data)
{
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    return(value);
  }

  return(AGS_CARTESIAN(data)->x_small_scale_factor * value);
}
 
gdouble
ags_spectrometer_x_big_scale_func(gdouble value, gpointer data)
{
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    return(value);
  }

  return(AGS_CARTESIAN(data)->x_big_scale_factor * value);
}

gchar*
ags_spectrometer_x_label_func(gdouble value,
			      gpointer data)
{
  gchar *format;
  gchar *str;

  gdouble correction;
  
  correction = (44100.0 - AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE) / (double) AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  format = g_strdup_printf("%%.%df",
			   (guint) ceil(AGS_CARTESIAN(data)->x_label_precision));

  str = g_strdup_printf(format,
			(correction / 2.0) * (exp(value / 12.0) - 1.0));
  g_free(format);

  return(str);
}

gchar*
ags_spectrometer_y_label_func(gdouble value,
			      gpointer data)
{
  gchar *format;
  gchar *str;
  
  format = g_strdup_printf("%%.%df",
			   (guint) ceil(AGS_CARTESIAN(data)->y_label_precision));

  str = g_strdup_printf(format,
			value / 20.0);

  return(str);
}
  
AgsPort*
ags_spectrometer_find_specifier(GList *recall, gchar *specifier)
{
  GList *port;
    
  while(recall != NULL){
    port = AGS_RECALL(recall->data)->port;

#ifdef AGS_DEBUG
    g_message("search port in %s", G_OBJECT_TYPE_NAME(recall->data));
#endif

    while(port != NULL){
      if(!g_strcmp0(AGS_PORT(port->data)->specifier,
		    specifier)){
	return(AGS_PORT(port->data));
      }

      port = port->next;
    }

    recall = recall->next;
  }

  return(NULL);
}

AgsPlot*
ags_spectrometer_fg_plot_alloc(AgsSpectrometer *spectrometer,
			       gdouble color_r, gdouble color_g, double color_b)
{
  AgsCartesian *cartesian;
  AgsPlot *plot;

  guint i, i_stop;

  cartesian = spectrometer->cartesian;

  i_stop = AGS_SPECTROMETER_PLOT_DEFAULT_POINT_COUNT + 1;
  
  plot = ags_plot_alloc(i_stop, 0, 0);
  plot->join_points = TRUE;

  for(i = 0; i < i_stop; i++){
    plot->point_color[i][0] = color_r;
    plot->point_color[i][1] = color_g;
    plot->point_color[i][2] = color_b;

    plot->point[i][0] = ((gdouble) i / (gdouble) AGS_SPECTROMETER_PLOT_DEFAULT_POINT_COUNT) * ((double) cartesian->x_end);
    plot->point[i][1] = 0.0;
  }
  
  return(plot);
}

/**
 * ags_spectrometer_cartesian_queue_draw_timeout:
 * @widget: the widget
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_spectrometer_cartesian_queue_draw_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_spectrometer_cartesian_queue_draw,
			 widget) != NULL){    
    AgsSpectrometer *spectrometer;
    AgsCartesian *cartesian;

    AgsPort *port;
    
    GList *fg_plot;
    GList *start_recall, *recall;

    guint samplerate;
    guint buffer_size;
    guint audio_buffer_size;
    gdouble nyquist;
    gdouble correction;
    gdouble frequency;
    gdouble gfrequency, gfrequency_next;
    double magnitude;
    guint copy_mode;
    guint i;
    guint j, j_stop;
    guint k, k_stop;
    guint nth;

    gboolean completed;
    
    GValue value = {0,};

    spectrometer = (AgsSpectrometer *) gtk_widget_get_ancestor(widget,
							       AGS_TYPE_SPECTROMETER);

    cartesian = spectrometer->cartesian;

    samplerate = AGS_MACHINE(spectrometer)->samplerate;
    buffer_size = AGS_MACHINE(spectrometer)->buffer_size;

    audio_buffer_size = 0;
    
    g_object_get(AGS_MACHINE(spectrometer)->audio,
		 "buffer-size", &audio_buffer_size,
		 NULL);

    if(buffer_size != audio_buffer_size){
      return(TRUE);
    }
    
    ags_audio_buffer_util_clear_double(spectrometer->magnitude, 1,
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
      ags_audio_buffer_util_copy_buffer_to_buffer(spectrometer->magnitude, 1, 0,
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
      ags_audio_buffer_util_copy_buffer_to_buffer(spectrometer->magnitude, 1, 0,
						  spectrometer->magnitude_cache, 1, 0,
						  buffer_size, copy_mode);
      
      recall = recall->next;
    }
    
    /* plot */
    fg_plot = spectrometer->fg_plot;
    
    nyquist = ((gdouble) samplerate / 2.0);
    correction = (44100.0 - AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE) / (double) AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

    while(fg_plot != NULL){
      completed = FALSE;

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
    gtk_widget_queue_draw(widget);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_spectrometer_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsSpectrometer
 *
 * Returns: a new #AgsSpectrometer
 *
 * Since: 3.0.0
 */
AgsSpectrometer*
ags_spectrometer_new(GObject *soundcard)
{
  AgsSpectrometer *spectrometer;

  spectrometer = (AgsSpectrometer *) g_object_new(AGS_TYPE_SPECTROMETER,
						  NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(spectrometer)->audio),
	       "output-soundcard", soundcard,
	       NULL);

  return(spectrometer);
}
