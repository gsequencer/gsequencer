/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/task/ags_change_tact.h>

#include <ags-lib/object/ags_connectable.h>

void ags_change_tact_class_init(AgsChangeTactClass *change_tact);
void ags_change_tact_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_change_tact_init(AgsChangeTact *change_tact);
void ags_change_tact_connect(AgsConnectable *connectable);
void ags_change_tact_disconnect(AgsConnectable *connectable);
void ags_change_tact_finalize(GObject *gobject);

void ags_change_tact_launch(AgsTask *task);

static gpointer ags_change_tact_parent_class = NULL;
static AgsConnectableInterface *ags_change_tact_parent_connectable_interface;

GType
ags_change_tact_get_type()
{
  static GType ags_type_change_tact = 0;

  if(!ags_type_change_tact){
    static const GTypeInfo ags_change_tact_info = {
      sizeof (AgsChangeTactClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_change_tact_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChangeTact),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_change_tact_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_change_tact_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_change_tact = g_type_register_static(AGS_TYPE_TASK,
						  "AgsChangeTact\0",
						  &ags_change_tact_info,
						  0);

    g_type_add_interface_static(ags_type_change_tact,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_change_tact);
}

void
ags_change_tact_class_init(AgsChangeTactClass *change_tact)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_change_tact_parent_class = g_type_class_peek_parent(change_tact);

  /* gobject */
  gobject = (GObjectClass *) change_tact;

  gobject->finalize = ags_change_tact_finalize;

  /* task */
  task = (AgsTaskClass *) change_tact;

  task->launch = ags_change_tact_launch;
}

void
ags_change_tact_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_change_tact_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_change_tact_connect;
  connectable->disconnect = ags_change_tact_disconnect;
}

void
ags_change_tact_init(AgsChangeTact *change_tact)
{
  change_tact->navigation = NULL;
  change_tact->new_tact = 1.0;
}

void
ags_change_tact_connect(AgsConnectable *connectable)
{
  ags_change_tact_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_change_tact_disconnect(AgsConnectable *connectable)
{
  ags_change_tact_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_change_tact_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_change_tact_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_change_tact_launch(AgsTask *task)
{
  AgsChangeTact *change_tact;
  gdouble new_tact;

  change_tact = AGS_CHANGE_TACT(task);

  gtk_spin_button_set_value(change_tact->navigation->position_tact,
			    change_tact->new_tact);
}

AgsChangeTact*
ags_change_tact_new(AgsNavigation *navigation,
		    gdouble new_tact)
{
  AgsChangeTact *change_tact;

  change_tact = (AgsChangeTact *) g_object_new(AGS_TYPE_CHANGE_TACT,
					       NULL);
  
  change_tact->navigation = navigation;
  change_tact->new_tact = new_tact;

  return(change_tact);
}
