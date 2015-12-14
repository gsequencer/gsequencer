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

#include <glib.h>
#include <glib-object.h>
#include <libinstpatch/libinstpatch.h>

#include <pthread.h>

int ags_audio_test_init_suite();
int ags_audio_test_clean_suite();

void ags_libinstpatch_test_concurrent_read();
void ags_libinstpatch_test_concurrent_read_thread0(void *ptr);
void ags_libinstpatch_test_concurrent_read_thread1(void *ptr);

IpatchFile *concurrent_read_file;
IpatchFileHandle *concurrent_read_handle;
IpatchBase *concurrent_read_base;
IpatchSF2 *concurrent_read_sf2;

#define AGS_LIBINSTPATCH_TEST_CONCURRENT_READ_SOUNDFONT2_FILENAME "/usr/share/sounds/sf2/FluidR3_GM.sf2\0"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_libinstpatch_test_init_suite()
{
  concurrent_read_file = NULL;
  concurrent_read_handle = NULL;
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_libinstpatch_test_clean_suite()
{
  return(0);
}

void*
ags_libinstpatch_test_concurrent_read_thread0(void *ptr)
{
  IpatchSF2Reader *reader;
  IpatchContainer *preset;
  IpatchContainer *instrument;
  IpatchContainer *sample;

  IpatchList *samples;
  IpatchList *ipatch_list;
  IpatchItem *ipatch_item;
  GList *list;
  GList *tmp;

  gboolean found_first;

  GError *error;
  
  error = NULL;
  concurrent_read_handle = ipatch_file_identify_open(AGS_LIBINSTPATCH_TEST_CONCURRENT_READ_SOUNDFONT2_FILENAME,
						     &error);

  if(error != NULL){
    g_error("%s\0", error->message);
  }

  /*  */

  reader = ipatch_sf2_reader_new(concurrent_read_handle);

  error = NULL;
  concurrent_read_base = (IpatchBase *) ipatch_sf2_reader_load(AGS_IPATCH_SF2_READER(ipatch->reader)->reader,
							       &error);
  
  error = NULL;
  concurrent_read_sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) handle->file,
								    IPATCH_TYPE_SF2,
								    &error);

  if(error != NULL){
    g_warning("%s\0", error->message);
  }

  /* load samples */
  samples = (IpatchList *) ipatch_container_get_children(IPATCH_CONTAINER(concurrent_read_base),
							 IPATCH_TYPE_SF2_SAMPLE);

  /* preset */
  ipatch_list = ipatch_container_get_children(IPATCH_CONTAINER(concurrent_read_sf2),
					      IPATCH_TYPE_SF2_PRESET);
  list = ipatch_list->items;
	
  while(list != NULL){
    if(!g_strcmp0(ipatch_sf2_preset_get_name(IPATCH_SF2_PRESET(list->data)), sublevel_name)){
      /* some extra code for bank and program */
      //	    ipatch_sf2_preset_get_midi_locale(IPATCH_SF2_PRESET(list->data),
      //				      &(ipatch_sf2_reader->bank),
      //				      &(ipatch_sf2_reader->program));

      //	    g_message("bank %d program %d\n\0", ipatch_sf2_reader->bank, ipatch_sf2_reader->program);

      this_error = NULL;
      preset = (IpatchContainer *) IPATCH_SF2_PRESET(list->data);

      break;
    }

    list = list->next;
  }

  /* instrument */
  found_first = FALSE;

  ipatch_list = ipatch_sf2_preset_get_zones(preset);
  list = NULL;
  tmp = ipatch_list->items;

  while(tmp != NULL){
    list = g_list_prepend(list, ipatch_sf2_zone_get_link_item(IPATCH_SF2_ZONE(tmp->data)));

    if(!g_strcmp0(IPATCH_SF2_INST(list->data)->name, sublevel_name)){
      instrument = (IpatchContainer *) IPATCH_SF2_INST(list->data);
    }

    tmp = tmp->next;
  }

  ipatch->iter = g_list_reverse(list);

  /* sample */
  ipatch_list = ipatch_sf2_preset_get_zones(instrument);
  list = NULL;
  tmp = ipatch_list->items;
	
  while(tmp != NULL){
    list = g_list_prepend(list, ipatch_sf2_zone_get_link_item(IPATCH_SF2_ZONE(tmp->data)));

    if(!strncmp(IPATCH_SF2_SAMPLE(list->data)->name, sublevel_name, 20)){
      sample = (IpatchContainer *) IPATCH_SF2_SAMPLE(list->data);
    }

    tmp = tmp->next;
  }
  
  pthread_exit(NULL);
}

