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

#include <ags/app/ags_plugin_browser.h>
#include <ags/app/ags_plugin_browser_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_lv2_browser.h>
#include <ags/app/ags_dssi_browser.h>
#include <ags/app/ags_ladspa_browser.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/ags_vst3_browser.h>
#endif

#include <ags/i18n.h>

void ags_plugin_browser_class_init(AgsPluginBrowserClass *plugin_browser);
void ags_plugin_browser_init(AgsPluginBrowser *plugin_browser);
void ags_plugin_browser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_plugin_browser_applicable_interface_init(AgsApplicableInterface *applicable);

gboolean ags_plugin_browser_delete_event(GtkWidget *widget, GdkEventAny *event);
void ags_plugin_browser_show(GtkWidget *widget);

void ags_plugin_browser_connect(AgsConnectable *connectable);
void ags_plugin_browser_disconnect(AgsConnectable *connectable);

void ags_plugin_browser_set_update(AgsApplicable *applicable, gboolean update);
void ags_plugin_browser_apply(AgsApplicable *applicable);
void ags_plugin_browser_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_plugin_browser
 * @short_description: A composite to select plugin effect.
 * @title: AgsPluginBrowser
 * @section_id:
 * @include: ags/X/ags_plugin_browser.h
 *
 * #AgsPluginBrowser is a composite widget to select plugin plugin and the desired
 * effect.
 */

static gpointer ags_plugin_browser_parent_class = NULL;

GType
ags_plugin_browser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_plugin_browser = 0;

    static const GTypeInfo ags_plugin_browser_info = {
      sizeof (AgsPluginBrowserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_plugin_browser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPluginBrowser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_plugin_browser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_plugin_browser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_plugin_browser_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_plugin_browser = g_type_register_static(GTK_TYPE_DIALOG,
						     "AgsPluginBrowser", &ags_plugin_browser_info,
						     0);

    g_type_add_interface_static(ags_type_plugin_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_plugin_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_plugin_browser);
  }

  return g_define_type_id__volatile;
}

void
ags_plugin_browser_class_init(AgsPluginBrowserClass *plugin_browser)
{
  GtkWidgetClass *widget;
  
  ags_plugin_browser_parent_class = g_type_class_peek_parent(plugin_browser);
  
  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) plugin_browser;
  
  widget->delete_event = ags_plugin_browser_delete_event;
  widget->show = ags_plugin_browser_show;
}

void
ags_plugin_browser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_plugin_browser_connect;
  connectable->disconnect = ags_plugin_browser_disconnect;
}

void
ags_plugin_browser_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_plugin_browser_set_update;
  applicable->apply = ags_plugin_browser_apply;
  applicable->reset = ags_plugin_browser_reset;
}

void
ags_plugin_browser_init(AgsPluginBrowser *plugin_browser)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  
  plugin_browser->flags = 0;

  gtk_window_set_title((GtkWindow *) plugin_browser,
		       i18n("Plugin browser"));

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(GTK_DIALOG(plugin_browser)),
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);
  
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("Plugin type:"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  plugin_browser->plugin_type = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) plugin_browser->plugin_type,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);
  
  gtk_combo_box_text_append_text(plugin_browser->plugin_type,
				 "lv2");

  gtk_combo_box_text_append_text(plugin_browser->plugin_type,
				 "ladspa");

#if defined(AGS_WITH_VST3)
  gtk_combo_box_text_append_text(plugin_browser->plugin_type,
				 "vst3");
#endif
  
  plugin_browser->active_browser = NULL;
  
  plugin_browser->lv2_browser = (GtkWidget *) ags_lv2_browser_new();
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) plugin_browser->lv2_browser,
		     FALSE, FALSE,
		     0);

  plugin_browser->dssi_browser = NULL;
  
  plugin_browser->ladspa_browser = (GtkWidget *) ags_ladspa_browser_new();
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) plugin_browser->ladspa_browser,
		     FALSE, FALSE,
		     0);

#if defined(AGS_WITH_VST3)
  plugin_browser->vst3_browser = (GtkWidget *) ags_vst3_browser_new();
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) plugin_browser->vst3_browser,
		     FALSE, FALSE,
		     0);
#else
  plugin_browser->vst3_browser = NULL;
#endif
  
  /* action area */
  gtk_dialog_add_buttons((GtkDialog *) plugin_browser,
			 i18n("_OK"), GTK_RESPONSE_ACCEPT,
			 i18n("_Cancel"), GTK_RESPONSE_REJECT,
			 NULL);
}

gboolean
ags_plugin_browser_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  return(TRUE);
}

void
ags_plugin_browser_show(GtkWidget *widget)
{
  AgsPluginBrowser *plugin_browser;

  plugin_browser = AGS_PLUGIN_BROWSER(widget);
  
  GTK_WIDGET_CLASS(ags_plugin_browser_parent_class)->show(widget);

  gtk_widget_hide(plugin_browser->lv2_browser);

  gtk_widget_hide(plugin_browser->ladspa_browser);

#if defined(AGS_WITH_VST3)
  gtk_widget_hide(plugin_browser->vst3_browser);
#endif
}

