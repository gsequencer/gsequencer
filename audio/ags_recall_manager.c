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

#include <ags/audio/ags_recall_manager.h>

#include <ags/object/ags_connectable.h>

#include <stdio.h>

void ags_recall_manager_class_init(AgsRecallManagerClass *recall_manager);
void ags_recall_manager_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_manager_init(AgsRecallManager *recall_manager);
void ags_recall_manager_connect(AgsConnectable *connectable);
void ags_recall_manager_disconnect(AgsConnectable *connectable);
void ags_recall_manager_finalize(GObject *gobject);

static gpointer ags_recall_manager_parent_class = NULL;

GType
ags_recall_manager_get_type(void)
{
  static GType ags_type_recall_manager = 0;

  if(!ags_type_recall_manager){
    static const GTypeInfo ags_recall_manager_info = {
      sizeof (AgsRecallManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_manager_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_manager_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_manager = g_type_register_static(G_TYPE_OBJECT,
						     "AgsRecallManager\0",
						     &ags_recall_manager_info,
						     0);
    
    g_type_add_interface_static(ags_type_recall_manager,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_recall_manager);
}

void
ags_recall_manager_class_init(AgsRecallManagerClass *recall_manager)
{
  GObjectClass *gobject;

  ags_recall_manager_parent_class = g_type_class_peek_parent(recall_manager);

  gobject = (GObjectClass *) recall_manager;
  gobject->finalize = ags_recall_manager_finalize;
}

void
ags_recall_manager_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_recall_manager_connect;
  connectable->disconnect = ags_recall_manager_disconnect;
}

void
ags_recall_manager_init(AgsRecallManager *recall_manager)
{
}

void
ags_recall_manager_connect(AgsConnectable *connectable)
{
}

void
ags_recall_manager_disconnect(AgsConnectable *connectable)
{
}

void
ags_recall_manager_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_manager_parent_class)->finalize(gobject);
}

AgsRecallManager*
ags_recall_manager_new()
{
  AgsRecallManager *recall_manager;

  recall_manager = (AgsRecallManager *) g_object_new(AGS_TYPE_RECALL_MANAGER,
						     NULL);

  return(recall_manager);
}
