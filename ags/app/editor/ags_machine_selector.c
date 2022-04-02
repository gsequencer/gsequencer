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

#include <ags/app/editor/ags_machine_selector.h>
#include <ags/app/editor/ags_machine_selector_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_composite_editor.h>

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

void ags_machine_selector_class_init(AgsMachineSelectorClass *machine_selector);
void ags_machine_selector_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_selector_init(AgsMachineSelector *machine_selector);
void ags_machine_selector_connect(AgsConnectable *connectable);
void ags_machine_selector_disconnect(AgsConnectable *connectable);
void ags_machine_selector_show(GtkWidget *widget);

void ags_machine_selector_real_changed(AgsMachineSelector *machine_selector, AgsMachine *machine);

/**
 * SECTION:ags_machine_selector
 * @short_description: machine selector
 * @title: AgsMachineSelector
 * @section_id:
 * @include: ags/app/editor/ags_machine_selector.h
 *
 * The #AgsMachineSelector enables you make choice of an #AgsMachine.
 */

enum{
  CHANGED,
  LAST_SIGNAL,
};

static gpointer ags_machine_selector_parent_class = NULL;
static guint machine_selector_signals[LAST_SIGNAL];

GType
ags_machine_selector_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_selector = 0;

    static const GTypeInfo ags_machine_selector_info = {
      sizeof (AgsMachineSelectorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_selector_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineSelector),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_selector_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_selector_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_selector = g_type_register_static(GTK_TYPE_BOX,
						       "AgsMachineSelector", &ags_machine_selector_info,
						       0);
    
    g_type_add_interface_static(ags_type_machine_selector,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_selector);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_selector_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_selector_connect;
  connectable->disconnect = ags_machine_selector_disconnect;
}

