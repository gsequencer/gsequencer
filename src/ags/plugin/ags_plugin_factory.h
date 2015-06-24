/* This file is part of GSequencer.
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
typedef struct _AgsPluginMetadata AgsPluginMetadata;

struct _AgsPluginFactory
{
  GObject object;

  GList *plugin;
};

struct _AgsPluginFactoryClass
{
  GObjectClass object;

  void (*add)(AgsPluginFactory *plugin_factory,
	      GType plugin_type,
	      gchar *plugin_name,
	      gchar *version, gchar *build_id,
	      gchar *xml_type,
	      GParameter *control, guint control_count);

  AgsPlugin* (*create)(AgsPluginFactory *plugin_factory,
		       gchar *plugin_name, gchar *version, gchar *build_id);
};

struct _AgsPluginMetadata
{
  GType plugin_type;
  gchar *plugin_name;

  gchar *version;
  gchar *build_id;

  gchar *xml_type;

  GParameter *control;
  guint control_count;
};

GType ags_plugin_factory_get_type(void);

void ags_plugin_factory_read_file(AgsPluginFactory *plugin_factory,
				  gchar *path);

GList* ags_plugin_factory_list_by_name(AgsPluginFactory *plugin_factory,
				       gchar *plugin_name);

void ags_plugin_factory_add(AgsPluginFactory *plugin_factory,
			    GType plugin_type,
			    gchar *plugin_name,
			    gchar *version, gchar *build_id,
			    gchar *xml_type,
			    GParameter *control, guint control_count);

AgsPlugin* ags_plugin_factory_create(AgsPluginFactory *plugin_factory,
				     gchar *plugin_name, gchar *version, gchar *build_id);

/* */
AgsPluginFactory* ags_plugin_factory_get_instance();

AgsPluginFactory* ags_plugin_factory_new_from_file(gchar *path);
AgsPluginFactory* ags_plugin_factory_new();

#endif /*__AGS_PLUGIN_FACTORY_H__*/
