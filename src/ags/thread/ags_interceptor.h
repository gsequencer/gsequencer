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

#ifndef __AGS_INTERCEPTOR_H__
#define __AGS_INTERCEPTOR_H__

#include <glib.h>
#include <glib-object.h>

#define __USE_GNU
#define __USE_UNIX98
#include <pthread.h>

typedef struct _AgsInterceptor AgsInterceptor;
typedef struct _AgsInterceptorClass AgsInterceptorClass;

typedef enum{
  AGS_INTERCEPTOR_WAIT       = 1,
  AGS_INTERCEPTOR_SYNC_WAIT  = 1 << 1,
}AgsInterceptorFlags;

struct _AgsInterceptor
{
  GObject object;

  guint flags;

  pthread_cond_t cond;

  pthread_t thread;
  pthread_mutex_t mutex;
  pthread_cond_t wait_cond;

  //  GObject *sync;
  //  GObject *thread;
};

struct _AgsInterceptorClass
{
  GObjectClass object;
};

GType ags_interceptor_get_type();

void* ags_interceptor_thread(void *ptr);

AgsInterceptor* ags_interceptor_new();

#endif /*__AGS_INTERCEPTOR_H__*/
