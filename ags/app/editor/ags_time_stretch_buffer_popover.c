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

#include <ags/app/editor/ags_time_stretch_buffer_popover.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_notation_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_time_stretch_buffer_popover_class_init(AgsTimeStretchBufferPopoverClass *time_stretch_buffer_popover);
void ags_time_stretch_buffer_popover_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_time_stretch_buffer_popover_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_time_stretch_buffer_popover_init(AgsTimeStretchBufferPopover *time_stretch_buffer_popover);
void ags_time_stretch_buffer_popover_finalize(GObject *gobject);

gboolean ags_time_stretch_buffer_popover_is_connected(AgsConnectable *connectable);
void ags_time_stretch_buffer_popover_connect(AgsConnectable *connectable);
void ags_time_stretch_buffer_popover_disconnect(AgsConnectable *connectable);

void ags_time_stretch_buffer_popover_set_update(AgsApplicable *applicable, gboolean update);
void ags_time_stretch_buffer_popover_apply(AgsApplicable *applicable);
void ags_time_stretch_buffer_popover_reset(AgsApplicable *applicable);

gboolean ags_time_stretch_buffer_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
							      guint keyval,
							      guint keycode,
							      GdkModifierType state,
							      AgsTimeStretchBufferPopover *time_stretch_buffer_popover);
void ags_time_stretch_buffer_popover_key_released_callback(GtkEventControllerKey *event_controller,
							   guint keyval,
							   guint keycode,
							   GdkModifierType state,
							   AgsTimeStretchBufferPopover *time_stretch_buffer_popover);
gboolean ags_time_stretch_buffer_popover_modifiers_callback(GtkEventControllerKey *event_controller,
							    GdkModifierType keyval,
							    AgsTimeStretchBufferPopover *time_stretch_buffer_popover);

void ags_time_stretch_buffer_popover_activate_button_callback(GtkButton *activate_button,
							      AgsTimeStretchBufferPopover *time_stretch_buffer_popover);

/**
 * SECTION:ags_time_stretch_buffer_popover
 * @short_description: time_stretch tool
 * @title: AgsTimeStretchBufferPopover
 * @section_id:
 * @include: ags/app/editor/ags_time_stretch_buffer_popover.h
 *
 * The #AgsTimeStretchBufferPopover lets you time_stretch buffers.
 */

static gpointer ags_time_stretch_buffer_popover_parent_class = NULL;

GType
ags_time_stretch_buffer_popover_get_type(void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_time_stretch_buffer_popover = 0;

    static const GTypeInfo ags_time_stretch_buffer_popover_info = {
      sizeof (AgsTimeStretchBufferPopoverClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_time_stretch_buffer_popover_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTimeStretchBufferPopover),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_time_stretch_buffer_popover_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_time_stretch_buffer_popover_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_time_stretch_buffer_popover_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_time_stretch_buffer_popover = g_type_register_static(GTK_TYPE_POPOVER,
								  "AgsTimeStretchBufferPopover", &ags_time_stretch_buffer_popover_info,
								  0);
    
    g_type_add_interface_static(ags_type_time_stretch_buffer_popover,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_time_stretch_buffer_popover,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_time_stretch_buffer_popover);
  }

  return g_define_type_id__volatile;
}

void
ags_time_stretch_buffer_popover_class_init(AgsTimeStretchBufferPopoverClass *time_stretch_buffer_popover)
{
  GObjectClass *gobject;

  ags_time_stretch_buffer_popover_parent_class = g_type_class_peek_parent(time_stretch_buffer_popover);

  /* GObjectClass */
  gobject = (GObjectClass *) time_stretch_buffer_popover;

  gobject->finalize = ags_time_stretch_buffer_popover_finalize;
}

void
ags_time_stretch_buffer_popover_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_time_stretch_buffer_popover_is_connected;  
  connectable->connect = ags_time_stretch_buffer_popover_connect;
  connectable->disconnect = ags_time_stretch_buffer_popover_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_time_stretch_buffer_popover_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_time_stretch_buffer_popover_set_update;
  applicable->apply = ags_time_stretch_buffer_popover_apply;
  applicable->reset = ags_time_stretch_buffer_popover_reset;
}

