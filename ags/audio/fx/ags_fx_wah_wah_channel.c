/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_wah_wah_channel.h>

#include <ags/plugin/ags_plugin_port.h>

#include <complex.h>

#include <ags/i18n.h>

void ags_fx_wah_wah_channel_class_init(AgsFxWahWahChannelClass *fx_wah_wah_channel);
void ags_fx_wah_wah_channel_init(AgsFxWahWahChannel *fx_wah_wah_channel);
void ags_fx_wah_wah_channel_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_fx_wah_wah_channel_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_fx_wah_wah_channel_dispose(GObject *gobject);
void ags_fx_wah_wah_channel_finalize(GObject *gobject);

static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_enabled_plugin_port();
static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_length_mode_plugin_port();
static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_fixed_length_plugin_port();
static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_attack_plugin_port();
static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_decay_plugin_port();
static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_sustain_plugin_port();
static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_release_plugin_port();
static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_ratio_plugin_port();
static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_lfo_freq_plugin_port();
static AgsPluginPort* ags_fx_wah_wah_channel_get_wah_wah_tuning_plugin_port();

/**
 * SECTION:ags_fx_wah_wah_channel
 * @short_description: fx wah_wah channel
 * @title: AgsFxWahWahChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_wah_wah_channel.h
 *
 * The #AgsFxWahWahChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_wah_wah_channel_parent_class = NULL;

const gchar *ags_fx_wah_wah_channel_plugin_name = "ags-fx-wah-wah";

const gchar* ags_fx_wah_wah_channel_specifier[] = {
  "./wah-wah-enabled[0]",
  "./wah-wah-length-mode[0]",
  "./wah-wah-fixed-length[0]",
  "./wah-wah-attack[0]",
  "./wah-wah-decay[0]",
  "./wah-wah-sustain[0]",
  "./wah-wah-release[0]",
  "./wah-wah-ratio[0]",
  "./wah-wah-lfo-depth[0]",
  "./wah-wah-lfo-freq[0]",
  "./wah-wah-tuning[0]",
  NULL,
};

const gchar* ags_fx_wah_wah_channel_control_port[] = {
  "1/11",
  "2/11",
  "3/11",
  "4/11",
  "5/11",
  "6/11",
  "7/11",
  "8/11",
  "9/11",
  "10/11",
  "11/11",
  NULL,
};

enum{
  PROP_0,
  PROP_WAH_WAH_ENABLED,
  PROP_WAH_WAH_LENGTH_MODE,
  PROP_WAH_WAH_FIXED_LENGTH,
  PROP_WAH_WAH_ATTACK,
  PROP_WAH_WAH_DECAY,
  PROP_WAH_WAH_SUSTAIN,
  PROP_WAH_WAH_RELEASE,
  PROP_WAH_WAH_RATIO,
  PROP_WAH_WAH_LFO_DEPTH,
  PROP_WAH_WAH_LFO_FREQ,
  PROP_WAH_WAH_TUNING,
};

GType
ags_fx_wah_wah_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_wah_wah_channel = 0;

    static const GTypeInfo ags_fx_wah_wah_channel_info = {
      sizeof (AgsFxWahWahChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_wah_wah_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxWahWahChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_wah_wah_channel_init,
    };

    ags_type_fx_wah_wah_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							 "AgsFxWahWahChannel",
							 &ags_fx_wah_wah_channel_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_wah_wah_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_wah_wah_channel_class_init(AgsFxWahWahChannelClass *fx_wah_wah_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_wah_wah_channel_parent_class = g_type_class_peek_parent(fx_wah_wah_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_wah_wah_channel;

  gobject->set_property = ags_fx_wah_wah_channel_set_property;
  gobject->get_property = ags_fx_wah_wah_channel_get_property;

  gobject->dispose = ags_fx_wah_wah_channel_dispose;
  gobject->finalize = ags_fx_wah_wah_channel_finalize;

  /* properties */
  /**
   * AgsFxWahWahChannel:wah-wah-enabled:
   *
   * The enabled switch.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-enabled",
				   i18n_pspec("enabled of recall"),
				   i18n_pspec("The recall's enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_ENABLED,
				  param_spec);

  /**
   * AgsFxWahWahChannel:wah-wah-length-mode:
   *
   * The length mode.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-length-mode",
				   i18n_pspec("length mode of recall"),
				   i18n_pspec("The recall's length mode"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_LENGTH_MODE,
				  param_spec);

  /**
   * AgsFxWahWahChannel:wah-wah-fixed-length:
   *
   * The fixed length.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-fixed-length",
				   i18n_pspec("fixed length of recall"),
				   i18n_pspec("The recall's fixed length"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_FIXED_LENGTH,
				  param_spec);

  /**
   * AgsFxWahWahChannel:wah-wah-attack:
   *
   * The attack.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-attack",
				   i18n_pspec("attack of recall"),
				   i18n_pspec("The recall's attack"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_ATTACK,
				  param_spec);

  /**
   * AgsFxWahWahChannel:wah-wah-decay:
   *
   * The decay.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-decay",
				   i18n_pspec("decay of recall"),
				   i18n_pspec("The recall's decay"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_DECAY,
				  param_spec);

  /**
   * AgsFxWahWahChannel:wah-wah-sustain:
   *
   * The sustain.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-sustain",
				   i18n_pspec("sustain of recall"),
				   i18n_pspec("The recall's sustain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_SUSTAIN,
				  param_spec);

  /**
   * AgsFxWahWahChannel:wah-wah-release:
   *
   * The release.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-release",
				   i18n_pspec("release of recall"),
				   i18n_pspec("The recall's release"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_RELEASE,
				  param_spec);

  /**
   * AgsFxWahWahChannel:wah-wah-ratio:
   *
   * The ratio.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-ratio",
				   i18n_pspec("ratio of recall"),
				   i18n_pspec("The recall's ratio"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_RATIO,
				  param_spec);

  /**
   * AgsFxWahWahChannel:wah-wah-lfo-depth:
   *
   * The lfo_depth.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-lfo-depth",
				   i18n_pspec("LFO depth of recall"),
				   i18n_pspec("The recall's LFO depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_LFO_DEPTH,
				  param_spec);

  /**
   * AgsFxWahWahChannel:wah-wah-lfo-freq:
   *
   * The lfo_freq.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-lfo-freq",
				   i18n_pspec("LFO freq of recall"),
				   i18n_pspec("The recall's LFO freq"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_LFO_FREQ,
				  param_spec);

  /**
   * AgsFxWahWahChannel:wah-wah-tuning:
   *
   * The tuning.
   * 
   * Since: 5.2.0
   */
  param_spec = g_param_spec_object("wah-wah-tuning",
				   i18n_pspec("tuning of recall"),
				   i18n_pspec("The recall's tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAH_WAH_TUNING,
				  param_spec);
}