void
ags_machine_selector_class_init(AgsMachineSelectorClass *machine_selector)
{
  ags_machine_selector_parent_class = g_type_class_peek_parent(machine_selector);

  /* AgsMachineSelectorClass */
  machine_selector->changed = ags_machine_selector_real_changed;

  /**
   * AgsMachineSelector::changed:
   * @machine_selector: the #AgsMachineSelector
   * @machine: the changed #AgsMachine
   *
   * The ::changed signal notifies changed #AgsMachine.
   *
   * Since: 3.0.0
   */
  machine_selector_signals[CHANGED] =
    g_signal_new("changed",
                 G_TYPE_FROM_CLASS(machine_selector),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineSelectorClass, changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_machine_selector_init(AgsMachineSelector *machine_selector)
{
  GtkBox *hbox;

  GMenuItem *item;

  GSimpleActionGroup *action_group;
  GSimpleAction *action;

  action_group = g_simple_action_group_new();
  gtk_widget_insert_action_group((GtkWidget *) machine_selector,
				 "machine_selector",
				 action_group);

  /* add index */
  action = g_simple_action_new("add_index",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_add_index_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* remove index */
  action = g_simple_action_new("remove_index",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_remove_index_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* link index */
  action = g_simple_action_new("link_index",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_link_index_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* reverse mapping */
  action =
    machine_selector->reverse_mapping_action = g_simple_action_new_stateful("reverse_mapping",
									    g_variant_type_new("b"),
									    g_variant_new_boolean(TRUE));
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_reverse_mapping_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - A */
  action = g_simple_action_new("shift_a",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_a_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - A# */
  action = g_simple_action_new("shift_a_sharp",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_a_sharp_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - H */
  action = g_simple_action_new("shift_h",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_h_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - C */
  action = g_simple_action_new("shift_c",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_c_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - C# */
  action = g_simple_action_new("shift_c_sharp",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_c_sharp_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - D# */
  action = g_simple_action_new("shift_d_sharp",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_d_sharp_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - D */
  action = g_simple_action_new("shift_d",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_d_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - E */
  action = g_simple_action_new("shift_e",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_e_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - F */
  action = g_simple_action_new("shift_f",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_f_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - F# */
  action = g_simple_action_new("shift_f_sharp",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_f_sharp_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - G */
  action = g_simple_action_new("shift_g",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_g_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* shift piano - G# */
  action = g_simple_action_new("shift_g_sharp",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_selector_shift_g_sharp_callback), machine_selector);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  machine_selector->flags = 0;
  machine_selector->connectable_flags = 0;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(machine_selector),
				 GTK_ORIENTATION_VERTICAL);
  
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(GTK_BOX(machine_selector),
		 GTK_WIDGET(hbox));

  machine_selector->label = (GtkLabel *) gtk_label_new(NULL);
  gtk_box_append(hbox,
		 GTK_WIDGET(machine_selector->label));

  machine_selector->current = NULL;

  machine_selector->menu_button = g_object_new(GTK_TYPE_MENU_BUTTON,
					       "icon-name", "system-run",
					       NULL);
  gtk_box_append(hbox,
		 GTK_WIDGET(machine_selector->menu_button));

  machine_selector->popup = (GMenu *) g_menu_new();
  gtk_menu_button_set_menu_model(machine_selector->menu_button,
				 machine_selector->popup);
  
  item = g_menu_item_new(i18n("add index"),
			 "machine_selector.add_index");
  g_menu_append_item(machine_selector->popup,
		     item);
  
  item = g_menu_item_new(i18n("remove index"),
			 "machine_selector.remove_index");
  g_menu_append_item(machine_selector->popup,
		     item);
  
  item = g_menu_item_new(i18n("link index"),
			 "machine_selector.link_index");
  g_menu_append_item(machine_selector->popup,
		     item);

  machine_selector->shift_piano = NULL;

  machine_selector->machine_radio_button = NULL;

  machine_selector->machine_selection = NULL;
}

void
ags_machine_selector_connect(AgsConnectable *connectable)
{
  AgsMachineSelector *machine_selector;

  GList *start_list, *list;
  
  machine_selector = AGS_MACHINE_SELECTOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_selector->connectable_flags)) != 0){
    return;
  }

  machine_selector->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  /* machine radio button */
  list =
    start_list = ags_machine_selector_get_machine_radio_button(machine_selector);
  
  list = list->next;
  
  while(list != NULL){
    g_signal_connect_after(G_OBJECT(list->data), "toggled",
			   G_CALLBACK(ags_machine_selector_radio_changed), machine_selector);

    list = list->next;
  }
  
  g_list_free(start_list);
}

void
ags_machine_selector_disconnect(AgsConnectable *connectable)
{
  AgsMachineSelector *machine_selector;

  GList *start_list, *list;
  
  machine_selector = AGS_MACHINE_SELECTOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_selector->connectable_flags)) == 0){
    return;
  }

  machine_selector->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  /* machine radio button */
  list =
    start_list = ags_machine_selector_get_machine_radio_button(machine_selector);
  
  list = list->next;
  
  while(list != NULL){
    g_object_disconnect(G_OBJECT(list->data),
			"any_signal::toggled",
			G_CALLBACK(ags_machine_selector_radio_changed),
			machine_selector,
			NULL);

    list = list->next;
  }
  
  g_list_free(start_list);
}

gboolean
ags_machine_selector_test_flags(AgsMachineSelector *machine_selector,
				guint flags)
{
  gboolean retval;

  g_return_val_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector), FALSE);

  retval = (flags & (machine_selector->flags)) ? TRUE: FALSE;

  return(retval);
}

void
ags_machine_selector_set_flags(AgsMachineSelector *machine_selector,
			       guint flags)
{
  g_return_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector));

  if((AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING & (flags)) != 0 &&
     (AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING & (machine_selector->flags)) == 0){
    GMenuItem *item;

    item = g_menu_item_new(i18n("reverse mapping"),
			   "machine_selector.reverse_mapping");
    g_menu_append_item(machine_selector->popup,
		       item);
  }

  if((AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO & (flags)) != 0 &&
     (AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO & (machine_selector->flags)) == 0){
    GMenu *menu;
    GMenuItem *item;

    item = g_menu_item_new(i18n("shift piano"),
			   "machine_selector.shift_piano");
    g_menu_append_item(machine_selector->popup,
		       item);

    menu =
      machine_selector->shift_piano = (GMenu *) g_menu_new();
    g_menu_item_set_submenu(item,
			    menu);

    item = g_menu_item_new(i18n("A"),
			   "machine_selector.shift_a");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("A#"),
			   "machine_selector.shift_a_sharp");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("H"),
			   "machine_selector.shift_h");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("C"),
			   "machine_selector.shift_c");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("C#"),
			   "machine_selector.shift_c_sharp");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("D"),
			   "machine_selector.shift_d");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("D#"),
			   "machine_selector.shift_d_sharp");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("E"),
			   "machine_selector.shift_e");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("F"),
			   "machine_selector.shift_f");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("F#"),
			   "machine_selector.shift_f_sharp");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("G"),
			   "machine_selector.shift_g");
    g_menu_append_item(menu,
		       item);

    item = g_menu_item_new(i18n("G#"),
			   "machine_selector.shift_g_sharp");
    g_menu_append_item(menu,
		       item);
  }
  
  machine_selector->flags |= flags;
}

