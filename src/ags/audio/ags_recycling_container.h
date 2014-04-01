/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Jo?l Krähemann
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

  GObject *recall_id;

  AgsRecycling **recycling;
  guint64 length;

  AgsRecyclingContainer *parent;
  GList *children;
};

struct _AgsRecyclingContainerClass
{
  GObjectClass object;
};

GType ags_recycling_container_get_type();

void ags_recycling_container_replace(AgsRecyclingContainer *recycling_container,
				     AgsRecycling *recycling,
				     gint position);

AgsRecyclingContainer* ags_recycling_container_add(AgsRecyclingContainer *recycling_container,
						   AgsRecycling *recycling);
AgsRecyclingContainer* ags_recycling_container_remove(AgsRecyclingContainer *recycling_container,
						      AgsRecycling *recycling);
AgsRecyclingContainer* ags_recycling_container_insert(AgsRecyclingContainer *recycling_container,
						      AgsRecycling *recycling,
						      gint position);

AgsRecyclingContainer* ags_recycling_container_get_toplevel(AgsRecyclingContainer *recycling_container);

gint ags_recycling_container_find(AgsRecyclingContainer *recycling_container,
				  AgsRecycling *recycling);
gint ags_recycling_container_find_child(AgsRecyclingContainer *recycling_container,
					AgsRecycling *recycling);
gint ags_recycling_container_find_parent(AgsRecyclingContainer *recycling_container,
					 AgsRecycling *recycling);

void ags_recycling_container_add_child(AgsRecyclingContainer *parent,
				       AgsRecyclingContainer *child);
void ags_recycling_container_remove_child(AgsRecyclingContainer *parent,
					  AgsRecyclingContainer *child);

GList* ags_recycling_container_get_child_recall_id(AgsRecyclingContainer *recycling_container);

void ags_recycling_container_reset_recycling(AgsRecyclingContainer *recycling_container,
					     AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
					     AgsRecycling *new_first_recycling, AgsRecycling *new_last_recycling);

AgsRecyclingContainer* ags_recycling_container_new(gint length);

#endif /*__AGS_RECYCLING_CONTAINER_H__*/
