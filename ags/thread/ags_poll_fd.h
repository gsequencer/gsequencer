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

#ifndef __AGS_POLL_FD_H__
#define __AGS_POLL_FD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_polling_thread.h>

#define AGS_TYPE_POLL_FD                (ags_poll_fd_get_type())
#define AGS_POLL_FD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_POLL_FD, AgsPollFd))
#define AGS_POLL_FD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_POLL_FD, AgsPollFdClass))
#define AGS_IS_POLL_FD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_POLL_FD))
#define AGS_IS_POLL_FD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_POLL_FD))
#define AGS_POLL_FD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_POLL_FD, AgsPollFdClass))

typedef struct _AgsPollFd AgsPollFd;
typedef struct _AgsPollFdClass AgsPollFdClass;

typedef enum{
  AGS_POLL_FD_READY            = 1,
  AGS_POLL_FD_THROUGHPUT       = 1 <<  1,
  AGS_POLL_FD_INPUT            = 1 <<  2,
  AGS_POLL_FD_PRIORITY_INPUT   = 1 <<  3,
  AGS_POLL_FD_OUTPUT           = 1 <<  4,
  AGS_POLL_FD_HANG_UP          = 1 <<  5,
}AgsPollFdFlags;

struct _AgsPollFd
{
  GObject gobject;

  guint flags;

  AgsPollingThread *polling_thread;
  
  int fd;
};

struct _AgsPollFdClass
{
  GObjectClass gobject;

  void (*dispatch)(AgsPollFd *poll_fd);
};

GType ags_poll_fd_get_type();

void ags_poll_fd_dispatch(AgsPollFd *poll_fd);

AgsPollFd* ags_poll_fd_new();

#endif /*__AGS_POLL_FD_H__*/
