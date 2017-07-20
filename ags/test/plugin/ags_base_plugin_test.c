/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2017 Joël Krähemann
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

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_base_plugin_test_init_suite();
int ags_base_plugin_test_clean_suite();

void ags_port_descriptor_test_find_symbol();
void ags_base_plugin_test_find_filename();
void ags_base_plugin_test_find_effect();
void ags_base_plugin_test_find_ui_effect_index();
void ags_base_plugin_test_sort();
void ags_base_plugin_test_apply_port_group_by_prefix();
void ags_base_plugin_test_instantiate();
void ags_base_plugin_test_connect_port();
void ags_base_plugin_test_activate();
void ags_base_plugin_test_deactivate();
void ags_base_plugin_test_run();
void ags_base_plugin_test_load_plugin();

gpointer ags_base_plugin_test_stub_instantiate(AgsBasePlugin *base_plugin,
					       guint samplerate);
void ags_base_plugin_test_stub_connect_port(AgsBasePlugin *base_plugin,
					    gpointer plugin_handle,
					    guint port_index,
					    gpointer data_location);
void ags_base_plugin_test_stub_activate(AgsBasePlugin *base_plugin,
					gpointer plugin_handle);
void ags_base_plugin_test_stub_deactivate(AgsBasePlugin *base_plugin,
					  gpointer plugin_handle);
void ags_base_plugin_test_stub_run(AgsBasePlugin *base_plugin,
				   gpointer plugin_handle,
				   snd_seq_event_t *seq_event,
				   guint frame_count);
void ags_base_plugin_test_stub_load_plugin(AgsBasePlugin *base_plugin);

#define AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DELAY "delay"
#define AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DRY "dry"
#define AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_WET "wet"
#define AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_FEEDBACK "feedback"

#define AGS_BASE_PLUGIN_TEST_FIND_FILENAME_DEFAULT "default"

#define AGS_BASE_PLUGIN_TEST_FIND_EFFECT_CMT "cmt.so"
#define AGS_BASE_PLUGIN_TEST_FIND_EFFECT_PINK_NOISE "pink noise"
#define AGS_BASE_PLUGIN_TEST_FIND_EFFECT_ECHO_DELAY "echo delay"
#define AGS_BASE_PLUGIN_TEST_FIND_EFFECT_GVERB "gverb"
#define AGS_BASE_PLUGIN_TEST_FIND_EFFECT_SWH "swh.so"
#define AGS_BASE_PLUGIN_TEST_FIND_EFFECT_AMPLIFIER "amplifier"
#define AGS_BASE_PLUGIN_TEST_FIND_EFFECT_ENVELOPE_TRACKER "envelope tracker"

#define AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_COMPRESSOR "compressor.so"
#define AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_COMPRESSOR_INDEX (3)
#define AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_CHORUS "chorus.so"
#define AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_CHORUS_INDEX (15)
#define AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_FLANGER "flanger.so"
#define AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_FLANGER_INDEX (37)
#define AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_SPATIALIZER "spatializer.so"
#define AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_SPATIALIZER_INDEX (1)

#define AGS_BASE_PLUGIN_TEST_SORT_PINK_NOISE "pink noise"
#define AGS_BASE_PLUGIN_TEST_SORT_ECHO_DELAY "echo delay"
#define AGS_BASE_PLUGIN_TEST_SORT_GVERB "gverb"
#define AGS_BASE_PLUGIN_TEST_SORT_AMPLIFIER "amplifier"
#define AGS_BASE_PLUGIN_TEST_SORT_ENVELOPE_TRACKER "envelope tracker"
#define AGS_BASE_PLUGIN_TEST_SORT_COMPRESSOR "compressor"
#define AGS_BASE_PLUGIN_TEST_SORT_CHORUS "chorus"
#define AGS_BASE_PLUGIN_TEST_SORT_FLANGER "flanger"
#define AGS_BASE_PLUGIN_TEST_SORT_SPATIALIZER "spatializer"

#define AGS_BASE_PLUGIN_TEST_INSTANTIATE_SAMPLERATE (48000)

