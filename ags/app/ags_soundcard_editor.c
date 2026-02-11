/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/app/ags_soundcard_editor.h>
#include <ags/app/ags_soundcard_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_audio_preferences.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_soundcard_editor_class_init(AgsSoundcardEditorClass *soundcard_editor);
void ags_soundcard_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_soundcard_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_soundcard_editor_init(AgsSoundcardEditor *soundcard_editor);
static void ags_soundcard_editor_finalize(GObject *gobject);

gboolean ags_soundcard_editor_is_connected(AgsConnectable *connectable);
void ags_soundcard_editor_connect(AgsConnectable *connectable);
void ags_soundcard_editor_disconnect(AgsConnectable *connectable);

void ags_soundcard_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_soundcard_editor_apply(AgsApplicable *applicable);
void ags_soundcard_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_soundcard_editor
 * @short_description: A composite widget to configure soundcard
 * @title: AgsSoundcardEditor
 * @section_id: 
 * @include: ags/app/ags_soundcard_editor.h
 *
 * #AgsSoundcardEditor enables you to make preferences of soundcard, audio channels,
 * samplerate and buffer size.
 */

static gpointer ags_soundcard_editor_parent_class = NULL;

GType
ags_soundcard_editor_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_soundcard_editor = 0;

    static const GTypeInfo ags_soundcard_editor_info = {
      sizeof (AgsSoundcardEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_soundcard_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSoundcardEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_soundcard_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_soundcard_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_soundcard_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_soundcard_editor = g_type_register_static(GTK_TYPE_BOX,
						       "AgsSoundcardEditor", &ags_soundcard_editor_info,
						       0);
    
    g_type_add_interface_static(ags_type_soundcard_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_soundcard_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_soundcard_editor);
  }

  return(g_define_type_id__static);
}

void
ags_soundcard_editor_class_init(AgsSoundcardEditorClass *soundcard_editor)
{
  GObjectClass *gobject;

  ags_soundcard_editor_parent_class = g_type_class_peek_parent(soundcard_editor);

  /* GtkObjectClass */
  gobject = (GObjectClass *) soundcard_editor;

  gobject->finalize = ags_soundcard_editor_finalize;
}

void
ags_soundcard_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_soundcard_editor_is_connected;  
  connectable->connect = ags_soundcard_editor_connect;
  connectable->disconnect = ags_soundcard_editor_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_soundcard_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_soundcard_editor_set_update;
  applicable->apply = ags_soundcard_editor_apply;
  applicable->reset = ags_soundcard_editor_reset;
}

void
ags_soundcard_editor_init(AgsSoundcardEditor *soundcard_editor)
{
  GtkGrid *grid;
  GtkLabel *label;

  guint y0;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(soundcard_editor),
				 GTK_ORIENTATION_VERTICAL);  

  gtk_box_set_spacing((GtkBox *) soundcard_editor,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  soundcard_editor->flags = 0;
  
  soundcard_editor->soundcard = NULL;
  soundcard_editor->soundcard_thread = NULL;
  
  grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_vexpand((GtkWidget *) grid,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) grid,
			 FALSE);

  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_START);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_box_append((GtkBox *) soundcard_editor,
		 (GtkWidget *) grid);

  y0 = 0;
  
  /* backend */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("backend"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, y0,
		  1, 1);

  soundcard_editor->backend = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->backend,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->backend,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->backend),
		  1, y0,
		  1, 1);

#ifdef AGS_WITH_CORE_AUDIO
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "core-audio");
#endif

#ifdef AGS_WITH_PULSE
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "pulse");
#endif

#ifdef AGS_WITH_JACK
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "jack");
#endif
  
#ifdef AGS_WITH_WASAPI
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "wasapi");
#endif

#ifdef AGS_WITH_ALSA
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "alsa");
#endif
  
#ifdef AGS_WITH_OSS
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "oss");
#endif

  gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			   0);

  gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->backend,
			   FALSE);
  
  y0++;
  
  /* sound card */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("soundcard"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, y0,
		  1, 1);

  soundcard_editor->card = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->card,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->card,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->card),
		  1, y0,
		  1, 1);

  y0++;
    
  /* capability */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("capability"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, y0,
		  1, 1);

  soundcard_editor->capability = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->capability,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->capability,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->capability),
		  1, y0,
		  1, 1);

  gtk_combo_box_text_append_text(soundcard_editor->capability,
				 "playback");

  gtk_combo_box_text_append_text(soundcard_editor->capability,
				 "capture");

#if 0
  gtk_combo_box_text_append_text(soundcard_editor->capability,
				 "duplex");
