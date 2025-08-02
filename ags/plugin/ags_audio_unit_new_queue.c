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

#include <ags/plugin/ags_audio_unit_new_queue.h>

#include <stdlib.h>

/**
 * SECTION:ags_audio_unit_new_queue
 * @short_description: boxed type of Audio Unit new queue
 * @title: AgsAudioUnitNewQueue
 * @section_id:
 * @include: ags/lib/ags_audio_unit_new_queue.h
 *
 * Boxed type of audio_unit_new_queue data type.
 */

GType
ags_audio_unit_new_queue_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_audio_unit_new_queue = 0;

    ags_type_audio_unit_new_queue =
      g_boxed_type_register_static("AgsAudioUnitNewQueue",
				   (GBoxedCopyFunc) ags_audio_unit_new_queue_copy,
				   (GBoxedFreeFunc) ags_audio_unit_new_queue_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_unit_new_queue);
  }

  return(g_define_type_id__static);
}

/**
 * ags_audio_unit_new_queue_alloc:
 *
 * Allocate #AgsAudioUnitNewQueue-struct
 *
 * Returns: a new #AgsAudioUnitNewQueue-struct
 *
 * Since: 8.1.2
 */
AgsAudioUnitNewQueue*
ags_audio_unit_new_queue_alloc()
{
  AgsAudioUnitNewQueue *ptr;

  ptr = (AgsAudioUnitNewQueue *) g_new0(AgsAudioUnitNewQueue,
					1);

  return(ptr);
}

/**
 * ags_audio_unit_new_queue_copy:
 * @ptr: the original #AgsAudioUnitNewQueue-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsAudioUnitNewQueue-struct
 *
 * Since: 8.1.2
 */
gpointer
ags_audio_unit_new_queue_copy(AgsAudioUnitNewQueue *ptr)
{
  AgsAudioUnitNewQueue *new_ptr;
  
  new_ptr = (AgsAudioUnitNewQueue *) g_malloc(sizeof(AgsAudioUnitNewQueue));
  
  new_ptr->in_use = ptr->in_use;

  new_ptr->creation_timestamp = ptr->creation_timestamp;

  new_ptr->plugin_name = g_strdup(ptr->plugin_name);

  new_ptr->audio_unit = ptr->audio_unit;

  return(new_ptr);
}

/**
 * ags_audio_unit_new_queue_free:
 * @ptr: the #AgsAudioUnitNewQueue-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 8.1.2
 */
void
ags_audio_unit_new_queue_free(AgsAudioUnitNewQueue *ptr)
{
  g_free(ptr->plugin_name);
  
  g_free(ptr);
}
