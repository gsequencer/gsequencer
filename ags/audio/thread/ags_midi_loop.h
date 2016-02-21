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

#ifndef __AGS_MIDI_LOOP_H__
#define __AGS_MIDI_LOOP_H__

#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#define AGS_TYPE_MIDI_LOOP                (ags_midi_loop_get_type())
#define AGS_MIDI_LOOP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_LOOP, AgsMidiLoop))
#define AGS_MIDI_LOOP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MIDI_LOOP, AgsMidiLoopClass))
#define AGS_IS_MIDI_LOOP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI_LOOP))
#define AGS_IS_MIDI_LOOP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI_LOOP))
#define AGS_MIDI_LOOP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MIDI_LOOP, AgsMidiLoopClass))

#define AGS_MIDI_LOOP_DEFAULT_JIFFIE (48.0)

typedef struct _AgsMidiLoop AgsMidiLoop;
typedef struct _AgsMidiLoopClass AgsMidiLoopClass;

struct _AgsMidiLoop
{
  AgsThread thread;

  guint flags;

  volatile guint tic;
  volatile guint last_sync;

  GObject *application_context;
  GObject *sequencer;
  
  GObject *async_queue;
    
  pthread_mutexattr_t tree_lock_mutexattr;
  pthread_mutex_t *tree_lock;
  pthread_mutex_t *recall_mutex;

  guint play_midi_ref;
  GList *play_midi; // play AgsMidi

  guint play_notation_ref;
  GList *play_notation;

  GList *tree_sanity;
};

struct _AgsMidiLoopClass
{
  AgsThreadClass thread;
};

GType ags_midi_loop_get_type();

void ags_midi_loop_add_midi(AgsMidiLoop *midi_loop, GObject *midi);
void ags_midi_loop_remove_midi(AgsMidiLoop *midi_loop, GObject *midi);

AgsMidiLoop* ags_midi_loop_new(GObject *sequencer, GObject *application_context);

#endif /*__AGS_MIDI_LOOP_H__*/
