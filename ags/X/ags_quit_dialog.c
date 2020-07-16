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

#include <ags/X/ags_quit_dialog.h>
#include <ags/X/ags_quit_dialog_callbacks.h>

#include <ags/X/machine/ags_audiorec.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_quit_dialog_class_init(AgsQuitDialogClass *quit_dialog);
void ags_quit_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_quit_dialog_init(AgsQuitDialog *quit_dialog);
void ags_quit_dialog_finalize(GObject *gobject);

void ags_quit_dialog_connect(AgsConnectable *connectable);
void ags_quit_dialog_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_quit_dialog
 * @short_description: The quit dialog
 * @title: AgsQuitDialog
 * @section_id:
 * @include: ags/X/ags_quit_dialog.h
 *
 * #AgsQuitDialog is shown as terminating GSequencer.
 */

static gpointer ags_quit_dialog_parent_class = NULL;

GType
ags_quit_dialog_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_quit_dialog = 0;

    static const GTypeInfo ags_quit_dialog_info = {
      sizeof (AgsQuitDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_quit_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsQuitDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_quit_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_quit_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_quit_dialog = g_type_register_static(GTK_TYPE_DIALOG,
						  "AgsQuitDialog", &ags_quit_dialog_info,
						  0);
    
    g_type_add_interface_static(ags_type_quit_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_quit_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_quit_dialog_class_init(AgsQuitDialogClass *quit_dialog)
{
  GObjectClass *gobject;

  ags_quit_dialog_parent_class = g_type_class_peek_parent(quit_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) quit_dialog;

  gobject->finalize = ags_quit_dialog_finalize;
}

void
ags_quit_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;

  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = NULL;
  connectable->connect = ags_quit_dialog_connect;
  connectable->disconnect = ags_quit_dialog_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_quit_dialog_init(AgsQuitDialog *quit_dialog)
{
  GtkVBox *vbox;
  
  quit_dialog->flags = 0;

  vbox = gtk_dialog_get_content_area(quit_dialog);
  
  quit_dialog->accept_all = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("accept all"));
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) quit_dialog->accept_all,
		     FALSE, FALSE,
		     0);

  quit_dialog->current_question = AGS_QUIT_DIALOG_QUESTION_SAVE_FILE;
  
  quit_dialog->question = (GtkLabel *) gtk_label_new(i18n("Do you want to save before quit?"));
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) quit_dialog->question,
		     FALSE, FALSE,
		     0);

  quit_dialog->nth_wave_export_machine = 0;

  quit_dialog->wave_export_machine = NULL;
  
  quit_dialog->yes = gtk_dialog_add_button(quit_dialog,
					   "Yes",
					   GTK_RESPONSE_YES);

  quit_dialog->no = gtk_dialog_add_button(quit_dialog,
					   "No",
					   GTK_RESPONSE_NO);

  quit_dialog->cancel = gtk_dialog_add_button(quit_dialog,
					      "Cancel",
					      GTK_RESPONSE_CANCEL);
  
  quit_dialog->confirm = NULL;
}

void
ags_quit_dialog_connect(AgsConnectable *connectable)
{
  AgsQuitDialog *quit_dialog;

  quit_dialog = AGS_QUIT_DIALOG(connectable);

  if((AGS_QUIT_DIALOG_CONNECTED & (quit_dialog->flags)) != 0){
    return;
  }

  quit_dialog->flags |= AGS_QUIT_DIALOG_CONNECTED;

  g_signal_connect(G_OBJECT(quit_dialog), "response",
		   G_CALLBACK(ags_quit_dialog_response_callback), NULL);
}

void
ags_quit_dialog_disconnect(AgsConnectable *connectable)
{
  AgsQuitDialog *quit_dialog;

  quit_dialog = AGS_QUIT_DIALOG(connectable);

  if((AGS_QUIT_DIALOG_CONNECTED & (quit_dialog->flags)) == 0){
    return;
  }

  quit_dialog->flags &= (~AGS_QUIT_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(quit_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_quit_dialog_response_callback),
		      NULL,
		      NULL);
}

void
ags_quit_dialog_finalize(GObject *gobject)
{
  AgsQuitDialog *quit_dialog;

  quit_dialog = (AgsQuitDialog *) gobject;
  
  G_OBJECT_CLASS(ags_quit_dialog_parent_class)->finalize(gobject);
}

/**
 * ags_quit_dialog_fast_export:
 * @quit_dialog: the #AgsQuitDialog
 * @machine: the #AgsMachine
 * 
 * Fast export @quit_dialog.
 * 
 * Since: 3.5.0
 */
