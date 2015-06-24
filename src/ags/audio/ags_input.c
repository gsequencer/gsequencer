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

#include <ags/audio/ags_input.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/file/ags_file_link.h>

#include <ags/audio/file/ags_audio_file.h>

void ags_input_class_init (AgsInputClass *input_class);
void ags_input_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_input_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_input_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_input_init (AgsInput *input);
void ags_input_finalize (GObject *gobject);
void ags_input_connect(AgsConnectable *connectable);
void ags_input_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_input
 * @short_description: Input of #AgsAudio
 * @title: AgsInput
 * @section_id:
 * @include: ags/audio/ags_input.h
 *
 * #AgsInput represents an input channel of #AgsAudio.
 */

static gpointer ags_input_parent_class = NULL;
static AgsConnectableInterface *ags_input_parent_connectable_interface;

enum{
  PROP_0,
  PROP_FILE_LINK,
};

GType
ags_input_get_type (void)
{
  static GType ags_type_input = 0;

  if(!ags_type_input){
    static const GTypeInfo ags_input_info = {
      sizeof (AgsInputClass),
      (GBaseInitFunc) NULL, /* base_init */
      (GBaseFinalizeFunc) NULL, /* base_finalize */
      (GClassInitFunc) ags_input_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsInput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_input_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_input_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_input = g_type_register_static(AGS_TYPE_CHANNEL,
					    "AgsInput\0",
					    &ags_input_info,
					    0);

    g_type_add_interface_static(ags_type_input,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_input);
}

void
ags_input_class_init(AgsInputClass *input)
{
  GObjectClass *gobject;
  AgsChannelClass *channel;
  GParamSpec *param_spec;

  ags_input_parent_class = g_type_class_peek_parent(input);
  
  gobject = (GObjectClass *) input;

  gobject->set_property = ags_input_set_property;
  gobject->get_property = ags_input_get_property;

  gobject->finalize = ags_input_finalize;
  
  /* properties */
  /**
   * AgsInput:file-link:
   *
   * The file containing audio data.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("file-link\0",
				   "file link assigned to\0",
				   "The file link to read from\0",
				   AGS_TYPE_FILE_LINK,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_LINK,
				  param_spec);

  /*  */
  channel = (AgsChannelClass *) input;
}

void
ags_input_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_input_connectable_parent_interface;

  ags_input_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_input_connect;
  connectable->disconnect = ags_input_disconnect;
}

void
ags_input_init(AgsInput *input)
{
  input->file_link = NULL;
}

void
ags_input_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsInput *input;

  input = AGS_INPUT(gobject);

  switch(prop_id){
  case PROP_FILE_LINK:
    {
      AgsAudioFile *audio_file;
      AgsFileLink *file_link;

      file_link = (AgsFileLink *) g_value_get_object(value);

      if(input->file_link != NULL){
	g_object_unref(G_OBJECT(input->file_link));
      }

      if(file_link != NULL){
	g_object_ref(G_OBJECT(file_link));
      }

      input->file_link = (GObject *) file_link;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_input_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsInput *input;

  input = AGS_INPUT(gobject);

  switch(prop_id){
  case PROP_FILE_LINK:
    g_value_set_object(value, input->file_link);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_input_finalize(GObject *gobject)
{
  AgsInput *input;

  input = AGS_INPUT(gobject);

  if(input->file_link != NULL){
    g_object_unref(G_OBJECT(input->file_link));
  }

  G_OBJECT_CLASS(ags_input_parent_class)->finalize(gobject);
}

void
ags_input_connect(AgsConnectable *connectable)
{
  ags_input_parent_connectable_interface->connect(connectable);
}

void
ags_input_disconnect(AgsConnectable *connectable)
{
  ags_input_parent_connectable_interface->disconnect(connectable);
}

/**
 * ags_input_new:
 * @audio: the #AgsAudio
 *
 * Creates a #AgsInput, linking tree to @audio.
 *
 * Returns: a new #AgsInput
 *
 * Since: 0.3
 */
AgsInput*
ags_input_new(GObject *audio)
{
  AgsInput *input;

  input = (AgsInput *) g_object_new(AGS_TYPE_INPUT,
				    "audio\0", audio,
				    NULL);

  return(input);
}
