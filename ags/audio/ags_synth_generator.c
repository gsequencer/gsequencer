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

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_plugin.h>

#include <ags/thread/ags_timestamp.h>

#include <ags/thread/file/ags_thread_file_xml.h>

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
  PROP_TIMESTAMP,
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
   * AgsSynthGenerator:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("samplerate\0",
				 "using samplerate\0",
				 "The samplerate to be used\0",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsSynthGenerator:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("buffer-size\0",
				 "using buffer size\0",
				 "The buffer size to be used\0",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsSynthGenerator:format:
   *
   * The format to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("format\0",
				 "using format\0",
				 "The format to be used\0",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsSynthGenerator:n-frames:
   *
   * The n-frames to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("n-frames\0",
				 "apply n-frames\0",
				 "To apply n-frames\0",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_N_FRAMES,
				  param_spec);

  /**
   * AgsSynthGenerator:oscillator:
   *
   * The oscillator to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("oscillator\0",
				 "using oscillator\0",
				 "The oscillator to be used\0",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OSCILLATOR,
				  param_spec);

  /**
   * AgsAudioSignal:frequency:
   *
   * The frequency to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_double("frequency\0",
				   "using frequency\0",
				   "The frequency to be used\0",
				   0.0,
				   65535.0,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQUENCY,
				  param_spec);

  /**
   * AgsAudioSignal:phase:
   *
   * The phase to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_double("phase\0",
				   "using phase\0",
				   "The phase to be used\0",
				   0.0,
				   65535.0,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PHASE,
				  param_spec);

  /**
   * AgsAudioSignal:volume:
   *
   * The volume to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_double("volume\0",
				   "using volume\0",
				   "The volume to be used\0",
				   0.0,
				   65535.0,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VOLUME,
				  param_spec);

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
  case PROP_SAMPLERATE:
    {
      synth_generator->samplerate = g_value_get_uint(value);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      synth_generator->buffer_size = g_value_get_uint(value);
    }
    break;
  case PROP_FORMAT:
    {
      synth_generator->format = g_value_get_uint(value);
    }
    break;
  case PROP_N_FRAMES:
    {
      synth_generator->n_frames = g_value_get_uint(value);
    }
    break;
  case PROP_OSCILLATOR:
    {
      synth_generator->oscillator = g_value_get_uint(value);
    }
    break;
  case PROP_FREQUENCY:
    {
      synth_generator->frequency = g_value_get_double(value);
    }
    break;
  case PROP_PHASE:
    {
      synth_generator->phase = g_value_get_double(value);
    }
    break;
  case PROP_VOLUME:
    {
      synth_generator->volume = g_value_get_double(value);
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      if(synth_generator->timestamp == (GObject *) timestamp){
	return;
      }

      if(synth_generator->timestamp != NULL){
	g_object_unref(synth_generator->timestamp);
      }

      if(timestamp != NULL){
	g_object_ref(timestamp);
      }

      synth_generator->timestamp = (GObject *) timestamp;
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
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, synth_generator->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, synth_generator->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, synth_generator->format);
    }
    break;
  case PROP_N_FRAMES:
    {
      g_value_set_uint(value, synth_generator->n_frames);
    }
    break;
  case PROP_OSCILLATOR:
    {
      g_value_set_uint(value, synth_generator->oscillator);
    }
    break;
  case PROP_FREQUENCY:
    {
      g_value_set_uint(value, synth_generator->frequency);
    }
    break;
  case PROP_PHASE:
    {
      g_value_set_uint(value, synth_generator->phase);
    }
    break;
  case PROP_VOLUME:
    {
      g_value_set_uint(value, synth_generator->volume);
    }
    break;
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
  //  AgsSynthGenerator *gobject;

  //  xmlChar *str;

  //  gobject = AGS_SYNTH_GENERATOR(plugin);
  
  //TODO:JK: implement me
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

  //TODO:JK: implement me

  /* write timestamp */
  if(synth_generator->timestamp != NULL){
    ags_file_write_timestamp(file,
			     node,
			     (AgsTimestamp *) synth_generator->timestamp);
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
ags_synth_generator_new()
{
  AgsSynthGenerator *synth_generator;

  synth_generator = (AgsSynthGenerator *) g_object_new(AGS_TYPE_SYNTH_GENERATOR,
						      NULL);

  return(synth_generator);
}