void
ags_fx_wah_wah_channel_init(AgsFxWahWahChannel *fx_wah_wah_channel)
{
  gint position;
  
  AGS_RECALL(fx_wah_wah_channel)->name = "ags-fx-wah-wah";
  AGS_RECALL(fx_wah_wah_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_wah_wah_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_wah_wah_channel)->xml_type = "ags-fx-wah-wah-channel";

  fx_wah_wah_channel->wah_wah_mode = AGS_FX_WAH_WAH_CHANNEL_MODE_STREAM_LENGTH;

  position = 0;
  
  /* wah-wah enabled */
  fx_wah_wah_channel->wah_wah_enabled = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_wah_wah_channel_plugin_name,
						     "specifier", ags_fx_wah_wah_channel_specifier[position],
						     "control-port", ags_fx_wah_wah_channel_control_port[position],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_wah_wah_channel->wah_wah_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_wah_wah_channel->wah_wah_enabled,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_enabled);

  position++;
  
  /* wah-wah length mode */
  fx_wah_wah_channel->wah_wah_length_mode = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_wah_wah_channel_plugin_name,
							 "specifier", ags_fx_wah_wah_channel_specifier[position],
							 "control-port", ags_fx_wah_wah_channel_control_port[position],
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_wah_wah_channel->wah_wah_length_mode->port_value.ags_port_float = (gfloat) AGS_FX_WAH_WAH_4_4_TH;

  g_object_set(fx_wah_wah_channel->wah_wah_length_mode,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_length_mode_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_length_mode);

  position++;
  
  /* wah-wah fixed length */
  fx_wah_wah_channel->wah_wah_fixed_length = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_wah_wah_channel_plugin_name,
							  "specifier", ags_fx_wah_wah_channel_specifier[position],
							  "control-port", ags_fx_wah_wah_channel_control_port[position],
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_wah_wah_channel->wah_wah_fixed_length->port_value.ags_port_float = (gfloat) 2.0 * M_PI;

  g_object_set(fx_wah_wah_channel->wah_wah_fixed_length,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_fixed_length_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_fixed_length);

  position++;
  
  /* wah-wah attack */
  fx_wah_wah_channel->wah_wah_attack = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_wah_wah_channel_plugin_name,
						    "specifier", ags_fx_wah_wah_channel_specifier[position],
						    "control-port", ags_fx_wah_wah_channel_control_port[position],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);

  fx_wah_wah_channel->wah_wah_attack->port_value.ags_port_float = (gfloat) (0.25 + 1.0 * I);

  g_object_set(fx_wah_wah_channel->wah_wah_attack,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_attack);

  position++;
  
  /* wah-wah decay */
  fx_wah_wah_channel->wah_wah_decay = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_wah_wah_channel_plugin_name,
						   "specifier", ags_fx_wah_wah_channel_specifier[position],
						   "control-port", ags_fx_wah_wah_channel_control_port[position],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_wah_wah_channel->wah_wah_decay->port_value.ags_port_float = (gfloat) (0.25 + 1.0 * I);

  g_object_set(fx_wah_wah_channel->wah_wah_decay,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_decay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_decay);

  position++;
  
  /* wah-wah sustain */
  fx_wah_wah_channel->wah_wah_sustain = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_wah_wah_channel_plugin_name,
						     "specifier", ags_fx_wah_wah_channel_specifier[position],
						     "control-port", ags_fx_wah_wah_channel_control_port[position],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_wah_wah_channel->wah_wah_sustain->port_value.ags_port_float = (gfloat) (0.25 + 1.0 * I);

  g_object_set(fx_wah_wah_channel->wah_wah_sustain,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_sustain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_sustain);
  
  position++;
  
  /* wah-wah release */
  fx_wah_wah_channel->wah_wah_release = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_wah_wah_channel_plugin_name,
						     "specifier", ags_fx_wah_wah_channel_specifier[position],
						     "control-port", ags_fx_wah_wah_channel_control_port[position],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_wah_wah_channel->wah_wah_release->port_value.ags_port_float = (gfloat) (0.25 + 1.0 * I);

  g_object_set(fx_wah_wah_channel->wah_wah_release,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_release_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_release);

  position++;
  
  /* wah-wah ratio */
  fx_wah_wah_channel->wah_wah_ratio = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_wah_wah_channel_plugin_name,
						   "specifier", ags_fx_wah_wah_channel_specifier[position],
						   "control-port", ags_fx_wah_wah_channel_control_port[position],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_wah_wah_channel->wah_wah_ratio->port_value.ags_port_float = 0.0;

  g_object_set(fx_wah_wah_channel->wah_wah_ratio,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_ratio_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_ratio);

  position++;
  
  /* wah-wah LFO depth */
  fx_wah_wah_channel->wah_wah_lfo_depth = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_wah_wah_channel_plugin_name,
						       "specifier", ags_fx_wah_wah_channel_specifier[position],
						       "control-port", ags_fx_wah_wah_channel_control_port[position],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_wah_wah_channel->wah_wah_lfo_depth->port_value.ags_port_float = 0.0;

  g_object_set(fx_wah_wah_channel->wah_wah_lfo_depth,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_lfo_depth);

  position++;
  
  /* wah-wah LFO freq */
  fx_wah_wah_channel->wah_wah_lfo_freq = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_wah_wah_channel_plugin_name,
						       "specifier", ags_fx_wah_wah_channel_specifier[position],
						       "control-port", ags_fx_wah_wah_channel_control_port[position],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_wah_wah_channel->wah_wah_lfo_freq->port_value.ags_port_float = 8.179;

  g_object_set(fx_wah_wah_channel->wah_wah_lfo_freq,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_lfo_freq_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_lfo_freq);

  position++;
  
  /* wah-wah tuning */
  fx_wah_wah_channel->wah_wah_tuning = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_wah_wah_channel_plugin_name,
						    "specifier", ags_fx_wah_wah_channel_specifier[position],
						    "control-port", ags_fx_wah_wah_channel_control_port[position],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_wah_wah_channel->wah_wah_tuning->port_value.ags_port_float = 0.0;

  g_object_set(fx_wah_wah_channel->wah_wah_tuning,
	       "plugin-port", ags_fx_wah_wah_channel_get_wah_wah_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_wah_wah_channel,
		      fx_wah_wah_channel->wah_wah_tuning);

  position++;  
}

