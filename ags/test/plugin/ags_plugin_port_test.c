/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2018 Joël Krähemann
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

int ags_plugin_port_test_init_suite();
int ags_plugin_port_test_clean_suite();

void ags_plugin_port_test_test_flags();
void ags_plugin_port_test_set_flags();
void ags_plugin_port_test_unset_flags();
void ags_plugin_port_test_find_symbol();

#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_VOLUME "./volume[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_MUTED "./muted[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_28HZ "./peak-28hz[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_56HZ "./peak-56hz[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_112HZ "./peak-112hz[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_224HZ "./peak-224hz[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_448HZ "./peak-448hz[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_896HZ "./peak-896hz[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_1792HZ "./peak-1792hz[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_3584HZ "./peak-3584hz[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_7168HZ "./peak-7168hz[0]"
#define AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_14336HZ "./peak-14336hz[0]"

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_plugin_port_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_plugin_port_test_clean_suite()
{
  return(0);
}

void
ags_plugin_port_test_test_flags()
{
  AgsPluginPort *plugin_port;

  plugin_port = ags_plugin_port_new();

  /* set flags */
  plugin_port->flags = 0;

  /* test flags - AGS_PLUGIN_PORT_ATOM */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_ATOM));
  
  /* test flags - AGS_PLUGIN_PORT_AUDIO */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_AUDIO));

  /* test flags - AGS_PLUGIN_PORT_CONTROL */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_CONTROL));

  /* test flags - AGS_PLUGIN_PORT_MIDI */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_MIDI));

  /* test flags - AGS_PLUGIN_PORT_EVENT */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_EVENT));

  /* test flags - AGS_PLUGIN_PORT_OUTPUT */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_OUTPUT));

  /* test flags - AGS_PLUGIN_PORT_INPUT */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_INPUT));

  /* test flags - AGS_PLUGIN_PORT_TOGGLED */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_TOGGLED));

  /* test flags - AGS_PLUGIN_PORT_ENUMERATION */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_ENUMERATION));

  /* test flags - AGS_PLUGIN_PORT_LOGARITHMIC */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_LOGARITHMIC));

  /* test flags - AGS_PLUGIN_PORT_INTEGER */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_INTEGER));

  /* test flags - AGS_PLUGIN_PORT_SAMPLERATE */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_SAMPLERATE));

  /* test flags - AGS_PLUGIN_PORT_BOUNDED_BELOW */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_BOUNDED_BELOW));

  /* test flags - AGS_PLUGIN_PORT_BOUNDED_ABOVE */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_BOUNDED_ABOVE));

  /* test flags - AGS_PLUGIN_PORT_UI_NOTIFICATION */
  CU_ASSERT(!ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_UI_NOTIFICATION));

  /* set flags */
  plugin_port->flags = (AGS_PLUGIN_PORT_ATOM |
			AGS_PLUGIN_PORT_AUDIO |
			AGS_PLUGIN_PORT_CONTROL |
			AGS_PLUGIN_PORT_MIDI |
			AGS_PLUGIN_PORT_EVENT |
			AGS_PLUGIN_PORT_OUTPUT |
			AGS_PLUGIN_PORT_INPUT |
			AGS_PLUGIN_PORT_TOGGLED |
			AGS_PLUGIN_PORT_ENUMERATION |
			AGS_PLUGIN_PORT_LOGARITHMIC |
			AGS_PLUGIN_PORT_INTEGER |
			AGS_PLUGIN_PORT_SAMPLERATE |
			AGS_PLUGIN_PORT_BOUNDED_BELOW |
			AGS_PLUGIN_PORT_BOUNDED_ABOVE |
			AGS_PLUGIN_PORT_UI_NOTIFICATION);

  /* test flags - AGS_PLUGIN_PORT_ATOM */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_ATOM));
  
  /* test flags - AGS_PLUGIN_PORT_AUDIO */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_AUDIO));

  /* test flags - AGS_PLUGIN_PORT_CONTROL */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_CONTROL));

  /* test flags - AGS_PLUGIN_PORT_MIDI */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_MIDI));

  /* test flags - AGS_PLUGIN_PORT_EVENT */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_EVENT));

  /* test flags - AGS_PLUGIN_PORT_OUTPUT */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_OUTPUT));

  /* test flags - AGS_PLUGIN_PORT_INPUT */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_INPUT));

  /* test flags - AGS_PLUGIN_PORT_TOGGLED */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_TOGGLED));

  /* test flags - AGS_PLUGIN_PORT_ENUMERATION */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_ENUMERATION));

  /* test flags - AGS_PLUGIN_PORT_LOGARITHMIC */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_LOGARITHMIC));

  /* test flags - AGS_PLUGIN_PORT_INTEGER */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_INTEGER));

  /* test flags - AGS_PLUGIN_PORT_SAMPLERATE */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_SAMPLERATE));

  /* test flags - AGS_PLUGIN_PORT_BOUNDED_BELOW */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_BOUNDED_BELOW));

  /* test flags - AGS_PLUGIN_PORT_BOUNDED_ABOVE */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_BOUNDED_ABOVE));

  /* test flags - AGS_PLUGIN_PORT_UI_NOTIFICATION */
  CU_ASSERT(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_UI_NOTIFICATION));
}

