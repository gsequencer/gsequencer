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

#ifndef __AGS_PLUGIN_H__
#define __AGS_PLUGIN_H__

#include <glib-object.h>

#include <ags/file/ags_file.h>

#define AGS_TYPE_PLUGIN                    (ags_plugin_get_type())
#define AGS_PLUGIN(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLUGIN, AgsPlugin))
#define AGS_PLUGIN_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_PLUGIN, AgsPluginInterface))
#define AGS_IS_PLUGIN(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLUGIN))
#define AGS_IS_PLUGIN_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_PLUGIN))
#define AGS_PLUGIN_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_PLUGIN, AgsPluginInterface))

typedef void AgsPlugin;
typedef struct _AgsPluginInterface AgsPluginInterface;

struct _AgsPluginInterface
{
  GTypeInterface interface;
  
  gchar* (*get_name)(AgsPlugin *plugin);
  void (*set_name)(AgsPlugin *plugin, gchar *name);

  gchar* (*get_version)(AgsPlugin *plugin);
  void (*set_version)(AgsPlugin *plugin, gchar *version);

  gchar* (*get_build_id)(AgsPlugin *plugin);
  void (*set_build_id)(AgsPlugin *plugin, gchar *build_id);

  gchar* (*get_xml_type)(AgsPlugin *plugin);
  void (*set_xml_type)(AgsPlugin *plugin, gchar *xml_type);  

  GList* (*get_ports)(AgsPlugin *plugin);
  void (*set_ports)(AgsPlugin *plugin, GList *ports);

  void (*read)(AgsFile *file,
	       xmlNode *node,
	       AgsPlugin *plugin);
  xmlNode* (*write)(AgsFile *file,
		    xmlNode *parent,
		    AgsPlugin *plugin);
};

GType ags_plugin_get_type();

gchar* ags_plugin_get_name(AgsPlugin *plugin);
void ags_plugin_set_name(AgsPlugin *plugin, gchar *name);

gchar* ags_plugin_get_version(AgsPlugin *plugin);
void ags_plugin_set_version(AgsPlugin *plugin, gchar *version);

gchar* ags_plugin_get_build_id(AgsPlugin *plugin);
void ags_plugin_set_build_id(AgsPlugin *plugin, gchar *build_id);

gchar* ags_plugin_get_xml_type(AgsPlugin *plugin);
void ags_plugin_set_xml_type(AgsPlugin *plugin, gchar *xml_type);

GList* ags_plugin_get_ports(AgsPlugin *plugin);
void ags_plugin_set_ports(AgsPlugin *plugin, GList *ports);

void ags_plugin_read(AgsFile *file,
		     xmlNode *node,
		     AgsPlugin *plugin);
xmlNode* ags_plugin_write(AgsFile *file,
			  xmlNode *parent,
			  AgsPlugin *plugin);

#endif /*__AGS_PLUGIN_H__*/