void
ags_fx_wah_wah_channel_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsFxWahWahChannel *fx_wah_wah_channel;

  GRecMutex *recall_mutex;

  fx_wah_wah_channel = AGS_FX_WAH_WAH_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_wah_wah_channel);

  switch(prop_id){
  case PROP_WAH_WAH_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_enabled != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_LENGTH_MODE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_length_mode){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_length_mode != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_length_mode));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_length_mode = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_FIXED_LENGTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_fixed_length){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_fixed_length != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_fixed_length));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_fixed_length = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_ATTACK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_attack){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_attack != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_attack));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_attack = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_DECAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_decay){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_decay != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_decay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_decay = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_SUSTAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_sustain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_sustain != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_sustain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_sustain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_RELEASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_release){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_release != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_release));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_release = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_RATIO:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_ratio){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_ratio != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_ratio));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_ratio = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_LFO_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_lfo_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_lfo_depth != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_lfo_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_lfo_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_LFO_FREQ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_lfo_freq){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_lfo_freq != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_lfo_freq));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_lfo_freq = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_wah_wah_channel->wah_wah_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_wah_wah_channel->wah_wah_tuning != NULL){
	g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_wah_wah_channel->wah_wah_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_wah_wah_channel_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsFxWahWahChannel *fx_wah_wah_channel;

  GRecMutex *recall_mutex;

  fx_wah_wah_channel = AGS_FX_WAH_WAH_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_wah_wah_channel);

  switch(prop_id){
  case PROP_WAH_WAH_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_LENGTH_MODE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_length_mode);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_FIXED_LENGTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_fixed_length);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_ATTACK:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_attack);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_DECAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_decay);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_SUSTAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_sustain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_RELEASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_release);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_RATIO:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_ratio);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_LFO_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_lfo_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_LFO_FREQ:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_lfo_freq);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_WAH_WAH_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_wah_wah_channel->wah_wah_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_wah_wah_channel_dispose(GObject *gobject)
{
  AgsFxWahWahChannel *fx_wah_wah_channel;
  
  fx_wah_wah_channel = AGS_FX_WAH_WAH_CHANNEL(gobject);
  
  /* fixed length */
  if(fx_wah_wah_channel->wah_wah_enabled != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_enabled));

    fx_wah_wah_channel->wah_wah_enabled = NULL;
  }  

  /* length mode */
  if(fx_wah_wah_channel->wah_wah_length_mode != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_length_mode));

    fx_wah_wah_channel->wah_wah_length_mode = NULL;
  }  

  /* fixed length */
  if(fx_wah_wah_channel->wah_wah_fixed_length != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_fixed_length));

    fx_wah_wah_channel->wah_wah_fixed_length = NULL;
  }  

  /* attack */
  if(fx_wah_wah_channel->wah_wah_attack != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_attack));

    fx_wah_wah_channel->wah_wah_attack = NULL;
  }  

  /* decay */
  if(fx_wah_wah_channel->wah_wah_decay != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_decay));

    fx_wah_wah_channel->wah_wah_decay = NULL;
  }  

  /* sustain */
  if(fx_wah_wah_channel->wah_wah_sustain != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_sustain));

    fx_wah_wah_channel->wah_wah_sustain = NULL;
  }  

  /* release */
  if(fx_wah_wah_channel->wah_wah_release != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_release));

    fx_wah_wah_channel->wah_wah_release = NULL;
  }  

  /* ratio */
  if(fx_wah_wah_channel->wah_wah_ratio != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_ratio));

    fx_wah_wah_channel->wah_wah_ratio = NULL;
  }  

  /* LFO depth */
  if(fx_wah_wah_channel->wah_wah_lfo_depth != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_lfo_depth));

    fx_wah_wah_channel->wah_wah_lfo_depth = NULL;
  }  

  /* LFO freq */
  if(fx_wah_wah_channel->wah_wah_lfo_freq != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_lfo_freq));

    fx_wah_wah_channel->wah_wah_lfo_freq = NULL;
  }  

  /* tuning */
  if(fx_wah_wah_channel->wah_wah_tuning != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_tuning));

    fx_wah_wah_channel->wah_wah_tuning = NULL;
  }  

  /* call parent */
  G_OBJECT_CLASS(ags_fx_wah_wah_channel_parent_class)->dispose(gobject);
}