#endif
  
  gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->capability),
			   0);

  gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->capability,
			   FALSE);
  
  y0++;

  /* audio channels */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("audio channels"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, y0,
		  1, 1);

  soundcard_editor->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 24.0, 1.0);
  gtk_spin_button_set_value(soundcard_editor->audio_channels,
			    (gdouble) AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS);

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->audio_channels,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->audio_channels,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->audio_channels),
		  1, y0,
		  1, 1);

  y0++;

  /* samplerate */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("samplerate"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, y0,
		  1, 1);

  soundcard_editor->samplerate = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 192000.0, 1.0);
  gtk_spin_button_set_value(soundcard_editor->samplerate,
			    (gdouble) AGS_SOUNDCARD_DEFAULT_SAMPLERATE);

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->samplerate,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->samplerate,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->samplerate),
		  1, y0,
		  1, 1);

  y0++;

  /* buffer size */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("buffer size"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, y0,
		  1, 1);

  soundcard_editor->buffer_size = (GtkSpinButton *) gtk_spin_button_new_with_range((gdouble) AGS_SOUNDCARD_MIN_BUFFER_SIZE, (gdouble) AGS_SOUNDCARD_MAX_BUFFER_SIZE, 1.0);
  gtk_spin_button_set_value(soundcard_editor->buffer_size,
			    (gdouble) AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE);

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->buffer_size,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->buffer_size,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->buffer_size),
		  1, y0,
		  1, 1);

  y0++;

  /* format */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("format"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, y0,
		  1, 1);

  soundcard_editor->format = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->format,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->format,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->format),
		  1, y0,
		  1, 1);

  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "8");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "16");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "24");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "32");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "64");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "float");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "double");
  
#if defined(AGS_WITH_CORE_AUDIO)
  gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			   5);
#else
  gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			   1);
#endif
  
  y0++;

  /* use cache */
  soundcard_editor->use_cache = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("use cache"));
  gtk_check_button_set_active(soundcard_editor->use_cache,
			      FALSE);

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->use_cache,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->use_cache,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->use_cache),
		  0, y0,
		  2, 1);

  y0++;
  
  /* cache buffer size */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("cache buffer size"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, y0,
		  1, 1);

  soundcard_editor->cache_buffer_size = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 65535.0, 1.0);
  gtk_spin_button_set_value(soundcard_editor->cache_buffer_size,
			    (gdouble) AGS_SOUNDCARD_DEFAULT_CACHE_BUFFER_SIZE);

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->cache_buffer_size,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->cache_buffer_size,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->cache_buffer_size),
		  1, y0,
		  1, 1);
  
  y0++;

#if defined(AGS_WITH_WASAPI)
  /* wasapi share mode */
  soundcard_editor->wasapi_share_mode_label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
									"label", i18n("WASAPI share mode"),
									NULL);

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->wasapi_share_mode_label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->wasapi_share_mode_label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) soundcard_editor->wasapi_share_mode_label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->wasapi_share_mode_label),
		  0, y0,
		  1, 1);

  soundcard_editor->wasapi_share_mode = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->wasapi_share_mode,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->wasapi_share_mode,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->wasapi_share_mode),
		  1, y0,
		  1, 1);

  gtk_combo_box_text_append_text(soundcard_editor->wasapi_share_mode,
				 "exclusive");

  gtk_combo_box_text_append_text(soundcard_editor->wasapi_share_mode,
				 "shared");

  y0++;

  /* wasapi buffer size */
  soundcard_editor->wasapi_buffer_size_label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
									 "label", i18n("WASAPI buffer size"),
									 NULL);

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->wasapi_buffer_size_label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->wasapi_buffer_size_label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) soundcard_editor->wasapi_buffer_size_label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->wasapi_buffer_size_label),
		  0, y0,
		  1, 1);

  soundcard_editor->wasapi_buffer_size = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 65535.0, 1.0);
  gtk_spin_button_set_value(soundcard_editor->wasapi_buffer_size,
			    8192.0);

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->wasapi_buffer_size,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->wasapi_buffer_size,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->wasapi_buffer_size),
		  1, y0,
		  1, 1);

  y0++;
#else
  soundcard_editor->wasapi_share_mode_label = NULL;
  soundcard_editor->wasapi_share_mode = NULL;

  soundcard_editor->wasapi_buffer_size_label = NULL;
  soundcard_editor->wasapi_buffer_size = NULL;
#endif
  
  /*  */
  //  soundcard_editor->remove = NULL;

  soundcard_editor->remove = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Remove"));

  gtk_widget_set_valign((GtkWidget *) soundcard_editor->remove,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) soundcard_editor->remove,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(soundcard_editor->remove),
		  2, y0,
		  1, 1);

  y0++;
}

static void
ags_soundcard_editor_finalize(GObject *gobject)
{
  //TODO:JK: implement me
  
  G_OBJECT_CLASS(ags_soundcard_editor_parent_class)->finalize(gobject);
}

