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

#include <ags/thread/ags_timestamp_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_timestamp.h>
#include <ags/audio/ags_devout.h>

void ags_timestamp_thread_class_init(AgsTimestampThreadClass *timestamp_thread);
void ags_timestamp_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_timestamp_thread_init(AgsTimestampThread *timestamp_thread);
void ags_timestamp_thread_connect(AgsConnectable *connectable);
void ags_timestamp_thread_disconnect(AgsConnectable *connectable);
void ags_timestamp_thread_finalize(GObject *gobject);

void ags_timestamp_thread_run(AgsThread *thread);

static gpointer ags_timestamp_thread_parent_class = NULL;
static AgsConnectableInterface *ags_timestamp_thread_parent_connectable_interface;

GType
ags_timestamp_thread_get_type()
{
  static GType ags_type_timestamp_thread = 0;

  if(!ags_type_timestamp_thread){
    static const GTypeInfo ags_timestamp_thread_info = {
      sizeof (AgsTimestampThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_timestamp_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTimestampThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_timestamp_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_timestamp_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_timestamp_thread = g_type_register_static(AGS_TYPE_THREAD,
						       "AgsTimestampThread\0",
						       &ags_timestamp_thread_info,
						       0);
    
    g_type_add_interface_static(ags_type_timestamp_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_timestamp_thread);
}

void
ags_timestamp_thread_class_init(AgsTimestampThreadClass *timestamp_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_timestamp_thread_parent_class = g_type_class_peek_parent(timestamp_thread);

  /* GObject */
  gobject = (GObjectClass *) timestamp_thread;

  gobject->finalize = ags_timestamp_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) timestamp_thread;

  thread->run = ags_timestamp_thread_run;
}

void
ags_timestamp_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_timestamp_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_timestamp_thread_connect;
  connectable->disconnect = ags_timestamp_thread_disconnect;
}

void
ags_timestamp_thread_init(AgsTimestampThread *timestamp_thread)
{
  timestamp_thread->current_timestamp = (GObject *) ags_timestamp_new();
  timestamp_thread->current_latency = (GObject *) ags_timestamp_new();
}

void
ags_timestamp_thread_connect(AgsConnectable *connectable)
{
  AgsThread *thread;

  thread = AGS_THREAD(connectable);

  ags_timestamp_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_timestamp_thread_disconnect(AgsConnectable *connectable)
{
  ags_timestamp_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_timestamp_thread_finalize(GObject *gobject)
{
  AgsTimestampThread *timestamp_thread;

  timestamp_thread = AGS_TIMESTAMP_THREAD(gobject);

  /*  */
  G_OBJECT_CLASS(ags_timestamp_thread_parent_class)->finalize(gobject);
}

void
ags_timestamp_thread_run(AgsThread *thread)
{
  AgsTimestampThread *timestamp_thread;
  AgsDevout *devout;
  guint duration;
  time_t timer;

  timestamp_thread = AGS_TIMESTAMP_THREAD(thread);
  devout = AGS_DEVOUT(thread->devout);

  duration = (devout->tic_counter /
	      (devout->frequency / devout->buffer_size) *
	      AGS_MICROSECONDS_PER_SECOND);
  
  timer = time(&(AGS_TIMESTAMP(timestamp_thread->current_timestamp)->timer.unix_time.time_val));
  AGS_TIMESTAMP(timestamp_thread->current_latency)->timer.unix_time.time_val = timer - duration;
  AGS_TIMESTAMP(timestamp_thread->timestamp)->timer.unix_time.time_val = duration;
}

AgsTimestampThread*
ags_timestamp_thread_new()
{
  AgsTimestampThread *timestamp_thread;
  
  timestamp_thread = (AgsTimestampThread *) g_object_new(AGS_TYPE_TIMESTAMP_THREAD,
							 NULL);

  return(timestamp_thread);
}
