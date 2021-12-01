/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/editor/ags_time_stretch_buffer_dialog.h>
#include <ags/X/editor/ags_time_stretch_buffer_dialog_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_wave_window.h>
#include <ags/X/ags_wave_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/i18n.h>

void ags_time_stretch_buffer_dialog_class_init(AgsTimeStretchBufferDialogClass *time_stretch_buffer_dialog);
void ags_time_stretch_buffer_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_time_stretch_buffer_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_time_stretch_buffer_dialog_init(AgsTimeStretchBufferDialog *time_stretch_buffer_dialog);
void ags_time_stretch_buffer_dialog_finalize(GObject *gobject);

void ags_time_stretch_buffer_dialog_connect(AgsConnectable *connectable);
void ags_time_stretch_buffer_dialog_disconnect(AgsConnectable *connectable);

void ags_time_stretch_buffer_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_time_stretch_buffer_dialog_apply(AgsApplicable *applicable);
void ags_time_stretch_buffer_dialog_reset(AgsApplicable *applicable);
gboolean ags_time_stretch_buffer_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_time_stretch_buffer_dialog
 * @short_description: time_stretch tool
 * @title: AgsTimeStretchBufferDialog
 * @section_id:
 * @include: ags/X/editor/ags_time_stretch_buffer_dialog.h
 *
 * The #AgsTimeStretchBufferDialog lets you time_stretch buffers.
 */

static gpointer ags_time_stretch_buffer_dialog_parent_class = NULL;

GType
ags_time_stretch_buffer_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_time_stretch_buffer_dialog = 0;

    static const GTypeInfo ags_time_stretch_buffer_dialog_info = {
      sizeof (AgsTimeStretchBufferDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_time_stretch_buffer_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTimeStretchBufferDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_time_stretch_buffer_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_time_stretch_buffer_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_time_stretch_buffer_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_time_stretch_buffer_dialog = g_type_register_static(GTK_TYPE_DIALOG,
								 "AgsTimeStretchBufferDialog", &ags_time_stretch_buffer_dialog_info,
								 0);
    
    g_type_add_interface_static(ags_type_time_stretch_buffer_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_time_stretch_buffer_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_time_stretch_buffer_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_time_stretch_buffer_dialog_class_init(AgsTimeStretchBufferDialogClass *time_stretch_buffer_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_time_stretch_buffer_dialog_parent_class = g_type_class_peek_parent(time_stretch_buffer_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) time_stretch_buffer_dialog;

  gobject->finalize = ags_time_stretch_buffer_dialog_finalize;

  /* properties */

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) time_stretch_buffer_dialog;

  widget->delete_event = ags_time_stretch_buffer_dialog_delete_event;
}

void
ags_time_stretch_buffer_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_time_stretch_buffer_dialog_connect;
  connectable->disconnect = ags_time_stretch_buffer_dialog_disconnect;
}

void
ags_time_stretch_buffer_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_time_stretch_buffer_dialog_set_update;
  applicable->apply = ags_time_stretch_buffer_dialog_apply;
  applicable->reset = ags_time_stretch_buffer_dialog_reset;
}

