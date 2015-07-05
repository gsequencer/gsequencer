/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/audio/task/ags_export_output.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/file/ags_audio_file.h>

void ags_export_output_class_init(AgsExportOutputClass *export_output);
void ags_export_output_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_export_output_init(AgsExportOutput *export_output);
void ags_export_output_connect(AgsConnectable *connectable);
void ags_export_output_disconnect(AgsConnectable *connectable);
void ags_export_output_finalize(GObject *gobject);

void ags_export_output_launch(AgsTask *task);

/**
 * SECTION:ags_export_output
 * @short_description: export output object
 * @title: AgsExportOutput
 * @section_id:
 * @include: ags/audio/task/ags_export_output.h
 *
 * The #AgsExportOutput task exports #AgsSoundcard to file.
 */

static gpointer ags_export_output_parent_class = NULL;
static AgsConnectableInterface *ags_export_output_parent_connectable_interface;

GType
ags_export_output_get_type()
{
  static GType ags_type_export_output = 0;

  if(!ags_type_export_output){
    static const GTypeInfo ags_export_output_info = {
      sizeof (AgsExportOutputClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_export_output_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsExportOutput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_export_output_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_export_output_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_export_output = g_type_register_static(AGS_TYPE_TASK,
						    "AgsExportOutput\0",
						    &ags_export_output_info,
						    0);

    g_type_add_interface_static(ags_type_export_output,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_export_output);
}

void
ags_export_output_class_init(AgsExportOutputClass *export_output)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_export_output_parent_class = g_type_class_peek_parent(export_output);

  /* gobject */
  gobject = (GObjectClass *) export_output;

  gobject->finalize = ags_export_output_finalize;

  /* task */
  task = (AgsTaskClass *) export_output;

  task->launch = ags_export_output_launch;
}

void
ags_export_output_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_export_output_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_export_output_connect;
  connectable->disconnect = ags_export_output_disconnect;
}

void
ags_export_output_init(AgsExportOutput *export_output)
{
  export_output->export_thread = NULL;
  export_output->soundcard = NULL;
  export_output->filename = NULL;
  export_output->tic = 0;
  export_output->live_performance = TRUE;
}

void
ags_export_output_connect(AgsConnectable *connectable)
{
  ags_export_output_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_export_output_disconnect(AgsConnectable *connectable)
{
  ags_export_output_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_export_output_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_export_output_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_export_output_launch(AgsTask *task)
{
  AgsExportOutput *export_output;
  AgsExportThread *export_thread;
  AgsSoundcard *soundcard;
  AgsAudioFile *audio_file;
  gchar *filename;
  guint samplerate, dsp_channels;
  guint tic;
  guint val;
  
  export_output = AGS_EXPORT_OUTPUT(task);

  soundcard = export_output->soundcard;
  export_thread = export_output->export_thread;

  filename = export_output->filename;

  ags_soundcard_get_presets(soundcard,
			    &dsp_channels,
			    &samplerate,
			    NULL,
			    NULL);
  
  tic = export_output->tic;

  /* open read/write audio file */
  audio_file = ags_audio_file_new(filename,
				  soundcard,
				  0, dsp_channels);

  audio_file->samplerate = (int) samplerate;
  audio_file->channels = dsp_channels;

  ags_audio_file_rw_open(audio_file,
			 TRUE);

  g_message("export output");

  /* start export thread */
  export_thread->tic = tic;
  g_object_set(G_OBJECT(export_thread),
	       "soundcard\0", soundcard,
	       "audio-file\0", audio_file,
	       NULL);
  ags_thread_start(export_thread);

  if((AGS_THREAD_SINGLE_LOOP & (AGS_THREAD(export_thread)->flags)) == 0){
    pthread_mutex_lock(&(AGS_THREAD(export_thread)->start_mutex));

    val = g_atomic_int_get(&(AGS_THREAD(export_thread)->flags));

    if((AGS_THREAD_INITIAL_RUN & val) != 0){
      while((AGS_THREAD_INITIAL_RUN & val) != 0){
	pthread_cond_wait(&(AGS_THREAD(export_thread)->start_cond),
			  &(AGS_THREAD(export_thread)->start_mutex));
	
	val = g_atomic_int_get(&(AGS_THREAD(export_thread)->flags));
      }
    }
    
    pthread_mutex_unlock(&(AGS_THREAD(export_thread)->start_mutex));
  }else{
    g_atomic_int_or(&(AGS_THREAD(export_thread)->flags),
		    AGS_THREAD_RUNNING);
  }
}

/**
 * ags_export_output_new:
 * @export_thread: the #AgsExportThread to start
 * @soundcard: the #AgsSoundcard to export
 * @filename: the filename to save
 * @tic: stream duration in tact
 * @live_performance: if %TRUE export is done during real-time
 *
 * Creates an #AgsExportOutput.
 *
 * Returns: an new #AgsExportOutput.
 *
 * Since: 0.4
 */
AgsExportOutput*
ags_export_output_new(AgsExportThread *export_thread,
		      GObject *soundcard,
		      gchar *filename,
		      guint tic,
		      gboolean live_performance)
{
  AgsExportOutput *export_output;

  export_output = (AgsExportOutput *) g_object_new(AGS_TYPE_EXPORT_OUTPUT,
						   NULL);

  export_output->export_thread = export_thread;
  export_output->soundcard = soundcard;
  export_output->filename = filename;
  export_output->tic = tic;
  export_output->live_performance = live_performance;

  return(export_output);
}
