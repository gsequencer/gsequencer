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

#ifndef __AGS_LADSPA_MANAGER_H__
#define __AGS_LADSPA_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/plugin/ags_ladspa_plugin.h>

#define AGS_TYPE_LADSPA_MANAGER                (ags_ladspa_manager_get_type())
#define AGS_LADSPA_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LADSPA_MANAGER, AgsLadspaManager))
#define AGS_LADSPA_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LADSPA_MANAGER, AgsLadspaManagerClass))
#define AGS_IS_LADSPA_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LADSPA_MANAGER))
#define AGS_IS_LADSPA_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LADSPA_MANAGER))
#define AGS_LADSPA_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LADSPA_MANAGER, AgsLadspaManagerClass))

typedef struct _AgsLadspaManager AgsLadspaManager;
typedef struct _AgsLadspaManagerClass AgsLadspaManagerClass;

struct _AgsLadspaManager
{
  GObject object;
  
  GList *ladspa_plugin;
};

struct _AgsLadspaManagerClass
{
  GObjectClass object;
};

GType ags_ladspa_manager_get_type(void);

gchar** ags_ladspa_manager_get_filenames();
AgsLadspaPlugin* ags_ladspa_manager_find_ladspa_plugin(gchar *filename, gchar *effect);

void ags_ladspa_manager_load_file(gchar *ladspa_plugin,
				  gchar *filename);
void ags_ladspa_manager_load_default_directory();

/*  */
AgsLadspaManager* ags_ladspa_manager_get_instance();

AgsLadspaManager* ags_ladspa_manager_new();

#endif /*__AGS_LADSPA_MANAGER_H__*/