gboolean
ags_soundcard_editor_is_connected(AgsConnectable *connectable)
{
  AgsSoundcardEditor *soundcard_editor;

  gboolean retval;

  soundcard_editor = AGS_SOUNDCARD_EDITOR(connectable);

  retval = ((AGS_CONNECTABLE_CONNECTED & (soundcard_editor->connectable_flags)) != 0) ? TRUE: FALSE;

  return(retval);
}

void
ags_soundcard_editor_connect(AgsConnectable *connectable)
{
  AgsSoundcardEditor *soundcard_editor;

  soundcard_editor = AGS_SOUNDCARD_EDITOR(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  soundcard_editor->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  /* backend and card */
  g_signal_connect_after(G_OBJECT(soundcard_editor->backend), "changed",
			 G_CALLBACK(ags_soundcard_editor_backend_changed_callback), soundcard_editor);

  g_signal_connect_after(G_OBJECT(soundcard_editor->card), "changed",
			 G_CALLBACK(ags_soundcard_editor_card_changed_callback), soundcard_editor);
  
  /* presets */
  g_signal_connect_after(G_OBJECT(soundcard_editor->audio_channels), "changed",
			 G_CALLBACK(ags_soundcard_editor_audio_channels_changed_callback), soundcard_editor);

  g_signal_connect_after(G_OBJECT(soundcard_editor->samplerate), "changed",
			 G_CALLBACK(ags_soundcard_editor_samplerate_changed_callback), soundcard_editor);

  g_signal_connect_after(G_OBJECT(soundcard_editor->buffer_size), "changed",
			 G_CALLBACK(ags_soundcard_editor_buffer_size_changed_callback), soundcard_editor);

  g_signal_connect_after(G_OBJECT(soundcard_editor->format), "changed",
			 G_CALLBACK(ags_soundcard_editor_format_changed_callback), soundcard_editor);
}

void
ags_soundcard_editor_disconnect(AgsConnectable *connectable)
{
  AgsSoundcardEditor *soundcard_editor;

  soundcard_editor = AGS_SOUNDCARD_EDITOR(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  soundcard_editor->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  /* backend and card */
  g_object_disconnect(G_OBJECT(soundcard_editor->backend),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_backend_changed_callback),
		      soundcard_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(soundcard_editor->card),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_card_changed_callback),
		      soundcard_editor,
		      NULL);

  /* presets */
  g_object_disconnect(G_OBJECT(soundcard_editor->audio_channels),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_audio_channels_changed_callback),
		      soundcard_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(soundcard_editor->samplerate),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_samplerate_changed_callback),
		      soundcard_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(soundcard_editor->buffer_size),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_buffer_size_changed_callback),
		      soundcard_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(soundcard_editor->format),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_format_changed_callback),
		      soundcard_editor,
		      NULL);
}

