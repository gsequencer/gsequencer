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

#ifndef __AGS_FREQUENCY_MAP_H__
#define __AGS_FREQUENCY_MAP_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_FREQUENCY_MAP                (ags_frequency_map_get_type())
#define AGS_FREQUENCY_MAP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FREQUENCY_MAP, AgsFrequencyMap))
#define AGS_FREQUENCY_MAP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FREQUENCY_MAP, AgsFrequencyMapClass))
#define AGS_IS_FREQUENCY_MAP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FREQUENCY_MAP))
#define AGS_IS_FREQUENCY_MAP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FREQUENCY_MAP))
#define AGS_FREQUENCY_MAP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FREQUENCY_MAP, AgsFrequencyMapClass))

#define AGS_FREQUENCY_MAP_GET_OBJ_MUTEX(obj) (&(((AgsFrequencyMap *) obj)->obj_mutex))

#define AGS_FREQUENCY_MAP_DEFAULT_FREQ (440.0)

typedef struct _AgsFrequencyMap AgsFrequencyMap;
typedef struct _AgsFrequencyMapClass AgsFrequencyMapClass;

/**
 * AgsFrequencyMapFlags:
 * @AGS_FREQUENCY_MAP_PROCESSED: the frequency map was processed
 */
typedef enum{
  AGS_FREQUENCY_MAP_PROCESSED          = 1,
}AgsFrequencyMapFlags;

struct _AgsFrequencyMap
{
  GObject gobject;

  AgsFrequencyMapFlags flags;
  AgsConnectableFlags connectable_flags;
  
  GRecMutex obj_mutex;

  AgsUUID *uuid;

  guint z_index;
  guint window_count;
  
  guint samplerate;  
  guint buffer_size;
  
  guint frame_count;
  
  guint attack;

  guint oscillator_mode;

  gdouble freq;
  
  AgsComplex *buffer;
};

struct _AgsFrequencyMapClass
{
  GObjectClass gobject;

  void (*process)(AgsFrequencyMap *frequency_map);

  void (*factorize)(AgsFrequencyMap *frequency_map,
		    AgsFrequencyMap *factorized_frequency_map);
  
  void (*compute_max_likelihood)(AgsFrequencyMap *frequency_map,
				 AgsComplex *source,
				 AgsComplex **retval);
};

GType ags_frequency_map_get_type();

gint ags_frequency_map_sort_func(gconstpointer a,
				 gconstpointer b);

gboolean ags_frequency_map_test_flags(AgsFrequencyMap *frequency_map, AgsFrequencyMapFlags flags);
void ags_frequency_map_set_flags(AgsFrequencyMap *frequency_map, AgsFrequencyMapFlags flags);
void ags_frequency_map_unset_flags(AgsFrequencyMap *frequency_map, AgsFrequencyMapFlags flags);

void ags_frequency_map_process(AgsFrequencyMap *frequency_map);

void ags_frequency_map_factorize(AgsFrequencyMap *frequency_map,
				 AgsFrequencyMap *factorized_frequency_map);

void ags_frequency_map_compute_max_likelihood(AgsFrequencyMap *frequency_map,
					      AgsComplex *source,
					      AgsComplex **retval);

/* instantiate */
AgsFrequencyMap* ags_frequency_map_new();

G_END_DECLS

#endif /*__AGS_FREQUENCY_MAP_H__*/
