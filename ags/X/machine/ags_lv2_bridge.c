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

#include <ags/X/machine/ags_lv2_bridge.h>
#include <ags/X/machine/ags_lv2_bridge_callbacks.h>

#include <lv2/lv2plug.in/ns/lv2ext/lv2_programs.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

#include <ags/i18n.h>

void ags_lv2_bridge_class_init(AgsLv2BridgeClass *lv2_bridge);
void ags_lv2_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_bridge_init(AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_bridge_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_bridge_finalize(GObject *gobject);

void ags_lv2_bridge_connect(AgsConnectable *connectable);
void ags_lv2_bridge_disconnect(AgsConnectable *connectable);

void ags_lv2_bridge_resize_audio_channels(AgsMachine *machine,
				       guint audio_channels, guint audio_channels_old,
				       gpointer data);
void ags_lv2_bridge_resize_pads(AgsMachine *machine, GType channel_type,
				guint pads, guint pads_old,
				gpointer data);

void ags_lv2_bridge_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_lv2_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsLv2Bridge
 * @section_id:
 * @include: ags/X/ags_lv2_bridge.h
 *
 * #AgsLv2Bridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_URI,
  PROP_INDEX,
  PROP_HAS_MIDI,
  PROP_HAS_GUI,
  PROP_GUI_FILENAME,
  PROP_GUI_URI,
};

static gpointer ags_lv2_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_lv2_bridge_parent_connectable_interface;

GHashTable *ags_lv2_bridge_lv2ui_handle = NULL;
GHashTable *ags_lv2_bridge_lv2ui_idle = NULL;

GType
ags_lv2_bridge_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_bridge = 0;

    static const GTypeInfo ags_lv2_bridge_info = {
      sizeof(AgsLv2BridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLv2Bridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_lv2_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						 "AgsLv2Bridge", &ags_lv2_bridge_info,
						 0);

    g_type_add_interface_static(ags_type_lv2_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_bridge);
  }

  return g_define_type_id__volatile;
}

void
ags_lv2_bridge_class_init(AgsLv2BridgeClass *lv2_bridge)
{
  AgsMachineClass *machine;
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_lv2_bridge_parent_class = g_type_class_peek_parent(lv2_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(lv2_bridge);

  gobject->set_property = ags_lv2_bridge_set_property;
  gobject->get_property = ags_lv2_bridge_get_property;

  gobject->finalize = ags_lv2_bridge_finalize;
  
  /* properties */
  /**
   * AgsRecallLv2:filename:
   *
   * The plugin's filename.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_string("filename",
				    i18n_pspec("the object file"),
				    i18n_pspec("The filename as string of object file"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);
  
  /**
   * AgsRecallLv2:effect:
   *
   * The effect's name.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_string("effect",
				    i18n_pspec("the effect"),
				    i18n_pspec("The effect's string representation"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsRecallLv2:uri:
   *
   * The uri's name.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_string("uri",
				    i18n_pspec("the uri"),
				    i18n_pspec("The uri's string representation"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URI,
				  param_spec);

  /**
   * AgsRecallLv2:index:
   *
   * The uri's index.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_ulong("index",
				   i18n_pspec("index of uri"),
				   i18n_pspec("The numerical index of uri"),
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INDEX,
				  param_spec);

  /**
   * AgsRecallLv2:has-midi:
   *
   * If has-midi is set to %TRUE appropriate flag is set
   * to audio in order to become a sequencer.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_boolean("has-midi",
				     i18n_pspec("has-midi"),
				     i18n_pspec("If effect has-midi"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HAS_MIDI,
				  param_spec);

  /**
   * AgsRecallLv2:has-gui:
   *
   * If has-gui is set to %TRUE 128 inputs are allocated and appropriate flag is set
   * to audio in order to become a sequencer.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_boolean("has-gui",
				     i18n_pspec("has-gui"),
				     i18n_pspec("If effect has-gui"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HAS_GUI,
				  param_spec);

  /**
   * AgsRecallLv2:gui-filename:
   *
   * The plugin's GUI filename.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_string("gui-filename",
				    i18n_pspec("the GUI object file"),
				    i18n_pspec("The filename as string of GUI object file"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GUI_FILENAME,
				  param_spec);

  /**
   * AgsRecallLv2:gui-uri:
   *
   * The GUI's uri name.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_string("gui-uri",
				    i18n_pspec("the gui-uri"),
				    i18n_pspec("The gui-uri's string representation"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GUI_URI,
				  param_spec);

  /* AgsMachine */
  machine = (AgsMachineClass *) lv2_bridge;

  machine->map_recall = ags_lv2_bridge_map_recall;
}

void
ags_lv2_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_lv2_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_lv2_bridge_connect;
  connectable->disconnect = ags_lv2_bridge_disconnect;
}

