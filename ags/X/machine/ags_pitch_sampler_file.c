/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_pitch_sampler_file.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>

#include <ags/i18n.h>

void ags_pitch_sampler_file_class_init(AgsPitchSamplerFileClass *pitch_sampler_file);
void ags_pitch_sampler_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pitch_sampler_file_init(AgsPitchSamplerFile *pitch_sampler_file);

void ags_pitch_sampler_file_connect(AgsConnectable *connectable);
void ags_pitch_sampler_file_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_pitch_sampler_file
 * @short_description: pitch sampler file
 * @title: AgsPitchSamplerFile
 * @section_id:
 * @include: ags/X/machine/ags_pitch_sampler_file.h
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
  static volatile gsize g_define_type_id__volatile = 0;

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
    
    ags_type_pitch_sampler_file = g_type_register_static(GTK_TYPE_VBOX,
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
  GParamSpec *param_spec;

  ags_pitch_sampler_file_parent_class = g_type_class_peek_parent(pitch_sampler_file);

  /* signals */
  /**
   * AgsPitchSamplerFile::control-change:
   * @pitch_sampler_file: the #AgsPitchSamplerFile
   *
   * The ::control-change signal notifies about controls modified.
   * 
   * Since: 2.3.0
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
  AgsConnectableInterface *ags_pitch_sampler_file_connectable_parent_interface;

  ags_pitch_sampler_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_pitch_sampler_file_connect;
  connectable->disconnect = ags_pitch_sampler_file_disconnect;
}

void
ags_pitch_sampler_file_init(AgsPitchSamplerFile *pitch_sampler_file)
{
  GtkTable *table;
  GtkHBox *filename_hbox;
  GtkLabel *label;
  
  table = (GtkTable *) gtk_table_new(2, 4,
				     FALSE);
  gtk_box_pack_start((GtkBox *) pitch_sampler_file,
		     (GtkWidget *) table,
		     FALSE, FALSE,
		     0);

  /* filename */
  filename_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
					   0);
  gtk_table_attach(table,
		   (GtkWidget *) filename_hbox,
		   0, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0); 

  pitch_sampler_file->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start((GtkBox *) filename_hbox,
		     (GtkWidget *) pitch_sampler_file->filename,
		     FALSE, FALSE,
		     0);

  pitch_sampler_file->open = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OPEN);
  gtk_box_pack_start((GtkBox *) filename_hbox,
		     (GtkWidget *) pitch_sampler_file->open,
		     FALSE, FALSE,
		     0);

  /*  */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("frequency"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pitch_sampler_file->freq = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
						       "label", i18n("440.0"),
						       "xalign", 0.0,
						       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(pitch_sampler_file->freq),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("base key"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pitch_sampler_file->base_key = (GtkSpinButton *) gtk_spin_button_new_with_range(AGS_PITCH_SAMPLER_FILE_BASE_KEY_MIN, AGS_PITCH_SAMPLER_FILE_BASE_KEY_MAX, 0.01);
  pitch_sampler_file->base_key->adjustment->value = 0.0;
  gtk_table_attach(table,
		   GTK_WIDGET(pitch_sampler_file->base_key),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* loop start */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("loop start"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pitch_sampler_file->loop_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, AGS_PITCH_SAMPLER_FILE_DEFAULT_FRAME_COUNT, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pitch_sampler_file->loop_start),
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* loop end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("loop end"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pitch_sampler_file->loop_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, AGS_PITCH_SAMPLER_FILE_DEFAULT_FRAME_COUNT, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pitch_sampler_file->loop_end),
		   1, 2,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_pitch_sampler_file_connect(AgsConnectable *connectable)
{
  AgsPitchSamplerFile *pitch_sampler_file;

  guint i;
  
  pitch_sampler_file = AGS_PITCH_SAMPLER_FILE(connectable);

  if((AGS_PITCH_SAMPLER_FILE_CONNECTED & (pitch_sampler_file->flags)) != 0){
    return;
  }

  pitch_sampler_file->flags |= AGS_PITCH_SAMPLER_FILE_CONNECTED;
}

void
ags_pitch_sampler_file_disconnect(AgsConnectable *connectable)
{
  AgsPitchSamplerFile *pitch_sampler_file;

  guint i;
  
  pitch_sampler_file = AGS_PITCH_SAMPLER_FILE(connectable);

  if((AGS_PITCH_SAMPLER_FILE_CONNECTED & (pitch_sampler_file->flags)) == 0){
    return;
  }

  pitch_sampler_file->flags &= (~AGS_PITCH_SAMPLER_FILE_CONNECTED);
}

/**
 * ags_pitch_sampler_file_new:
 *
 * Create a new instance of #AgsPitchSamplerFile
 *
 * Returns: the new #AgsPitchSamplerFile
 *
 * Since: 2.3.0
 */
AgsPitchSamplerFile*
ags_pitch_sampler_file_new()
{
  AgsPitchSamplerFile *pitch_sampler_file;

  pitch_sampler_file = (AgsPitchSamplerFile *) g_object_new(AGS_TYPE_PITCH_SAMPLER_FILE,
							    NULL);

  return(pitch_sampler_file);
}
