/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/thread/ags_record_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

void ags_record_thread_class_init(AgsRecordThreadClass *record_thread);
void ags_record_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_record_thread_init(AgsRecordThread *record_thread);
void ags_record_thread_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_record_thread_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_record_thread_connect(AgsConnectable *connectable);
void ags_record_thread_disconnect(AgsConnectable *connectable);
void ags_record_thread_finalize(GObject *gobject);

void ags_record_thread_start(AgsThread *thread);
void ags_record_thread_run(AgsThread *thread);
void ags_record_thread_stop(AgsThread *thread);

enum{
  PROP_0,
  PROP_REGISTRY,
};

static gpointer ags_record_thread_parent_class = NULL;
static AgsConnectableInterface *ags_record_thread_parent_connectable_interface;

GType
ags_record_thread_get_type()
{
  static GType ags_type_record_thread = 0;

  if(!ags_type_record_thread){
    static const GTypeInfo ags_record_thread_info = {
      sizeof (AgsRecordThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_record_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecordThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_record_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_record_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_record_thread = g_type_register_static(AGS_TYPE_THREAD,
						    "AgsRecordThread\0",
						    &ags_record_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_record_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_record_thread);
}

void
ags_record_thread_class_init(AgsRecordThreadClass *record_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_record_thread_parent_class = g_type_class_peek_parent(record_thread);

  /* GObject */
  gobject = (GObjectClass *) record_thread;

  gobject->get_property = ags_record_thread_get_property;
  gobject->set_property = ags_record_thread_set_property;

  gobject->finalize = ags_record_thread_finalize;

  /* properties */
  param_spec = g_param_spec_object("registry\0",
				   "registry to check against\0",
				   "The registry to check against serialization.\0",
				   AGS_TYPE_REGISTRY,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REGISTRY,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) record_thread;

  thread->start = ags_record_thread_start;
  thread->run = ags_record_thread_run;
  thread->stop = ags_record_thread_stop;
}

void
ags_record_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_record_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_record_thread_connect;
  connectable->disconnect = ags_record_thread_disconnect;
}

void
ags_record_thread_init(AgsRecordThread *record_thread)
{
  AgsThread *thread;

  thread = AGS_THREAD(record_thread);

  record_thread->registry = NULL;

  record_thread->delay = AGS_RECORD_THREAD_DEFAULT_DELAY;
  record_thread->counter = 0;
}

void
ags_record_thread_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsRecordThread *record_thread;

  record_thread = AGS_RECORD_THREAD(gobject);

  switch(prop_id){
  case PROP_REGISTRY:
    {
      AgsRegistry *registry;

      registry = g_value_get_object(value);

      if(record_thread->registry == registry){
	return;
      }

      if(record_thread->registry != NULL){
	g_object_unref(record_thread->registry);
      }

      if(registry != NULL){
	g_object_ref(registry);
      }

      record_thread->registry = registry;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_record_thread_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsRecordThread *record_thread;

  record_thread = AGS_RECORD_THREAD(gobject);

  switch(prop_id){
  case PROP_REGISTRY:
    {
      g_value_set_object(value, record_thread->registry);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_record_thread_connect(AgsConnectable *connectable)
{
  ags_record_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_record_thread_disconnect(AgsConnectable *connectable)
{
  ags_record_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_record_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_record_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_record_thread_start(AgsThread *thread)
{
  //TODO:JK: implement me

  AGS_THREAD_CLASS(ags_record_thread_parent_class)->start(thread);
}

void
ags_record_thread_run(AgsThread *thread)
{
  AgsRecordThread *record_thread;

  record_thread = AGS_RECORD_THREAD(thread);
  
  if(record_thread->counter != record_thread->delay){
    record_thread->counter += 1;
  }else{
    record_thread->counter = 0;
    
    if((AGS_REGISTRY_DIRTY & (record_thread->registry->flags)) != 0){
      AgsFile *next_file, *current_file;

      current_file = record_thread->registry->current;
      next_file = ags_file_new(current_file->filename);

      pthread_mutex_lock(&(record_thread->registry->mutex));

      g_object_unref(record_thread->registry->previous);

      record_thread->registry->previous = current_file;
      record_thread->registry->current = next_file;

      pthread_mutex_unlock(&(record_thread->registry->mutex));

      ags_file_write(current_file);

      record_thread->registry->flags &= (~AGS_REGISTRY_DIRTY);
    }
  }
}

void
ags_record_thread_stop(AgsThread *thread)
{
  //TODO:JK: implement me
}

AgsRecordThread*
ags_record_thread_new(GObject *devout, AgsRegistry *registry)
{
  AgsRecordThread *record_thread;

  record_thread = (AgsRecordThread *) g_object_new(AGS_TYPE_RECORD_THREAD,
						   "devout\0", devout,
						   "registry\0", registry,
						   NULL);
  
  return(record_thread);
}