void
ags_time_stretch_buffer_dialog_init(AgsTimeStretchBufferDialog *time_stretch_buffer_dialog)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  time_stretch_buffer_dialog->flags = 0;

  g_object_set(time_stretch_buffer_dialog,
	       "title", i18n("time stretch buffers"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(time_stretch_buffer_dialog),
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  
  
  /* frequency - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* frequency - label */
  label = (GtkLabel *) gtk_label_new(i18n("frequency"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* frequency - spin button */
  time_stretch_buffer_dialog->frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											   AGS_TIME_STRETCH_BUFFER_MAX_FREQUENCY,
											   0.001);
  gtk_spin_button_set_digits(time_stretch_buffer_dialog->frequency,
			     3);
  gtk_spin_button_set_value(time_stretch_buffer_dialog->frequency,
			    AGS_TIME_STRETCH_BUFFER_DEFAULT_FREQUENCY);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) time_stretch_buffer_dialog->frequency,
		     FALSE, FALSE,
		     0);
  
  /* orig BPM - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* orig BPM - label */
  label = (GtkLabel *) gtk_label_new(i18n("orig BPM"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* orig BPM - spin button */
  time_stretch_buffer_dialog->orig_bpm = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											  AGS_TIME_STRETCH_BUFFER_MAX_BPM,
											  1.0);
  gtk_spin_button_set_digits(time_stretch_buffer_dialog->orig_bpm,
			     2);
  gtk_spin_button_set_value(time_stretch_buffer_dialog->orig_bpm,
			    AGS_TIME_STRETCH_BUFFER_DEFAULT_BPM);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) time_stretch_buffer_dialog->orig_bpm,
		     FALSE, FALSE,
		     0);
  
  /* new BPM - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* new BPM - label */
  label = (GtkLabel *) gtk_label_new(i18n("new BPM"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* new BPM - spin button */
  time_stretch_buffer_dialog->new_bpm = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											 AGS_TIME_STRETCH_BUFFER_MAX_BPM,
											 1.0);
  gtk_spin_button_set_digits(time_stretch_buffer_dialog->new_bpm,
			     2);
  gtk_spin_button_set_value(time_stretch_buffer_dialog->new_bpm,
			    AGS_TIME_STRETCH_BUFFER_DEFAULT_BPM);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) time_stretch_buffer_dialog->new_bpm,
		     FALSE, FALSE,
		     0);

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) time_stretch_buffer_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_time_stretch_buffer_dialog_connect(AgsConnectable *connectable)
{
  AgsTimeStretchBufferDialog *time_stretch_buffer_dialog;

  time_stretch_buffer_dialog = AGS_TIME_STRETCH_BUFFER_DIALOG(connectable);

  if((AGS_TIME_STRETCH_BUFFER_DIALOG_CONNECTED & (time_stretch_buffer_dialog->flags)) != 0){
    return;
  }

  time_stretch_buffer_dialog->flags |= AGS_TIME_STRETCH_BUFFER_DIALOG_CONNECTED;

  g_signal_connect(time_stretch_buffer_dialog, "response",
		   G_CALLBACK(ags_time_stretch_buffer_dialog_response_callback), time_stretch_buffer_dialog);
}

void
ags_time_stretch_buffer_dialog_disconnect(AgsConnectable *connectable)
{
  AgsTimeStretchBufferDialog *time_stretch_buffer_dialog;

  time_stretch_buffer_dialog = AGS_TIME_STRETCH_BUFFER_DIALOG(connectable);

  if((AGS_TIME_STRETCH_BUFFER_DIALOG_CONNECTED & (time_stretch_buffer_dialog->flags)) == 0){
    return;
  }

  time_stretch_buffer_dialog->flags &= (~AGS_TIME_STRETCH_BUFFER_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(time_stretch_buffer_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_time_stretch_buffer_dialog_response_callback),
		      time_stretch_buffer_dialog,
		      NULL);
}

void
ags_time_stretch_buffer_dialog_finalize(GObject *gobject)
{
  AgsTimeStretchBufferDialog *time_stretch_buffer_dialog;

  time_stretch_buffer_dialog = (AgsTimeStretchBufferDialog *) gobject;
  
  G_OBJECT_CLASS(ags_time_stretch_buffer_dialog_parent_class)->finalize(gobject);
}

