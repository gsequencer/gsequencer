/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015,2017 Joël Krähemann
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

#ifndef __AGS_LV2_OPTION_MANAGER_H__
#define __AGS_LV2_OPTION_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>

#define AGS_TYPE_LV2_OPTION_MANAGER                (ags_lv2_option_manager_get_type())
#define AGS_LV2_OPTION_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_OPTION_MANAGER, AgsLv2OptionManager))
#define AGS_LV2_OPTION_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_OPTION_MANAGER, AgsLv2OptionManagerClass))
#define AGS_IS_LV2_OPTION_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_OPTION_MANAGER))
#define AGS_IS_LV2_OPTION_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_OPTION_MANAGER))
#define AGS_LV2_OPTION_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_OPTION_MANAGER, AgsLv2OptionManagerClass))

#define AGS_LV2_OPTION_RESSOURCE(ptr) ((AgsLv2OptionRessource *)(ptr))

typedef struct _AgsLv2OptionManager AgsLv2OptionManager;
typedef struct _AgsLv2OptionManagerClass AgsLv2OptionManagerClass;
typedef struct _AgsLv2OptionRessource AgsLv2OptionRessource;

struct _AgsLv2OptionManager
{
  GObject gobject;

  GHashTable *ressource;
};

struct _AgsLv2OptionManagerClass
{
  GObjectClass gobject;

  void (*get_option)(AgsLv2OptionManager *option_manager,
		     LV2_Handle instance,
		     LV2_Options_Option *option,
		     uint32_t *retval);
  void (*set_option)(AgsLv2OptionManager *option_manager,
		     LV2_Handle instance,
		     LV2_Options_Option *option,
		     uint32_t *retval);
};

struct _AgsLv2OptionRessource{
  LV2_Handle instance;
  LV2_Options_Option *option;
};

GType ags_lv2_option_manager_get_type(void);

/* option ressource */
AgsLv2OptionRessource* ags_lv2_option_ressource_alloc();

gboolean ags_lv2_option_manager_ressource_insert(AgsLv2OptionManager *lv2_option_manager,
						 AgsLv2OptionRessource *lv2_option_ressource, gpointer data);
gboolean ags_lv2_option_manager_ressource_remove(AgsLv2OptionManager *lv2_option_manager,
						 AgsLv2OptionRessource *lv2_option_ressource);

gpointer ags_lv2_option_manager_ressource_lookup(AgsLv2OptionManager *lv2_option_manager,
						 AgsLv2OptionRessource *lv2_option_ressource);

/* get and set option */
void ags_lv2_option_manager_get_option(AgsLv2OptionManager *lv2_option_manager,
				       LV2_Handle instance,
				       LV2_Options_Option *option,
				       uint32_t *retval);
void ags_lv2_option_manager_set_option(AgsLv2OptionManager *lv2_option_manager,
				       LV2_Handle instance,
				       LV2_Options_Option *option,
				       uint32_t *retval);

/* the lv2 interface */
uint32_t ags_lv2_option_manager_lv2_options_get(LV2_Handle instance,
						LV2_Options_Option* options);
uint32_t ags_lv2_option_manager_lv2_options_set(LV2_Handle instance,
						LV2_Options_Option* options);

/*  */
AgsLv2OptionManager* ags_lv2_option_manager_get_instance();
AgsLv2OptionManager* ags_lv2_option_manager_new();

#endif /*__AGS_LV2_OPTION_MANAGER_H__*/