void
ags_lv2_bridge_init(AgsLv2Bridge *lv2_bridge)
{
  GtkTable *table;
  GtkImageMenuItem *item;

  AgsAudio *audio;

  g_signal_connect_after((GObject *) lv2_bridge, "parent-set",
			 G_CALLBACK(ags_lv2_bridge_parent_set_callback), (gpointer) lv2_bridge);

  if(ags_lv2_bridge_lv2ui_handle == NULL){
    ags_lv2_bridge_lv2ui_handle = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						      NULL,
						      NULL);
  }

  if(ags_lv2_bridge_lv2ui_idle == NULL){
    ags_lv2_bridge_lv2ui_idle = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						      NULL,
						      NULL);
  }

  audio = AGS_MACHINE(lv2_bridge)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC));
  g_object_set(audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       NULL);

  g_signal_connect_after(G_OBJECT(lv2_bridge), "resize-audio-channels",
			 G_CALLBACK(ags_lv2_bridge_resize_audio_channels), NULL);
  
  g_signal_connect_after(G_OBJECT(lv2_bridge), "resize-pads",
			 G_CALLBACK(ags_lv2_bridge_resize_pads), NULL);
  
  lv2_bridge->flags = 0;

  lv2_bridge->name = NULL;

  lv2_bridge->version = AGS_LV2_BRIDGE_DEFAULT_VERSION;
  lv2_bridge->build_id = AGS_LV2_BRIDGE_DEFAULT_BUILD_ID;

  lv2_bridge->xml_type = "ags-lv2-bridge";
  
  lv2_bridge->mapped_output_pad = 0;
  lv2_bridge->mapped_input_pad = 0;

  lv2_bridge->lv2_play_container = ags_recall_container_new();
  lv2_bridge->lv2_recall_container = ags_recall_container_new();

  lv2_bridge->envelope_play_container = ags_recall_container_new();
  lv2_bridge->envelope_recall_container = ags_recall_container_new();

  lv2_bridge->buffer_play_container = ags_recall_container_new();
  lv2_bridge->buffer_recall_container = ags_recall_container_new();
  
  lv2_bridge->filename = NULL;
  lv2_bridge->effect = NULL;
  lv2_bridge->uri = NULL;
  lv2_bridge->uri_index = 0;

  lv2_bridge->has_midi = FALSE;

  lv2_bridge->lv2_plugin = NULL;
  
  lv2_bridge->lv2_descriptor = NULL;
  lv2_bridge->lv2_handle = NULL;
  lv2_bridge->port_value = NULL;

  lv2_bridge->has_gui = FALSE;
  lv2_bridge->gui_filename = NULL;
  lv2_bridge->gui_uri = NULL;

  lv2_bridge->lv2ui_plugin = NULL;
  
  lv2_bridge->ui_descriptor = NULL;

  lv2_bridge->ui_feature = (LV2_Feature **) malloc(3 * sizeof(LV2_Feature *));

  lv2_bridge->ui_feature[0] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  lv2_bridge->ui_feature[0]->data = NULL;
  
  lv2_bridge->ui_feature[1] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  lv2_bridge->ui_feature[1]->data = NULL;

  lv2_bridge->ui_feature[2] = NULL;
  
  lv2_bridge->ui_handle = NULL;

  /**/
  lv2_bridge->vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer *) gtk_bin_get_child((GtkBin *) lv2_bridge),
		    (GtkWidget *) lv2_bridge->vbox);
  
  lv2_bridge->program = NULL;
  lv2_bridge->preset = NULL;
  
  /* effect bridge */  
  AGS_MACHINE(lv2_bridge)->bridge = (GtkContainer *) ags_effect_bridge_new(audio);
  gtk_box_pack_start((GtkBox *) lv2_bridge->vbox,
		     (GtkWidget *) AGS_MACHINE(lv2_bridge)->bridge,
		     FALSE, FALSE,
		     0);

  table = (GtkTable *) gtk_table_new(1, 2, FALSE);
  gtk_box_pack_start((GtkBox *) AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge),
		     (GtkWidget *) table,
		     FALSE, FALSE,
		     0);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
												     AGS_TYPE_INPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
											     AGS_EFFECT_BULK_HIDE_ENTRIES |
											     AGS_EFFECT_BULK_SHOW_LABELS);
  gtk_table_attach(table,
		   (GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* lv2 menu */
  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Lv2");
  gtk_menu_shell_append((GtkMenuShell *) AGS_MACHINE(lv2_bridge)->popup,
			(GtkWidget *) item);
  gtk_widget_show((GtkWidget *) item);
  
  lv2_bridge->lv2_menu = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem *) item,
			    (GtkWidget *) lv2_bridge->lv2_menu);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label(i18n("show GUI"));
  gtk_menu_shell_append((GtkMenuShell *) lv2_bridge->lv2_menu,
			(GtkWidget *) item);

  gtk_widget_show_all((GtkWidget *) lv2_bridge->lv2_menu);

  /* plugin widget */
  lv2_bridge->lv2_gui = NULL;
  lv2_bridge->ui_widget = NULL;

  lv2_bridge->lv2_window = NULL;
}

