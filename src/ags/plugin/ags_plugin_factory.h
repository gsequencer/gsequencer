/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_PLUGIN_FACTORY_H__
#define __AGS_PLUGIN_FACTORY_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_plugin.h>

#define AGS_TYPE_PLUGIN_FACTORY                (ags_plugin_factory_get_type())
#define AGS_PLUGIN_FACTORY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLUGIN_FACTORY, AgsPluginFactory))
#define AGS_PLUGIN_FACTORY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLUGIN_FACTORY, AgsPluginFactoryClass))
#define AGS_IS_PLUGIN_FACTORY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLUGIN_FACTORY))
#define AGS_IS_PLUGIN_FACTORY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLUGIN_FACTORY))
#define AGS_PLUGIN_FACTORY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLUGIN_FACTORY, AgsPluginFactoryClass))

typedef struct _AgsPluginFactory AgsPluginFactory;
typedef struct _AgsPluginFactoryClass AgsPluginFactoryClass;

struct _AgsPluginFactory
{
  GObject object;

  GList *plugin;
};

struct _AgsPluginFactoryClass
{
  GObjectClass object;
  
  AgsPlugin* (*create)(AgsPluginFactory *plugin_factory,
		       gchar *plugin_name, gchar *version, gchar *build_id);
};

GType ags_plugin_factory_get_type(void);

AgsPlugin* ags_plugin_factory_create(AgsPluginFactory *plugin_factory,
				     gchar *plugin_name, gchar *version, gchar *build_id);

/* */
AgsPluginFactory* ags_plugin_factory_get_instance();
AgsPluginFactory* ags_plugin_factory_new();

#endif /*__AGS_PLUGIN_FACTORY_H__*/