void
ags_soundcard_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_soundcard_editor_apply(AgsApplicable *applicable)
{
  AgsAudioPreferences *audio_preferences;
  AgsSoundcardEditor *soundcard_editor;
  
  GtkListStore *model;
  GtkTreeIter current;

  AgsConfig *config;

  GList *start_list;	

  gchar *soundcard_group;
  gchar *backend;
  gchar *capability;
  char *device, *str;
#if defined AGS_WITH_WASAPI
  gchar *wasapi_share_mode;
#endif
  
  gint nth;
  guint channels;
  guint first_samplerate;
  guint first_buffer_size;
  guint samplerate;
  guint buffer_size;
  guint cache_buffer_size;
  guint format;
#if defined AGS_WITH_WASAPI
  guint wasapi_buffer_size;
#endif
  gboolean use_core_audio, use_pulse, use_jack, use_wasapi, use_alsa, use_oss;
  
  GValue value =  {0,};

  soundcard_editor = AGS_SOUNDCARD_EDITOR(applicable);
  audio_preferences = (AgsAudioPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
								      AGS_TYPE_AUDIO_PREFERENCES);

  config = ags_config_get_instance();

  start_list = ags_audio_preferences_get_soundcard_editor(audio_preferences);
  nth = g_list_index(start_list,
		     soundcard_editor);

  if(nth < 0){
    g_list_free(start_list);
    
    return;
  }

  /* first buffer size and samplerate */
  if(start_list != NULL){
    first_buffer_size = gtk_spin_button_get_value(AGS_SOUNDCARD_EDITOR(start_list->data)->buffer_size);
    
    first_samplerate = gtk_spin_button_get_value(AGS_SOUNDCARD_EDITOR(start_list->data)->samplerate);
  }
  
  g_list_free(start_list);
  
  soundcard_group = g_strdup_printf("%s-%d",
				    AGS_CONFIG_SOUNDCARD,
				    nth);
  
  /* backend */
#if defined AGS_WITH_WASAPI
  use_wasapi = TRUE;
  
  use_core_audio = FALSE;
  
  use_pulse = FALSE;
#elif defined AGS_WITH_CORE_AUDIO
  use_wasapi = FALSE;

  use_core_audio = TRUE;

  use_pulse = FALSE;
#else
  use_wasapi = FALSE;

  use_core_audio = FALSE;
  
  use_pulse = TRUE;
#endif
  
  use_jack = FALSE;
  use_alsa = FALSE;
  use_oss = FALSE;

  backend = gtk_combo_box_text_get_active_text(soundcard_editor->backend);
  ags_config_set_value(config,
		       soundcard_group,
		       "backend",
		       backend);

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "core-audio",
			    11)){
      use_core_audio = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
				  "pulse",
				  6)){
      use_pulse = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
				  "jack",
				  5)){
      use_jack = TRUE;

      use_wasapi = FALSE;
      use_core_audio = FALSE;
      use_pulse = FALSE;
    }else if(!g_ascii_strncasecmp(backend,
				  "wasapi",
				  7)){
      use_wasapi = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
				  "alsa",
				  5)){
      use_alsa = TRUE;

      use_wasapi = FALSE;
      use_core_audio = FALSE;
      use_pulse = FALSE;
    }else if(!g_ascii_strncasecmp(backend,
				  "oss",
				  4)){
      use_oss = TRUE;

      use_wasapi = FALSE;
      use_core_audio = FALSE;
      use_pulse = FALSE;
    }
  }
  
  /* capability */
  capability = gtk_combo_box_text_get_active_text(soundcard_editor->capability);

  ags_config_set_value(config,
		       soundcard_group,
		       "capability",
		       capability);

  /* buffer size */
  buffer_size = gtk_spin_button_get_value(soundcard_editor->buffer_size);
  
  str = g_strdup_printf("%u",
			buffer_size);
  
  ags_config_set_value(config,
		       soundcard_group,
		       "buffer-size",
		       str);
  g_free(str);

  /* pcm channels */
  channels = gtk_spin_button_get_value(soundcard_editor->audio_channels);
  
  str = g_strdup_printf("%u",
			channels);
  
  ags_config_set_value(config,
		       soundcard_group,
		       "pcm-channels",
		       str);
  g_free(str);

  /* format */
  format = 0;

  str = NULL;
  
  switch(gtk_combo_box_get_active(GTK_COMBO_BOX(soundcard_editor->format))){
  case 0:
    {
      format = AGS_SOUNDCARD_SIGNED_8_BIT;
      
      str = g_strdup_printf("%u",
			    format);
    }
    break;
  case 1:
    {
      format = AGS_SOUNDCARD_SIGNED_16_BIT;
      
      str = g_strdup_printf("%u",
			    format);
    }
    break;
  case 2:
    {
      format = AGS_SOUNDCARD_SIGNED_24_BIT;
      
      str = g_strdup_printf("%u",
			    format);
    }
    break;
  case 3:
    {
      format = AGS_SOUNDCARD_SIGNED_32_BIT;
      
      str = g_strdup_printf("%u",
			    format);
    }
    break;
  case 4:
    {
      format = AGS_SOUNDCARD_SIGNED_64_BIT;
      
      str = g_strdup_printf("%u",
			    format);
    }
    break;
  case 5:
    {
      format = AGS_SOUNDCARD_FLOAT;
      
      str = g_strdup("float");
    }
    break;
  case 6:
    {
      format = AGS_SOUNDCARD_DOUBLE;
      
      str = g_strdup("double");
    }
    break;
  }
  
  ags_config_set_value(config,
		       soundcard_group,
		       "format",
		       str);
  g_free(str);

  /* samplerate */
  samplerate = gtk_spin_button_get_value(soundcard_editor->samplerate);

  str = g_strdup_printf("%u",
			samplerate);

  ags_config_set_value(config,
		       soundcard_group,
		       "samplerate",
		       str);
  g_free(str);
  
  /* device */
  model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card)));
  
  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(soundcard_editor->card),
				   &current)){
    gtk_tree_model_get_value(GTK_TREE_MODEL(model),
			     &current,
			     0,
			     &value);
    device = g_value_get_string(&value);

    if(!g_ascii_strcasecmp(device, "(null)")){
      device = NULL;
    }
  }else{
    device = NULL;
  }

  /* handle */
#ifdef AGS_DEBUG
  g_message("%s", device);