void
ags_lv2_bridge_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Bridge *lv2_bridge;

  lv2_bridge = AGS_LV2_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == lv2_bridge->filename){
	return;
      }

      if(lv2_bridge->filename != NULL){
	g_free(lv2_bridge->filename);
      }

      if(filename != NULL){
	if(!g_file_test(filename,
			G_FILE_TEST_EXISTS)){
	  AgsWindow *window;

	  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) lv2_bridge);

	  ags_window_show_error(window,
				g_strdup_printf("Plugin file not present %s",
						filename));
	}
      }

      lv2_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == lv2_bridge->effect){
	return;
      }

      if(lv2_bridge->effect != NULL){
	g_free(lv2_bridge->effect);
      }

      lv2_bridge->effect = g_strdup(effect);
    }
    break;
  case PROP_URI:
    {
      gchar *uri;
      
      uri = g_value_get_string(value);

      if(uri == lv2_bridge->uri){
	return;
      }

      if(lv2_bridge->uri != NULL){
	g_free(lv2_bridge->uri);
      }

      lv2_bridge->uri = g_strdup(uri);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long uri_index;
      
      uri_index = g_value_get_ulong(value);

      if(uri_index == lv2_bridge->uri_index){
	return;
      }

      lv2_bridge->uri_index = uri_index;
    }
    break;
  case PROP_HAS_MIDI:
    {
      gboolean has_midi;

      has_midi = g_value_get_boolean(value);

      if(lv2_bridge->has_midi == has_midi){
	return;
      }

      lv2_bridge->has_midi = has_midi;
    }
    break;
  case PROP_HAS_GUI:
    {
      GtkWindow *window;
      gboolean has_gui;

      has_gui = g_value_get_boolean(value);

      if(lv2_bridge->has_gui == has_gui){
	return;
      }

      lv2_bridge->has_gui = has_gui;
    }
    break;
  case PROP_GUI_FILENAME:
    {
      GtkWindow *window;
      
      gchar *gui_filename;

      gui_filename = g_value_get_string(value);

      if(lv2_bridge->gui_filename == gui_filename){
	return;
      }

      lv2_bridge->gui_filename = g_strdup(gui_filename);
    }
    break;
  case PROP_GUI_URI:
    {
      gchar *gui_uri;
      
      gui_uri = g_value_get_string(value);

      if(gui_uri == lv2_bridge->gui_uri){
	return;
      }

      if(lv2_bridge->gui_uri != NULL){
	g_free(lv2_bridge->gui_uri);
      }

      lv2_bridge->gui_uri = g_strdup(gui_uri);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_bridge_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Bridge *lv2_bridge;

  lv2_bridge = AGS_LV2_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, lv2_bridge->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, lv2_bridge->effect);
    }
    break;
  case PROP_URI:
    {
      g_value_set_string(value, lv2_bridge->uri);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_ulong(value, lv2_bridge->uri_index);
    }
    break;
  case PROP_HAS_MIDI:
    {
      g_value_set_boolean(value, lv2_bridge->has_midi);
    }
    break;
  case PROP_HAS_GUI:
    {
      g_value_set_boolean(value, lv2_bridge->has_gui);
    }
    break;
  case PROP_GUI_FILENAME:
    {
      g_value_set_string(value, lv2_bridge->gui_filename);
    }
    break;
  case PROP_GUI_URI:
    {
      g_value_set_string(value, lv2_bridge->gui_uri);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_bridge_finalize(GObject *gobject)
{
  AgsLv2Bridge *lv2_bridge;

  lv2_bridge = AGS_LV2_BRIDGE(gobject);

  g_object_disconnect(G_OBJECT(lv2_bridge),
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_lv2_bridge_resize_audio_channels),
		      NULL,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_lv2_bridge_resize_pads),
		      NULL,
		      NULL);

  /* lv2 plugin */
  if(lv2_bridge->lv2_plugin != NULL){
    g_object_unref(lv2_bridge->lv2_plugin);
  }

  /* lv2ui plugin */
  if(lv2_bridge->lv2ui_plugin != NULL){
    g_object_unref(lv2_bridge->lv2ui_plugin);
  }

  /* plugin widget */
  if(lv2_bridge->ui_handle != NULL){
    lv2_bridge->ui_descriptor->cleanup(lv2_bridge->ui_handle[0]);
    
    g_hash_table_remove(ags_lv2_bridge_lv2ui_handle,
			lv2_bridge->ui_handle);

    g_hash_table_remove(ags_lv2_bridge_lv2ui_idle,
			lv2_bridge->ui_handle);
  }

  if(lv2_bridge->lv2_window != NULL){
    gtk_widget_destroy(lv2_bridge->lv2_window);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_bridge_parent_class)->finalize(gobject);
}

