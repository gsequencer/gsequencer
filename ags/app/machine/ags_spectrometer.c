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

#include <ags/app/machine/ags_spectrometer.h>
#include <ags/app/machine/ags_spectrometer_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>

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
 * @include: ags/app/machine/ags_spectrometer.h
 *
 * The #AgsSpectrometer is a composite widget to act as spectrometer sequencer.
 */

static gpointer ags_spectrometer_parent_class = NULL;

static AgsConnectableInterface *ags_spectrometer_parent_connectable_interface;

GType
ags_spectrometer_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
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

    g_once_init_leave(&g_define_type_id__static, ags_type_spectrometer);
  }

  return(g_define_type_id__static);
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
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  AgsCartesian *cartesian;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;

  AgsApplicationContext *application_context;
  
  AgsPlot *fg_plot;
  
  gchar *machine_name;

  gint position;
  guint buffer_size;
  gdouble width, height;
  gdouble default_width, default_height;

  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_SPECTROMETER);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(spectrometer,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) spectrometer);
  
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

  spectrometer->name = NULL;
  spectrometer->xml_type = "ags-spectrometer";

  spectrometer->mapped_output_pad = 0;
  spectrometer->mapped_input_pad = 0;

  spectrometer->analyse_play_container = ags_recall_container_new();
  spectrometer->analyse_recall_container = ags_recall_container_new();

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);

  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 FALSE);

  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_START);
  
  gtk_frame_set_child(AGS_MACHINE(spectrometer)->frame,
		      (GtkWidget *) vbox);

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
  gtk_box_append(vbox,
		 (GtkWidget *) cartesian);

  gtk_widget_queue_draw((GtkWidget *) cartesian);

  /* buffer */
  spectrometer->audio_buffer_util = ags_audio_buffer_util_alloc();
  
  buffer_size = AGS_MACHINE(spectrometer)->buffer_size;
  
  spectrometer->magnitude_cache = (double *) g_malloc(buffer_size * sizeof(double));
  ags_audio_buffer_util_clear_double(spectrometer->audio_buffer_util,
				     spectrometer->magnitude_cache, 1,
				     buffer_size);

  spectrometer->magnitude = (double *) g_malloc(buffer_size * sizeof(double));
  ags_audio_buffer_util_clear_double(spectrometer->audio_buffer_util,
				     spectrometer->magnitude, 1,
				     buffer_size);
  
  /* update-ui */
  g_signal_connect(application_context, "update-ui",
		   G_CALLBACK(ags_spectrometer_update_ui_callback), spectrometer);
}

void
ags_spectrometer_finalize(GObject *gobject)
{
  AgsSpectrometer *spectrometer;

  AgsApplicationContext *application_context;

  spectrometer = (AgsSpectrometer *) gobject;

  application_context = ags_application_context_get_instance();

  g_object_disconnect(application_context,
		      "any_signal::update-ui",
		      G_CALLBACK(ags_spectrometer_update_ui_callback),
		      (gpointer) spectrometer,
		      NULL);
  
  g_free(spectrometer->magnitude_cache);
  
  /* call parent */
  G_OBJECT_CLASS(ags_spectrometer_parent_class)->finalize(gobject);
}

void
ags_spectrometer_connect(AgsConnectable *connectable)
{
  AgsSpectrometer *spectrometer;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
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

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
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
  
  spectrometer = (AgsSpectrometer *) machine;  
  
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

  guint audio_channels;
  gboolean grow;

  spectrometer = (AgsSpectrometer *) machine;

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
      ags_audio_buffer_util_clear_double(spectrometer->audio_buffer_util,
					 spectrometer->magnitude_cache, 1,
					 buffer_size);

      spectrometer->magnitude = (double *) g_malloc(buffer_size * sizeof(double));
      ags_audio_buffer_util_clear_double(spectrometer->audio_buffer_util,
					 spectrometer->magnitude, 1,
					 buffer_size);
    }else{
      spectrometer->magnitude_cache = (double *) g_realloc(spectrometer->magnitude_cache,
							   buffer_size * sizeof(double));
      ags_audio_buffer_util_clear_double(spectrometer->audio_buffer_util,
					 spectrometer->magnitude_cache, 1,
					 buffer_size);

      spectrometer->magnitude = (double *) g_realloc(spectrometer->magnitude,
						     buffer_size * sizeof(double));
      ags_audio_buffer_util_clear_double(spectrometer->audio_buffer_util,
					 spectrometer->magnitude, 1,
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
  start_recall = ags_fx_factory_create(audio,
				       spectrometer->analyse_play_container, spectrometer->analyse_recall_container,
				       "ags-fx-analyse",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

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
  guint i;
  guint j;

  audio = AGS_MACHINE(spectrometer)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  for(i = 0; i < input_pads; i++){
    for(j = 0; j < audio_channels; j++){
      AgsMachineInputLine *input_line;

      input_line = g_list_nth_data(AGS_MACHINE(spectrometer)->machine_input_line,
				   (i * audio_channels) + j);

      if(input_line != NULL &&
	 input_line->mapped_recall == FALSE){
	/* add to machine */
	start_recall = ags_fx_factory_create(AGS_MACHINE(spectrometer)->audio,
					     spectrometer->analyse_play_container, spectrometer->analyse_recall_container,
					     "ags-fx-analyse",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);
  
	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* now input line is mapped */
	input_line->mapped_recall = TRUE;	
      }
    }
  }
  
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