#endif

  if(soundcard_editor->soundcard != NULL){
    if(use_core_audio){
      ags_config_set_value(config,
			   soundcard_group,
			   "device",
			   device);
    }else if(use_pulse){
      ags_config_set_value(config,
			   soundcard_group,
			   "device",
			   device);
    }else if(use_jack){
      ags_config_set_value(config,
			   soundcard_group,
			   "device",
			   device);
    }else if(use_wasapi){
      ags_config_set_value(config,
			   soundcard_group,
			   "device",
			   device);
    }else if(use_alsa){
      ags_config_set_value(config,
			   soundcard_group,
			   "device",
			   device);
    }else if(use_oss){
      ags_config_set_value(config,
			   soundcard_group,
			   "device",
			   device);
    }
  }
  
  if(AGS_IS_PULSE_DEVOUT(soundcard_editor->soundcard) ||
     AGS_IS_CORE_AUDIO_DEVOUT(soundcard_editor->soundcard)){
    ags_config_set_value(config,
			 soundcard_group,
			 "use-cache",
			 (gtk_check_button_get_active(soundcard_editor->use_cache) ? "true": "false"));
  
    cache_buffer_size = gtk_spin_button_get_value(soundcard_editor->cache_buffer_size);
    str = g_strdup_printf("%u",
			  cache_buffer_size);
    ags_config_set_value(config,
			 soundcard_group,
			 "cache-buffer-size",
			 str);
    g_free(str);
  }

#if defined AGS_WITH_WASAPI
  wasapi_share_mode = gtk_combo_box_text_get_active_text(soundcard_editor->wasapi_share_mode);
  ags_config_set_value(config,
		       soundcard_group,
		       "wasapi-share-mode",
		       wasapi_share_mode);

  /* buffer size */
  wasapi_buffer_size = gtk_spin_button_get_value(soundcard_editor->wasapi_buffer_size);
  str = g_strdup_printf("%u",
			wasapi_buffer_size);
  ags_config_set_value(config,
		       soundcard_group,
		       "wasapi-buffer-size",
		       str);
  g_free(str);
#endif
}

