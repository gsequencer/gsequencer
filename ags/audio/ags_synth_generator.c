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

#include <ags/audio/ags_synth_generator.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>
#include <ags/file/ags_file_link.h>

#include <math.h>

void ags_synth_generator_class_init(AgsSynthGeneratorClass *synth_generator);
void ags_synth_generator_plugin_interface_init(AgsPluginInterface *plugin);
void ags_synth_generator_init(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_synth_generator_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_synth_generator_read(AgsFile *file,
			      xmlNode *node,
			      AgsPlugin *plugin);
xmlNode* ags_synth_generator_write(AgsFile *file,
				   xmlNode *parent,
				   AgsPlugin *plugin);
void ags_synth_generator_finalize(GObject *gobject);

/**
 * SECTION:ags_synth_generator
 * @short_description: generate wave-form
 * @title: AgsSynthGenerator
 * @section_id:
 * @include: ags/file/ags_synth_generator.h
 *
 * The #AgsSynthGenerator generates wave-forms with oscillators.
 */

enum{
  PROP_0,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_OSCILLATOR,
  PROP_FREQUENCY,
  PROP_PHASE,
  PROP_VOLUME,
  PROP_N_FRAMES,
};

static gpointer ags_synth_generator_parent_class = NULL;
static AgsPluginInterface *ags_synth_generator_parent_plugin_interface;

static const gchar *ags_synth_generator_plugin_name = "ags-synth-generator\0";

GType
ags_synth_generator_get_type()
{
  static GType ags_type_synth_generator = 0;

  if(!ags_type_synth_generator){
    static const GTypeInfo ags_synth_generator_info = {
      sizeof (AgsSynthGeneratorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_synth_generator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSynthGenerator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_synth_generator_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_synth_generator_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_synth_generator = g_type_register_static(AGS_TYPE_FUNCTION,
						      "AgsSynthGenerator\0",
						      &ags_synth_generator_info,
						      0);

    g_type_add_interface_static(ags_type_synth_generator,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_synth_generator);
}

void
ags_synth_generator_class_init(AgsSynthGeneratorClass *synth_generator)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_synth_generator_parent_class = g_type_class_peek_parent(synth_generator);

  /* GObjectClass */
  gobject = (GObjectClass *) synth_generator;

  gobject->set_property = ags_synth_generator_set_property;
  gobject->get_property = ags_synth_generator_get_property;

  gobject->finalize = ags_synth_generator_finalize;

  /* properties */
  /**
   * AgsSynthGenerator:timestamp:
   *
   * The assigned timestamp.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_object("timestamp\0",
				   "timestamp\0",
				   "The timestamp\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);
}

void
ags_synth_generator_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_synth_generator_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_synth_generator_read;
  plugin->write = ags_synth_generator_write;
}

void
ags_synth_generator_init(AgsSynthGenerator *synth_generator)
{
  synth_generator->timestamp = NULL;
}

void
ags_synth_generator_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsSynthGenerator *synth_generator;

  synth_generator = AGS_SYNTH_GENERATOR(gobject);
  
  switch(prop_id){
  case PROP_TIMESTAMP:
    {
      GObject *timestamp;

      timestamp = (GObject *) g_value_get_object(value);

      if((AgsTimestamp *) timestamp == synth_generator->timestamp){
	return;
      }

      if(synth_generator->timestamp != NULL){
	g_object_unref(synth_generator->timestamp);
      }

      if(timestamp != NULL){
	g_object_ref(timestamp);
      }

      synth_generator->timestamp = (AgsTimestamp *) timestamp;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_synth_generator_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsSynthGenerator *synth_generator;

  synth_generator = AGS_SYNTH_GENERATOR(gobject);
  
  switch(prop_id){
  case PROP_TIMESTAMP:
    {
      g_value_set_object(value, synth_generator->timestamp);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_synth_generator_read(AgsFile *file,
			 xmlNode *node,
			 AgsPlugin *plugin)
{
  AgsSynthGenerator *gobject;
  AgsFileLaunch *file_launch;

  xmlChar *str;

  gobject = AGS_SYNTH_GENERATOR(plugin);
  
  //TODO:JK: implement me
  
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "file\0", file,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_synth_generator_read_launch), gobject);
  ags_file_add_launch(file,
		      (GObject *) file_launch);
}

xmlNode*
ags_synth_generator_write(AgsFile *file,
			  xmlNode *parent,
			  AgsPlugin *plugin)
{
  AgsSynthGenerator *synth_generator;
  xmlNode *node;
  gchar *id;

  synth_generator = AGS_SYNTH_GENERATOR(plugin);

  //TODO:JK: implement me
  
  /* allocate new node with uuid */
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-synth-generator\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  /* add reference and node to file object */
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", synth_generator,
				   NULL));

  /* write timestamp */
  if(synth_generator->timestamp != NULL){
    ags_file_write_timestamp(file,
			     node,
			     synth_generator->timestamp);
  }
}

void
ags_synth_generator_finalize(GObject *gobject)
{
  AgsSynthGenerator *synth_generator;

  synth_generator = AGS_SYNTH_GENERATOR(gobject);

  if(synth_generator->timestamp != NULL){
    g_object_unref(synth_generator->timestamp);
  }
}

/**
 * ags_synth_generator_new:
 *
 * Creates an #AgsSynthGenerator
 *
 * Returns: a new #AgsSynthGenerator
 *
 * Since: 0.7.45
 */
AgsSynthGenerator*
ags_synth_generator_new(GObject *soundcard)
{
  AgsSynthGenerator *synth_generator;

  synth_generator = (AgsSynthGenerator *) g_object_new(AGS_TYPE_SYNTH_GENERATOR,
						      NULL);

  return(synth_generator);
}
