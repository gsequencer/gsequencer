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

#include <ags/audio/ags_timestamp_factory.h>

void ags_timestamp_factory_class_init(AgsTimestampFactoryClass *timestamp_factory);
void ags_timestamp_factory_init (AgsTimestampFactory *timestamp_factory);
void ags_timestamp_factory_finalize(GObject *gobject);

static gpointer ags_timestamp_factory_parent_class = NULL;
static guint timestamp_factory_signals[LAST_SIGNAL];

GType
ags_timestamp_factory_get_type (void)
{
  static GType ags_type_timestamp_factory = 0;

  if(!ags_type_timestamp_factory){
    static const GTypeInfo ags_timestamp_factory_info = {
      sizeof (AgsTimestampFactoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_timestamp_factory_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTimestampFactory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_timestamp_factory_init,
    };

    ags_type_timestamp_factory = g_type_register_static(G_TYPE_OBJECT,
							"AgsTimestampFactory\0",
							&ags_timestamp_factory_info,
							0);
  }

  return (ags_type_timestamp_factory);
}

void
ags_timestamp_factory_class_init(AgsTimestampFactoryClass *timestamp_factory)
{
  GObjectClass *gobject;

  ags_timestamp_factory_parent_class = g_type_class_peek_parent(timestamp_factory);

  /* GObjectClass */
  gobject = (GObjectClass *) timestamp_factory;

  gobject->finalize = ags_timestamp_factory_finalize;
}

void
ags_timestamp_factory_init(AgsTimestampFactory *timestamp_factory)
{
  timestamp_factory->flags = 0;
}

AgsTimestampFactory*
ags_timestamp_factory_new()
{
  AgsTimestampFactory *timestamp_factory;

  timestamp_factory = (AgsTimestampFactory *) g_object_new(AGS_TYPE_TIMESTAMP_FACTORY,
							   NULL);

  return(timestamp_factory);
}