void
ags_machine_selector_unset_flags(AgsMachineSelector *machine_selector,
				 guint flags)
{
  g_return_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector));

  if((AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING & (flags)) != 0 &&
     (AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING & (machine_selector->flags)) != 0){
    g_menu_remove(machine_selector->popup,
		  3);
    
    machine_selector->flags &= (~AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING);
  }

  if((AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO & (flags)) != 0 &&
     (AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO & (machine_selector->flags)) != 0){
    if((AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING & (machine_selector->flags)) == 0){
      g_menu_remove(machine_selector->popup,
		    3);
    }else{
      g_menu_remove(machine_selector->popup,
		    4);
    }
    
    machine_selector->flags &= (~AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO);
  }
  
  machine_selector->flags &= (~flags);
}

/**
 * ags_machine_selector_get_machine_radio_button:
 * @machine_selector: the #AgsMachineSelector
 * 
 * Get machine radio button of @machine_selector.
 * 
 * Returns: the #GList-struct containing #AgsMachineRadioButton
 *
 * Since: 4.0.0
 */
GList*
ags_machine_selector_get_machine_radio_button(AgsMachineSelector *machine_selector)
{
  g_return_val_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector), NULL);

  return(g_list_reverse(g_list_copy(machine_selector->machine_radio_button)));
}

/**
 * ags_machine_selector_add_machine_radio_button:
 * @machine_selector: the #AgsMachineSelector
 * @machine_radio_button: the #AgsMachineRadioButton
 * 
 * Add @machine_radio_button to @machine_selector.
 *
 * Since: 4.0.0
 */
void
ags_machine_selector_add_machine_radio_button(AgsMachineSelector *machine_selector,
					      AgsMachineRadioButton *machine_radio_button)
{
  g_return_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector));
  g_return_if_fail(AGS_IS_MACHINE_RADIO_BUTTON(machine_radio_button));

  if(g_list_find(machine_selector->machine_radio_button, machine_radio_button) == NULL){
    AgsMachineRadioButton *group;
    
    GList *start_list, *list;

    list =
      start_list = ags_machine_selector_get_machine_radio_button(machine_selector);
    
    machine_selector->machine_radio_button = g_list_prepend(machine_selector->machine_radio_button,
							    machine_radio_button);
  
    if(list != NULL){
      group = AGS_MACHINE_RADIO_BUTTON(list->data);

      g_object_set(machine_radio_button,
		   "group", group,
		   NULL);
    }
    
    gtk_box_append((GtkBox *) machine_selector,
		   machine_radio_button);

    g_list_free(start_list);
  }
}

/**
 * ags_machine_selector_remove_machine_radio_button:
 * @machine_selector: the #AgsMachineSelector
 * @machine_radio_button: the #AgsMachineRadioButton
 * 
 * Remove @machine_radio_button to @machine_selector.
 *
 * Since: 4.0.0
 */
void
ags_machine_selector_remove_machine_radio_button(AgsMachineSelector *machine_selector,
						 AgsMachineRadioButton *machine_radio_button)
{
  g_return_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector));
  g_return_if_fail(AGS_IS_MACHINE_RADIO_BUTTON(machine_radio_button));

  if(g_list_find(machine_selector->machine_radio_button, machine_radio_button) != NULL){
    machine_selector->machine_radio_button = g_list_remove(machine_selector->machine_radio_button,
							   machine_radio_button);
    
    gtk_box_remove((GtkBox *) machine_selector,
		   machine_radio_button);
  }
}

void
ags_machine_selector_set_edit(AgsMachineSelector *machine_selector, guint edit)
{
  if(!AGS_IS_MACHINE_SELECTOR(machine_selector)){
    return;
  }

  machine_selector->edit |= edit;
}

void
ags_machine_selector_add_index(AgsMachineSelector *machine_selector)
{
  AgsMachineRadioButton *machine_radio_button;

  g_return_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector));

  machine_radio_button = (AgsMachineRadioButton *) g_object_new(AGS_TYPE_MACHINE_RADIO_BUTTON,
								NULL);
  
  ags_machine_selector_add_machine_radio_button(machine_selector,
						machine_radio_button);
  
  g_signal_connect_after(G_OBJECT(machine_radio_button), "toggled",
			 G_CALLBACK(ags_machine_selector_radio_changed), machine_selector);

  gtk_widget_show((GtkWidget *) machine_radio_button);
}

void
ags_machine_selector_remove_index(AgsMachineSelector *machine_selector,
				  guint nth)
{
  AgsMachineRadioButton *machine_radio_button;
  
  GList *start_list, *list;

  g_return_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector));

  /* get machine radio button */
  machine_radio_button = NULL;

  list =
    start_list = ags_machine_selector_get_machine_radio_button(machine_selector);

  list = g_list_nth(list,
		    nth);
  
  if(list != NULL){
    machine_radio_button = list->data;
  }
  
  g_list_free(start_list);

  if(machine_radio_button == NULL){
    return;
  }  
  
  /*  */
  ags_machine_selector_remove_machine_radio_button(machine_selector,
						   machine_radio_button);

  g_object_run_dispose(machine_radio_button);
  g_object_unref(machine_radio_button);
}