void
ags_lv2_bridge_connect(AgsConnectable *connectable)
{
  AgsLv2Bridge *lv2_bridge;
  AgsEffectBridge *effect_bridge;
  AgsBulkMember *bulk_member;
  GtkWidget *control;

  GList *list, *list_start;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_lv2_bridge_parent_connectable_interface->connect(connectable);

  lv2_bridge = AGS_LV2_BRIDGE(connectable);

  /* menu */
  list =
    list_start = gtk_container_get_children((GtkContainer *) lv2_bridge->lv2_menu);

  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_lv2_bridge_show_gui_callback), lv2_bridge);

  g_list_free(list_start);
  
  /* program */
  if(lv2_bridge->program != NULL){
    g_signal_connect_after(G_OBJECT(lv2_bridge->program), "changed",
			   G_CALLBACK(ags_lv2_bridge_program_changed_callback), lv2_bridge);
  }

  /* bulk member */
  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge);
  
  list =
    list_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(effect_bridge->bulk_input)->table);

  while(list != NULL){
    bulk_member = list->data;

    control = gtk_bin_get_child(GTK_BIN(bulk_member));

    if(bulk_member->widget_type == AGS_TYPE_DIAL){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_lv2_bridge_dial_changed_callback), lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_VSCALE){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_lv2_bridge_vscale_changed_callback), lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_HSCALE){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_lv2_bridge_hscale_changed_callback), lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_lv2_bridge_spin_button_changed_callback), lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_lv2_bridge_check_button_clicked_callback), lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_lv2_bridge_toggle_button_clicked_callback), lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_lv2_bridge_button_clicked_callback), lv2_bridge);
    }

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_lv2_bridge_disconnect(AgsConnectable *connectable)
{
  AgsLv2Bridge *lv2_bridge;
  AgsEffectBridge *effect_bridge;
  AgsBulkMember *bulk_member;
  GtkWidget *control;

  GList *list, *list_start;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_lv2_bridge_parent_connectable_interface->disconnect(connectable);

  lv2_bridge = AGS_LV2_BRIDGE(connectable);

  /* menu */
  list =
    list_start = gtk_container_get_children((GtkContainer *) lv2_bridge->lv2_menu);

  g_object_disconnect(G_OBJECT(list->data),
		      "any_signal::activate",
		      G_CALLBACK(ags_lv2_bridge_show_gui_callback),
		      lv2_bridge,
		      NULL);

  g_list_free(list_start);
  
  /* program */
  if(lv2_bridge->program != NULL){
    g_object_disconnect(G_OBJECT(lv2_bridge->program),
			"any_signal::changed",
			G_CALLBACK(ags_lv2_bridge_program_changed_callback),
			lv2_bridge,
			NULL);
  }

  /* bulk member */
  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge);
  
  list =
    list_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(effect_bridge->bulk_input)->table);

  while(list != NULL){
    bulk_member = list->data;

    control = gtk_bin_get_child(GTK_BIN(bulk_member));

    if(bulk_member->widget_type == AGS_TYPE_DIAL){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::value-changed",
			  G_CALLBACK(ags_lv2_bridge_dial_changed_callback),
			  lv2_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_VSCALE){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::value-changed",
			  G_CALLBACK(ags_lv2_bridge_vscale_changed_callback),
			  lv2_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_HSCALE){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::value-changed",
			  G_CALLBACK(ags_lv2_bridge_hscale_changed_callback),
			  lv2_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::value-changed",
			  G_CALLBACK(ags_lv2_bridge_spin_button_changed_callback),
			  lv2_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::clicked",
			  G_CALLBACK(ags_lv2_bridge_check_button_clicked_callback),
			  lv2_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::clicked",
			  G_CALLBACK(ags_lv2_bridge_toggle_button_clicked_callback),
			  lv2_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::clicked",
			  G_CALLBACK(ags_lv2_bridge_button_clicked_callback),
			  lv2_bridge,
			  NULL);
    }

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_lv2_bridge_resize_audio_channels(AgsMachine *machine,
				     guint audio_channels, guint audio_channels_old,
				     gpointer data)
{
  AgsLv2Bridge *lv2_bridge;

  lv2_bridge = (AgsLv2Bridge *) machine;

  if(audio_channels > audio_channels_old){
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_lv2_bridge_input_map_recall(lv2_bridge,
				      audio_channels_old,
				      0);

      ags_lv2_bridge_output_map_recall(lv2_bridge,
				       audio_channels_old,
				       0);
    }
  }
}

