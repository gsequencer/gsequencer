/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/machine/ags_matrix_bulk_input.h>
#include <ags/app/machine/ags_matrix_bulk_input_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

void ags_matrix_bulk_input_class_init(AgsMatrixBulkInputClass *matrix_bulk_input);
void ags_matrix_bulk_input_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_matrix_bulk_input_init(AgsMatrixBulkInput *matrix_bulk_input);
void ags_matrix_bulk_input_finalize(GObject *gobject);

void ags_matrix_bulk_input_connect(AgsConnectable *connectable);
void ags_matrix_bulk_input_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_matrix_bulk_input
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsMatrixBulkInput
 * @section_id:
 * @include: ags/app/machine/ags_matrix_bulk_input.h
 *
 * #AgsMatrixBulkInput is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsMatrixBulkInput.
 */

static gpointer ags_matrix_bulk_input_parent_class = NULL;
static AgsConnectableInterface *ags_matrix_bulk_input_parent_connectable_interface;

GType
ags_matrix_bulk_input_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_matrix_bulk_input = 0;
 
    static const GTypeInfo ags_matrix_bulk_input_info = {
      sizeof(AgsMatrixBulkInputClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_matrix_bulk_input_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMatrixBulkInput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_matrix_bulk_input_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_matrix_bulk_input_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_matrix_bulk_input = g_type_register_static(AGS_TYPE_EFFECT_BULK,
							"AgsMatrixBulkInput", &ags_matrix_bulk_input_info,
							0);

    g_type_add_interface_static(ags_type_matrix_bulk_input,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_matrix_bulk_input);
  }

  return g_define_type_id__static;
}

void
ags_matrix_bulk_input_class_init(AgsMatrixBulkInputClass *matrix_bulk_input)
{
  GObjectClass *gobject;

  ags_matrix_bulk_input_parent_class = g_type_class_peek_parent(matrix_bulk_input);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(matrix_bulk_input);

  gobject->finalize = ags_matrix_bulk_input_finalize;
}

void
ags_matrix_bulk_input_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_matrix_bulk_input_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_matrix_bulk_input_connect;
  connectable->disconnect = ags_matrix_bulk_input_disconnect;
}

void
ags_matrix_bulk_input_init(AgsMatrixBulkInput *matrix_bulk_input)
{
  //TODO:JK: implement me
}

void
ags_matrix_bulk_input_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_matrix_bulk_input_parent_class)->finalize(gobject);
}

void
ags_matrix_bulk_input_connect(AgsConnectable *connectable)
{
  if((AGS_CONNECTABLE_CONNECTED & (AGS_EFFECT_BULK(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_matrix_bulk_input_parent_connectable_interface->connect(connectable);

  //TODO:JK: implement me
}

void
ags_matrix_bulk_input_disconnect(AgsConnectable *connectable)
{
  if((AGS_CONNECTABLE_CONNECTED & (AGS_EFFECT_BULK(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_matrix_bulk_input_parent_connectable_interface->disconnect(connectable);

  //TODO:JK: implement me
}

/**
 * ags_matrix_bulk_input_new:
 * @audio: the #AgsAudio to visualize
 * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 *
 * Create a new instance of #AgsMatrixBulkInput
 *
 * Returns: the new #AgsMatrixBulkInput
 *
 * Since: 3.0.0
 */
AgsMatrixBulkInput*
ags_matrix_bulk_input_new(AgsAudio *audio,
			  GType channel_type)
{
  AgsMatrixBulkInput *matrix_bulk_input;

  matrix_bulk_input = (AgsMatrixBulkInput *) g_object_new(AGS_TYPE_MATRIX_BULK_INPUT,
							  "audio", audio,
							  "channel-type", channel_type,
							  NULL);

  return(matrix_bulk_input);
}