void
ags_soundcard_editor_reset(AgsApplicable *applicable)
{
  AgsAudioPreferences *audio_preferences;
  AgsSoundcardEditor *soundcard_editor;

  GObject *soundcard;

  GList *start_list;	
  GList *card_id, *start_card_id;
  GList *card_name, *start_card_name;

  gchar *backend, *device, *tmp;
#if defined AGS_WITH_WASAPI
  gchar *wasapi_share_mode;
#endif

  guint capability;
  guint nth;
  guint first_samplerate;
  guint first_buffer_size;
  guint channels, channels_min, channels_max;
  guint samplerate, samplerate_min, samplerate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;
  guint format;
#if defined AGS_WITH_WASAPI
  guint wasapi_buffer_size;
#endif
  gboolean found_card;
  
  GError *error;
  
  soundcard_editor = AGS_SOUNDCARD_EDITOR(applicable);

  if((AGS_SOUNDCARD_EDITOR_BLOCK_RESET & (soundcard_editor->flags)) != 0){
    return;
  }

  soundcard_editor->flags |= AGS_SOUNDCARD_EDITOR_BLOCK_RESET;

  soundcard = soundcard_editor->soundcard;

  audio_preferences = (AgsAudioPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
								      AGS_TYPE_AUDIO_PREFERENCES);

  /* first buffer size and samplerate */
  start_list = ags_audio_preferences_get_soundcard_editor(audio_preferences);

  if(start_list != NULL){
    first_buffer_size = gtk_spin_button_get_value(AGS_SOUNDCARD_EDITOR(start_list->data)->buffer_size);
    
    first_samplerate = gtk_spin_button_get_value(AGS_SOUNDCARD_EDITOR(start_list->data)->samplerate);
  }
  
  g_list_free(start_list);
  
  /* refresh */
  backend = NULL;
  capability = 0;
  
  if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    backend = "core-audio";

    capability = AGS_SOUNDCARD_CAPABILITY_PLAYBACK;
  }else if(AGS_IS_PULSE_DEVOUT(soundcard)){
    backend = "pulse";

    capability = AGS_SOUNDCARD_CAPABILITY_PLAYBACK;
  }else if(AGS_IS_JACK_DEVOUT(soundcard)){
    backend = "jack";

    capability = AGS_SOUNDCARD_CAPABILITY_PLAYBACK;
  }else if(AGS_IS_WASAPI_DEVOUT(soundcard)){
    backend = "wasapi";

    capability = AGS_SOUNDCARD_CAPABILITY_PLAYBACK;
  }else if(AGS_IS_ALSA_DEVOUT(soundcard)){
    backend = "alsa";

    capability = AGS_SOUNDCARD_CAPABILITY_PLAYBACK;
  }else if(AGS_IS_OSS_DEVOUT(soundcard)){
    backend = "oss";

    capability = AGS_SOUNDCARD_CAPABILITY_PLAYBACK;
  }else if(AGS_IS_CORE_AUDIO_DEVIN(soundcard)){
    backend = "core-audio";

    capability = AGS_SOUNDCARD_CAPABILITY_CAPTURE;
  }else if(AGS_IS_PULSE_DEVIN(soundcard)){
    backend = "pulse";

    capability = AGS_SOUNDCARD_CAPABILITY_CAPTURE;
  }else if(AGS_IS_JACK_DEVIN(soundcard)){
    backend = "jack";

    capability = AGS_SOUNDCARD_CAPABILITY_CAPTURE;
  }else if(AGS_IS_WASAPI_DEVIN(soundcard)){
    backend = "wasapi";

    capability = AGS_SOUNDCARD_CAPABILITY_CAPTURE;
  }else if(AGS_IS_ALSA_DEVIN(soundcard)){
    backend = "alsa";

    capability = AGS_SOUNDCARD_CAPABILITY_CAPTURE;
  }else if(AGS_IS_OSS_DEVIN(soundcard)){
    backend = "oss";

    capability = AGS_SOUNDCARD_CAPABILITY_CAPTURE;
  }

  if(backend != NULL){
    GtkTreeModel *model;
    GtkTreeIter current;
    
    gint nth_backend;
    guint i;
    
    GValue value =  {0,};
    
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->backend));
    nth_backend = -1;
    
    if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model),
				     &current)){
      i = 0;
      
      do{
	gchar *str;
	
	gtk_tree_model_get_value(GTK_TREE_MODEL(model),
				 &current,
				 0,
				 &value);

	str = g_value_get_string(&value);
		
	if(str != NULL &&
	   !g_strcmp0(backend,
		      str)){
	  nth_backend = i;

	  g_value_unset(&value);
	  
	  break;
	}

	g_value_unset(&value);
	
	i++;
      }while(gtk_tree_model_iter_next(GTK_TREE_MODEL(model),
				      &current));
    }
    
    if(!g_ascii_strncasecmp(backend,
			    "core-audio",
			    11)){
#ifdef AGS_WITH_CORE_AUDIO
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       nth_backend);
#else
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       -1);
#endif
      
      // ags_soundcard_editor_load_core_audio_card(soundcard_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "pulse",
				  6)){
#ifdef AGS_WITH_PULSE
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       nth_backend);
#else
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       -1);
#endif
      
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->capability),
			       0);
      //      ags_soundcard_editor_load_pulse_card(soundcard_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "jack",
				  5)){
#ifdef AGS_WITH_JACK
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       nth_backend);
#else
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       -1);
#endif

      //      ags_soundcard_editor_load_jack_card(soundcard_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "wasapi",
				  7)){
#ifdef AGS_WITH_WASAPI
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       nth_backend);
#else
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       -1);
#endif

      //      ags_soundcard_editor_load_wasapi_card(soundcard_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "alsa",
				  5)){
#ifdef AGS_WITH_ALSA
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       nth_backend);
#else
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       -1);
#endif

      //      ags_soundcard_editor_load_alsa_card(soundcard_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "oss",
				  4)){
#ifdef AGS_WITH_OSS      
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       nth_backend);      
#else
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       -1);
#endif
      
      //      ags_soundcard_editor_load_oss_card(soundcard_editor);
    }
  }

  /*  */
  device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard));
  
  card_id = 
    start_card_id = NULL;

  card_name = 
    start_card_name = NULL;

  ags_soundcard_list_cards(AGS_SOUNDCARD(soundcard),
			   &start_card_id, &start_card_name);

  card_id = start_card_id;
  card_name = start_card_name;
  
  nth = 0;
  found_card = FALSE;

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

  while(card_id != NULL){
    //FIXME:JK: work-around for alsa-handle
#ifdef AGS_WITH_CORE_AUDIO
    tmp = card_name->data;
#else
    tmp = card_id->data;
#endif
        
    if(tmp != NULL &&
       device != NULL){
      if(!g_ascii_strcasecmp(tmp,
			     device)){
	found_card = TRUE;
      }
    }else{
      if(tmp == NULL &&
	 device == NULL){
	found_card = TRUE;
      }
    }

    if(tmp != NULL){
      gtk_combo_box_text_append_text(soundcard_editor->card,
				     tmp);
    }
        
    if(!found_card){
      nth++;
    }
    
    card_id = card_id->next;
    card_name = card_name->next;
  }
  
  if(!found_card){
    nth = 0;
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->card),
			   nth);

  g_list_free_full(start_card_id,
		   (GDestroyNotify) g_free);

  g_list_free_full(start_card_name,
		   (GDestroyNotify) g_free);

  /* capability */
  switch(capability){
  case AGS_SOUNDCARD_CAPABILITY_PLAYBACK:
    {
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->capability),
			       0);
    }
    break;
  case AGS_SOUNDCARD_CAPABILITY_CAPTURE:
    {
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->capability),
			       1);
    }
    break;
  case AGS_SOUNDCARD_CAPABILITY_DUPLEX:
    {
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->capability),
			       2);
    }
    break;
  }
  
  /* set range */
  if(device != NULL &&
     soundcard != NULL){
    error = NULL;
    ags_soundcard_pcm_info(AGS_SOUNDCARD(soundcard),
			   device,
			   &channels_min, &channels_max,
			   &samplerate_min, &samplerate_max,
			   &buffer_size_min, &buffer_size_max,
			   &error);

    if(error != NULL){
      channels_min = (gdouble) AGS_SOUNDCARD_MIN_PCM_CHANNELS;
      channels_max = (gdouble) AGS_SOUNDCARD_MAX_PCM_CHANNELS;
      samplerate_min = (gdouble) AGS_SOUNDCARD_MIN_SAMPLERATE;
      samplerate_max = (gdouble) AGS_SOUNDCARD_MAX_SAMPLERATE;
      buffer_size_min = (gdouble) AGS_SOUNDCARD_MIN_BUFFER_SIZE;
      buffer_size_max = (gdouble) AGS_SOUNDCARD_MAX_BUFFER_SIZE;

      g_message("%s", error->message);
      
      g_error_free(error);
    }
  }else{
    channels_min = (gdouble) AGS_SOUNDCARD_MIN_PCM_CHANNELS;
    channels_max = (gdouble) AGS_SOUNDCARD_MAX_PCM_CHANNELS;
    samplerate_min = (gdouble) AGS_SOUNDCARD_MIN_SAMPLERATE;
    samplerate_max = (gdouble) AGS_SOUNDCARD_MAX_SAMPLERATE;
    buffer_size_min = (gdouble) AGS_SOUNDCARD_MIN_BUFFER_SIZE;
    buffer_size_max = (gdouble) AGS_SOUNDCARD_MAX_BUFFER_SIZE;
  }
  
  gtk_spin_button_set_range(soundcard_editor->audio_channels,
			    channels_min, channels_max);
  gtk_spin_button_set_range(soundcard_editor->samplerate,
			    samplerate_min, samplerate_max);
  gtk_spin_button_set_range(soundcard_editor->buffer_size,
			    buffer_size_min, buffer_size_max);

  /* get presets */
  channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  if(soundcard != NULL){
    ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			      &channels,
			      &samplerate,
			      &buffer_size,
			      &format);
  }
  
  /* format */
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     0);
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     1);
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     2);
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     3);
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     4);
    break;
  case AGS_SOUNDCARD_FLOAT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     5);
    break;
  case AGS_SOUNDCARD_DOUBLE:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     6);
    break;
  }

  /* set presets value */
  gtk_spin_button_set_value(soundcard_editor->audio_channels,
			    (gdouble) channels);
  gtk_spin_button_set_value(soundcard_editor->samplerate,
			    (gdouble) samplerate);
  gtk_spin_button_set_value(soundcard_editor->buffer_size,
			    (gdouble) buffer_size);

  soundcard_editor->flags &= (~AGS_SOUNDCARD_EDITOR_BLOCK_RESET);

  if(AGS_IS_PULSE_DEVOUT(soundcard)){
    GList *start_port, *port;

    guint cache_buffer_size;
    gboolean use_cache;
    
    GRecMutex *pulse_port_mutex;
    
    g_object_get(soundcard,
		 "pulse-port", &start_port,
		 NULL);

    port = start_port;

    if(port != NULL){
      pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(port->data);

      /* read use cache and cache buffer size */
      g_rec_mutex_lock(pulse_port_mutex);

      use_cache = AGS_PULSE_PORT(port->data)->use_cache;
      cache_buffer_size = AGS_PULSE_PORT(port->data)->cache_buffer_size;
      
      g_rec_mutex_unlock(pulse_port_mutex);

      /* reset */
      gtk_check_button_set_active(soundcard_editor->use_cache,
				  use_cache);

      gtk_spin_button_set_value(soundcard_editor->cache_buffer_size,
				cache_buffer_size);
    }

    g_list_free_full(start_port,
		     g_object_unref);
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    GList *start_port, *port;

    guint cache_buffer_size;
    gboolean use_cache;
    
    GRecMutex *core_audio_port_mutex;
    
    g_object_get(soundcard,
		 "core-audio-port", &start_port,
		 NULL);

    port = start_port;

    if(port != NULL){
      core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(port->data);

      /* read use cache and cache buffer size */
      g_rec_mutex_lock(core_audio_port_mutex);

      use_cache = AGS_CORE_AUDIO_PORT(port->data)->use_cache;
      cache_buffer_size = AGS_CORE_AUDIO_PORT(port->data)->cache_buffer_size;
      
      g_rec_mutex_unlock(core_audio_port_mutex);

      /* reset */
      gtk_check_button_set_active(soundcard_editor->use_cache,
				  use_cache);

      gtk_spin_button_set_value(soundcard_editor->cache_buffer_size,
				(gdouble) cache_buffer_size);
    }

    g_list_free_full(start_port,
		     g_object_unref);
  }else{
    gtk_check_button_set_active(soundcard_editor->use_cache,
				FALSE);
  }

