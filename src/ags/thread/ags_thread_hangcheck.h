/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_THREAD_HANGCHECK_H__
#define __AGS_THREAD_HANGCHECK_H__

#include <glib.h>
#include <glib-object.h>

#include <signal.h>
#include <pthread.h>

#define AGS_TYPE_THREAD_HANGCHECK                (ags_thread_hangcheck_get_type())
#define AGS_THREAD_HANGCHECK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_THREAD_HANGCHECK, AgsThreadHangcheck))
#define AGS_THREAD_HANGCHECK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_THREAD_HANGCHECK, AgsThreadHangcheckClass))
#define AGS_IS_THREAD_HANGCHECK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_THREAD_HANGCHECK))
#define AGS_IS_THREAD_HANGCHECK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_THREAD_HANGCHECK))
#define AGS_THREAD_HANGCHECK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_THREAD_HANGCHECK, AgsThreadHangcheckClass))

typedef struct _AgsThreadHangcheck AgsThreadHangcheck;
typedef struct _AgsThreadHangcheckClass AgsThreadHangcheckClass;

struct _AgsThreadHangcheck
{
  GObject object;

  GList *thread;
};

struct _AgsThreadHangcheckClass
{
  GObjectClass object;
};

GType ags_thread_hangcheck_get_type();

AgsThreadHangcheck* ags_thread_hangcheck_new();

#endif /*__AGS_THREAD_HANGCHECK_H__*/
