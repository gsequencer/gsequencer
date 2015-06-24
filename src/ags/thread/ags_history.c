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

#include <ags/thread/ags_history.h>

#include <ags-lib/object/ags_connectable.h>

void ags_history_class_init(AgsHistoryClass *history);
void ags_history_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_history_init(AgsHistory *history);
void ags_history_connect(AgsConnectable *connectable);
void ags_history_disconnect(AgsConnectable *connectable);
void ags_history_finalize(GObject *gobject);

enum{
  LAUNCH,
  FAILURE,
  LAST_SIGNAL,
};

static gpointer ags_history_parent_class = NULL;
static guint history_signals[LAST_SIGNAL];

GType
ags_history_get_type()
{
  static GType ags_type_history = 0;

  if(!ags_type_history){
    static const GTypeInfo ags_history_info = {
      sizeof (AgsHistoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_history_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsHistory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_history_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_history_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_history = g_type_register_static(G_TYPE_OBJECT,
					   "AgsHistory\0",
					   &ags_history_info,
					   0);

    g_type_add_interface_static(ags_type_history,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_history);
}

void
ags_history_class_init(AgsHistoryClass *history)
{
  GObjectClass *gobject;

  ags_history_parent_class = g_type_class_peek_parent(history);

  /* GObjectClass */
  gobject = (GObjectClass *) history;

  gobject->finalize = ags_history_finalize;
}

void
ags_history_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_history_connect;
  connectable->disconnect = ags_history_disconnect;
}

void
ags_history_init(AgsHistory *history)
{
}

void
ags_history_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_history_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_history_finalize(GObject *gobject)
{
}

AgsHistory*
ags_history_new()
{
  AgsHistory *history;

  history = (AgsHistory *) g_object_new(AGS_TYPE_HISTORY,
					NULL);

  return(history);
}

