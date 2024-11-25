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

#include <ags/app/machine/ags_pitch_sampler_file.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/i18n.h>

void ags_pitch_sampler_file_class_init(AgsPitchSamplerFileClass *pitch_sampler_file);
void ags_pitch_sampler_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pitch_sampler_file_init(AgsPitchSamplerFile *pitch_sampler_file);

gboolean ags_pitch_sampler_file_is_connected(AgsConnectable *connectable);
void ags_pitch_sampler_file_connect(AgsConnectable *connectable);
void ags_pitch_sampler_file_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_pitch_sampler_file
 * @short_description: pitch sampler file
 * @title: AgsPitchSamplerFile
 * @section_id:
 * @include: ags/app/machine/ags_pitch_sampler_file.h
 *
 * The #AgsPitchSamplerFile is a composite widget to act as pitch sampler file.
 */

enum{
  CONTROL_CHANGED,
  LAST_SIGNAL,
};

static gpointer ags_pitch_sampler_file_parent_class = NULL;
static guint pitch_sampler_file_signals[LAST_SIGNAL];

static AgsConnectableInterface *ags_pitch_sampler_file_parent_connectable_interface;

GType
ags_pitch_sampler_file_get_type(void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pitch_sampler_file = 0;

    static const GTypeInfo ags_pitch_sampler_file_info = {
      sizeof(AgsPitchSamplerFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pitch_sampler_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPitchSamplerFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pitch_sampler_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pitch_sampler_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_pitch_sampler_file = g_type_register_static(GTK_TYPE_BOX,
							 "AgsPitchSamplerFile",
							 &ags_pitch_sampler_file_info,
							 0);
    
    g_type_add_interface_static(ags_type_pitch_sampler_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pitch_sampler_file);
  }

  return g_define_type_id__volatile;
}

void
ags_pitch_sampler_file_class_init(AgsPitchSamplerFileClass *pitch_sampler_file)
{
  ags_pitch_sampler_file_parent_class = g_type_class_peek_parent(pitch_sampler_file);

  /* signals */
  /**
   * AgsPitchSamplerFile::control-change:
   * @pitch_sampler_file: the #AgsPitchSamplerFile
   *
   * The ::control-change signal notifies about controls modified.
   * 
   * Since: 3.0.0
   */
  pitch_sampler_file_signals[CONTROL_CHANGED] =
    g_signal_new("control-changed",
		 G_TYPE_FROM_CLASS(pitch_sampler_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPitchSamplerFileClass, control_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_pitch_sampler_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  //  ags_pitch_sampler_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_pitch_sampler_file_is_connected;  
  connectable->connect = ags_pitch_sampler_file_connect;
  connectable->disconnect = ags_pitch_sampler_file_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_pitch_sampler_file_init(AgsPitchSamplerFile *pitch_sampler_file)
{
  GtkGrid *grid;
  GtkBox *filename_hbox;
  GtkLabel *label;

  pitch_sampler_file->connectable_flags = 0;
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(pitch_sampler_file),
				 GTK_ORIENTATION_VERTICAL);
  
  grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append((GtkBox *) pitch_sampler_file,
		 (GtkWidget *) grid);

  /* filename */
  filename_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					 0);

  gtk_widget_set_valign((GtkWidget *) filename_hbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) filename_hbox,
			GTK_ALIGN_FILL);

  gtk_box_set_spacing(filename_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) filename_hbox,
		  0, 0,
		  2, 1); 

  pitch_sampler_file->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_append(filename_hbox,
		 (GtkWidget *) pitch_sampler_file->filename);

  /* frequency */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("frequency"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  pitch_sampler_file->freq = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
						       "label", "440.0",
						       NULL);

  gtk_widget_set_valign((GtkWidget *) pitch_sampler_file->freq,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pitch_sampler_file->freq,
			GTK_ALIGN_START);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) pitch_sampler_file->freq,
		  1, 1,
		  1, 1);

  /* base key */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("base key"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  pitch_sampler_file->base_key = g_object_new(GTK_TYPE_LABEL,
					      "label", "48",
					      NULL);

  gtk_widget_set_valign((GtkWidget *) pitch_sampler_file->base_key,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pitch_sampler_file->base_key,
			GTK_ALIGN_START);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) pitch_sampler_file->base_key,
		  1, 2,
		  1, 1);

  /* loop start */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("loop start"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  pitch_sampler_file->loop_start = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
							     "label", "0",
							     NULL);

  gtk_widget_set_valign((GtkWidget *) pitch_sampler_file->loop_start,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pitch_sampler_file->loop_start,
			GTK_ALIGN_START);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) pitch_sampler_file->loop_start,
		  1, 3,
		  1, 1);

  /* loop end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("loop end"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  pitch_sampler_file->loop_end = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
							   "label", "0",
							   NULL);

  gtk_widget_set_valign((GtkWidget *) pitch_sampler_file->loop_end,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pitch_sampler_file->loop_end,
			GTK_ALIGN_START);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) pitch_sampler_file->loop_end,
		  1, 4,
		  1, 1);
}

gboolean
ags_pitch_sampler_file_is_connected(AgsConnectable *connectable)
{
  AgsPitchSamplerFile *pitch_sampler_file;
  
  gboolean is_connected;
  
  pitch_sampler_file = AGS_PITCH_SAMPLER_FILE(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (pitch_sampler_file->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_pitch_sampler_file_connect(AgsConnectable *connectable)
{
  AgsPitchSamplerFile *pitch_sampler_file;
  
  pitch_sampler_file = AGS_PITCH_SAMPLER_FILE(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  pitch_sampler_file->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_pitch_sampler_file_disconnect(AgsConnectable *connectable)
{
  AgsPitchSamplerFile *pitch_sampler_file;
  
  pitch_sampler_file = AGS_PITCH_SAMPLER_FILE(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  pitch_sampler_file->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
}

/**
 * ags_pitch_sampler_file_new:
 *
 * Create a new instance of #AgsPitchSamplerFile
 *
 * Returns: the new #AgsPitchSamplerFile
 *
 * Since: 3.0.0
 */
AgsPitchSamplerFile*
ags_pitch_sampler_file_new()
{
  AgsPitchSamplerFile *pitch_sampler_file;

  pitch_sampler_file = (AgsPitchSamplerFile *) g_object_new(AGS_TYPE_PITCH_SAMPLER_FILE,
							    NULL);

  return(pitch_sampler_file);
}
