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

#include <ags/audio/ags_timestamp.h>

void ags_timestamp_class_init(AgsTimestampClass *timestamp);
void ags_timestamp_init (AgsTimestamp *timestamp);
void ags_timestamp_finalize(GObject *gobject);

/**
 * SECTION:ags_timestamp
 * @short_description: Timestamp unix and alike
 * @title: AgsTimestamp
 * @section_id:
 * @include: ags/audio/ags_timestamp.h
 *
 * #AgsTimestamp measure of time.
 */

static gpointer ags_timestamp_parent_class = NULL;

GType
ags_timestamp_get_type (void)
{
  static GType ags_type_timestamp = 0;

  if(!ags_type_timestamp){
    static const GTypeInfo ags_timestamp_info = {
      sizeof (AgsTimestampClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_timestamp_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTimestamp),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_timestamp_init,
    };

    ags_type_timestamp = g_type_register_static(G_TYPE_OBJECT,
						"AgsTimestamp\0",
						&ags_timestamp_info,
						0);
  }

  return (ags_type_timestamp);
}

void
ags_timestamp_class_init(AgsTimestampClass *timestamp)
{
  GObjectClass *gobject;

  ags_timestamp_parent_class = g_type_class_peek_parent(timestamp);

  /* GObjectClass */
  gobject = (GObjectClass *) timestamp;

  gobject->finalize = ags_timestamp_finalize;
}

void
ags_timestamp_init(AgsTimestamp *timestamp)
{
  timestamp->flags = 0;

  time(&(timestamp->timer.unix_time.time_val));

  timestamp->delay = 0;
  timestamp->attack = 0;
}

void
ags_timestamp_finalize(GObject *gobject)
{
  /* empty */
}

/**
 * ags_timestamp_new:
 *
 * Creates an #AgsTimestamp
 *
 * Returns: a new #AgsTimestamp
 *
 * Since: 0.4
 */
AgsTimestamp*
ags_timestamp_new()
{
  AgsTimestamp *timestamp;

  timestamp = (AgsTimestamp *) g_object_new(AGS_TYPE_TIMESTAMP,
					    NULL);

  return(timestamp);
}