void
ags_lv2_bridge_resize_pads(AgsMachine *machine, GType channel_type,
			   guint pads, guint pads_old,
			   gpointer data)
{
  AgsLv2Bridge *lv2_bridge;

  gboolean grow;

  lv2_bridge = (AgsLv2Bridge *) machine;

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(grow){      
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_lv2_bridge_input_map_recall(lv2_bridge,
					0,
					pads_old);
      }
    }else{
      lv2_bridge->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_lv2_bridge_output_map_recall(lv2_bridge,
					 0,
					 pads_old);
      }
    }else{
      lv2_bridge->mapped_output_pad = pads;
    }
  }
}

void
ags_lv2_bridge_map_recall(AgsMachine *machine)
{  
  AgsLv2Bridge *lv2_bridge;
  
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  lv2_bridge = (AgsLv2Bridge *) machine;

  audio = machine->audio;

  position = 0;
  
  /* add new controls */
  ags_effect_bulk_add_plugin(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input),
			     NULL,
			     lv2_bridge->lv2_play_container, lv2_bridge->lv2_recall_container,
			     "ags-fx-lv2",
			     lv2_bridge->filename,
			     lv2_bridge->effect,
			     0, 0,
			     0, 0,
			     position,
			     (AGS_FX_FACTORY_ADD), 0);

  if((AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
    /* ags-fx-envelope */
    start_recall = ags_fx_factory_create(audio,
					 lv2_bridge->envelope_play_container, lv2_bridge->envelope_recall_container,
					 "ags-fx-envelope",
					 NULL,
					 NULL,
					 0, 0,
					 0, 0,
					 position,
					 (AGS_FX_FACTORY_ADD), 0);

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /* ags-fx-buffer */
    start_recall = ags_fx_factory_create(audio,
					 lv2_bridge->buffer_play_container, lv2_bridge->buffer_recall_container,
					 "ags-fx-buffer",
					 NULL,
					 NULL,
					 0, 0,
					 0, 0,
					 position,
					 (AGS_FX_FACTORY_ADD), 0);

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
  }

  /* depending on destination */
  ags_lv2_bridge_input_map_recall(lv2_bridge,
				  0,
				  0);

  /* depending on destination */
  ags_lv2_bridge_output_map_recall(lv2_bridge,
				   0,
				   0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_lv2_bridge_parent_class)->map_recall(machine);
}

