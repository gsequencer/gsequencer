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

#include <ags/X/ags_sequencer_editor.h>
#include <ags/X/ags_sequencer_editor_callbacks.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_midiin.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/config.h>

void ags_sequencer_editor_class_init(AgsSequencerEditorClass *sequencer_editor);
void ags_sequencer_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sequencer_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_sequencer_editor_init(AgsSequencerEditor *sequencer_editor);
void ags_sequencer_editor_connect(AgsConnectable *connectable);
void ags_sequencer_editor_disconnect(AgsConnectable *connectable);
void ags_sequencer_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_sequencer_editor_apply(AgsApplicable *applicable);
void ags_sequencer_editor_reset(AgsApplicable *applicable);
static void ags_sequencer_editor_finalize(GObject *gobject);

/**
 * SECTION:ags_sequencer_editor
 * @short_description: A composite widget to configure sequencer
 * @title: AgsSequencerEditor
 * @section_id: 
 * @include: ags/X/ags_sequencer_editor.h
 *
 * #AgsSequencerEditor enables you to make preferences of sequencer, audio channels,
 * samplerate and buffer size.
 */

static gpointer ags_sequencer_editor_parent_class = NULL;

GType
ags_sequencer_editor_get_type(void)
{
  static GType ags_type_sequencer_editor = 0;

  if(!ags_type_sequencer_editor){
    static const GTypeInfo ags_sequencer_editor_info = {
      sizeof (AgsSequencerEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sequencer_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSequencerEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sequencer_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sequencer_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_sequencer_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_sequencer_editor = g_type_register_static(GTK_TYPE_VBOX,
						       "AgsSequencerEditor\0", &ags_sequencer_editor_info,
						       0);
    
    g_type_add_interface_static(ags_type_sequencer_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_sequencer_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_sequencer_editor);
}

void
ags_sequencer_editor_class_init(AgsSequencerEditorClass *sequencer_editor)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_sequencer_editor_parent_class = g_type_class_peek_parent(sequencer_editor);

  /* GtkObjectClass */
  gobject = (GObjectClass *) sequencer_editor;

  gobject->finalize = ags_sequencer_editor_finalize;
}

void
ags_sequencer_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_sequencer_editor_connect;
  connectable->disconnect = ags_sequencer_editor_disconnect;
}

void
ags_sequencer_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_sequencer_editor_set_update;
  applicable->apply = ags_sequencer_editor_apply;
  applicable->reset = ags_sequencer_editor_reset;
}

void
ags_sequencer_editor_init(AgsSequencerEditor *sequencer_editor)
{
  GtkTable *table;
  GtkLabel *label;

  sequencer_editor->flags = 0;
  
  sequencer_editor->sequencer = NULL;
  sequencer_editor->sequencer_thread = NULL;
  
  table = (GtkTable *) gtk_table_new(3, 8, FALSE);
  gtk_box_pack_start(GTK_BOX(sequencer_editor),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     2);

  /* backend */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "backend\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  sequencer_editor->backend = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(sequencer_editor->backend),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "jack\0");
  
#ifdef AGS_WITH_ALSA
  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "alsa\0");