void
ags_fx_wah_wah_channel_finalize(GObject *gobject)
{
  AgsFxWahWahChannel *fx_wah_wah_channel;
  
  fx_wah_wah_channel = AGS_FX_WAH_WAH_CHANNEL(gobject);

  /* fixed length */
  if(fx_wah_wah_channel->wah_wah_enabled != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_enabled));
  }

  /* length mode */
  if(fx_wah_wah_channel->wah_wah_length_mode != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_length_mode));
  }

  /* fixed length */
  if(fx_wah_wah_channel->wah_wah_fixed_length != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_fixed_length));
  }

  /* attack */
  if(fx_wah_wah_channel->wah_wah_attack != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_attack));
  }

  /* decay */
  if(fx_wah_wah_channel->wah_wah_decay != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_decay));
  }

  /* sustain */
  if(fx_wah_wah_channel->wah_wah_sustain != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_sustain));
  }

  /* release */
  if(fx_wah_wah_channel->wah_wah_release != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_release));
  }

  /* ratio */
  if(fx_wah_wah_channel->wah_wah_ratio != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_ratio));
  }

  /* LFO depth */
  if(fx_wah_wah_channel->wah_wah_lfo_depth != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_lfo_depth));
  }

  /* LFO freq */
  if(fx_wah_wah_channel->wah_wah_lfo_freq != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_lfo_freq));
  }

  /* tuning */
  if(fx_wah_wah_channel->wah_wah_tuning != NULL){
    g_object_unref(G_OBJECT(fx_wah_wah_channel->wah_wah_tuning));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_wah_wah_channel_parent_class)->finalize(gobject);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_enabled_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_TOGGLED);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_length_mode_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) AGS_FX_WAH_WAH_4_4_TH);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) AGS_FX_WAH_WAH_16_16_TH);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_fixed_length_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2.0 * M_PI);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0 * M_PI);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_attack_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) (0.25 + 1.0 * I));
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) (1.0 + 1.0 * I));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_decay_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) (0.25 + 1.0 * I));
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) (1.0 + 1.0 * I));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_sustain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) (0.25 + 1.0 * I));
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) (1.0 + 1.0 * I));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_release_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) (0.25 + 1.0 * I));
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) (1.0 + 1.0 * I));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_ratio_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) (0.0 + 0.0 * I));
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) (1.0 + 0.0 * I));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_lfo_depth_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_lfo_freq_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      6.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      10.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_wah_wah_channel_get_wah_wah_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

/**
 * ags_fx_wah_wah_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxWahWahChannel
 *
 * Returns: the new #AgsFxWahWahChannel
 *
 * Since: 5.2.0
 */
AgsFxWahWahChannel*
ags_fx_wah_wah_channel_new(AgsChannel *channel)
{
  AgsFxWahWahChannel *fx_wah_wah_channel;

  fx_wah_wah_channel = (AgsFxWahWahChannel *) g_object_new(AGS_TYPE_FX_WAH_WAH_CHANNEL,
							   "source", channel,
							   NULL);

  return(fx_wah_wah_channel);
}
