/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/audio/task/ags_unref_audio_signal.h>

#include <ags/object/ags_connectable.h>

void ags_unref_audio_signal_class_init(AgsUnrefAudioSignalClass *unref_audio_signal);
void ags_unref_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_unref_audio_signal_init(AgsUnrefAudioSignal *unref_audio_signal);
void ags_unref_audio_signal_connect(AgsConnectable *connectable);
void ags_unref_audio_signal_disconnect(AgsConnectable *connectable);
void ags_unref_audio_signal_finalize(GObject *gobject);

void ags_unref_audio_signal_launch(AgsTask *task);

static gpointer ags_unref_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_unref_audio_signal_parent_connectable_interface;

GType
ags_unref_audio_signal_get_type()
{
  static GType ags_type_unref_audio_signal = 0;

  if(!ags_type_unref_audio_signal){
    static const GTypeInfo ags_unref_audio_signal_info = {
      sizeof (AgsUnrefAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_unref_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsUnrefAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_unref_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_unref_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_unref_audio_signal = g_type_register_static(AGS_TYPE_TASK,
						    "AgsUnrefAudioSignal\0",
						    &ags_unref_audio_signal_info,
						    0);
    
    g_type_add_interface_static(ags_type_unref_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_unref_audio_signal);
}

void
ags_unref_audio_signal_class_init(AgsUnrefAudioSignalClass *unref_audio_signal)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_unref_audio_signal_parent_class = g_type_class_peek_parent(unref_audio_signal);

  /* gobject */
  gobject = (GObjectClass *) unref_audio_signal;

  gobject->finalize = ags_unref_audio_signal_finalize;

  /* task */
  task = (AgsTaskClass *) unref_audio_signal;

  task->launch = ags_unref_audio_signal_launch;
}

void
ags_unref_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_unref_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_unref_audio_signal_connect;
  connectable->disconnect = ags_unref_audio_signal_disconnect;
}

void
ags_unref_audio_signal_init(AgsUnrefAudioSignal *unref_audio_signal)
{
  unref_audio_signal->audio_signal = NULL;
}

void
ags_unref_audio_signal_connect(AgsConnectable *connectable)
{
  ags_unref_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_unref_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_unref_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_unref_audio_signal_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_unref_audio_signal_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_unref_audio_signal_launch(AgsTask *task)
{
  AgsUnrefAudioSignal *unref_audio_signal;

  unref_audio_signal = AGS_UNREF_AUDIO_SIGNAL(task);

  /* cancel AgsRecall */
  g_object_unref(unref_audio_signal->audio_signal);
}

AgsUnrefAudioSignal*
ags_unref_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsUnrefAudioSignal *unref_audio_signal;

  unref_audio_signal = (AgsUnrefAudioSignal *) g_object_new(AGS_TYPE_UNREF_AUDIO_SIGNAL,
							    NULL);

  unref_audio_signal->audio_signal = audio_signal;

  return(unref_audio_signal);
}
