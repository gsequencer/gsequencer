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

#include <ags/thread/ags_sync.h>

void ags_sync_class_init(AgsSyncClass *sync);
void ags_sync_init(AgsSync *sync);
void ags_sync_finalize(GObject *gobject);

void* ags_sync_supervisor_thread(void *ptr);

static gpointer ags_sync_parent_class = NULL;
static guint sync_signals[LAST_SIGNAL];

GType
ags_sync_get_type (void)
{
  static GType ags_type_sync = 0;

  if(!ags_type_sync){
    static const GTypeInfo ags_sync_info = {
      sizeof (AgsSyncClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sync_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSync),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sync_init,
    };

    ags_type_sync = g_type_register_static(G_TYPE_OBJECT,
					   "AgsSync",
					   &ags_sync_info,
					   0);
  }

  return (ags_type_sync);
}

void
ags_sync_class_init(AgsSyncClass *sync)
{
}

void
ags_sync_init(AgsSync *sync)
{
  sync->flags = 0;

  pthread_mutexattr_init(&(sync->supervisor_mutex_attr));
  pthread_mutex_init(&(sync->supervisor_mutex), &(sync->mutex_attr));

  pthread_cond_init(&(sync->wait_cond), NULL);

  sync->sync_thread = NULL;
  sync->wait_sync = 0;
}

void*
ags_sync_supervisor_thread(void *ptr)
{
  AgsSync *sync;

  sync = AGS_SYNC(ptr);

  while((AGS_SYNC_QUIT & (sync->flags)) == 0){

    //TODO:JK: implement/copy me from ags_devout.c

  }
}

void
ags_sync_start_supervisor(AgsSync *sync)
{
  pthread_create(&(sync->thread), NULL, &ags_sync_supervisor_thread, devout);
  pthread_setschedprio(sync->thread, 99);
}

AgsSync*
ags_sync_new()
{
  AgsSync *sync;

  sync = (AgsSync *) g_object_new(AGS_TYPE_SYNC,
				  NULL);

  return(sync);
}