#define AGS_BASE_PLUGIN_TEST_RUN_FRAME_COUNT (512)

gboolean stub_instantiate = FALSE;
gboolean stub_connect_port = FALSE;
gboolean stub_activate = FALSE;
gboolean stub_deactivate = FALSE;
gboolean stub_run = FALSE;
gboolean stub_load_plugin = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_base_plugin_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_base_plugin_test_clean_suite()
{
  return(0);
}

void
ags_port_descriptor_test_find_symbol()
{
  AgsPortDescriptor *port_descriptor;

  GList *list, *current;

  list = NULL;
  
  /* delay */
  port_descriptor = ags_port_descriptor_alloc();
  port_descriptor->port_symbol = AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DELAY;

  list = g_list_prepend(list,
			port_descriptor);
  
  /* dry */
  port_descriptor = ags_port_descriptor_alloc();
  port_descriptor->port_symbol = AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DRY;

  list = g_list_prepend(list,
			port_descriptor);

  /* wet */
  port_descriptor = ags_port_descriptor_alloc();
  port_descriptor->port_symbol = AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_WET;

  list = g_list_prepend(list,
			port_descriptor);

  /* feedback */
  port_descriptor = ags_port_descriptor_alloc();
  port_descriptor->port_symbol = AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_FEEDBACK;

  list = g_list_prepend(list,
			port_descriptor);

  /* assert find */
  CU_ASSERT((current = ags_port_descriptor_find_symbol(list,
						       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DELAY)) != NULL &&
	    !g_strcmp0(AGS_PORT_DESCRIPTOR(current->data)->port_symbol,
		       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DELAY));

  CU_ASSERT((current = ags_port_descriptor_find_symbol(list,
						       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DRY)) != NULL &&
	    !g_strcmp0(AGS_PORT_DESCRIPTOR(current->data)->port_symbol,
		       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_DRY));

  CU_ASSERT((current = ags_port_descriptor_find_symbol(list,
						       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_WET)) != NULL &&
	    !g_strcmp0(AGS_PORT_DESCRIPTOR(current->data)->port_symbol,
		       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_WET));

  CU_ASSERT((current = ags_port_descriptor_find_symbol(list,
						       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_FEEDBACK)) != NULL &&
	    !g_strcmp0(AGS_PORT_DESCRIPTOR(current->data)->port_symbol,
		       AGS_PORT_DESCRIPTOR_TEST_FIND_SYMBOL_FEEDBACK));
}

void
ags_base_plugin_test_find_filename()
{
  AgsBasePlugin *base_plugin;

  GList *list, *current;
  
  gchar *str[3];

  guint i;

  list = NULL;
  i = 0;

  /* base plugin #0 */
  str[0] = g_strdup_printf("%s %d",
			   AGS_BASE_PLUGIN_TEST_FIND_FILENAME_DEFAULT,
			   i);
  i++;
  
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", str[0],
			     NULL);
  list = g_list_prepend(list,
			base_plugin);
  
  /* base plugin #1 */
  str[1] = g_strdup_printf("%s %d",
			   AGS_BASE_PLUGIN_TEST_FIND_FILENAME_DEFAULT,
			   i);
  i++;
  
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", str[1],
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  /* base plugin #2 */
  str[2] = g_strdup_printf("%s %d",
			   AGS_BASE_PLUGIN_TEST_FIND_FILENAME_DEFAULT,
			   i);
  i++;
  
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", str[2],
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  /* assert find filename */
  CU_ASSERT((current = ags_base_plugin_find_filename(list,
						     str[0])) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       str[0]));
  
  CU_ASSERT((current = ags_base_plugin_find_filename(list,
						     str[1])) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       str[1]));

  CU_ASSERT((current = ags_base_plugin_find_filename(list,
						     str[2])) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       str[2]));
}

