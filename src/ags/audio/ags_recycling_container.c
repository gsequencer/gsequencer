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

void ags_recycling_container_container_add(AgsRecyclingContainerContainer *recycling_container,
					   AgsRecycling *recycling);
void ags_recycling_container_container_remove(AgsRecyclingContainerContainer *recycling_container,
					      AgsRecycling *recycling);

void
ags_recycling_container_class_init(AgsRecyclingContainerClass *recycling_container_class)
{
}

void
ags_recycling_container_init(AgsRecycling *recycling_container)
{
}

void
ags_recycling_container_finalize(GObject *gobject)
{
}

void
ags_recycling_container_container_add(AgsRecyclingContainerContainer *recycling_container,
				      AgsRecycling *recycling)
{
}

void
ags_recycling_container_container_remove(AgsRecyclingContainerContainer *recycling_container,
					 AgsRecycling *recycling)
{
}

AgsRecyclingContainer*
ags_recycling_container_new()
{
  AgsRecyclingContaienr *recycling_container;

  recycling_container = NULL;

  return(recycling_container);
}
