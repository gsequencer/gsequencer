/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_AUDIO_TREE_DISPATCHER_H__
#define __AGS_AUDIO_TREE_DISPATCHER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_TREE_DISPATCHER                (ags_audio_tree_dispatcher_get_type())
#define AGS_AUDIO_TREE_DISPATCHER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_TREE_DISPATCHER, AgsAudioTreeDispatcher))
#define AGS_AUDIO_TREE_DISPATCHER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_TREE_DISPATCHER, AgsAudioTreeDispatcherClass))
#define AGS_IS_AUDIO_TREE_DISPATCHER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO_TREE_DISPATCHER))
#define AGS_IS_AUDIO_TREE_DISPATCHER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO_TREE_DISPATCHER))
#define AGS_AUDIO_TREE_DISPATCHER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO_TREE_DISPATCHER, AgsAudioTreeDispatcherClass))

#define AGS_AUDIO_TREE_DISPATCHER_GET_OBJ_MUTEX(obj) (&(((AgsAudioTreeDispatcher *) obj)->obj_mutex))

typedef struct _AgsAudioTreeDispatcher AgsAudioTreeDispatcher;
typedef struct _AgsAudioTreeDispatcherClass AgsAudioTreeDispatcherClass;
typedef struct _AgsDispatchAudio AgsDispatchAudio;

struct _AgsAudioTreeDispatcher
{
  GObject gobject;

  guint flags;
  
  GRecMutex obj_mutex;
  
  guint *staging_program;
  guint staging_program_count;
  
  GList *tree_list;
  gint64 tree_list_stamp;
};

struct _AgsAudioTreeDispatcherClass
{
  GObjectClass gobject;
};

struct _AgsDispatchAudio
{
  GObject *dispatch_source;
  
  gint scope;

  GType tree_element_type;
  
  union{
    GObject *audio;
    GObject *output;
    GObject *input;
  }tree_element;

  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
};

GType ags_audio_tree_dispatcher_get_type(void);

/* tree list */
AgsDispatchAudio* ags_dispatch_audio_alloc(GObject *dispatch_source,
					   gint scope);
void ags_dispatch_audio_free(AgsDispatchAudio *dispatch_audio);

GList* ags_audio_tree_dispatcher_compile_tree_list(AgsAudioTreeDispatcher *audio_tree_list,
						   GObject *dispatch_source,
						   gint scope);

/* getter and setter */
guint* ags_audio_tree_dispatcher_get_staging_program(AgsAudioTreeDispatcher *audio_tree_dispatcher,
						     guint *staging_program_count);
void ags_audio_tree_dispatcher_set_staging_program(AgsAudioTreeDispatcher *audio_tree_dispatcher,
						   guint *staging_program, guint staging_program_count);

GList* ags_audio_tree_dispatcher_get_tree_list(AgsAudioTreeDispatcher *audio_tree_dispatcher);
void ags_audio_tree_dispatcher_set_tree_list(AgsAudioTreeDispatcher *audio_tree_dispatcher,
					     GList *tree_list);

gint64 ags_audio_tree_dispatcher_get_tree_list_stamp(AgsAudioTreeDispatcher *audio_tree_dispatcher);
void ags_audio_tree_dispatcher_set_tree_list_stamp(AgsAudioTreeDispatcher *audio_tree_dispatcher,
						   gint64 tree_list_stamp);

/* run */
void ags_audio_tree_dispatcher_run(AgsAudioTreeDispatcher *audio_tree_dispatcher);

/* instantiate */
AgsAudioTreeDispatcher* ags_audio_tree_dispatcher_new();

G_END_DECLS

#endif /*__AGS_AUDIO_TREE_DISPATCHER_H__*/