void
ags_base_plugin_test_find_effect()
{
  AgsBasePlugin *base_plugin;

  GList *list, *current;

  list = NULL;
  
  /* cmt */
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", AGS_BASE_PLUGIN_TEST_FIND_EFFECT_CMT,
			     "effect", AGS_BASE_PLUGIN_TEST_FIND_EFFECT_PINK_NOISE,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", AGS_BASE_PLUGIN_TEST_FIND_EFFECT_CMT,
			     "effect", AGS_BASE_PLUGIN_TEST_FIND_EFFECT_ECHO_DELAY,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", AGS_BASE_PLUGIN_TEST_FIND_EFFECT_CMT,
			     "effect", AGS_BASE_PLUGIN_TEST_FIND_EFFECT_GVERB,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  /* swh */
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", AGS_BASE_PLUGIN_TEST_FIND_EFFECT_SWH,
			     "effect", AGS_BASE_PLUGIN_TEST_FIND_EFFECT_AMPLIFIER,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "filename", AGS_BASE_PLUGIN_TEST_FIND_EFFECT_SWH,
			     "effect", AGS_BASE_PLUGIN_TEST_FIND_EFFECT_ENVELOPE_TRACKER,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  /* assert find - cmt effect */
  CU_ASSERT((current = ags_base_plugin_find_effect(list,
						   AGS_BASE_PLUGIN_TEST_FIND_EFFECT_CMT,
						   AGS_BASE_PLUGIN_TEST_FIND_EFFECT_PINK_NOISE)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       AGS_BASE_PLUGIN_TEST_FIND_EFFECT_CMT) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_FIND_EFFECT_PINK_NOISE));

  CU_ASSERT((current = ags_base_plugin_find_effect(list,
						   AGS_BASE_PLUGIN_TEST_FIND_EFFECT_CMT,
						   AGS_BASE_PLUGIN_TEST_FIND_EFFECT_ECHO_DELAY)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       AGS_BASE_PLUGIN_TEST_FIND_EFFECT_CMT) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_FIND_EFFECT_ECHO_DELAY));

  CU_ASSERT((current = ags_base_plugin_find_effect(list,
						   AGS_BASE_PLUGIN_TEST_FIND_EFFECT_CMT,
						   AGS_BASE_PLUGIN_TEST_FIND_EFFECT_GVERB)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       AGS_BASE_PLUGIN_TEST_FIND_EFFECT_CMT) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_FIND_EFFECT_GVERB));

  /* assert find - swh effect */
  CU_ASSERT((current = ags_base_plugin_find_effect(list,
						   AGS_BASE_PLUGIN_TEST_FIND_EFFECT_SWH,
						   AGS_BASE_PLUGIN_TEST_FIND_EFFECT_AMPLIFIER)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       AGS_BASE_PLUGIN_TEST_FIND_EFFECT_SWH) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_FIND_EFFECT_AMPLIFIER));

  CU_ASSERT((current = ags_base_plugin_find_effect(list,
						   AGS_BASE_PLUGIN_TEST_FIND_EFFECT_SWH,
						   AGS_BASE_PLUGIN_TEST_FIND_EFFECT_ENVELOPE_TRACKER)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->filename,
		       AGS_BASE_PLUGIN_TEST_FIND_EFFECT_SWH) &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_FIND_EFFECT_ENVELOPE_TRACKER));
}

