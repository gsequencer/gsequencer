/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/editor/ags_machine_selection.h>

#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_machine_radio_button.h>

#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_fm_synth.h>
#include <ags/X/machine/ags_syncsynth.h>
#include <ags/X/machine/ags_fm_syncsynth.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/X/machine/ags_ffplayer.h>
#include <ags/X/machine/ags_sf2_synth.h>
#endif

#include <ags/X/machine/ags_pitch_sampler.h>
#include <ags/X/machine/ags_sfz_synth.h>

#include <ags/X/machine/ags_audiorec.h>
#include <ags/X/machine/ags_dssi_bridge.h>
#include <ags/X/machine/ags_lv2_bridge.h>
#include <ags/X/machine/ags_live_dssi_bridge.h>
#include <ags/X/machine/ags_live_lv2_bridge.h>

#include <ags/i18n.h>

void ags_machine_selection_class_init(AgsMachineSelectionClass *machine_selection);
void ags_machine_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_selection_init(AgsMachineSelection *machine_selection);
void ags_machine_selection_connect(AgsConnectable *connectable);
void ags_machine_selection_disconnect(AgsConnectable *connectable);
void ags_machine_selection_finalize(GObject *gobject);

/**
 * SECTION:ags_machine_selection
 * @short_description: machine selection
 * @title: AgsMachineSelection
 * @section_id:
 * @include: ags/X/editor/ags_machine_selection.h
 *
 * The #AgsMachineSelection enables you make choice of an #AgsMachine.
 */

static gpointer ags_machine_selection_parent_class = NULL;

GType
ags_machine_selection_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_selection = 0;

    static const GTypeInfo ags_machine_selection_info = {
      sizeof (AgsMachineSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_selection = g_type_register_static(GTK_TYPE_DIALOG,
							"AgsMachineSelection", &ags_machine_selection_info,
							0);
    
    g_type_add_interface_static(ags_type_machine_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_selection);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_selection_connect;
  connectable->disconnect = ags_machine_selection_disconnect;
}

void
ags_machine_selection_class_init(AgsMachineSelectionClass *machine_selection)
{
  GObjectClass *gobject;

  ags_machine_selection_parent_class = g_type_class_peek_parent(machine_selection);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_selection;

  gobject->finalize = ags_machine_selection_finalize;
}

void
ags_machine_selection_init(AgsMachineSelection *machine_selection)
{
  machine_selection->flags = 0;
  
  g_object_set(G_OBJECT(machine_selection),
	       "modal", TRUE,
	       "title", i18n("select machines"),
	       NULL);

  gtk_dialog_add_buttons(GTK_DIALOG(machine_selection),
			 i18n("_OK"), GTK_RESPONSE_ACCEPT,
			 i18n("_Cancel"), GTK_RESPONSE_REJECT,
			 NULL);
}

void
ags_machine_selection_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_machine_selection_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_machine_selection_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_machine_selection_parent_class)->finalize(gobject);
}

void
ags_machine_selection_load_defaults(AgsMachineSelection *machine_selection)
{
  AgsMachine *machine;
  AgsMachineRadioButton *machine_radio_button;
  GtkVBox *vbox;
  GtkRadioButton *group;

  GList *list, *list_start, *index, *index_start;

  gchar *str;

  gint response;

  machine_selection->machine =
    list = gtk_container_get_children(GTK_CONTAINER(machine_selection->window->machines));
  machine = NULL;

  vbox = (GtkVBox *) gtk_dialog_get_content_area(GTK_DIALOG(machine_selection));
  group = NULL;

  while(list != NULL){
    GtkRadioButton *radio_button;

    if((AGS_MACHINE_SELECTION_NOTATION & (machine_selection->flags)) != 0){
      if(AGS_IS_DRUM(list->data) ||
	 AGS_IS_MATRIX(list->data)  ||
	 AGS_IS_SYNCSYNTH(list->data) ||
	 AGS_IS_FM_SYNCSYNTH(list->data) ||
#ifdef AGS_WITH_LIBINSTPATCH
	 AGS_IS_FFPLAYER(list->data) ||
	 AGS_IS_SF2_SYNTH(list->data) ||
#endif
	 AGS_IS_PITCH_SAMPLER(list->data) ||
	 AGS_IS_SFZ_SYNTH(list->data) ||
	 AGS_IS_DSSI_BRIDGE(list->data) ||
	 (AGS_IS_LV2_BRIDGE(list->data) && (AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(list->data)->flags)) != 0) ||
	 AGS_IS_LIVE_DSSI_BRIDGE(list->data) ||
	 AGS_IS_LIVE_LV2_BRIDGE(list->data)){
	str = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(list->data),
			      AGS_MACHINE(list->data)->machine_name);
	radio_button = (GtkRadioButton *) gtk_radio_button_new_with_label_from_widget(group,
										      str);
	g_object_set_data((GObject *) radio_button,
			  AGS_MACHINE_SELECTION_INDEX, list->data);
	gtk_box_pack_start(GTK_BOX(vbox),
			   GTK_WIDGET(radio_button),
			   FALSE, FALSE,
			   0);

	g_free(str);
	      
	if(group == NULL){
	  group = radio_button;
	}
      }
    }else if((AGS_MACHINE_SELECTION_AUTOMATION & (machine_selection->flags)) != 0){
      str = g_strdup_printf("%s: %s",
			    G_OBJECT_TYPE_NAME(list->data),
			    AGS_MACHINE(list->data)->machine_name);
      radio_button = (GtkRadioButton *) gtk_radio_button_new_with_label_from_widget(group,
										    str);
      g_object_set_data((GObject *) radio_button,
			AGS_MACHINE_SELECTION_INDEX, list->data);
      gtk_box_pack_start(GTK_BOX(vbox),
			 GTK_WIDGET(radio_button),
			 FALSE, FALSE,
			 0);

      g_free(str);
      
      if(group == NULL){
	group = radio_button;
      }
    }else if((AGS_MACHINE_SELECTION_WAVE & (machine_selection->flags)) != 0){
      if(AGS_IS_AUDIOREC(list->data)){
	str = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(list->data),
			      AGS_MACHINE(list->data)->machine_name);
	radio_button = (GtkRadioButton *) gtk_radio_button_new_with_label_from_widget(group,
										      str);
	g_object_set_data((GObject *) radio_button,
			  AGS_MACHINE_SELECTION_INDEX, list->data);
	gtk_box_pack_start(GTK_BOX(vbox),
			   GTK_WIDGET(radio_button),
			   FALSE, FALSE,
			   0);

	g_free(str);
      
	if(group == NULL){
	  group = radio_button;
	}
      }
    }
    
    list = list->next;
  }
}

/**
 * ags_machine_selection_new:
 * @window: the #AgsWindow
 *
 * Create a new #AgsMachineSelection.
 *
 * Returns: a new #AgsMachineSelection
 *
 * Since: 3.0.0
 */
AgsMachineSelection*
ags_machine_selection_new(AgsWindow *window)
{
  AgsMachineSelection *machine_selection;

  machine_selection = (AgsMachineSelection *) g_object_new(AGS_TYPE_MACHINE_SELECTION,
							   NULL);
  machine_selection->window = window;

  return(machine_selection);
}
