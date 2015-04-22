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

#ifndef __AGS_MAIN_LOOP_H__
#define __AGS_MAIN_LOOP_H__

#include <glib-object.h>

#define AGS_TYPE_MAIN_LOOP                    (ags_main_loop_get_type())
#define AGS_MAIN_LOOP(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MAIN_LOOP, AgsMainLoop))
#define AGS_MAIN_LOOP_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_MAIN_LOOP, AgsMainLoopInterface))
#define AGS_IS_MAIN_LOOP(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MAIN_LOOP))
#define AGS_IS_MAIN_LOOP_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_MAIN_LOOP))
#define AGS_MAIN_LOOP_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_MAIN_LOOP, AgsMainLoopInterface))

typedef void AgsMainLoop;
typedef struct _AgsMainLoopInterface AgsMainLoopInterface;

struct _AgsMainLoopInterface
{
  GTypeInterface interface;

  void (*set_async_queue)(AgsMainLoop *main_loop, GObject *async_queue);
  GObject* (*get_async_queue)(AgsMainLoop *main_loop);
  
  void (*set_tic)(AgsMainLoop *main_loop, guint tic);
  guint (*get_tic)(AgsMainLoop *main_loop);

  void (*set_last_sync)(AgsMainLoop *main_loop, guint last_sync);
  guint (*get_last_sync)(AgsMainLoop *main_loop);
};

GType ags_main_loop_get_type();

void ags_main_loop_set_async_queue(AgsMainLoop *main_loop, GObject *async_queue);
GObject* ags_main_loop_get_async_queue(AgsMainLoop *main_loop);

void ags_main_loop_set_tic(AgsMainLoop *main_loop, guint tic);
guint ags_main_loop_get_tic(AgsMainLoop *main_loop);

void ags_main_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync);
guint ags_main_loop_get_last_sync(AgsMainLoop *main_loop);

#endif /*__AGS_MAIN_LOOP_H__*/
