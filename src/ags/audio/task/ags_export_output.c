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

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

void ags_export_output_class_init(AgsExportOutputClass *export_output);
void ags_export_output_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_export_output_init(AgsExportOutput *export_output);
void ags_export_output_connect(AgsConnectable *connectable);
void ags_export_output_disconnect(AgsConnectable *connectable);
void ags_export_output_finalize(GObject *gobject);

void ags_export_output_launch(AgsTask *task);

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
  export_output->flags = 0;
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

  export_output = AGS_EXPORT_OUTPUT(task);

  audio_loop = AGS_AUDIO_LOOP(export_output->audio_loop);

  /* to implement me */
}

AgsExportOutput*
ags_export_output_new(gboolean live_performance)
{
  AgsExportOutput *export_output;

  export_output = (AgsExportOutput *) g_object_new(AGS_TYPE_EXPORT_OUTPUT,
						   NULL);

  return(export_output);
}
