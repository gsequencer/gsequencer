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

#include <ags/plugin/ags_plugin_factory.h>

#include <ags/object/ags_marshal.h>

void ags_plugin_factory_class_init(AgsPluginFactoryClass *plugin_factory);
void ags_plugin_factory_init (AgsPluginFactory *plugin_factory);
void ags_plugin_factory_finalize(GObject *gobject);

void ags_plugin_factory_real_add(AgsPluginFactory *plugin_factory,
				 GType type, gchar *plugin_name, gchar *version, gchar *build_id, gchar *xml_type);
AgsPlugin* ags_plugin_factory_real_create(AgsPluginFactory *plugin_factory,
					  gchar *plugin_name, gchar *version, gchar *build_id);

enum{
  ADD,
  CREATE,
  LAST_SIGNAL,
};

static gpointer ags_plugin_factory_parent_class = NULL;
static guint plugin_factory_signals[LAST_SIGNAL];

static AgsPluginFactory *ags_plugin_factory = NULL;

GType
ags_plugin_factory_get_type (void)
{
  static GType ags_type_plugin_factory = 0;

  if(!ags_type_plugin_factory){
    static const GTypeInfo ags_plugin_factory_info = {
      sizeof (AgsPluginFactoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_plugin_factory_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPluginFactory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_plugin_factory_init,
    };

    ags_type_plugin_factory = g_type_register_static(G_TYPE_OBJECT,
							"AgsPluginFactory\0",
							&ags_plugin_factory_info,
							0);
  }

  return (ags_type_plugin_factory);
}

void
ags_plugin_factory_class_init(AgsPluginFactoryClass *plugin_factory)
{
  GObjectClass *gobject;

  ags_plugin_factory_parent_class = g_type_class_peek_parent(plugin_factory);

  /* GObjectClass */
  gobject = (GObjectClass *) plugin_factory;

  gobject->finalize = ags_plugin_factory_finalize;

  /*  */
  plugin_factory->add = ags_plugin_factory_real_add;
  plugin_factory->create = ags_plugin_factory_real_create;

  plugin_factory_signals[ADD] =
    g_signal_new("add\0",
		 G_TYPE_FROM_CLASS (plugin_factory),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPluginFactoryClass, add),
		 NULL, NULL,
		 g_cclosure_user_marshal_STRING__ULONG_STRING_STRING_STRING_STRING,
		 G_TYPE_OBJECT, 4,
		 G_TYPE_ULONG, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);


  plugin_factory_signals[CREATE] =
    g_signal_new("create\0",
		 G_TYPE_FROM_CLASS (plugin_factory),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPluginFactoryClass, create),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__STRING_STRING_STRING,
		 G_TYPE_OBJECT, 3,
		 G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
}

void
ags_plugin_factory_init(AgsPluginFactory *plugin_factory)
{
  plugin_factory->plugin = NULL;
}

void
ags_plugin_factory_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

GList*
ags_plugin_factory_list_by_name(AgsPluginFactory *plugin_factory,
				gchar *plugin_name)
{
  GList *list;

  list = NULL;

  //TODO:JK: implement me

  return(list);
}

void
ags_plugin_factory_real_add(AgsPluginFactory *plugin_factory,
			    GType type, gchar *plugin_name, gchar *version, gchar *build_id, gchar *xml_type)
{
  //TODO:JK: implement me
}

void
ags_plugin_factory_add(AgsPluginFactory *plugin_factory,
		       GType type, gchar *plugin_name, gchar *version, gchar *build_id, gchar *xml_type)
{
  //TODO:JK: implement me
}

AgsPlugin*
ags_plugin_factory_real_create(AgsPluginFactory *plugin_factory,
			       gchar *plugin_name, gchar *version, gchar *build_id)
{
  AgsPlugin *plugin;

  plugin = NULL;

  //TODO:JK: implement me

  return(plugin);
}

AgsPlugin*
ags_plugin_factory_create(AgsPluginFactory *plugin_factory,
			  gchar *plugin_name, gchar *version, gchar *build_id)
{
  //TODO:JK: implement me
}

AgsPluginFactory*
ags_plugin_factory_get_instance()
{
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_plugin_factory == NULL){
    ags_plugin_factory = ags_plugin_factory_new();
  }

  pthread_mutex_unlock(&(mutex));

  return(ags_plugin_factory);
}

AgsPluginFactory*
ags_plugin_factory_new()
{
  AgsPluginFactory *plugin_factory;

  plugin_factory = (AgsPluginFactory *) g_object_new(AGS_TYPE_PLUGIN_FACTORY,
						     NULL);

  return(plugin_factory);
}
