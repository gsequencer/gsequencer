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

#ifndef __AGS_SYNC_H__
#define __AGS_SYNC_H__

#define AGS_TYPE_SYNC                (ags_sync_get_type())
#define AGS_SYNC(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNC, AgsSync))
#define AGS_SYNC_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SYNC, AgsSync))
#define AGS_IS_SYNC(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SYNC))
#define AGS_IS_SYNC_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SYNC))
#define AGS_SYNC_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SYNC, AgsSyncClass))

typedef struct _AgsSync AgsSync;
typedef struct _AgsSyncClass AgsSyncClass;
typedef struct _AgsSyncThread AgsSyncThread;

typedef enum
{
  AGS_SYNC_QUIT   = 1,
}AgsSyncFlags;

struct _AgsSync
{
  GObject object;

  guint flags;

  pthread_t thread;
  pthread_mutexattr_t mutex_attr;
  pthread_mutex_t mutex;
  pthread_cond_t wait_cond;

  GList *sync_thread;
  guint wait_sync;
};

struct _AgsSyncClass
{
  GObjectClass object;
};

struct _AgsSyncThread
{
  AgsThread *thread;
  pthread_cond_t wait_cond;
};

GType ags_sync_get_type();

void ags_sync_start_supervisor(AgsSync *sync);

AgsSync* ags_sync_new();

#endif /*__AGS_SYNC_H__*/
