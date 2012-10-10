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

#include <ags/audio/task/ags_change_bpm.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_tactable.h>

#include <math.h>

void ags_change_bpm_class_init(AgsChangeBpmClass *change_bpm);
void ags_change_bpm_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_change_bpm_init(AgsChangeBpm *change_bpm);
void ags_change_bpm_connect(AgsConnectable *connectable);
void ags_change_bpm_disconnect(AgsConnectable *connectable);
void ags_change_bpm_finalize(GObject *gobject);

void ags_change_bpm_launch(AgsTask *task);

static gpointer ags_change_bpm_parent_class = NULL;
static AgsConnectableInterface *ags_change_bpm_parent_connectable_interface;

GType
ags_change_bpm_get_type()
{
  static GType ags_type_change_bpm = 0;

  if(!ags_type_change_bpm){
    static const GTypeInfo ags_change_bpm_info = {
      sizeof (AgsChangeBpmClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_change_bpm_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChangeBpm),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_change_bpm_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_change_bpm_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_change_bpm = g_type_register_static(AGS_TYPE_TASK,
						 "AgsChangeBpm\0",
						 &ags_change_bpm_info,
						 0);

    g_type_add_interface_static(ags_type_change_bpm,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_change_bpm);
}

void
ags_change_bpm_class_init(AgsChangeBpmClass *change_bpm)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_change_bpm_parent_class = g_type_class_peek_parent(change_bpm);

  /* GObjectClass */
  gobject = (GObjectClass *) change_bpm;

  gobject->finalize = ags_change_bpm_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) change_bpm;
  
  task->launch = ags_change_bpm_launch;
}

void
ags_change_bpm_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_change_bpm_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_change_bpm_connect;
  connectable->disconnect = ags_change_bpm_disconnect;
}

void
ags_change_bpm_init(AgsChangeBpm *change_bpm)
{
  change_bpm->devout = NULL;

  change_bpm->bpm = 0.0;
}

void
ags_change_bpm_connect(AgsConnectable *connectable)
{
  ags_change_bpm_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_change_bpm_disconnect(AgsConnectable *connectable)
{
  ags_change_bpm_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_change_bpm_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_change_bpm_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_change_bpm_launch(AgsTask *task)
{
  AgsChangeBpm *change_bpm;
  AgsDevout *devout;
  AgsAttack *attack;
  double frames; // delay in frames
  guint delay, delay_old;
  GList *list;

  change_bpm = AGS_CHANGE_BPM(task);

  /* AgsDevout */
  devout = change_bpm->devout;

  frames = (double) devout->frequency / (60.0 / (double)change_bpm->bpm / 64.0);

  delay = (guint) floor(floor(frames) / (double) devout->buffer_size);

  delay_old = devout->delay;
  devout->delay = delay;

  if(devout->delay_counter > devout->delay)
    devout->delay_counter = devout->delay_counter % devout->delay;

  attack = devout->attack;

  if((AGS_DEVOUT_ATTACK_FIRST & (devout->flags)) != 0){
    attack->first_start = attack->first_start;
    attack->first_length = attack->first_length;
  }else{
    attack->first_start = attack->second_start;
    attack->first_length = attack->second_length;
  }

  attack->second_start = (attack->first_start + (guint) round(frames)) % devout->buffer_size;
  attack->second_length = devout->buffer_size - attack->second_start;

  /* AgsTactable */
  list = devout->tactable;

  while(list != NULL){
    ags_tactable_change_bpm(AGS_TACTABLE(list->data),
			    change_bpm->bpm);

    list = list->next;
  }
}

AgsChangeBpm*
ags_change_bpm_new(AgsDevout *devout,
		   gdouble bpm)
{
  AgsChangeBpm *change_bpm;

  change_bpm = (AgsChangeBpm *) g_object_new(AGS_TYPE_CHANGE_BPM,
					     NULL);

  change_bpm->devout = devout;
  change_bpm->bpm = bpm;

  return(change_bpm);
}
