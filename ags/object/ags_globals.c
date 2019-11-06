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

#include <ags/object/ags_globals.h>

GHashTable *ags_globals = NULL;

static GMutex ags_globals_mutex;

/**
 * ags_globals_get_hash_table:
 * 
 * Get globals hash table.
 * 
 * Returns: the #GHashTable containig globals
 * 
 * Since: 3.0.0
 */
GHashTable*
ags_globals_get_hash_table()
{
  g_mutex_lock(ags_globals_mutex);

  if(ags_globals == NULL){
    ags_globals = g_hash_table_new_full(g_direct_hash, g_string_equal,
					NULL,
					NULL);    
  }
  
  g_mutex_unlock(ags_globals_mutex);

  return(ags_globals);
}

/**
 * ags_globals_set:
 * @key: the key as string
 * @value: the #GValue-struct
 * 
 * Set global key/value.
 * 
 * Since: 3.0.0
 */
void
ags_globals_set(gchar *key,
		GValue *value)
{
  GHashTable *globals;

  globals = ags_globals_get_hash_table();  

  g_mutex_lock(ags_globals_mutex);
  
  g_hash_table_insert(globals,
		      key, value);

  g_mutex_unlock(ags_globals_mutex);
}

/**
 * ags_globals_get:
 * @key: the key as string
 * 
 * Get global value for @key.
 * 
 * Returns: the #GValue-struct if @key found, otherwise %NULL
 * 
 * Since: 3.0.0
 */
GValue*
ags_globals_get(gchar *key)
{
  GHashTable *globals;
  GValue *value;
  
  globals = ags_globals_get_hash_table();  

  g_mutex_lock(ags_globals_mutex);

  value = g_hash_table_lookup(globals,
			      key);

  g_mutex_unlock(ags_globals_mutex);

  return(value);
}