void
ags_plugin_browser_connect(AgsConnectable *connectable)
{
  AgsPluginBrowser *plugin_browser;

  plugin_browser = AGS_PLUGIN_BROWSER(connectable);

  if((AGS_PLUGIN_BROWSER_CONNECTED & (plugin_browser->flags)) != 0){
    return;
  }

  plugin_browser->flags |= AGS_PLUGIN_BROWSER_CONNECTED;
  
  g_signal_connect(plugin_browser->plugin_type, "changed",
		   G_CALLBACK(ags_plugin_browser_plugin_type_changed_callback), plugin_browser);
  
  ags_connectable_connect(AGS_CONNECTABLE(plugin_browser->lv2_browser));

  ags_connectable_connect(AGS_CONNECTABLE(plugin_browser->ladspa_browser));

#if defined(AGS_WITH_VST3)
  ags_connectable_connect(AGS_CONNECTABLE(plugin_browser->vst3_browser));
#endif
  
  /* AgsPluginBrowser response */
  g_signal_connect((GObject *) plugin_browser, "response",
		   G_CALLBACK(ags_plugin_browser_response_callback), NULL);
}

void
ags_plugin_browser_disconnect(AgsConnectable *connectable)
{
  AgsPluginBrowser *plugin_browser;

  plugin_browser = AGS_PLUGIN_BROWSER(connectable);

  if((AGS_PLUGIN_BROWSER_CONNECTED & (plugin_browser->flags)) == 0){
    return;
  }

  plugin_browser->flags &= (~AGS_PLUGIN_BROWSER_CONNECTED);
  
  g_object_disconnect(plugin_browser->plugin_type,
		      "any_signal::changed",
		      G_CALLBACK(ags_plugin_browser_plugin_type_changed_callback),
		      plugin_browser,
		      NULL);
  
  ags_connectable_disconnect(AGS_CONNECTABLE(plugin_browser->lv2_browser));

  ags_connectable_disconnect(AGS_CONNECTABLE(plugin_browser->ladspa_browser));

#if defined(AGS_WITH_VST3)
  ags_connectable_disconnect(AGS_CONNECTABLE(plugin_browser->vst3_browser));
#endif
  
  /* AgsPluginBrowser buttons */
  g_object_disconnect((GObject *) plugin_browser,
		      "any_signal::response",
		      G_CALLBACK(ags_plugin_browser_response_callback),
		      NULL,
		      NULL);
}

void
ags_plugin_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_plugin_browser_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_plugin_browser_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_plugin_browser_get_plugin_filename:
 * @plugin_browser: the #AgsPluginBrowser
 *
 * Retrieve selected plugin plugin filename.
 *
 * Returns: the active plugin filename
 *
 * Since: 3.0.0
 */
gchar*
ags_plugin_browser_get_plugin_filename(AgsPluginBrowser *plugin_browser)
{
  if(AGS_IS_LV2_BROWSER(plugin_browser->active_browser)){
    return(ags_lv2_browser_get_plugin_filename((AgsLv2Browser *) plugin_browser->lv2_browser));
  }else if(AGS_IS_LADSPA_BROWSER(plugin_browser->active_browser)){
    return(ags_ladspa_browser_get_plugin_filename((AgsLadspaBrowser *) plugin_browser->ladspa_browser));
#if defined(AGS_WITH_VST3)
  }else if(AGS_IS_VST3_BROWSER(plugin_browser->active_browser)){
    return(ags_vst3_browser_get_plugin_filename((AgsVst3Browser *) plugin_browser->vst3_browser));
#endif
  }else{
    return(NULL);
  }
}

/**
 * ags_plugin_browser_get_plugin_effect:
 * @plugin_browser: the #AgsPluginBrowser
 *
 * Retrieve selected plugin effect.
 *
 * Returns: the active plugin effect
 *
 * Since: 3.0.0
 */
gchar*
ags_plugin_browser_get_plugin_effect(AgsPluginBrowser *plugin_browser)
{
  if(AGS_IS_LV2_BROWSER(plugin_browser->active_browser)){
    return(ags_lv2_browser_get_plugin_effect((AgsLv2Browser *) plugin_browser->lv2_browser));
  }else if(AGS_IS_LADSPA_BROWSER(plugin_browser->active_browser)){
    return(ags_ladspa_browser_get_plugin_effect((AgsLadspaBrowser *) plugin_browser->ladspa_browser));
#if defined(AGS_WITH_VST3)
  }else if(AGS_IS_VST3_BROWSER(plugin_browser->active_browser)){
    return(ags_vst3_browser_get_plugin_effect((AgsVst3Browser *) plugin_browser->vst3_browser));
#endif
  }else{
    return(NULL);
  }
}

/**
 * ags_plugin_browser_new:
 * @parent_window: the #AgsWindow
 *
 * Create a new instance of #AgsPluginBrowser
 *
 * Returns: the new #AgsPluginBrowser
 *
 * Since: 3.0.0
 */
AgsPluginBrowser*
ags_plugin_browser_new(GtkWidget *parent_window)
{
  AgsPluginBrowser *plugin_browser;

  plugin_browser = (AgsPluginBrowser *) g_object_new(AGS_TYPE_PLUGIN_BROWSER,
						     NULL);

  plugin_browser->parent_window = parent_window;

  return(plugin_browser);
}
