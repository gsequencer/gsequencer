/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_MAIN_LOOP_H__
#define __AGS_MAIN_LOOP_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_MAIN_LOOP                    (ags_main_loop_get_type())
#define AGS_MAIN_LOOP(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MAIN_LOOP, AgsMainLoop))
#define AGS_MAIN_LOOP_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_MAIN_LOOP, AgsMainLoopInterface))
#define AGS_IS_MAIN_LOOP(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MAIN_LOOP))
#define AGS_IS_MAIN_LOOP_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_MAIN_LOOP))
#define AGS_MAIN_LOOP_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_MAIN_LOOP, AgsMainLoopInterface))

typedef struct _AgsMainLoop AgsMainLoop;
typedef struct _AgsMainLoopInterface AgsMainLoopInterface;

struct _AgsMainLoopInterface
{
  GTypeInterface ginterface;
  
  GRecMutex* (*get_tree_lock)(AgsMainLoop *main_loop);
    
  void (*set_tic)(AgsMainLoop *main_loop, guint tic);
  guint (*get_tic)(AgsMainLoop *main_loop);

  void (*set_last_sync)(AgsMainLoop *main_loop, guint last_sync);
  guint (*get_last_sync)(AgsMainLoop *main_loop);

  void (*set_sync_tic)(AgsMainLoop *main_loop, guint sync_tic);
  guint (*get_sync_tic)(AgsMainLoop *main_loop);

  void (*sync_counter_inc)(AgsMainLoop *main_loop, guint tic);
  void (*sync_counter_dec)(AgsMainLoop *main_loop, guint tic);
  gboolean (*sync_counter_test)(AgsMainLoop *main_loop, guint tic);

  void (*change_frequency)(AgsMainLoop *main_loop,
			   gdouble frequency);
};

GType ags_main_loop_get_type();

GRecMutex* ags_main_loop_get_tree_lock(AgsMainLoop *main_loop);

void ags_main_loop_set_tic(AgsMainLoop *main_loop, guint tic);
guint ags_main_loop_get_tic(AgsMainLoop *main_loop);

void ags_main_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync);
guint ags_main_loop_get_last_sync(AgsMainLoop *main_loop);

void ags_main_loop_set_sync_tic(AgsMainLoop *main_loop, guint sync_tic);
guint ags_main_loop_get_sync_tic(AgsMainLoop *main_loop);

void ags_main_loop_sync_counter_inc(AgsMainLoop *main_loop, guint tic);
void ags_main_loop_sync_counter_dec(AgsMainLoop *main_loop, guint tic);
gboolean ags_main_loop_sync_counter_test(AgsMainLoop *main_loop, guint tic);

void ags_main_loop_change_frequency(AgsMainLoop *main_loop,
				    gdouble frequency);

G_END_DECLS

#endif /*__AGS_MAIN_LOOP_H__*/
