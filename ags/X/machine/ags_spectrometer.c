/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

  if(ags_spectrometer_cartesian_queue_draw == NULL){
    ags_spectrometer_cartesian_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								  NULL,
								  NULL);
  }

  spectrometer->name = NULL;
  spectrometer->xml_type = "ags-spectrometer";

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

  /* cartesian - size, pack and redraw */
  gtk_widget_set_size_request((GtkWidget *) cartesian,
			      (gint) (width + 2.0 * cartesian->x_margin), (gint) (height + 2.0 * cartesian->y_margin));
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(cartesian),
		     FALSE, FALSE,
		     0);

  gtk_widget_queue_draw((GtkWidget *) cartesian);

  /* buffer-size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size");
  }
  
  if(str != NULL){
    buffer_size = g_ascii_strtoull(str,
				   NULL,
				   10);
    free(str);
  }else{
    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  }

  /* port */
  spectrometer->frequency_buffer_play_port = NULL;
  spectrometer->frequency_buffer_recall_port = NULL;

  spectrometer->magnitude_buffer_play_port = NULL;
  spectrometer->magnitude_buffer_recall_port = NULL;
  
  /* buffer */
  spectrometer->buffer_size = ceil(buffer_size / 2.0);
  
  spectrometer->frequency_buffer = (double *) malloc(ceil(buffer_size / 2.0) * sizeof(double));
  ags_audio_buffer_util_clear_double(spectrometer->frequency_buffer, 1,
				     ceil(buffer_size / 2.0));
  
  spectrometer->magnitude_buffer = (double *) malloc(ceil(buffer_size / 2.0) * sizeof(double));
  ags_audio_buffer_util_clear_double(spectrometer->magnitude_buffer, 1,
				     ceil(buffer_size / 2.0));
  
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

  g_hash_table_remove(ags_spectrometer_cartesian_queue_draw,
		      spectrometer->cartesian);
  
  g_list_free_full(spectrometer->frequency_buffer_play_port,
		   g_object_unref);

  g_list_free_full(spectrometer->frequency_buffer_recall_port,
		   g_object_unref);

  g_list_free_full(spectrometer->magnitude_buffer_play_port,
		   g_object_unref);

  g_list_free_full(spectrometer->magnitude_buffer_recall_port,
		   g_object_unref);

  g_free(spectrometer->frequency_buffer);
  g_free(spectrometer->magnitude_buffer);
  
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

  g_signal_connect_after(spectrometer, "resize-audio-channels",
			 G_CALLBACK(ags_spectrometer_resize_audio_channels_callback), NULL);
  
  g_signal_connect_after(spectrometer, "resize-pads",
			 G_CALLBACK(ags_spectrometer_resize_pads_callback), NULL);

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

  g_object_disconnect(spectrometer,
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_spectrometer_resize_audio_channels_callback),
		      NULL,
		      NULL);

  g_object_disconnect(spectrometer,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_spectrometer_resize_pads_callback),
		      NULL,
		      NULL);

  /* call parent */
  ags_spectrometer_parent_connectable_interface->disconnect(connectable);
}

