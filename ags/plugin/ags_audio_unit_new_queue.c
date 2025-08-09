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

  ptr->in_use = FALSE;

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

/**
 * ags_audio_unit_new_queue_get_in_use:
 * @new_queue: the #AgsAudioUnitNewQueue-struct
 *
 * Get in use of @new_queue.
 * 
 * Returns: %TRUE if in use, otherwise %FALSE
 *
 * Since: 8.1.2
 */
gboolean
ags_audio_unit_new_queue_get_in_use(AgsAudioUnitNewQueue *new_queue)
{
  g_return_val_if_fail(new_queue != NULL, FALSE);
  
  return(new_queue->in_use);
}

/**
 * ags_audio_unit_new_queue_set_in_use:
 * @new_queue: the #AgsAudioUnitNewQueue-struct
 * @in_use: %TRUE if in use, otherwise %FALSE
 *
 * Set in use of @new_queue.
 *
 * Since: 8.1.2
 */
void
ags_audio_unit_new_queue_set_in_use(AgsAudioUnitNewQueue *new_queue,
				    gboolean in_use)
{
  g_return_if_fail(new_queue != NULL);

  new_queue->in_use = in_use;
}

/**
 * ags_audio_unit_new_queue_get_creation_timestamp:
 * @new_queue: the #AgsAudioUnitNewQueue-struct
 *
 * Get creation timestamp of @new_queue.
 * 
 * Returns: the creation timestamp in unix time, micro seconds
 *
 * Since: 8.1.2
 */
gint64
ags_audio_unit_new_queue_get_creation_timestamp(AgsAudioUnitNewQueue *new_queue)
{
  g_return_val_if_fail(new_queue != NULL, 0);
  
  return(new_queue->creation_timestamp);
}

/**
 * ags_audio_unit_new_queue_set_creation_timestamp:
 * @new_queue: the #AgsAudioUnitNewQueue-struct
 * @creation_timestamp: the creation timestamp in unix time [usecs]
 *
 * Set creation timestamp of @new_queue.
 *
 * Since: 8.1.2
 */
void
ags_audio_unit_new_queue_set_creation_timestamp(AgsAudioUnitNewQueue *new_queue,
						gint64 creation_timestamp)
{
  g_return_if_fail(new_queue != NULL);

  new_queue->creation_timestamp = creation_timestamp;
}

/**
 * ags_audio_unit_new_queue_get_plugin_name:
 * @new_queue: the #AgsAudioUnitNewQueue-struct
 *
 * Get plugin name of @new_queue.
 * 
 * Returns: (transfer none): the plugin name as string
 *
 * Since: 8.1.2
 */
gchar*
ags_audio_unit_new_queue_get_plugin_name(AgsAudioUnitNewQueue *new_queue)
{
  g_return_val_if_fail(new_queue != NULL, NULL);
  
  return(new_queue->plugin_name);
}

/**
 * ags_audio_unit_new_queue_set_plugin_name:
 * @new_queue: the #AgsAudioUnitNewQueue-struct
 * @plugin_name: (transfer none): the plugin name as string
 *
 * Set plugin name of @new_queue.
 *
 * Since: 8.1.2
 */
void
ags_audio_unit_new_queue_set_plugin_name(AgsAudioUnitNewQueue *new_queue,
					 gchar *plugin_name)
{
  g_return_if_fail(new_queue != NULL);

  new_queue->plugin_name = g_strdup(plugin_name);
}

/**
 * ags_audio_unit_new_queue_get_audio_unit:
 * @new_queue: the #AgsAudioUnitNewQueue-struct
 *
 * Get audio unit of @new_queue.
 * 
 * Returns: (transfer none): the audio unit as AVAudioUnit
 *
 * Since: 8.1.2
 */
gpointer
ags_audio_unit_new_queue_get_audio_unit(AgsAudioUnitNewQueue *new_queue)
{
  g_return_val_if_fail(new_queue != NULL, NULL);
  
  return(new_queue->audio_unit);
}

/**
 * ags_audio_unit_new_queue_set_audio_unit:
 * @new_queue: the #AgsAudioUnitNewQueue-struct
 * @audio_unit: (transfer full): the audio unit as AVAudioUnit
 *
 * Set audio unit of @new_queue.
 *
 * Since: 8.1.2
 */
void
ags_audio_unit_new_queue_set_audio_unit(AgsAudioUnitNewQueue *new_queue,
					gpointer audio_unit)
{
  g_return_if_fail(new_queue != NULL);

  new_queue->audio_unit = audio_unit;
}