void
ags_time_stretch_buffer_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_time_stretch_buffer_dialog_apply(AgsApplicable *applicable)
{
  AgsTimeStretchBufferDialog *time_stretch_buffer_dialog;
    
  AgsWindow *window;
  AgsMachine *machine;
  AgsNotebook *notebook;
  AgsWaveEdit *focused_wave_edit;

  AgsAudio *audio;
  
  AgsTimestamp *timestamp;

  AgsApplicationContext *application_context;

  GObject *output_soundcard;

  AgsTimeStretchUtil time_stretch_util;

  GList *start_wave, *wave;
  GList *start_new_wave, *new_wave;
  
  gboolean use_composite_editor;
  guint samplerate;
  guint buffer_size;
  guint format;
  gdouble delay;
  guint64 relative_offset;
  guint64 x0;
  guint64 x1;
  gdouble factor;
  guint copy_mode;
  gboolean has_selection;
  gint i;

  time_stretch_buffer_dialog = AGS_TIME_STRETCH_BUFFER_DIALOG(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  machine = NULL;
  
  if(use_composite_editor){
    AgsCompositeEditor *composite_editor;
    
    composite_editor = window->composite_editor;

    machine = composite_editor->selected_machine;

    focused_wave_edit = composite_editor->wave_edit->focused_edit;
    
    notebook = composite_editor->wave_edit->channel_selector;
  }else{
    AgsWaveEditor *wave_editor;
    
    wave_editor = window->wave_window->wave_editor;

    machine = wave_editor->selected_machine;

    focused_wave_edit = wave_editor->focused_wave_edit;

    notebook = wave_editor->notebook;
  }
  
  audio = machine->audio;

  output_soundcard = NULL;

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  start_wave = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
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
  
  time_stretch_util.buffer_size = buffer_size;
  time_stretch_util.format = format;
  time_stretch_util.samplerate = samplerate;

  time_stretch_util.frequency = gtk_spin_button_get_value(time_stretch_buffer_dialog->frequency);

  time_stretch_util.orig_bpm = gtk_spin_button_get_value(time_stretch_buffer_dialog->orig_bpm);
  time_stretch_util.new_bpm = gtk_spin_button_get_value(time_stretch_buffer_dialog->new_bpm);

  factor = time_stretch_util.new_bpm / time_stretch_util.orig_bpm;
  
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(format));
  
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
	  
	  x1 = ags_buffer_get_x(buffer->data) + buffer_size;
	  
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
	      
	      ags_audio_buffer_util_copy_buffer_to_buffer(time_stretch_util.source, 1, current_x - x0,
							  ags_buffer_get_data(buffer->data), 1, 0,
							  buffer_size, copy_mode);

	      ags_audio_buffer_util_clear_buffer(ags_buffer_get_data(buffer->data), 1,
						 buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
	      
	      ags_buffer_unlock(buffer->data);
	    }
	    
	    has_selection = TRUE;
	  }
      
	  /* iterate */	
	  buffer = buffer->next;
	}

	ags_time_stretch_util_stretch(&time_stretch_util);

	current_new_wave = ags_wave_new(audio,
					i);

	g_object_set(current_new_wave,
		     "samplerate", samplerate,
		     "buffer-size", buffer_size,
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
		       "buffer-size", buffer_size,
		       "format", format,
		       NULL);

	  ags_wave_add_buffer(current_new_wave,
			      current_new_buffer,
			      FALSE);
	      
	  frame_count = buffer_size;
	  
	  if(floor(j / relative_offset) < floor((j + frame_count) / relative_offset)){
	    frame_count = buffer_size - ((j + frame_count) % relative_offset);
	  }
	  
	  ags_buffer_lock(current_new_buffer);

	  ags_audio_buffer_util_copy_buffer_to_buffer(ags_buffer_get_data(current_new_buffer), 1, 0,
						      time_stretch_util.destination, 1, j - x0,
						      frame_count, copy_mode);

	  ags_buffer_unlock(current_new_buffer);
      
	  /* iterate */
	  if(floor(j / relative_offset) < floor((j + buffer_size) / relative_offset)){
	    current_new_wave = ags_wave_new(audio,
					    i);

	    g_object_set(current_new_wave,
			 "samplerate", samplerate,
			 "buffer-size", buffer_size,
			 "format", format,
			 NULL);
	
	    current_new_wave->timestamp->timer.ags_offset.offset = (guint64) relative_offset * floor((j + buffer_size) / relative_offset);

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
  
  g_list_free_full(start_wave,
		   (GDestroyNotify) g_object_unref);

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  g_object_unref(timestamp);
}

void
ags_time_stretch_buffer_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_time_stretch_buffer_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_time_stretch_buffer_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_time_stretch_buffer_dialog_new:
 *
 * Create a new #AgsTimeStretchBufferDialog.
 *
 * Returns: a new #AgsTimeStretchBufferDialog
 *
 * Since: 3.13.0
 */
AgsTimeStretchBufferDialog*
ags_time_stretch_buffer_dialog_new()
{
  AgsTimeStretchBufferDialog *time_stretch_buffer_dialog;

  time_stretch_buffer_dialog = (AgsTimeStretchBufferDialog *) g_object_new(AGS_TYPE_TIME_STRETCH_BUFFER_DIALOG,
									   NULL);

  return(time_stretch_buffer_dialog);
}