#if defined AGS_WITH_WASAPI
  if(AGS_IS_WASAPI_DEVOUT(soundcard)){
    GRecMutex *mutex;

    mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(AGS_WASAPI_DEVOUT(soundcard));

    g_rec_mutex_lock(mutex);
    
    wasapi_buffer_size = AGS_WASAPI_DEVOUT(soundcard)->wasapi_buffer_size;
    
    g_rec_mutex_unlock(mutex);

    gtk_spin_button_set_value(soundcard_editor->wasapi_buffer_size,
			      (gdouble) wasapi_buffer_size);

    if(ags_wasapi_devout_test_flags(soundcard,
				    AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE)){
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->wasapi_share_mode),
			       0);
    }else{
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->wasapi_share_mode),
			       1);
    }
  }else if(AGS_IS_WASAPI_DEVIN(soundcard)){
    GRecMutex *mutex;

    mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(AGS_WASAPI_DEVIN(soundcard));

    g_rec_mutex_lock(mutex);
    
    wasapi_buffer_size = AGS_WASAPI_DEVIN(soundcard)->wasapi_buffer_size;
    
    g_rec_mutex_unlock(mutex);

    gtk_spin_button_set_value(soundcard_editor->wasapi_buffer_size,
			      (gdouble) wasapi_buffer_size);

    if(ags_wasapi_devin_test_flags(soundcard,
				   AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE)){
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->wasapi_share_mode),
			       0);
    }else{
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->wasapi_share_mode),
			       1);
    }    
  }
