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

#define AGS_RX_GATE_STACK_SIZE 2048
#define AGS_RX_GATE_COMMAND_BUFFER_SIZE 512

#define AGS_RX_GATE_CMD_R "r"
#define AGS_RX_GATE_CMD_W "w"
#define AGS_RX_GATE_CMD_X "x"
#define AGS_RX_GATE_SYN_R "r+"
#define AGS_RX_GATE_SYN_W "w+"
#define AGS_RX_GATE_SYN_X "(null)"

#define AGS_RX_GATE_0 "super"
#define AGS_RX_GATE_1 "this"
#define AGS_RX_GATE_N "next"

#define AGS_RX_GATE_FX(c,w,x) (g_strdup_printf("%s:fx(%x, %s)", w, x, c))

typedef struct _AgsThreadHangcheck AgsThreadHangcheck;
typedef struct _AgsThreadHangcheckClass AgsThreadHangcheckClass;
typedef struct _AgsRXGate AgsRXGate;

typedef enum{
  AGS_RX_GATE_MON_R     =  1,
  AGS_RX_GATE_MON_X     =  1 << 1,
  AGS_RX_GATE_MON_RX    =  1 << 2,
  AGS_RX_GATE_SYN_R     =  1 << 3,
  AGS_RX_GATE_SYN_X     =  1 << 4,
}AgsRXGateMonitor;

struct _AgsThreadHangcheck
{
  GObject object;

  GList *gate;
};

struct _AgsThreadHangcheckClass
{
  GObjectClass object;

  void (*error_event)(AgsThreadHangcheck *hangcheck);
};

struct _AgsRXGate {
  AgsThread *thread;

  volatile guint lock;

  volatile guint monitor_parent;
  volatile guint monitor_next;

  volatile gchar **command_stack;

  guint (*gate_control)(AgsRxGate *rx_gate,
			gchar *command, guint value);
};

GType ags_thread_hangcheck_get_type();

AgsThreadHangcheck* ags_thread_hangcheck_new();

#endif /*__AGS_THREAD_HANGCHECK_H__*/