void
ags_lv2_bridge_input_map_recall(AgsLv2Bridge *lv2_bridge,
				guint audio_channel_start,
				guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(lv2_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(lv2_bridge)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* add to effect bridge */
  ags_effect_bulk_add_plugin((AgsEffectBulk *) AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input,
			     NULL,
			     lv2_bridge->lv2_play_container, lv2_bridge->lv2_recall_container,
			     "ags-fx-lv2",
			     lv2_bridge->filename,
			     lv2_bridge->effect,
			     audio_channel_start, audio_channels,
			     input_pad_start, input_pads,
			     position,
			     (AGS_FX_FACTORY_REMAP), 0);

  /* source */
  if((AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(lv2_bridge)->flags)) != 0){
    /* ags-fx-envelope */
    start_recall = ags_fx_factory_create(audio,
					 lv2_bridge->envelope_play_container, lv2_bridge->envelope_recall_container,
					 "ags-fx-envelope",
					 NULL,
					 NULL,
					 audio_channel_start, audio_channels,
					 input_pad_start, input_pads,
					 position,
					 (AGS_FX_FACTORY_REMAP), 0);

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /* ags-fx-buffer */
    start_recall = ags_fx_factory_create(audio,
					 lv2_bridge->buffer_play_container, lv2_bridge->buffer_recall_container,
					 "ags-fx-buffer",
					 NULL,
					 NULL,
					 audio_channel_start, audio_channels,
					 input_pad_start, input_pads,
					 position,
					 (AGS_FX_FACTORY_REMAP), 0);

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
  }
  
  lv2_bridge->mapped_input_pad = input_pads;
}

void
ags_lv2_bridge_output_map_recall(AgsLv2Bridge *lv2_bridge,
				 guint audio_channel_start,
				 guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(lv2_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(lv2_bridge)->audio;

  output_pads = 0;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);

  lv2_bridge->mapped_output_pad = output_pads;
}

void
ags_lv2_bridge_load_program(AgsLv2Bridge *lv2_bridge)
{
  AgsLv2Plugin *lv2_plugin;
    
  LV2_Descriptor *plugin_descriptor;
  LV2_Programs_Interface *program_interface;

  /* retrieve lv2 plugin */
  lv2_plugin = lv2_bridge->lv2_plugin;
  
  if(lv2_plugin == NULL){
    lv2_plugin =
      lv2_bridge->lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
							       lv2_bridge->filename,
							       lv2_bridge->effect);
    g_object_ref(lv2_plugin);

    lv2_bridge->lv2_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;
  }
  
  if(lv2_plugin == NULL){
    return;
  }

  plugin_descriptor = lv2_bridge->lv2_descriptor;

  if(plugin_descriptor != NULL &&
     plugin_descriptor->extension_data != NULL &&
     (program_interface = plugin_descriptor->extension_data(LV2_PROGRAMS__Interface)) != NULL){
    GtkListStore *model;
    
    GtkTreeIter iter;
    
    LV2_Program_Descriptor *program_descriptor;
    
    uint32_t i;

    if(lv2_bridge->lv2_handle == NULL){
      guint samplerate;
      guint buffer_size;

      g_object_get(AGS_MACHINE(lv2_bridge)->audio,
		   "samplerate", &samplerate,
		   "buffer-size", &buffer_size,
		   NULL);
      
      lv2_bridge->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
							   samplerate, buffer_size);
    }
    
    if(lv2_bridge->port_value == NULL){
      GList *start_plugin_port, *plugin_port;
      
      guint port_count;

      g_object_get(lv2_plugin,
		   "plugin-port", &start_plugin_port,
		   NULL);

      plugin_port = start_plugin_port;
      port_count = g_list_length(start_plugin_port);

      if(port_count > 0){
	lv2_bridge->port_value = (float *) malloc(port_count * sizeof(float));
      }
      
      while(plugin_port != NULL){
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
	  if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INPUT)){
	    plugin_descriptor->connect_port(lv2_bridge->lv2_handle[0],
					    AGS_PLUGIN_PORT(plugin_port->data)->port_index,
					    &(lv2_bridge->port_value[AGS_PLUGIN_PORT(plugin_port->data)->port_index]));
	  }
	}

	/* iterate */
	plugin_port = plugin_port->next;
      }

      g_list_free_full(start_plugin_port,
		       g_object_unref);
    }

    if(lv2_bridge->program == NULL){
      GtkHBox *hbox;
      GtkLabel *label;

      /* program */
      hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
      gtk_box_pack_start((GtkBox *) lv2_bridge->vbox,
			 (GtkWidget *) hbox,
			 FALSE, FALSE,
			 0);
      gtk_box_reorder_child(GTK_BOX(lv2_bridge->vbox),
			    GTK_WIDGET(hbox),
			    0);
  
      label = (GtkLabel *) gtk_label_new(i18n("program"));
      gtk_box_pack_start((GtkBox *) hbox,
			 (GtkWidget *) label,
			 FALSE, FALSE,
			 0);

      lv2_bridge->program = (GtkComboBoxText *) gtk_combo_box_text_new();
      gtk_box_pack_start((GtkBox *) hbox,
			 (GtkWidget *) lv2_bridge->program,
			 FALSE, FALSE,
			 0);

      if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(lv2_bridge)->flags)) != 0){
	g_signal_connect_after(G_OBJECT(lv2_bridge->program), "changed",
			       G_CALLBACK(ags_lv2_bridge_program_changed_callback), lv2_bridge);
      }
      
      model = gtk_list_store_new(3,
				 G_TYPE_STRING,
				 G_TYPE_ULONG,
				 G_TYPE_ULONG);
      
      gtk_combo_box_set_model(GTK_COMBO_BOX(lv2_bridge->program),
			      GTK_TREE_MODEL(model));
    }else{
      model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(lv2_bridge->program)));
      
      gtk_list_store_clear(GTK_LIST_STORE(model));
    }

    for(i = 0; (program_descriptor = program_interface->get_program(lv2_bridge->lv2_handle[0], i)) != NULL; i++){
      gtk_list_store_append(model, &iter);
      gtk_list_store_set(model, &iter,
			 0, program_descriptor->name,
			 1, program_descriptor->bank,
			 2, program_descriptor->program,
			 -1);
    }
  }
}