void
ags_machine_selector_link_index(AgsMachineSelector *machine_selector,
				AgsMachine *machine)
{
  AgsMachineRadioButton *machine_radio_button, *existing_radio_button;

  GList *start_list, *list;

  g_return_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector));

  /* retrieve selected */    
  machine_radio_button = NULL;
  existing_radio_button = NULL;

  list =
    start_list = ags_machine_selector_get_machine_radio_button(machine_selector);

  while(list != NULL){
    if(GTK_IS_CHECK_BUTTON(list->data)){
      if(gtk_check_button_get_active(GTK_CHECK_BUTTON(list->data))){
	machine_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);
      }

      if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine){
	existing_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);
      }
    }
      
    list = list->next;
  }

  /* decide if link to editor or change index */
  if(existing_radio_button != NULL){
    g_signal_emit_by_name(existing_radio_button,
			  "toggled");
  }else{
    g_object_set(G_OBJECT(machine_radio_button),
		 "machine", machine,
		 NULL);
  }

  g_list_free(start_list);
}

void
ags_machine_selector_real_changed(AgsMachineSelector *machine_selector, AgsMachine *machine)
{
  AgsCompositeEditor *composite_editor;
  
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  composite_editor = gtk_widget_get_ancestor(machine_selector,
					     AGS_TYPE_COMPOSITE_EDITOR);
    
  if(AGS_IS_DRUM(machine) ||
     AGS_IS_MATRIX(machine)  ||
     AGS_IS_SYNCSYNTH(machine) ||
     AGS_IS_FM_SYNCSYNTH(machine) ||
     AGS_IS_HYBRID_SYNTH(machine) ||
     AGS_IS_HYBRID_FM_SYNTH(machine) ||
#ifdef AGS_WITH_LIBINSTPATCH
     AGS_IS_FFPLAYER(machine) ||
     AGS_IS_SF2_SYNTH(machine) ||
#endif
#if defined(AGS_WITH_VST3)
     (AGS_IS_VST3_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(machine)->flags)) != 0) ||
     AGS_IS_LIVE_VST3_BRIDGE(machine) ||	 
#endif
     AGS_IS_PITCH_SAMPLER(machine) ||
     AGS_IS_SFZ_SYNTH(machine) ||
     AGS_IS_DSSI_BRIDGE(machine) ||
     (AGS_IS_LV2_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(machine)->flags)) != 0) ||
     AGS_IS_LIVE_DSSI_BRIDGE(machine) ||
     AGS_IS_LIVE_LV2_BRIDGE(machine)){
    gtk_widget_show(composite_editor->notation_edit);

    //TODO:JK: implement me
  }else if(AGS_IS_AUDIOREC(machine)){
    //TODO:JK: implement me
  }else{
    //TODO:JK: implement me
  }
  
  if((AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING & (machine_selector->flags)) != 0){
    machine_selector->flags |= AGS_MACHINE_SELECTOR_BLOCK_REVERSE_MAPPING;

    if(machine != NULL){
      if(ags_audio_test_behaviour_flags(machine->audio,
					AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
	g_object_set(machine_selector->reverse_mapping_action,
		     "state", g_variant_new_boolean(TRUE),
		     NULL);
      }else{
	g_object_set(machine_selector->reverse_mapping_action,
		     "state", g_variant_new_boolean(FALSE),
		     NULL);
      }
    }else{
      g_object_set(machine_selector->reverse_mapping_action,
		   "state", g_variant_new_boolean(FALSE),
		   NULL);
    }

    machine_selector->flags &= (~AGS_MACHINE_SELECTOR_BLOCK_REVERSE_MAPPING);
  }
}

/**
 * ags_machine_selector_changed:
 * @machine_selector: the #AgsMachineSelector
 * @machine: the selected #AgsMachine
 *
 * Emitted as #AgsMachineSelector modified.
 *
 * Since: 3.0.0
 */
void
ags_machine_selector_changed(AgsMachineSelector *machine_selector, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector));

  g_object_ref((GObject *) machine_selector);
  g_signal_emit((GObject *) machine_selector,
		machine_selector_signals[CHANGED], 0,
		machine);
  g_object_unref((GObject *) machine_selector);
}

/**
 * ags_machine_selector_new:
 *
 * Create a new #AgsMachineSelector.
 *
 * Returns: a new #AgsMachineSelector
 *
 * Since: 3.0.0
 */
AgsMachineSelector*
ags_machine_selector_new()
{
  AgsMachineSelector *machine_selector;

  machine_selector = (AgsMachineSelector *) g_object_new(AGS_TYPE_MACHINE_SELECTOR,
							 NULL);

  return(machine_selector);
}