void
ags_time_stretch_buffer_popover_init(AgsTimeStretchBufferPopover *time_stretch_buffer_popover)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  time_stretch_buffer_popover->connectable_flags = 0;

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) time_stretch_buffer_popover,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_time_stretch_buffer_popover_key_pressed_callback), time_stretch_buffer_popover);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_time_stretch_buffer_popover_key_released_callback), time_stretch_buffer_popover);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_time_stretch_buffer_popover_modifiers_callback), time_stretch_buffer_popover);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_popover_set_child((GtkPopover *) time_stretch_buffer_popover,
			(GtkWidget *) vbox);

  /* frequency - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* frequency - label */
  label = (GtkLabel *) gtk_label_new(i18n("frequency"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* frequency - spin button */
  time_stretch_buffer_popover->frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											    AGS_TIME_STRETCH_BUFFER_MAX_FREQUENCY,
											    0.001);
  gtk_spin_button_set_digits(time_stretch_buffer_popover->frequency,
			     3);
  gtk_spin_button_set_value(time_stretch_buffer_popover->frequency,
			    AGS_TIME_STRETCH_BUFFER_DEFAULT_FREQUENCY);
  gtk_box_append(hbox,
		 (GtkWidget *) time_stretch_buffer_popover->frequency);
  
  /* orig BPM - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* orig BPM - label */
  label = (GtkLabel *) gtk_label_new(i18n("orig BPM"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* orig BPM - spin button */
  time_stretch_buffer_popover->orig_bpm = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											   AGS_TIME_STRETCH_BUFFER_MAX_BPM,
											   1.0);
  gtk_spin_button_set_digits(time_stretch_buffer_popover->orig_bpm,
			     2);
  gtk_spin_button_set_value(time_stretch_buffer_popover->orig_bpm,
			    AGS_TIME_STRETCH_BUFFER_DEFAULT_BPM);
  gtk_box_append(hbox,
		 (GtkWidget *) time_stretch_buffer_popover->orig_bpm);
  
  /* new BPM - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* new BPM - label */
  label = (GtkLabel *) gtk_label_new(i18n("new BPM"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* new BPM - spin button */
  time_stretch_buffer_popover->new_bpm = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											  AGS_TIME_STRETCH_BUFFER_MAX_BPM,
											  1.0);
  gtk_spin_button_set_digits(time_stretch_buffer_popover->new_bpm,
			     2);
  gtk_spin_button_set_value(time_stretch_buffer_popover->new_bpm,
			    AGS_TIME_STRETCH_BUFFER_DEFAULT_BPM);
  gtk_box_append(hbox,
		 (GtkWidget *) time_stretch_buffer_popover->new_bpm);

  /* buttons */
  time_stretch_buffer_popover->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
								    AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) time_stretch_buffer_popover->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) time_stretch_buffer_popover->action_area);

  time_stretch_buffer_popover->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("apply"));

  gtk_box_append(time_stretch_buffer_popover->action_area,
		 (GtkWidget *) time_stretch_buffer_popover->activate_button);

  g_signal_connect(time_stretch_buffer_popover->activate_button, "clicked",
		   G_CALLBACK(ags_time_stretch_buffer_popover_activate_button_callback), time_stretch_buffer_popover);

  gtk_popover_set_default_widget((GtkPopover *) time_stretch_buffer_popover,
				 (GtkWidget *) time_stretch_buffer_popover->activate_button);
}