void*
ags_libinstpatch_test_concurrent_read_thread1(void *ptr)
{
  IpatchContainer *preset;
  IpatchContainer *instrument;
  IpatchContainer *sample;

  IpatchList *samples;
  IpatchList *ipatch_list;
  IpatchItem *ipatch_item;
  GList *list;
  GList *tmp;

  gboolean found_first;

  GError *error;

  /* preset */
  ipatch_list = ipatch_container_get_children(IPATCH_CONTAINER(concurrent_read_sf2),
					      IPATCH_TYPE_SF2_PRESET);
  list = ipatch_list->items;
	
  while(list != NULL){
    if(!g_strcmp0(ipatch_sf2_preset_get_name(IPATCH_SF2_PRESET(list->data)), sublevel_name)){
      /* some extra code for bank and program */
      //	    ipatch_sf2_preset_get_midi_locale(IPATCH_SF2_PRESET(list->data),
      //				      &(ipatch_sf2_reader->bank),
      //				      &(ipatch_sf2_reader->program));

      //	    g_message("bank %d program %d\n\0", ipatch_sf2_reader->bank, ipatch_sf2_reader->program);

      this_error = NULL;
      preset = (IpatchContainer *) IPATCH_SF2_PRESET(list->data);

      break;
    }

    list = list->next;
  }

  /* instrument */
  found_first = FALSE;

  ipatch_list = ipatch_sf2_preset_get_zones(preset);
  list = NULL;
  tmp = ipatch_list->items;

  while(tmp != NULL){
    list = g_list_prepend(list, ipatch_sf2_zone_get_link_item(IPATCH_SF2_ZONE(tmp->data)));

    if(!g_strcmp0(IPATCH_SF2_INST(list->data)->name, sublevel_name)){
      instrument = (IpatchContainer *) IPATCH_SF2_INST(list->data);
    }

    tmp = tmp->next;
  }

  ipatch->iter = g_list_reverse(list);

  /* sample */
  ipatch_list = ipatch_sf2_preset_get_zones(instrument);
  list = NULL;
  tmp = ipatch_list->items;
	
  while(tmp != NULL){
    list = g_list_prepend(list, ipatch_sf2_zone_get_link_item(IPATCH_SF2_ZONE(tmp->data)));

    if(!strncmp(IPATCH_SF2_SAMPLE(list->data)->name, sublevel_name, 20)){
      sample = (IpatchContainer *) IPATCH_SF2_SAMPLE(list->data);
    }

    tmp = tmp->next;
  }
  
  pthread_exit(NULL);
}

void
ags_libinstpatch_test_concurrent_read()
{
  pthread_t thread0, thread1;

  pthread_create(&thread0, NULL,
		 &ags_libinstpatch_test_concurrent_read_thread0, NULL);
  pthread_join(thread0);
  
  pthread_create(&thread1, NULL,
		 &ags_libinstpatch_test_concurrent_read_thread1, NULL);
  pthread_join(thread1);
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
  pSuite = CU_add_suite("AgsLibinstpatchTest\0", ags_libinstpatch_test_init_suite, ags_libinstpatch_test_clean_suite);

  if(pSuite == NULL){
    CU_cleanup_registry();

    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLibinstpatch concurrent read\0", ags_libinstpatch_test_set_pads) == NULL)){
    CU_cleanup_registry();

    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();

  CU_cleanup_registry();

  return(CU_get_error());
}
