/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Jo?l Krähemann
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
#ifndef __AGS_RECYCLING_CONTAINER_H__
#define __AGS_RECYCLING_CONTAINER_H__

#include <glib-object.h>

#include <ags/audio/ags_recycling.h>

#define AGS_TYPE_RECYCLING_CONTAINER                (ags_recycling_container_get_type())
#define AGS_RECYCLING_CONTAINER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECYCLING_CONTAINER, AgsRecyclingContainer))
#define AGS_RECYCLING_CONTAINER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECYCLING_CONTAINER, AgsRecyclingContainerClass))
#define AGS_IS_RECYCLING_CONTAINER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECYCLING_CONTAINER))
#define AGS_IS_RECYCLING_CONTAINER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECYCLING_CONTAINER))
#define AGS_RECYCLING_CONTAINER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECYCLING_CONTAINER, AgsRecyclingContainerClass))

typedef struct _AgsRecyclingContainer AgsRecyclingContainer;
typedef struct _AgsRecyclingContainerClass AgsRecyclingContainerClass;

struct _AgsRecyclingContainer
{
  GObject object;

  GList *children;
};

struct _AgsRecyclingContainerClass
{
  GObjectClass object;
};

GType ags_recycling_container_get_type();

void ags_recycling_container_add(AgsRecyclingContainer *recycling_container,
				 AgsRecycling *recycling);
void ags_recycling_container_remove(AgsRecyclingContainer *recycling_container,
				    AgsRecycling *recycling);

GList* ags_recycling_container_find(AgsRecyclingContainer *recycling_container,
				    AgsRecycling *recycling);

AgsRecyclingContainer* ags_recycling_container_new();

#endif /*__AGS_RECYCLING_CONTAINER_H__*/