void
ags_quit_dialog_fast_export(AgsQuitDialog *quit_dialog,
			    AgsMachine *machine)
{
  AgsApplicationContext *application_context;

  AgsBuffer *buffer;
  AgsAudioFile *audio_file;

  AgsTimestamp *timestamp;
  
  GObject *soundcard;

  GList *start_wave, *end_wave, *wave;
  
  void *data;
  
  gchar *filename;
  
  guint default_offset;
  guint64 start_frame, end_frame;
  guint destination_offset, source_offset;
  guint audio_channels;
  guint copy_mode;
  guint samplerate;
  guint format;
  guint source_format;
  guint buffer_size;
  guint i, j;

  if(!AGS_IS_QUIT_DIALOG(quit_dialog) ||
     !AGS_IS_MACHINE(machine)){
    return;
  }

  application_context = ags_application_context_get_instance();

  soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  /* get some fields */
  g_object_get(machine->audio,
	       "wave", &start_wave,
	       "audio-channels", &audio_channels,
	       "samplerate", &samplerate,
	       "format", &format,
	       "buffer-size", &buffer_size,	       
	       NULL);

  filename = NULL;

  if(AGS_IS_AUDIOREC(machine)){
    filename = gtk_entry_get_text(AGS_AUDIOREC(machine)->filename);
  }
  
  if(g_file_test(filename,
		 G_FILE_TEST_EXISTS)){
    g_remove(filename);
  }

  audio_file = ags_audio_file_new(filename,
				  soundcard,
				  -1);

  audio_file->file_audio_channels = audio_channels;
  audio_file->file_samplerate = samplerate;  
  
  ags_audio_file_rw_open(audio_file,
			 TRUE);

  default_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

  timestamp = ags_timestamp_new();
  timestamp->flags = AGS_TIMESTAMP_OFFSET;
  
  data = ags_stream_alloc(audio_channels * buffer_size,
			  format);

  start_frame = 0;
  end_frame = 0;
  
  end_wave = g_list_last(start_wave);  

  if(end_wave != NULL){
    GList *end_buffer;

    guint64 x;
    
    end_buffer = g_list_last(AGS_WAVE(end_wave->data)->buffer);

    x = 0;
    
    if(end_buffer != NULL){
      g_object_get(AGS_BUFFER(end_buffer->data),
		   "x", &x,
		   NULL);

      end_frame = x;
    }else{
      x = ags_timestamp_get_ags_offset(AGS_WAVE(end_wave->data)->timestamp);

      end_frame = x;
    }
  }
  
  for(i = start_frame; i + buffer_size < end_frame; ){
    guint current_buffer_size;

    GRecMutex *buffer_mutex;
    
    ags_timestamp_set_ags_offset(timestamp,
				 default_offset * floor((gdouble) i / (gdouble) default_offset));
    
    ags_audio_buffer_util_clear_buffer(data, audio_channels,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

    current_buffer_size = buffer_size;
    
    if(i == start_frame){
      source_offset = start_frame % buffer_size;

      current_buffer_size -= source_offset;
    }else{
      source_offset = 0;
    }
    
    for(j = 0; j < audio_channels; j++){
      wave = ags_wave_find_near_timestamp(start_wave, j,
					  timestamp);

      if(wave == NULL){
	continue;
      }

      buffer = ags_wave_find_point(wave->data,
				   i,
				   FALSE);
      
      if(buffer != NULL){
	g_object_get(buffer,
		     "format", &source_format,
		     NULL);
	
	copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
							ags_audio_buffer_util_format_from_soundcard(source_format));
	
	buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

	destination_offset = j;

	g_rec_mutex_lock(buffer_mutex);
      
	ags_audio_buffer_util_copy_buffer_to_buffer(data, audio_channels, destination_offset,
						    buffer->data, 1, source_offset,
						    current_buffer_size, copy_mode);

	g_rec_mutex_unlock(buffer_mutex);
      }
    }
    
    ags_audio_file_write(audio_file,
			 data,
			 current_buffer_size,
			 format);
    
    i += buffer_size;
  }
  
  ags_audio_file_flush(audio_file);
  ags_audio_file_close(audio_file);
}

/**
 * ags_quit_dialog_new:
 * 
 * Create a new instance of #AgsQuitDialog
 * 
 * Returns: the #AgsQuitDialog
 * 
 * Since: 3.5.0
 */
AgsQuitDialog*
ags_quit_dialog_new()
{
  AgsQuitDialog *quit_dialog;

  quit_dialog = (AgsQuitDialog *) g_object_new(AGS_TYPE_QUIT_DIALOG,
					       NULL);

  return(quit_dialog);
}