void
ags_lv2_bridge_load_preset(AgsLv2Bridge *lv2_bridge)
{
  GtkHBox *hbox;
  GtkLabel *label;
  
  AgsLv2Plugin *lv2_plugin;

  GList *list;  
  
  /* retrieve lv2 plugin */
  lv2_plugin = lv2_bridge->lv2_plugin;
  
  if(lv2_plugin == NULL){
    lv2_plugin =
      lv2_bridge->lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
							       lv2_bridge->filename,
							       lv2_bridge->effect);
    g_object_ref(lv2_plugin);

    lv2_bridge->lv2_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;
  }
  
  if(lv2_plugin == NULL){
    return;
  }

  /* preset */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) lv2_bridge->vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);
  gtk_box_reorder_child(GTK_BOX(lv2_bridge->vbox),
  			GTK_WIDGET(hbox),
  			0);
  
  label = (GtkLabel *) gtk_label_new(i18n("preset"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  lv2_bridge->preset = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) lv2_bridge->preset,
		     FALSE, FALSE,
		     0);
  
  /* preset */
  list = lv2_plugin->preset;

  while(list != NULL){
    if(AGS_LV2_PRESET(list->data)->preset_label != NULL){
      gtk_combo_box_text_append_text(lv2_bridge->preset,
				     AGS_LV2_PRESET(list->data)->preset_label);
    }
    
    list = list->next;
  }

  gtk_widget_show_all((GtkWidget *) hbox);

  /* connect preset */
  g_signal_connect_after(G_OBJECT(lv2_bridge->preset), "changed",
			 G_CALLBACK(ags_lv2_bridge_preset_changed_callback), lv2_bridge);
}

void
ags_lv2_bridge_load_midi(AgsLv2Bridge *lv2_bridge)
{
  //TODO:JK: implement me
}

void
ags_lv2_bridge_load_gui(AgsLv2Bridge *lv2_bridge)
{
  AgsLv2Plugin *lv2_plugin;
  AgsLv2uiPlugin *lv2ui_plugin;

  GList *list;
  
  /* retrieve lv2 plugin */
  lv2_plugin = lv2_bridge->lv2_plugin;
  
  if(lv2_plugin == NULL){
    lv2_plugin =
      lv2_bridge->lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
							       lv2_bridge->filename,
							       lv2_bridge->effect);
    g_object_ref(lv2_plugin);

    lv2_bridge->lv2_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;
  }
  
  if(lv2_plugin == NULL ||
     lv2_plugin->ui_uri == NULL){
    return;
  }

  /* retrieve lv2ui plugin */
  lv2ui_plugin = lv2_bridge->lv2ui_plugin;
  
  if(lv2ui_plugin == NULL){
    list = ags_lv2ui_plugin_find_gui_uri(ags_lv2ui_manager_get_instance()->lv2ui_plugin,
					 lv2_plugin->ui_uri);

    if(list == NULL){
      return;
    }

    lv2ui_plugin =
      lv2_bridge->lv2ui_plugin = list->data;
    g_object_ref(lv2ui_plugin);

    lv2_bridge->ui_descriptor = AGS_BASE_PLUGIN(lv2ui_plugin)->ui_plugin_descriptor;
  }
  