#endif
  
  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "oss\0");
  gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			   0);
  
  /* sound card */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "sound card\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  sequencer_editor->card = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(sequencer_editor->card),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* jack */
  sequencer_editor->jack_hbox = NULL;
  sequencer_editor->add_jack = NULL;
  sequencer_editor->remove_jack = NULL;

  sequencer_editor->jack_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
							 0);
  gtk_table_attach(table,
		   GTK_WIDGET(sequencer_editor->jack_hbox),
		   2, 3,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  sequencer_editor->add_jack = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) sequencer_editor->jack_hbox,
		     (GtkWidget *) sequencer_editor->add_jack,
		     FALSE, FALSE,
		     0);
  
  sequencer_editor->remove_jack = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) sequencer_editor->jack_hbox,
		     (GtkWidget *) sequencer_editor->remove_jack,
		     FALSE, FALSE,
		     0);
  
  /*  */
  sequencer_editor->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_table_attach(table,
		   GTK_WIDGET(sequencer_editor->remove),
		   2, 3,
		   7, 8,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_sequencer_editor_connect(AgsConnectable *connectable)
{
  AgsSequencerEditor *sequencer_editor;

  sequencer_editor = AGS_SEQUENCER_EDITOR(connectable);

  if((AGS_SEQUENCER_EDITOR_CONNECTED & (sequencer_editor->flags)) != 0){
    return;
  }

  sequencer_editor->flags |= AGS_SEQUENCER_EDITOR_CONNECTED;
  
  /* backend and card */
  g_signal_connect(G_OBJECT(sequencer_editor->backend), "changed\0",
		   G_CALLBACK(ags_sequencer_editor_backend_changed_callback), sequencer_editor);

  g_signal_connect(G_OBJECT(sequencer_editor->card), "changed\0",
		   G_CALLBACK(ags_sequencer_editor_card_changed_callback), sequencer_editor);

  /* add / remove jack */
  g_signal_connect(G_OBJECT(sequencer_editor->add_jack), "clicked\0",
		   G_CALLBACK(ags_sequencer_editor_add_jack_callback), sequencer_editor);

  g_signal_connect(G_OBJECT(sequencer_editor->remove_jack), "clicked\0",
		   G_CALLBACK(ags_sequencer_editor_remove_jack_callback), sequencer_editor);
}

void
ags_sequencer_editor_disconnect(AgsConnectable *connectable)
{
  AgsSequencerEditor *sequencer_editor;

  sequencer_editor = AGS_SEQUENCER_EDITOR(connectable);

  if((AGS_SEQUENCER_EDITOR_CONNECTED & (sequencer_editor->flags)) == 0){
    return;
  }

  sequencer_editor->flags &= (~AGS_SEQUENCER_EDITOR_CONNECTED);

  /* backend and card */
  g_object_disconnect(G_OBJECT(sequencer_editor->backend),
		      "changed\0",
		      G_CALLBACK(ags_sequencer_editor_backend_changed_callback),
		      sequencer_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(sequencer_editor->card),
		      "changed\0",
		      G_CALLBACK(ags_sequencer_editor_card_changed_callback),
		      sequencer_editor,
		      NULL);

  /* add / remove jack */
  g_object_disconnect(G_OBJECT(sequencer_editor->add_jack),
		      "clicked\0",
		      G_CALLBACK(ags_sequencer_editor_add_jack_callback),
		      sequencer_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(sequencer_editor->remove_jack),
		      "clicked\0",
		      G_CALLBACK(ags_sequencer_editor_remove_jack_callback),
		      sequencer_editor,
		      NULL);
}

static void
ags_sequencer_editor_finalize(GObject *gobject)
{
  //TODO:JK: implement me
  
  G_OBJECT_CLASS(ags_sequencer_editor_parent_class)->finalize(gobject);
}

void
ags_sequencer_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_add_jack(AgsSequencerEditor *sequencer_editor,
			      gchar *device)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_remove_jack(AgsSequencerEditor *sequencer_editor,
				 gchar *device)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_add_sequencer(AgsSequencerEditor *sequencer_editor,
				   GObject *sequencer)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_remove_sequencer(AgsSequencerEditor *sequencer_editor,
				      GObject *sequencer)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_load_jack_card(AgsSequencerEditor *sequencer_editor)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_load_alsa_card(AgsSequencerEditor *sequencer_editor)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_load_oss_card(AgsSequencerEditor *sequencer_editor)
{
  //TODO:JK: implement me
}

/**
 * ags_sequencer_editor_new:
 *
 * Creates an #AgsSequencerEditor
 *
 * Returns: a new #AgsSequencerEditor
 *
 * Since: 0.7.65
 */
AgsSequencerEditor*
ags_sequencer_editor_new()
{
  AgsSequencerEditor *sequencer_editor;

  sequencer_editor = (AgsSequencerEditor *) g_object_new(AGS_TYPE_SEQUENCER_EDITOR,
							 NULL);
  
  return(sequencer_editor);
}
