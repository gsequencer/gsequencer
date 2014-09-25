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

#include <ags/thread/ags_export_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

void ags_export_thread_class_init(AgsExportThreadClass *export_thread);
void ags_export_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_export_thread_init(AgsExportThread *export_thread);
void ags_export_thread_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_export_thread_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_export_thread_connect(AgsConnectable *connectable);
void ags_export_thread_disconnect(AgsConnectable *connectable);
void ags_export_thread_finalize(GObject *gobject);

void ags_export_thread_start(AgsThread *thread);
void ags_export_thread_run(AgsThread *thread);
void ags_export_thread_stop(AgsThread *thread);

enum{
  PROP_0,
  PROP_AUDIO_FILE,
};

static gpointer ags_export_thread_parent_class = NULL;
static AgsConnectableInterface *ags_export_thread_parent_connectable_interface;

GType
ags_export_thread_get_type()
{
  static GType ags_type_export_thread = 0;

  if(!ags_type_export_thread){
    static const GTypeInfo ags_export_thread_info = {
      sizeof (AgsExportThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_export_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsExportThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_export_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_export_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_export_thread = g_type_register_static(AGS_TYPE_THREAD,
						    "AgsExportThread\0",
						    &ags_export_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_export_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_export_thread);
}

void
ags_export_thread_class_init(AgsExportThreadClass *export_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_export_thread_parent_class = g_type_class_peek_parent(export_thread);

  /* GObject */
  gobject = (GObjectClass *) export_thread;

  gobject->get_property = ags_export_thread_get_property;
  gobject->set_property = ags_export_thread_set_property;

  gobject->finalize = ags_export_thread_finalize;

  /* properties */
  param_spec = g_param_spec_object("audio-file\0",
				   "audio file to write\0",
				   "The audio file to write output.\0",
				   AGS_TYPE_AUDIO_FILE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FILE,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) export_thread;

  thread->start = ags_export_thread_start;
  thread->run = ags_export_thread_run;
  thread->stop = ags_export_thread_stop;
}

void
ags_export_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_export_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_export_thread_connect;
  connectable->disconnect = ags_export_thread_disconnect;
}

void
ags_export_thread_init(AgsExportThread *export_thread)
{
  AgsThread *thread;

  thread = AGS_THREAD(export_thread);
  thread->freq = AGS_EXPORT_THREAD_DEFAULT_JIFFIE;

  export_thread->flags = 0;

  export_thread->audio_file = NULL;
}

void
ags_export_thread_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsExportThread *export_thread;

  export_thread = AGS_EXPORT_THREAD(gobject);

  switch(prop_id){
  case PROP_AUDIO_FILE:
    {
      AgsAudioFile *audio_file;

      audio_file = g_value_get_object(value);

      if(export_thread->audio_file == audio_file){
	return;
      }

      if(export_thread->audio_file != NULL){
	g_object_unref(export_thread->audio_file);
      }

      if(audio_file != NULL){
	g_object_ref(audio_file);
      }

      export_thread->audio_file = audio_file;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_export_thread_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsExportThread *export_thread;

  export_thread = AGS_EXPORT_THREAD(gobject);

  switch(prop_id){
  case PROP_AUDIO_FILE:
    {
      g_value_set_object(value, export_thread->audio_file);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_export_thread_connect(AgsConnectable *connectable)
{
  ags_export_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_export_thread_disconnect(AgsConnectable *connectable)
{
  ags_export_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_export_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_export_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_export_thread_start(AgsThread *thread)
{
  //TODO:JK: implement me

  AGS_THREAD_CLASS(ags_export_thread_parent_class)->start(thread);
}

void
ags_export_thread_run(AgsThread *thread)
{
  AgsExportThread *export_thread;
  AgsDevout *devout;

  export_thread = AGS_EXPORT_THREAD(thread);

  devout =  thread->devout;

  if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    ags_audio_file_write(export_thread->audio_file,
			 devout->buffer[0], devout->buffer_size);
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    ags_audio_file_write(export_thread->audio_file,
			 devout->buffer[1], devout->buffer_size);
  }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
    ags_audio_file_write(export_thread->audio_file,
			 devout->buffer[2], devout->buffer_size);
  }else if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    ags_audio_file_write(export_thread->audio_file,
			 devout->buffer[3], devout->buffer_size);
  }
}

void
ags_export_thread_stop(AgsThread *thread)
{
  AgsExportThread *export_thread;

  export_thread = AGS_EXPORT_THREAD(thread);

  AGS_THREAD_CLASS(ags_export_thread_parent_class)->stop(thread);

  ags_audio_file_flush(export_thread->audio_file);
  ags_audio_file_close(export_thread->audio_file);
}

AgsExportThread*
ags_export_thread_new(GObject *devout, AgsAudioFile *audio_file)
{
  AgsExportThread *export_thread;

  export_thread = (AgsExportThread *) g_object_new(AGS_TYPE_EXPORT_THREAD,
						   "devout\0", devout,
						   "audio-file\0", audio_file,
						   NULL);
  
  return(export_thread);
}
