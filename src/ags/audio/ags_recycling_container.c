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
  recycling_container->recycling = NULL;
  recycling_container->length = 0;

  recycling_container->parent = NULL;
  recycling_container->children = NULL;
}

void
ags_recycling_container_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recycling_container_parent_class)->finalize(gobject);
}

void
ags_recycling_container_replace(AgsRecyclingContainer *recycling_container,
				AgsRecycling *recycling,
				gint position)
{
  if(recycling_container == NULL){
    return;
  }

  recycling_container->recycling[position] = recycling;
}

AgsRecyclingContainer*
ags_recycling_container_add(AgsRecyclingContainer *recycling_container,
			    AgsRecycling *recycling)
{
  AgsRecyclingContainer *new_recyclingcontainer;
  gint new_length;

  new_recycling_container = (AgsRecyclingContainer *) g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
								   NULL);

  new_length = recycling_container->length + 1;

  new_recycling_container->recycling = (AgsRecycling **) malloc(new_length * sizeof(AgsRecycling *));

  memcpy(new_recycling_container->recycling, recycling_container->recycling, new_length * sizeof(AgsRecycling *));
  new_recycling_container->recycling[new_length] = recycling;

  return(new_recycling_container);
}

AgsRecyclingContainer*
ags_recycling_container_remove(AgsRecyclingContainer *recycling_container,
			       AgsRecycling *recycling)
{
  AgsRecyclingContainer *new_recyclingcontainer;
  gint new_length;

  new_recycling_container = (AgsRecyclingContainer *) g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
								   NULL);

  new_length = recycling_container->length - 1;

  new_recycling_container->recycling = (AgsRecycling **) malloc(new_length * sizeof(AgsRecycling *));

  memcpy(new_recycling_container->recycling, recycling_container->recycling, (new_length - position) * sizeof(AgsRecycling *));
  memcpy(&(new_recycling_container->recycling[position + 1]), recycling_container->recycling, (-1 * (position - new_length)) * sizeof(AgsRecycling *));

  return(new_recycling_container);
}

AgsRecyclingContainer*
ags_recycling_container_insert(AgsRecyclingContainer *recycling_container,
			       AgsRecycling *recycling,
			       gint position)
{
  AgsRecyclingContainer *new_recyclingcontainer;
  gint new_length;

  new_recycling_container = (AgsRecyclingContainer *) g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
								   NULL);

  new_length = recycling_container->length + 1;

  new_recycling_container->recycling = (AgsRecycling **) malloc(new_length * sizeof(AgsRecycling *));

  memcpy(new_recycling_container->recycling, recycling_container->recycling, (new_length - position) * sizeof(AgsRecycling *));
  new_recycling_container->recycling[position] = recycling;
  memcpy(&(new_recycling_container->recycling[position + 1]), recycling_container->recycling, (-1 * (position - new_length)) * sizeof(AgsRecycling *));

  return(new_recycling_container);
}

gint
ags_recycling_container_find(AgsRecyclingContainer *recycling_container,
			     AgsRecycling *recycling)
{
  gint i;

  for(i = 0; i < recycling_container->length; i++){
    if(recycling_container->recycling == recycling){
      return(i);
    }
  }

  return(-1);
}

gint
ags_recycling_container_find_parent(AgsRecyclingContainer *recycling_container,
				    AgsRecycling *recycling)
{
  gint i;

  for(i = 0; recycling != NULL; i++){
    if(AGS_RECALL_ID(recycling_container->parent)->recycling == recycling){
      return(i);
    }

    recycling_container = AGS_RECALL_ID(recycling_container->parent)->recycling_container;
  }

  return(-1);
}

AgsRecyclingContainer*
ags_recycling_container_new(gint length)
{
  AgsRecyclingContainer *recycling_container;
  gint i;

  recycling_container = g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
				     NULL);

  recycling_container->recycling = (AgsRecycling **) malloc(length * sizeof(AgsRecycling *));

  for(i = 0; i < length; i++){
    recycling_container->recycling = NULL;
  }

  return(recycling_container);
}
