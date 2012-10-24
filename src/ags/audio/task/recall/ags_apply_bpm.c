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

#include <ags/audio/task/ags_apply_bpm.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_tactable.h>

#include <math.h>

void ags_apply_bpm_class_init(AgsApplyBpmClass *apply_bpm);
void ags_apply_bpm_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_bpm_init(AgsApplyBpm *apply_bpm);
void ags_apply_bpm_connect(AgsConnectable *connectable);
void ags_apply_bpm_disconnect(AgsConnectable *connectable);
void ags_apply_bpm_finalize(GObject *gobject);

void ags_apply_bpm_launch(AgsTask *task);

static gpointer ags_apply_bpm_parent_class = NULL;
static AgsConnectableInterface *ags_apply_bpm_parent_connectable_interface;

GType
ags_apply_bpm_get_type()
{
  static GType ags_type_apply_bpm = 0;

  if(!ags_type_apply_bpm){
    static const GTypeInfo ags_apply_bpm_info = {
      sizeof (AgsApplyBpmClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_bpm_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsApplyBpm),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_bpm_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_apply_bpm_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_apply_bpm = g_type_register_static(AGS_TYPE_TASK,
						"AgsApplyBpm\0",
						&ags_apply_bpm_info,
						0);
    
    g_type_add_interface_static(ags_type_apply_bpm,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_apply_bpm);
}

void
ags_apply_bpm_class_init(AgsApplyBpmClass *apply_bpm)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_apply_bpm_parent_class = g_type_class_peek_parent(apply_bpm);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_bpm;

  gobject->finalize = ags_apply_bpm_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_bpm;
  
  task->launch = ags_apply_bpm_launch;
}

void
ags_apply_bpm_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_apply_bpm_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_apply_bpm_connect;
  connectable->disconnect = ags_apply_bpm_disconnect;
}

void
ags_apply_bpm_init(AgsApplyBpm *apply_bpm)
{
  apply_bpm->delay_audio = NULL;

  apply_bpm->bpm = 0.0;
}

void
ags_apply_bpm_connect(AgsConnectable *connectable)
{
  ags_apply_bpm_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_apply_bpm_disconnect(AgsConnectable *connectable)
{
  ags_apply_bpm_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_apply_bpm_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_apply_bpm_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_apply_bpm_launch(AgsTask *task)
{
  //TODO:JK: implement me
}

AgsApplyBpm*
ags_apply_bpm_new(AgsDelayAudio *delay_audio,
			   gdouble bpm)
{
  AgsApplyBpm *apply_bpm;

  apply_bpm = (AgsApplyBpm *) g_object_new(AGS_TYPE_APPLY_BPM,
					   NULL);

  apply_bpm->delay_audio = delay_audio;
  apply_bpm->bpm = bpm;

  return(apply_bpm);
}