void
ags_base_plugin_test_find_ui_effect_index()
{
  AgsBasePlugin *base_plugin;

  GList *list, *current;

  list = NULL;
  
  /* create some base plugin */
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "ui-filename", AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_COMPRESSOR,
			     "ui-effect-index", AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_COMPRESSOR_INDEX,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "ui-filename", AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_CHORUS,
			     "ui-effect-index", AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_CHORUS_INDEX,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "ui-filename", AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_FLANGER,
			     "ui-effect-index", AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_FLANGER_INDEX,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);
  
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "ui-filename", AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_SPATIALIZER,
			     "ui-effect-index", AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_SPATIALIZER_INDEX,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  /* assert find UI effect index */
  CU_ASSERT((current = ags_base_plugin_find_ui_effect_index(list,
							    AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_COMPRESSOR,
							    AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_COMPRESSOR_INDEX)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->ui_filename,
		       AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_COMPRESSOR) &&
	    AGS_BASE_PLUGIN(current->data)->ui_effect_index == AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_COMPRESSOR_INDEX);
  
  CU_ASSERT((current = ags_base_plugin_find_ui_effect_index(list,
							    AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_CHORUS,
							    AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_CHORUS_INDEX)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->ui_filename,
		       AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_CHORUS) &&
	    AGS_BASE_PLUGIN(current->data)->ui_effect_index == AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_CHORUS_INDEX);
  
  CU_ASSERT((current = ags_base_plugin_find_ui_effect_index(list,
							    AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_FLANGER,
							    AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_FLANGER_INDEX)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->ui_filename,
		       AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_FLANGER) &&
	    AGS_BASE_PLUGIN(current->data)->ui_effect_index == AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_FLANGER_INDEX);

  CU_ASSERT((current = ags_base_plugin_find_ui_effect_index(list,
							    AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_SPATIALIZER,
							    AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_SPATIALIZER_INDEX)) != NULL &&
	    !g_strcmp0(AGS_BASE_PLUGIN(current->data)->ui_filename,
		       AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_SPATIALIZER) &&
	    AGS_BASE_PLUGIN(current->data)->ui_effect_index == AGS_BASE_PLUGIN_TEST_FIND_UI_EFFECT_INDEX_SPATIALIZER_INDEX);
}

void
ags_base_plugin_test_sort()
{
  AgsBasePlugin *base_plugin;

  GList *list, *current;

  list = NULL;
  
  /* add some effects */
  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "effect", AGS_BASE_PLUGIN_TEST_SORT_PINK_NOISE,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "effect", AGS_BASE_PLUGIN_TEST_SORT_ECHO_DELAY,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "effect", AGS_BASE_PLUGIN_TEST_SORT_GVERB,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "effect", AGS_BASE_PLUGIN_TEST_SORT_AMPLIFIER,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "effect", AGS_BASE_PLUGIN_TEST_SORT_ENVELOPE_TRACKER,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "effect", AGS_BASE_PLUGIN_TEST_SORT_COMPRESSOR,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "effect", AGS_BASE_PLUGIN_TEST_SORT_CHORUS,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "effect", AGS_BASE_PLUGIN_TEST_SORT_FLANGER,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     "effect", AGS_BASE_PLUGIN_TEST_SORT_SPATIALIZER,
			     NULL);
  list = g_list_prepend(list,
			base_plugin);

  /* sort base plugins */
  list = ags_base_plugin_sort(list);

  /* assert sorted alphabetically */
  current = list;
  
  CU_ASSERT(!g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_SORT_AMPLIFIER));
  current = current->next;
  
  CU_ASSERT(!g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_SORT_CHORUS));
  current = current->next;

  CU_ASSERT(!g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_SORT_COMPRESSOR));
  current = current->next;

  CU_ASSERT(!g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_SORT_ECHO_DELAY));
  current = current->next;

  CU_ASSERT(!g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_SORT_ENVELOPE_TRACKER));
  current = current->next;

  CU_ASSERT(!g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_SORT_FLANGER));
  current = current->next;

  CU_ASSERT(!g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_SORT_GVERB));
  current = current->next;

  CU_ASSERT(!g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_SORT_PINK_NOISE));
  current = current->next;

  CU_ASSERT(!g_strcmp0(AGS_BASE_PLUGIN(current->data)->effect,
		       AGS_BASE_PLUGIN_TEST_SORT_SPATIALIZER));
  current = current->next;

  CU_ASSERT(current == NULL);
}

void
ags_base_plugin_test_apply_port_group_by_prefix()
{
  //TODO:JK: implement me
}

void
ags_base_plugin_test_instantiate()
{
  AgsBasePlugin *base_plugin;

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     NULL);
  AGS_BASE_PLUGIN_GET_CLASS(base_plugin)->instantiate = ags_base_plugin_test_stub_instantiate;

  ags_base_plugin_instantiate(base_plugin,
			      AGS_BASE_PLUGIN_TEST_INSTANTIATE_SAMPLERATE);

  CU_ASSERT(stub_instantiate == TRUE);
}