#ifdef AGS_DEBUG
  g_message("ui filename - %s, %s", AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename, lv2_plugin->ui_uri);
#endif

  /* apply ui */
  g_object_set(lv2_bridge,
	       "has-gui", TRUE,
	       "gui-uri", lv2_plugin->ui_uri,
	       "gui-filename", AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
	       NULL);
}

void
ags_lv2_bridge_load(AgsLv2Bridge *lv2_bridge)
{
  AgsLv2Plugin *lv2_plugin;
    
  guint samplerate;
  guint buffer_size;

  /* retrieve lv2 plugin */
  lv2_plugin = lv2_bridge->lv2_plugin;

  if(lv2_plugin == NULL){
    lv2_plugin =
      lv2_bridge->lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
							       lv2_bridge->filename,
							       lv2_bridge->effect);
    g_object_ref(lv2_plugin);

    lv2_bridge->lv2_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;
  }
  
  if(lv2_plugin == NULL){
    return;
  }
  
  /* URI */
  g_object_set(lv2_bridge,
	       "uri", lv2_plugin->uri,
	       NULL);

  /* samplerate and buffer size */
  samplerate = ags_soundcard_helper_config_get_samplerate(ags_config_get_instance());
  buffer_size = ags_soundcard_helper_config_get_buffer_size(ags_config_get_instance());

  /* program */
  lv2_bridge->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
						       samplerate, buffer_size);

  if((AGS_LV2_PLUGIN_HAS_PROGRAM_INTERFACE & (lv2_plugin->flags)) != 0){
    ags_lv2_bridge_load_program(lv2_bridge);
  }
  
  /* preset */
  if(lv2_plugin->preset != NULL){
    ags_lv2_bridge_load_preset(lv2_bridge);
  }

  /* load gui */
  ags_lv2_bridge_load_gui(lv2_bridge);
}

/**
 * ags_lv2_bridge_lv2ui_idle_timeout:
 * @widget: LV2UI_Handle
 *
 * Idle lv2 ui.
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_lv2_bridge_lv2ui_idle_timeout(GtkWidget *widget)
{
  AgsLv2Bridge *lv2_bridge;

  AgsLv2uiPlugin *lv2ui_plugin;

  int retval;

  /* retrieve lv2ui plugin */
  if((lv2_bridge = g_hash_table_lookup(ags_lv2_bridge_lv2ui_idle,
				       widget)) != NULL){
    lv2ui_plugin = lv2_bridge->lv2ui_plugin;
    
    if(lv2ui_plugin->feature != NULL &&
       lv2ui_plugin->feature[0]->data != NULL){
      retval = ((LV2UI_Idle_Interface *) lv2ui_plugin->feature[0]->data)->idle(lv2_bridge->ui_handle[0]);

      if(retval != 0){
	g_hash_table_remove(ags_lv2_bridge_lv2ui_handle,
			    lv2_bridge->ui_handle);
	
	lv2_bridge->ui_handle = NULL;

	return(FALSE);
      }
    }
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_lv2_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Create a new instance of #AgsLv2Bridge
 *
 * Returns: the new #AgsLv2Bridge
 *
 * Since: 3.0.0
 */
AgsLv2Bridge*
ags_lv2_bridge_new(GObject *soundcard,
		   gchar *filename,
		   gchar *effect)
{
  AgsLv2Bridge *lv2_bridge;

  lv2_bridge = (AgsLv2Bridge *) g_object_new(AGS_TYPE_LV2_BRIDGE,
					     NULL);

  g_object_set(AGS_MACHINE(lv2_bridge)->audio,
	       "output-soundcard", soundcard,
	       NULL);
  
  g_object_set(lv2_bridge,
	       "filename", filename,
	       "effect", effect,
	       NULL);

  return(lv2_bridge);
}