#endif
}

void
ags_soundcard_editor_load_core_audio_card(AgsSoundcardEditor *soundcard_editor)
{
  //empty
}

void
ags_soundcard_editor_load_pulse_card(AgsSoundcardEditor *soundcard_editor)
{
  //empty
}

void
ags_soundcard_editor_load_jack_card(AgsSoundcardEditor *soundcard_editor)
{
  //empty
}

void
ags_soundcard_editor_load_wasapi_card(AgsSoundcardEditor *soundcard_editor)
{
  //empty
}

void
ags_soundcard_editor_load_alsa_card(AgsSoundcardEditor *soundcard_editor)
{
  //empty
}

void
ags_soundcard_editor_load_oss_card(AgsSoundcardEditor *soundcard_editor)
{
  //empty
}

void
ags_soundcard_editor_show_wasapi_control(AgsSoundcardEditor *soundcard_editor)
{
  if(!AGS_IS_SOUNDCARD_EDITOR(soundcard_editor)){
    return;
  }

#if defined(AGS_WITH_WASAPI)
  gtk_widget_show(soundcard_editor->wasapi_share_mode_label);
  gtk_widget_show(soundcard_editor->wasapi_share_mode);

  gtk_widget_show(soundcard_editor->wasapi_buffer_size_label);
  gtk_widget_show(soundcard_editor->wasapi_buffer_size);
#endif
}

void
ags_soundcard_editor_hide_wasapi_control(AgsSoundcardEditor *soundcard_editor)
{
  if(!AGS_IS_SOUNDCARD_EDITOR(soundcard_editor)){
    return;
  }

#if defined(AGS_WITH_WASAPI)
  gtk_widget_hide(soundcard_editor->wasapi_share_mode_label);
  gtk_widget_hide(soundcard_editor->wasapi_share_mode);

  gtk_widget_hide(soundcard_editor->wasapi_buffer_size_label);
  gtk_widget_hide(soundcard_editor->wasapi_buffer_size);
#endif
}

/**
 * ags_soundcard_editor_new:
 *
 * Create a new instance of #AgsSoundcardEditor
 *
 * Returns: the new #AgsSoundcardEditor
 *
 * Since: 3.0.0
 */
AgsSoundcardEditor*
ags_soundcard_editor_new()
{
  AgsSoundcardEditor *soundcard_editor;

  soundcard_editor = (AgsSoundcardEditor *) g_object_new(AGS_TYPE_SOUNDCARD_EDITOR,
							 NULL);
  
  return(soundcard_editor);
}
