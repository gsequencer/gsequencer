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

#include <ags/app/editor/ags_machine_selection.h>

#include <ags/app/editor/ags_machine_selector.h>
#include <ags/app/editor/ags_machine_radio_button.h>

#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_fm_synth.h>
#include <ags/app/machine/ags_syncsynth.h>
#include <ags/app/machine/ags_fm_syncsynth.h>
#include <ags/app/machine/ags_hybrid_synth.h>
#include <ags/app/machine/ags_hybrid_fm_synth.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/app/machine/ags_ffplayer.h>
#include <ags/app/machine/ags_sf2_synth.h>
#endif

#include <ags/app/machine/ags_pitch_sampler.h>
#include <ags/app/machine/ags_sfz_synth.h>

#include <ags/app/machine/ags_audiorec.h>
#include <ags/app/machine/ags_dssi_bridge.h>
#include <ags/app/machine/ags_lv2_bridge.h>
#include <ags/app/machine/ags_live_dssi_bridge.h>
#include <ags/app/machine/ags_live_lv2_bridge.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/machine/ags_vst3_bridge.h>
#include <ags/app/machine/ags_live_vst3_bridge.h>
#endif

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
 * @include: ags/app/editor/ags_machine_selection.h
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
  machine_selection->connectable_flags = 0;
  
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
ags_machine_selection_set_edit(AgsMachineSelection *machine_selection, guint edit)
{
  if(!AGS_IS_MACHINE_SELECTION(machine_selection)){
    return;
  }

  machine_selection->edit |= edit;
}

void
ags_machine_selection_load_defaults(AgsMachineSelection *machine_selection)
{
  AgsMachine *machine;
  AgsMachineRadioButton *machine_radio_button;
  GtkBox *vbox;
  GtkCheckButton *group;

  GList *start_list, *list;
  GList *start_index, *index;

  gchar *str;

  gint response;

  list =
    start_list = 
    machine_selection->machine = ags_window_get_machine(machine_selection->window);
  
  machine = NULL;

  vbox = (GtkBox *) gtk_dialog_get_content_area(GTK_DIALOG(machine_selection));
  group = NULL;

  while(list != NULL){
    GtkCheckButton *radio_button;

    gboolean success;

    success = FALSE;
    
    if((AGS_MACHINE_SELECTION_EDIT_NOTATION & (machine_selection->edit)) != 0){
      if(AGS_IS_DRUM(list->data) ||
	 AGS_IS_MATRIX(list->data)  ||
	 AGS_IS_SYNCSYNTH(list->data) ||
	 AGS_IS_FM_SYNCSYNTH(list->data) ||
	 AGS_IS_HYBRID_SYNTH(list->data) ||
	 AGS_IS_HYBRID_FM_SYNTH(list->data) ||
#ifdef AGS_WITH_LIBINSTPATCH
	 AGS_IS_FFPLAYER(list->data) ||
	 AGS_IS_SF2_SYNTH(list->data) ||
#endif
#if defined(AGS_WITH_VST3)
	 (AGS_IS_VST3_BRIDGE(list->data) && (AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(list->data)->flags)) != 0) ||
	 AGS_IS_LIVE_VST3_BRIDGE(list->data) ||	 
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
	radio_button = (GtkCheckButton *) gtk_check_button_new_with_label(str);
	      
	if(group == NULL){
	  group = radio_button;
	}

	gtk_check_button_set_group(radio_button,
				   group);
	
	g_object_set_data((GObject *) radio_button,
			  AGS_MACHINE_SELECTION_INDEX, list->data);
	gtk_box_append(vbox,
		       GTK_WIDGET(radio_button));

	g_free(str);

	success = TRUE;
      }
    }

    if(!success &&
       (AGS_MACHINE_SELECTION_EDIT_AUTOMATION & (machine_selection->edit)) != 0){
      str = g_strdup_printf("%s: %s",
			    G_OBJECT_TYPE_NAME(list->data),
			    AGS_MACHINE(list->data)->machine_name);
      radio_button = (GtkCheckButton *) gtk_check_button_new_with_label(str);
      
      if(group == NULL){
	group = radio_button;
      }

      gtk_check_button_set_group(radio_button,
				 group);

      g_object_set_data((GObject *) radio_button,
			AGS_MACHINE_SELECTION_INDEX, list->data);
      gtk_box_append(vbox,
		     GTK_WIDGET(radio_button));

      g_free(str);

      success = TRUE;
    }

    if(!success &&
       (AGS_MACHINE_SELECTION_EDIT_WAVE & (machine_selection->edit)) != 0){
      if(AGS_IS_AUDIOREC(list->data)){
	str = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(list->data),
			      AGS_MACHINE(list->data)->machine_name);
	radio_button = (GtkCheckButton *) gtk_check_button_new_with_label(str);
      
	if(group == NULL){
	  group = radio_button;
	}

	gtk_check_button_set_group(radio_button,
				   group);

	g_object_set_data((GObject *) radio_button,
			  AGS_MACHINE_SELECTION_INDEX, list->data);
	gtk_box_append(vbox,
		       GTK_WIDGET(radio_button));

	g_free(str);

	success = TRUE;
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
