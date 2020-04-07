/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_LV2_TURTLE_SCANNER_H__
#define __AGS_LV2_TURTLE_SCANNER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_LV2_TURTLE_SCANNER                (ags_lv2_turtle_scanner_get_type())
#define AGS_LV2_TURTLE_SCANNER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_TURTLE_SCANNER, AgsLv2TurtleScanner))
#define AGS_LV2_TURTLE_SCANNER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_TURTLE_SCANNER, AgsLv2TurtleScannerClass))
#define AGS_IS_LV2_TURTLE_SCANNER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_TURTLE_SCANNER))
#define AGS_IS_LV2_TURTLE_SCANNER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_TURTLE_SCANNER))
#define AGS_LV2_TURTLE_SCANNER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_TURTLE_SCANNER, AgsLv2TurtleScannerClass))

#define AGS_LV2_CACHE_TURTLE(ptr) ((AgsLv2CacheTurtle *) ptr)

#define AGS_LV2_TURTLE_SCANNER_GET_OBJ_MUTEX(obj) (&(((AgsLv2TurtleScanner *) obj)->obj_mutex))

typedef struct _AgsLv2TurtleScanner AgsLv2TurtleScanner;
typedef struct _AgsLv2TurtleScannerClass AgsLv2TurtleScannerClass;
typedef struct _AgsLv2CacheTurtle AgsLv2CacheTurtle;

typedef enum{
  AGS_LV2_TURTLE_SCANNER_A             = 1,
  AGS_LV2_TURTLE_SCANNER_SEEALSO       = 1 <<  2,
  AGS_LV2_TURTLE_SCANNER_BINARY        = 1 <<  3,
  AGS_LV2_TURTLE_SCANNER_NAME          = 1 <<  4,
}AgsLv2TurtleScannerStatusFlags;

struct _AgsLv2TurtleScanner
{
  GObject gobject;

  guint flags;
  guint status_flags;
  
  GRecMutex obj_mutex;

  GList *cache_turtle;

  gchar *current_subject_iriref;
  gint plugin_position;
};

struct _AgsLv2TurtleScannerClass
{
  GObjectClass gobject;
};

struct _AgsLv2CacheTurtle
{
  AgsLv2CacheTurtle *parent;

  gchar *turtle_filename;
    
  GHashTable *prefix_id;
  
  GHashTable *plugin_filename;
  GHashTable *plugin_effect;

  GHashTable *is_plugin;
  GHashTable *is_instrument;
  
  guint plugin_count;
  
  gchar **see_also;
};

GType ags_lv2_turtle_scanner_get_type(void);

AgsLv2CacheTurtle* ags_lv2_cache_turtle_alloc(AgsLv2CacheTurtle *parent,
					      gchar *turtle_filename);
void ags_lv2_cache_turtle_free(AgsLv2CacheTurtle *lv2_cache_turtle);

void ags_lv2_turtle_scanner_quick_scan_see_also(AgsLv2TurtleScanner *lv2_turtle_scanner,
						AgsLv2CacheTurtle *parent,
						gchar *turtle_filename);

void ags_lv2_turtle_scanner_quick_scan(AgsLv2TurtleScanner *lv2_turtle_scanner,
				       gchar *manifest_filename);

AgsLv2TurtleScanner* ags_lv2_turtle_scanner_new();

G_END_DECLS

#endif /*__AGS_LV2_TURTLE_SCANNER_H__*/