void
ags_base_plugin_test_connect_port()
{
  AgsBasePlugin *base_plugin;

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     NULL);
  AGS_BASE_PLUGIN_GET_CLASS(base_plugin)->connect_port = ags_base_plugin_test_stub_connect_port;

  ags_base_plugin_connect_port(base_plugin,
			       NULL,
			       0,
			       NULL);

  CU_ASSERT(stub_connect_port == TRUE);
}

void
ags_base_plugin_test_activate()
{
  AgsBasePlugin *base_plugin;

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     NULL);
  AGS_BASE_PLUGIN_GET_CLASS(base_plugin)->activate = ags_base_plugin_test_stub_activate;

  ags_base_plugin_activate(base_plugin,
			   NULL);

  CU_ASSERT(stub_activate == TRUE);
}

void
ags_base_plugin_test_deactivate()
{
  AgsBasePlugin *base_plugin;

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     NULL);
  AGS_BASE_PLUGIN_GET_CLASS(base_plugin)->deactivate = ags_base_plugin_test_stub_deactivate;

  ags_base_plugin_deactivate(base_plugin,
			     NULL);

  CU_ASSERT(stub_deactivate == TRUE);
}

void
ags_base_plugin_test_run()
{
  AgsBasePlugin *base_plugin;

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     NULL);
  AGS_BASE_PLUGIN_GET_CLASS(base_plugin)->run = ags_base_plugin_test_stub_run;

  ags_base_plugin_run(base_plugin,
		      NULL,
		      NULL,
		      AGS_BASE_PLUGIN_TEST_RUN_FRAME_COUNT);

  CU_ASSERT(stub_run == TRUE);
}

void
ags_base_plugin_test_load_plugin()
{
  AgsBasePlugin *base_plugin;

  base_plugin = g_object_new(AGS_TYPE_BASE_PLUGIN,
			     NULL);
  AGS_BASE_PLUGIN_GET_CLASS(base_plugin)->load_plugin = ags_base_plugin_test_stub_load_plugin;

  ags_base_plugin_load_plugin(base_plugin);

  CU_ASSERT(stub_load_plugin == TRUE);
}

gpointer
ags_base_plugin_test_stub_instantiate(AgsBasePlugin *base_plugin,
				      guint samplerate)
{
  stub_instantiate = TRUE;

  return(NULL);
}

void
ags_base_plugin_test_stub_connect_port(AgsBasePlugin *base_plugin,
				       gpointer plugin_handle,
				       guint port_index,
				       gpointer data_location)
{
  stub_connect_port = TRUE;
}

void
ags_base_plugin_test_stub_activate(AgsBasePlugin *base_plugin,
				   gpointer plugin_handle)
{
  stub_activate = TRUE;
}

void
ags_base_plugin_test_stub_deactivate(AgsBasePlugin *base_plugin,
				     gpointer plugin_handle)
{
  stub_deactivate = TRUE;
}

void
ags_base_plugin_test_stub_run(AgsBasePlugin *base_plugin,
			      gpointer plugin_handle,
			      snd_seq_event_t *seq_event,
			      guint frame_count)
{
  stub_run = TRUE;
}

void
ags_base_plugin_test_stub_load_plugin(AgsBasePlugin *base_plugin)
{
  stub_load_plugin = TRUE;
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsBasePluginTest\0", ags_base_plugin_test_init_suite, ags_base_plugin_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPortDescriptor find symbol\0", ags_port_descriptor_test_find_symbol) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin find filename\0", ags_base_plugin_test_find_filename) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin find effect\0", ags_base_plugin_test_find_effect) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin find UI effect index\0", ags_base_plugin_test_find_ui_effect_index) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin sort\0", ags_base_plugin_test_sort) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin apply port group by prefix\0", ags_base_plugin_test_apply_port_group_by_prefix) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin instantiate\0", ags_base_plugin_test_instantiate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin connect port\0", ags_base_plugin_test_connect_port) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin activate\0", ags_base_plugin_test_activate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin deactivate\0", ags_base_plugin_test_deactivate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin run\0", ags_base_plugin_test_run) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBasePlugin load plugin\0", ags_base_plugin_test_load_plugin) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
