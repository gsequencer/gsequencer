/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/editor/ags_wave_meta.h>
#include <ags/X/editor/ags_wave_meta_callbacks.h>

#include <ags/i18n.h>

void ags_wave_meta_class_init(AgsWaveMetaClass *wave_meta);
void ags_wave_meta_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wave_meta_init(AgsWaveMeta *wave_meta);

void ags_wave_meta_connect(AgsConnectable *connectable);
void ags_wave_meta_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_wave_meta
 * @short_description: wave_meta
 * @title: AgsWaveMeta
 * @section_id:
 * @include: ags/X/editor/ags_wave_meta.h
 *
 * The #AgsWaveMeta provides you information about wave editor.
 */

GType
ags_wave_meta_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wave_meta = 0;

    static const GTypeInfo ags_wave_meta_info = {
      sizeof (AgsWaveMetaClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wave_meta_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWaveMeta),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wave_meta_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_wave_meta_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_wave_meta = g_type_register_static(GTK_TYPE_VBOX,
						"AgsWaveMeta", &ags_wave_meta_info,
						0);
    
    g_type_add_interface_static(ags_type_wave_meta,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wave_meta);
  }

  return g_define_type_id__volatile;
}

void
ags_wave_meta_class_init(AgsWaveMetaClass *wave_meta)
{
  /* empty */
}

void
ags_wave_meta_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_wave_meta_connect;
  connectable->disconnect = ags_wave_meta_disconnect;
}

void
ags_wave_meta_init(AgsWaveMeta *wave_meta)
{
  GtkGrid *grid;
  GtkLabel *label;

  guint i;
  
  wave_meta->flags = 0;

  grid = gtk_grid_new();
  gtk_box_pack_start((GtkBox *) wave_meta,
		     (GtkWidget *) grid,
		     FALSE,
		     TRUE,
		     0);

  /* machine type */
  i = 0;
  
  label = (GtkLabel *) gtk_label_new(i18n("machine type"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  wave_meta->machine_type = (GtkLabel *) gtk_label_new("(null)");
  gtk_grid_attach(grid,
		  (GtkWidget *) wave_meta->machine_type,
		  1, i,
		  1, 1);
  
  /* machine name */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("machine name"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  wave_meta->machine_name = (GtkLabel *) gtk_label_new("(null)");
  gtk_grid_attach(grid,
		  (GtkWidget *) wave_meta->machine_name,
		  1, i,
		  1, 1);

  /* audio channels */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("audio channels"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  wave_meta->audio_channels = (GtkLabel *) gtk_label_new("-1");
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  1, i,
		  1, 1);

  /* output pads */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("output pads"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  wave_meta->output_pads = (GtkLabel *) gtk_label_new("-1");
  gtk_grid_attach(grid,
		  (GtkWidget *) wave_meta->output_pads,
		  1, i,
		  1, 1);

  /* input pads */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("input pads"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  wave_meta->input_pads = (GtkLabel *) gtk_label_new("-1");
  gtk_grid_attach(grid,
		  (GtkWidget *) wave_meta->input_pads,
		  1, i,
		  1, 1);

  /* editor tool */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("editor tool"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  wave_meta->editor_tool = (GtkLabel *) gtk_label_new("(null)");
  gtk_grid_attach(grid,
		  (GtkWidget *) wave_meta->editor_tool,
		  1, i,
		  1, 1);

  /* active audio channel */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("active audio channel"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  wave_meta->active_audio_channel = (GtkLabel *) gtk_label_new("(null)");
  gtk_grid_attach(grid,
		  (GtkWidget *) wave_meta->active_audio_channel,
		  1, i,
		  1, 1);

  /* cursor x-position */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("cursor x-position"));
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  wave_meta->cursor_x_position = (GtkLabel *) gtk_label_new("-1");
  gtk_grid_attach(grid,
		  (GtkWidget *) wave_meta->cursor_x_position,
		  1, i,
		  1, 1);
}

void
ags_wave_meta_connect(AgsConnectable *connectable)
{
  AgsWaveMeta *wave_meta;
  
  wave_meta = AGS_WAVE_META(connectable);

  if((AGS_WAVE_META_CONNECTED & (wave_meta->flags)) != 0){
    return;
  }

  wave_meta->flags |= AGS_WAVE_META_CONNECTED;

  //TODO:JK: implement me
}

void
ags_wave_meta_disconnect(AgsConnectable *connectable)
{
  AgsWaveMeta *wave_meta;

  GList *list;
  
  wave_meta = AGS_WAVE_META(connectable);

  if((AGS_WAVE_META_CONNECTED & (wave_meta->flags)) == 0){
    return;
  }

  wave_meta->flags &= (~AGS_WAVE_META_CONNECTED);

  //TODO:JK: implement me
}

/**
 * ags_wave_meta_new:
 *
 * Create a new #AgsWaveMeta.
 *
 * Returns: a new #AgsWaveMeta
 *
 * Since: 3.1.0
 */
AgsWaveMeta*
ags_wave_meta_new()
{
  AgsWaveMeta *wave_meta;

  wave_meta = (AgsWaveMeta *) g_object_new(AGS_TYPE_WAVE_META,
					   NULL);

  return(wave_meta);
}