gboolean
ags_time_stretch_buffer_popover_is_connected(AgsConnectable *connectable)
{
  AgsTimeStretchBufferPopover *time_stretch_buffer_popover;
  
  gboolean is_connected;
  
  time_stretch_buffer_popover = AGS_TIME_STRETCH_BUFFER_POPOVER(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (time_stretch_buffer_popover->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_time_stretch_buffer_popover_connect(AgsConnectable *connectable)
{
  AgsTimeStretchBufferPopover *time_stretch_buffer_popover;

  time_stretch_buffer_popover = AGS_TIME_STRETCH_BUFFER_POPOVER(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  time_stretch_buffer_popover->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_time_stretch_buffer_popover_disconnect(AgsConnectable *connectable)
{
  AgsTimeStretchBufferPopover *time_stretch_buffer_popover;

  time_stretch_buffer_popover = AGS_TIME_STRETCH_BUFFER_POPOVER(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  time_stretch_buffer_popover->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
}

void
ags_time_stretch_buffer_popover_finalize(GObject *gobject)
{
  AgsTimeStretchBufferPopover *time_stretch_buffer_popover;

  time_stretch_buffer_popover = (AgsTimeStretchBufferPopover *) gobject;
  
  G_OBJECT_CLASS(ags_time_stretch_buffer_popover_parent_class)->finalize(gobject);
}

void
ags_time_stretch_buffer_popover_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_time_stretch_buffer_popover_apply(AgsApplicable *applicable)
{
  AgsTimeStretchBufferPopover *time_stretch_buffer_popover;  
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsNotebook *notebook;

  AgsAudio *audio;
  AgsAudioBufferUtil *audio_buffer_util;
  
  AgsTimestamp *timestamp;

  AgsApplicationContext *application_context;

  GObject *output_soundcard;

  AgsTimeStretchUtil time_stretch_util;

  GList *start_wave, *wave;
  GList *start_new_wave, *new_wave;
  
  guint samplerate;
  guint buffer_length;
  guint format;
  gdouble delay;
  guint64 relative_offset;
  guint64 x0;
  guint64 x1;
  gdouble factor;
  guint copy_mode;
  gboolean has_selection;
  gint i;

  time_stretch_buffer_popover = AGS_TIME_STRETCH_BUFFER_POPOVER(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;

  notebook = composite_editor->wave_edit->channel_selector;
  
  audio = machine->audio;

  audio_buffer_util = ags_audio_buffer_util_alloc();
  
  output_soundcard = NULL;

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_length = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  start_wave = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_length,
	       "format", &format,
	       "wave", &start_wave,
	       NULL);

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  relative_offset = samplerate * AGS_WAVE_DEFAULT_BUFFER_LENGTH;
  
  start_new_wave = NULL;

  time_stretch_util.source = NULL;
  time_stretch_util.source_stride = 1;
  time_stretch_util.source_buffer_length = 0;

  time_stretch_util.destination = NULL;
  time_stretch_util.destination_stride = 1;
  time_stretch_util.destination_buffer_length = 0;
  
  time_stretch_util.buffer_length = buffer_length;
  time_stretch_util.format = format;
  time_stretch_util.samplerate = samplerate;

  time_stretch_util.frequency = gtk_spin_button_get_value(time_stretch_buffer_popover->frequency);

  time_stretch_util.orig_bpm = gtk_spin_button_get_value(time_stretch_buffer_popover->orig_bpm);
  time_stretch_util.new_bpm = gtk_spin_button_get_value(time_stretch_buffer_popover->new_bpm);

  factor = time_stretch_util.new_bpm / time_stretch_util.orig_bpm;
  
  copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(audio_buffer_util,
							      ags_audio_buffer_util_format_from_soundcard(audio_buffer_util,
													  format),
							      ags_audio_buffer_util_format_from_soundcard(audio_buffer_util,
													  format));
  
  i = 0;
  
  while((i = ags_notebook_next_active_tab(notebook,
					  i)) != -1){
    wave = start_wave;
    timestamp->timer.ags_offset.offset = 0;

    while((wave = ags_wave_find_near_timestamp(wave, i,
					       timestamp)) != NULL){
      AgsWave *current_new_wave;

      GList *start_buffer, *buffer;

      buffer = 
	start_buffer = ags_wave_get_buffer(wave->data);

      time_stretch_util.source = NULL;
      time_stretch_util.source_buffer_length = 0;
      
      time_stretch_util.destination = NULL;
      time_stretch_util.destination_buffer_length = 0;
      
      x0 = 0;
      x1 = 0;

      has_selection = FALSE;

      while(buffer != NULL){
	if(ags_buffer_test_flags(buffer->data, AGS_BUFFER_IS_SELECTED)){
	  if(!has_selection){
	    x0 = ags_buffer_get_x(buffer->data);
	  }
	  
	  x1 = ags_buffer_get_x(buffer->data) + buffer_length;
	  
	  has_selection = TRUE;
	}
      
	/* iterate */
	buffer = buffer->next;
      }

      if(has_selection){
	guint64 j;
	
	buffer = start_buffer;

	time_stretch_util.source = ags_stream_alloc((x1 - x0),
						    format);
	time_stretch_util.source_buffer_length = x1 - x0;

	time_stretch_util.destination = ags_stream_alloc((guint) floor((1.0 / factor) * (x1 - x0)),
							 format);
	time_stretch_util.destination_buffer_length = (guint) floor((1.0 / factor) * (x1 - x0));
	
	while(buffer != NULL){
	  if(ags_buffer_test_flags(buffer->data, AGS_BUFFER_IS_SELECTED)){
	    guint64 current_x;
	    
	    current_x = ags_buffer_get_x(buffer->data);

	    if(current_x >= x0 && current_x < x1){
	      ags_buffer_lock(buffer->data);
	      
	      ags_audio_buffer_util_copy_buffer_to_buffer(audio_buffer_util,
							  time_stretch_util.source, 1, current_x - x0,
							  ags_buffer_get_data(buffer->data), 1, 0,
							  buffer_length, copy_mode);

	      ags_audio_buffer_util_clear_buffer(audio_buffer_util,
						 ags_buffer_get_data(buffer->data), 1,
						 buffer_length, ags_audio_buffer_util_format_from_soundcard(audio_buffer_util,
													  format));
	      
	      ags_buffer_unlock(buffer->data);
	    }
	    
	    has_selection = TRUE;
	  }
      
	  /* iterate */	
	  buffer = buffer->next;
	}

	ags_time_stretch_util_stretch(&time_stretch_util);

	current_new_wave = ags_wave_new((GObject *) audio,
					i);

	g_object_set(current_new_wave,
		     "samplerate", samplerate,
		     "buffer-size", buffer_length,
		     "format", format,
		     NULL);
	
	current_new_wave->timestamp->timer.ags_offset.offset = (guint64) relative_offset * floor(x0 / relative_offset);

	start_new_wave = g_list_insert_sorted(start_new_wave,
					      current_new_wave,
					      (GCompareFunc) ags_wave_sort_func);

	for(j = x0; j < x1;){
	  AgsBuffer *current_new_buffer;

	  guint frame_count;
	  
	  current_new_buffer = ags_buffer_new();

	  g_object_set(current_new_buffer,
		       "x", j,
		       "samplerate", samplerate,
		       "buffer-size", buffer_length,
		       "format", format,
		       NULL);

	  ags_wave_add_buffer(current_new_wave,
			      current_new_buffer,
			      FALSE);
	      
	  frame_count = buffer_length;
	  
	  if(floor(j / relative_offset) < floor((j + frame_count) / relative_offset)){
	    frame_count = buffer_length - ((j + frame_count) % relative_offset);
	  }
	  
	  ags_buffer_lock(current_new_buffer);

	  ags_audio_buffer_util_copy_buffer_to_buffer(audio_buffer_util,
						      ags_buffer_get_data(current_new_buffer), 1, 0,
						      time_stretch_util.destination, 1, j - x0,
						      frame_count, copy_mode);

	  ags_buffer_unlock(current_new_buffer);
      
	  /* iterate */
	  if(floor(j / relative_offset) < floor((j + buffer_length) / relative_offset)){
	    current_new_wave = ags_wave_new((GObject *) audio,
					    i);

	    g_object_set(current_new_wave,
			 "samplerate", samplerate,
			 "buffer-size", buffer_length,
			 "format", format,
			 NULL);
	
	    current_new_wave->timestamp->timer.ags_offset.offset = (guint64) relative_offset * floor((j + buffer_length) / relative_offset);

	    start_new_wave = g_list_insert_sorted(start_new_wave,
						  current_new_wave,
						  (GCompareFunc) ags_wave_sort_func);
	  }
	  
	  j += frame_count;
	}
      }

      ags_stream_free(time_stretch_util.source);
      ags_stream_free(time_stretch_util.destination);

      time_stretch_util.source = NULL;
      time_stretch_util.destination = NULL;
      
      /* iterate */
      timestamp->timer.ags_offset.offset += relative_offset;

      wave = wave->next;
    }

    new_wave = start_new_wave;

    while(new_wave != NULL){
      AgsTimestamp *current_new_timestamp;
      
      GList *orig_wave;

      current_new_timestamp = ags_wave_get_timestamp(new_wave->data);
      
      orig_wave = ags_wave_find_near_timestamp(start_wave, i,
					       current_new_timestamp);

      if(orig_wave == NULL){
	ags_audio_add_wave(audio,
			   new_wave->data);
      }else{
	GList *start_new_buffer, *new_buffer;

	new_buffer =
	  start_new_buffer = ags_wave_get_buffer(new_wave->data);

	while(new_buffer != NULL){
	  AgsBuffer *orig_buffer;
	  
	  guint64 new_buffer_x;
	  
	  GRecMutex *orig_wave_mutex;
	  
	  new_buffer_x = ags_buffer_get_x(new_buffer->data);

	  orig_buffer = ags_wave_find_point(orig_wave->data,
					    new_buffer_x,
					    FALSE);

	  orig_wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(orig_wave->data);

	  g_rec_mutex_lock(orig_wave_mutex);

	  /* remove original */
	  AGS_WAVE(orig_wave->data)->buffer = g_list_remove(AGS_WAVE(orig_wave->data)->buffer,
							    orig_buffer);
	  g_object_unref(orig_buffer);

	  /* add new */
	  AGS_WAVE(orig_wave->data)->buffer = g_list_insert_sorted(AGS_WAVE(orig_wave->data)->buffer,
								   new_buffer->data,
								   (GCompareFunc) ags_buffer_sort_func);
	  g_object_ref(new_buffer->data);

	  g_rec_mutex_unlock(orig_wave_mutex);

	  /* iterate */
	  new_buffer = new_buffer->next;
	}
      }
      
      /* iterate */
      new_wave = new_wave->next;
    }
        
    /* iterate */
    i++;
  }    
  
  ags_audio_buffer_util_free(audio_buffer_util);
  
  g_list_free_full(start_wave,
		   (GDestroyNotify) g_object_unref);

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  g_object_unref(timestamp);
}

void
ags_time_stretch_buffer_popover_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_time_stretch_buffer_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
						     guint keyval,
						     guint keycode,
						     GdkModifierType state,
						     AgsTimeStretchBufferPopover *time_stretch_buffer_popover)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }
  
  return(key_handled);
}

void
ags_time_stretch_buffer_popover_key_released_callback(GtkEventControllerKey *event_controller,
						      guint keyval,
						      guint keycode,
						      GdkModifierType state,
						      AgsTimeStretchBufferPopover *time_stretch_buffer_popover)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }else{
    switch(keyval){
    case GDK_KEY_Escape:
      {
	gtk_popover_popdown((GtkPopover *) time_stretch_buffer_popover);	
      }
      break;
    }
  }
}

gboolean
ags_time_stretch_buffer_popover_modifiers_callback(GtkEventControllerKey *event_controller,
						   GdkModifierType keyval,
						   AgsTimeStretchBufferPopover *time_stretch_buffer_popover)
{
  return(FALSE);
}

void
ags_time_stretch_buffer_popover_activate_button_callback(GtkButton *activate_button,
							 AgsTimeStretchBufferPopover *time_stretch_buffer_popover)
{
  ags_applicable_apply(AGS_APPLICABLE(time_stretch_buffer_popover));
}

/**
 * ags_time_stretch_buffer_popover_new:
 *
 * Create a new #AgsTimeStretchBufferPopover.
 *
 * Returns: a new #AgsTimeStretchBufferPopover
 *
 * Since: 6.11.0
 */
AgsTimeStretchBufferPopover*
ags_time_stretch_buffer_popover_new()
{
  AgsTimeStretchBufferPopover *time_stretch_buffer_popover;

  time_stretch_buffer_popover = (AgsTimeStretchBufferPopover *) g_object_new(AGS_TYPE_TIME_STRETCH_BUFFER_POPOVER,
									     NULL);

  return(time_stretch_buffer_popover);
}
