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

#include <ags/audio/ags_recycling_container.h>

void ags_recycling_container_class_init(AgsRecyclingContainerClass *recycling_container_class);
void ags_recycling_container_init(AgsRecyclingContainer *recycling_container);
void ags_recycling_container_finalize(GObject *gobject);

static gpointer ags_recycling_container_parent_class = NULL;

GType
ags_recycling_container_get_type (void)
{
  static GType ags_type_recycling_container = 0;

  if(!ags_type_recycling_container){
    static const GTypeInfo ags_recycling_container_info = {
      sizeof (AgsRecyclingContainerClass),
      (GBaseInitFunc) NULL, /* base_init */
      (GBaseFinalizeFunc) NULL, /* base_finalize */
      (GClassInitFunc) ags_recycling_container_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecyclingContainer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recycling_container_init,
    };

    ags_type_recycling_container = g_type_register_static(G_TYPE_OBJECT,
							  "AgsRecyclingContainer\0",
							  &ags_recycling_container_info,
							  0);
  }

  return (ags_type_recycling_container);
}

void
ags_recycling_container_class_init(AgsRecyclingContainerClass *recycling_container)
{
  GObjectClass *gobject;

  ags_recycling_container_parent_class = g_type_class_peek_parent(recycling_container);
  
  gobject = (GObjectClass *) recycling_container;
  gobject->finalize = ags_recycling_container_finalize;
}

void
ags_recycling_container_init(AgsRecyclingContainer *recycling_container)
{
}

void
ags_recycling_container_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recycling_container_parent_class)->finalize(gobject);
}

void
ags_recycling_container_add(AgsRecyclingContainer *recycling_container,
			    AgsRecycling *recycling)
{
}

void
ags_recycling_container_remove(AgsRecyclingContainer *recycling_container,
			       AgsRecycling *recycling)
{
}

GList*
ags_recycling_container_find(AgsRecyclingContainer *recycling_container,
			     AgsRecycling *recycling)
{
  return(NULL);
}

GList*
ags_recycling_container_find_parent(AgsRecyclingContainer *recycling_container,
				    AgsRecycling *recycling)
{
  return(NULL);
}

AgsRecyclingContainer*
ags_recycling_container_new()
{
  AgsRecyclingContainer *recycling_container;

  recycling_container = NULL;

  return(recycling_container);
}