void
ags_spectrometer_map_recall(AgsMachine *machine)
{
  AgsSpectrometer *spectrometer;
  AgsCartesian *cartesian;
  
  AgsPlot *fg_plot;
  
  AgsAudio *audio;
  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;
  
  AgsPeakChannelRun *recall_peak_channel_run, *play_peak_channel_run;

  guint audio_channels;
  guint i;

  spectrometer = (AgsSpectrometer *) machine;

  cartesian = spectrometer->cartesian;
  
  audio = machine->audio;
  
  /* get some fields */
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "input", &start_channel,
	       NULL);
  
  /* ags-analyse */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-analyse",
			    0, audio_channels,
			    0, 1,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }

  next_channel = NULL;
  
  for(i = 0; i < audio_channels; i++){
    AgsPort *port;

    GList *start_play;
    GList *start_recall;
    
    fg_plot = ags_spectrometer_fg_plot_alloc(spectrometer,
					     0.125, 0.5, 1.0);
    ags_cartesian_add_plot(cartesian,
			   fg_plot);
    
    spectrometer->fg_plot = g_list_prepend(spectrometer->fg_plot,
					   fg_plot);

    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);
    
    /* frequency - find port */
    port = ags_spectrometer_find_specifier(start_play,
					   "./frequency-buffer[0]");

    if(port != NULL){
      g_object_ref(port);

      spectrometer->frequency_buffer_play_port = g_list_prepend(spectrometer->frequency_buffer_play_port,
								port);
    }

    port = ags_spectrometer_find_specifier(start_recall,
					   "./frequency-buffer[0]");

    if(port != NULL){
      g_object_ref(port);

      spectrometer->frequency_buffer_recall_port = g_list_prepend(spectrometer->frequency_buffer_recall_port,
								  port);
    }

    /* magnitude - find port */
    port = ags_spectrometer_find_specifier(start_play,
					   "./magnitude-buffer[0]");

    if(port != NULL){
      g_object_ref(port);

      spectrometer->magnitude_buffer_play_port = g_list_prepend(spectrometer->magnitude_buffer_play_port,
								port);
    }

    port = ags_spectrometer_find_specifier(start_recall,
					   "./magnitude-buffer[0]");

    if(port != NULL){
      g_object_ref(port);

      spectrometer->magnitude_buffer_recall_port = g_list_prepend(spectrometer->magnitude_buffer_recall_port,
								  port);
    }

    g_list_free_full(start_play,
		     g_object_unref);
    g_list_free_full(start_recall,
		     g_object_unref);
    
    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  /* call parent */
  AGS_MACHINE_CLASS(ags_spectrometer_parent_class)->map_recall(machine);
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
  
  correction = (double) 44100.0 / (double) AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

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
  AgsSpectrometer *spectrometer;

  if(g_hash_table_lookup(ags_spectrometer_cartesian_queue_draw,
			 widget) != NULL){    
    GList *fg_plot;
    GList *frequency_buffer_port;
    GList *magnitude_buffer_port;

    gdouble correction;
    double frequency;
    double magnitude;
    guint i;
    guint j, j_stop;
    guint k, k_stop;
    guint nth;
      
    GValue value = {0,};

    spectrometer = (AgsSpectrometer *) gtk_widget_get_ancestor(widget,
							       AGS_TYPE_SPECTROMETER);

    fg_plot = spectrometer->fg_plot;
    correction = (double) 44100.0 / (double) AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

    frequency_buffer_port = spectrometer->frequency_buffer_play_port;
    magnitude_buffer_port = spectrometer->magnitude_buffer_play_port;

    if(frequency_buffer_port == NULL ||
       magnitude_buffer_port == NULL){
      return(TRUE);
    }
    
    for(i = 0; fg_plot != NULL; i++){
      g_value_init(&value, G_TYPE_POINTER);
      
      g_value_set_pointer(&value, spectrometer->frequency_buffer);
      ags_port_safe_read(frequency_buffer_port->data, &value);

      g_value_reset(&value);

      g_value_set_pointer(&value, spectrometer->magnitude_buffer);
      ags_port_safe_read(magnitude_buffer_port->data, &value);

      g_value_unset(&value);

      magnitude = 0.0;

      for(nth = 1, j = 1, k = 0; nth < spectrometer->buffer_size; nth++){
	frequency = nth * correction;

	magnitude += spectrometer->magnitude_buffer[nth];
	k++;
	
	if(frequency > ((correction / 2.0) * (exp(((nth / spectrometer->buffer_size) * 18.0) / 12.0) - 1.0))){
	  if(nth - 1 != 0){
	    AGS_PLOT(fg_plot->data)->point[j][1] = 20.0 * log10(((double) magnitude / (double) k) + 1.0) * AGS_SPECTROMETER_EXTRA_SCALE;
	    //	    g_message("plot[%d]: %f %f", j, frequency, magnitude);
	  }
	  
	  j++;

	  if(j >= AGS_SPECTROMETER_PLOT_DEFAULT_POINT_COUNT){
	    break;
	  }
	  
	  magnitude = 0.0;
	  k = 0;
	}
      }

      /* iterate */
      fg_plot = fg_plot->next;

      frequency_buffer_port = frequency_buffer_port->next;
      magnitude_buffer_port = magnitude_buffer_port->next;
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