void
ags_plugin_port_test_set_flags()
{
  AgsPluginPort *plugin_port;

  plugin_port = ags_plugin_port_new();

  /* set flags */
  plugin_port->flags = 0;

  /* set flags - AGS_PLUGIN_PORT_ATOM */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_ATOM);

  CU_ASSERT((AGS_PLUGIN_PORT_ATOM & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_AUDIO */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_AUDIO);

  CU_ASSERT((AGS_PLUGIN_PORT_AUDIO & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_CONTROL */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_CONTROL);

  CU_ASSERT((AGS_PLUGIN_PORT_CONTROL & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_MIDI */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_MIDI);

  CU_ASSERT((AGS_PLUGIN_PORT_MIDI & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_EVENT */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_EVENT);

  CU_ASSERT((AGS_PLUGIN_PORT_EVENT & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_OUTPUT */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_OUTPUT);

  CU_ASSERT((AGS_PLUGIN_PORT_OUTPUT & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_INPUT */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_INPUT);

  CU_ASSERT((AGS_PLUGIN_PORT_INPUT & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_TOGGLED */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_TOGGLED);

  CU_ASSERT((AGS_PLUGIN_PORT_TOGGLED & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_ENUMERATION */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_ENUMERATION);

  CU_ASSERT((AGS_PLUGIN_PORT_ENUMERATION & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_LOGARITHMIC */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_LOGARITHMIC);

  CU_ASSERT((AGS_PLUGIN_PORT_LOGARITHMIC & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_INTEGER */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_INTEGER);

  CU_ASSERT((AGS_PLUGIN_PORT_INTEGER & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_SAMPLERATE */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_SAMPLERATE);

  CU_ASSERT((AGS_PLUGIN_PORT_SAMPLERATE & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_BOUNDED_BELOW */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_BOUNDED_BELOW);

  CU_ASSERT((AGS_PLUGIN_PORT_BOUNDED_BELOW & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_BOUNDED_ABOVE */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_BOUNDED_ABOVE);

  CU_ASSERT((AGS_PLUGIN_PORT_BOUNDED_ABOVE & (plugin_port->flags)) != 0);
  
  /* set flags - AGS_PLUGIN_PORT_UI_NOTIFICATION */
  ags_plugin_port_set_flags(plugin_port, AGS_PLUGIN_PORT_UI_NOTIFICATION);

  CU_ASSERT((AGS_PLUGIN_PORT_UI_NOTIFICATION & (plugin_port->flags)) != 0);
}

void
ags_plugin_port_test_unset_flags()
{
  AgsPluginPort *plugin_port;

  plugin_port = ags_plugin_port_new();

  /* set flags */
  plugin_port->flags = (AGS_PLUGIN_PORT_ATOM |
			AGS_PLUGIN_PORT_AUDIO |
			AGS_PLUGIN_PORT_CONTROL |
			AGS_PLUGIN_PORT_MIDI |
			AGS_PLUGIN_PORT_EVENT |
			AGS_PLUGIN_PORT_OUTPUT |
			AGS_PLUGIN_PORT_INPUT |
			AGS_PLUGIN_PORT_TOGGLED |
			AGS_PLUGIN_PORT_ENUMERATION |
			AGS_PLUGIN_PORT_LOGARITHMIC |
			AGS_PLUGIN_PORT_INTEGER |
			AGS_PLUGIN_PORT_SAMPLERATE |
			AGS_PLUGIN_PORT_BOUNDED_BELOW |
			AGS_PLUGIN_PORT_BOUNDED_ABOVE |
			AGS_PLUGIN_PORT_UI_NOTIFICATION);

  /* unset flags - AGS_PLUGIN_PORT_ATOM */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_ATOM);

  CU_ASSERT((AGS_PLUGIN_PORT_ATOM & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_AUDIO */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_AUDIO);

  CU_ASSERT((AGS_PLUGIN_PORT_AUDIO & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_CONTROL */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_CONTROL);

  CU_ASSERT((AGS_PLUGIN_PORT_CONTROL & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_MIDI */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_MIDI);

  CU_ASSERT((AGS_PLUGIN_PORT_MIDI & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_EVENT */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_EVENT);

  CU_ASSERT((AGS_PLUGIN_PORT_EVENT & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_OUTPUT */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_OUTPUT);

  CU_ASSERT((AGS_PLUGIN_PORT_OUTPUT & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_INPUT */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_INPUT);

  CU_ASSERT((AGS_PLUGIN_PORT_INPUT & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_TOGGLED */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_TOGGLED);

  CU_ASSERT((AGS_PLUGIN_PORT_TOGGLED & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_ENUMERATION */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_ENUMERATION);

  CU_ASSERT((AGS_PLUGIN_PORT_ENUMERATION & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_LOGARITHMIC */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_LOGARITHMIC);

  CU_ASSERT((AGS_PLUGIN_PORT_LOGARITHMIC & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_INTEGER */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_INTEGER);

  CU_ASSERT((AGS_PLUGIN_PORT_INTEGER & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_SAMPLERATE */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_SAMPLERATE);

  CU_ASSERT((AGS_PLUGIN_PORT_SAMPLERATE & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_BOUNDED_BELOW */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_BOUNDED_BELOW);

  CU_ASSERT((AGS_PLUGIN_PORT_BOUNDED_BELOW & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_BOUNDED_ABOVE */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_BOUNDED_ABOVE);

  CU_ASSERT((AGS_PLUGIN_PORT_BOUNDED_ABOVE & (plugin_port->flags)) == 0);
  
  /* unset flags - AGS_PLUGIN_PORT_UI_NOTIFICATION */
  ags_plugin_port_unset_flags(plugin_port, AGS_PLUGIN_PORT_UI_NOTIFICATION);

  CU_ASSERT((AGS_PLUGIN_PORT_UI_NOTIFICATION & (plugin_port->flags)) == 0);
}

void
ags_plugin_port_test_find_symbol()
{
  AgsPluginPort *plugin_port;
  
  GList *list;
  GList *current;

  list = NULL;
  
  /* volume */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_VOLUME,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* muted */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_MUTED,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* peak-28hz */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_28HZ,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* peak-56hz */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_56HZ,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* peak-112hz */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_112HZ,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* peak-224hz */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_224HZ,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* peak-448hz */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_448HZ,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* peak-896hz */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_896HZ,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* peak-1792hz */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_1792HZ,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* peak-3584hz */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_3584HZ,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* peak-7168hz */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_7168HZ,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);

  /* peak-14336hz */
  plugin_port = g_object_new(AGS_TYPE_PLUGIN_PORT,
			     "port-symbol", AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_14336HZ,
			     NULL);
  list = g_list_prepend(list,
			plugin_port);


  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_VOLUME */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_VOLUME);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_VOLUME));
  
  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_MUTED */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_MUTED);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_MUTED));

  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_28HZ */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_28HZ);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_28HZ));

  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_56HZ */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_56HZ);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_56HZ));

  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_112HZ */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_112HZ);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_112HZ));

  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_224HZ */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_224HZ);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_224HZ));

  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_448HZ */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_448HZ);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_448HZ));

  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_896HZ */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_896HZ);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_896HZ));

  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_1792HZ */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_1792HZ);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_1792HZ));

  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_3584HZ */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_3584HZ);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_3584HZ));

  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_7168HZ */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_7168HZ);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_7168HZ));

  /* test find symbol - AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_14336HZ */
  current = ags_plugin_port_find_symbol(list,
					AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_14336HZ);
  CU_ASSERT(current != NULL &&
	    !g_strcmp0(AGS_PLUGIN_PORT(current->data)->port_symbol,
		       AGS_PLUGIN_PORT_TEST_PORT_SYMBOL_PEAK_14336HZ));
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
  pSuite = CU_add_suite("AgsPluginPortTest", ags_plugin_port_test_init_suite, ags_plugin_port_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPluginPort test flags", ags_plugin_port_test_test_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPluginPort set flags", ags_plugin_port_test_set_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPluginPort unset flags", ags_plugin_port_test_unset_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPluginPort find symbol", ags_plugin_port_test_find_symbol) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
