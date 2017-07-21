/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2016 Joël Krähemann
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

#include <ags/lib/ags_turtle.h>

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_turtle_test_init_suite();
int ags_turtle_test_clean_suite();

void ags_turtle_test_read_iriref();
void ags_turtle_test_read_pname_ns();
void ags_turtle_test_read_pname_ln();
void ags_turtle_test_read_blank_node_label();
void ags_turtle_test_read_langtag();
void ags_turtle_test_read_boolean();
void ags_turtle_test_read_integer();
void ags_turtle_test_read_decimal();
void ags_turtle_test_read_double();
void ags_turtle_test_read_exponent();
void ags_turtle_test_read_string();
void ags_turtle_test_read_string_literal_quote();
void ags_turtle_test_read_string_literal_single_quote();
void ags_turtle_test_read_string_literal_long_quote();
void ags_turtle_test_read_string_literal_long_single_quote();
void ags_turtle_test_read_uchar();
void ags_turtle_test_read_echar();
void ags_turtle_test_read_ws();
void ags_turtle_test_read_anon();
void ags_turtle_test_read_pn_chars_base();
void ags_turtle_test_read_pn_chars_u();
void ags_turtle_test_read_pn_chars();
void ags_turtle_test_read_pn_prefix();
void ags_turtle_test_read_pn_local();
void ags_turtle_test_read_plx();
void ags_turtle_test_read_percent();
void ags_turtle_test_read_hex();
void ags_turtle_test_read_pn_local_esc();

#define AGS_TURTLE_TEST_READ_INTEGER_COUNT (1024)

#define AGS_TURTLE_TEST_READ_DECIMAL_COUNT (1024)

#define AGS_TURTLE_TEST_READ_DOUBLE_COUNT (1024)

#define AGS_TURTLE_TEST_READ_PN_CHARS_BASE_ITERATION_COUNT (65535)
#define AGS_TURTLE_TEST_READ_PN_CHARS_BASE_ITERATION_COUNT (65535)
#define AGS_TURTLE_TEST_READ_PN_CHARS_ITERATION_COUNT (256)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_turtle_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_turtle_test_clean_suite()
{
  return(0);
}

void
ags_turtle_test_read_iriref()
{
  gchar *iriref_lv2 = "<lv2>\0";
  gchar *no_iriref = "<nongnu dot org>\0";
  gchar *str;

  /* assert iriref lv2 */
  str = ags_turtle_read_iriref(iriref_lv2,
			       iriref_lv2 + strlen(iriref_lv2));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(str,
				 "<lv2>\0",
				 6));

  /* assert no iriref */
  str = ags_turtle_read_iriref(no_iriref,
			       no_iriref + strlen(no_iriref));

  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_pname_ns()
{
  gchar *pname_ns_port = ":port\0";
  gchar *pname_ns_swh_plugin = "swh:plugin\0";
  gchar *no_pname_ns = "port\0";
  gchar *str;

  /* assert pname ns port */
  str = ags_turtle_read_pname_ns(pname_ns_port,
				 pname_ns_port + strlen(pname_ns_port));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(str,
				 ":\0",
				 2));

  /* assert pname ns swh plugin */
  str = ags_turtle_read_pname_ns(pname_ns_swh_plugin,
				 pname_ns_swh_plugin + strlen(pname_ns_swh_plugin));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(str,
				 "swh:\0",
				 5));

  /* assert no pname ns */
  str = ags_turtle_read_pname_ns(no_pname_ns,
				 no_pname_ns + strlen(no_pname_ns));

  CU_ASSERT(str == NULL);  
}

void
ags_turtle_test_read_pname_ln()
{
  gchar *pname_ln_port = ":port\0";
  gchar *pname_ln_swh_plugin = "swh:plugin\0";
  gchar *no_pname_ln = "port\0";
  gchar *str;

  /* assert pname ln port */
  str = ags_turtle_read_pname_ln(pname_ln_port,
				 pname_ln_port + strlen(pname_ln_port));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(str,
				 ":port\0",
				 6));

  /* assert pname ln swh plugin */
  str = ags_turtle_read_pname_ln(pname_ln_swh_plugin,
				 pname_ln_swh_plugin + strlen(pname_ln_swh_plugin));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(str,
				 "swh:plugin\0",
				 11));

  /* assert no pname ln */
  str = ags_turtle_read_pname_ln(no_pname_ln,
				 no_pname_ln + strlen(no_pname_ln));

  CU_ASSERT(str == NULL);  
}

void
ags_turtle_test_read_blank_node_label()
{
  gchar *blank_node_myid = "_:myid\0";
  gchar *no_blank_node = "_ no id\0";
  gchar *str;

  /* assert blank node myid */
  str = ags_turtle_read_blank_node_label(blank_node_myid,
					 blank_node_myid + strlen(blank_node_myid));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(str,
				 "_:myid\0",
				 6));

  /* assert no blank_node */
  str = ags_turtle_read_blank_node_label(no_blank_node,
					 no_blank_node + strlen(no_blank_node));

  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_langtag()
{
  gchar *langtag_DE = "@DE\0";
  gchar *langtag_DE_CH = "@DE-CH\0";
  gchar *langtag_DE_AT = "@DE-AT\0";
  gchar *no_langtag = "Fr\0";
  gchar *str;

  /* assert DE */
  str = ags_turtle_read_langtag(langtag_DE,
				langtag_DE + strlen(langtag_DE));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(langtag_DE,
				 str,
				 4));

  /* assert DE-CH */
  str = ags_turtle_read_langtag(langtag_DE_CH,
				langtag_DE_CH + strlen(langtag_DE_CH));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(langtag_DE_CH,
				 str,
				 4));
  
  /* assert DE-AT */
  str = ags_turtle_read_langtag(langtag_DE_AT,
				langtag_DE_AT + strlen(langtag_DE_AT));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(langtag_DE_AT,
				 str,
				 4));
    
  /* assert no */
  str = ags_turtle_read_langtag(no_langtag,
				no_langtag + strlen(no_langtag));
  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_boolean()
{
  gchar *boolean_true = "true\0";
  gchar *boolean_false = "false\0";
  gchar *not_boolean_0 = "0";
  gchar *not_boolean_1 = "1";
  gchar *str;

  /* assert true */
  str = ags_turtle_read_boolean(boolean_true,
				boolean_true + strlen(boolean_true));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(boolean_true,
				 str,
				 5));
  
  /* assert false */
  str = ags_turtle_read_boolean(boolean_false,
				boolean_false + strlen(boolean_false));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(boolean_false,
				 str,
				 5));
  
  /* assert not 0 */
  str = ags_turtle_read_boolean(not_boolean_0,
				not_boolean_0 + strlen(not_boolean_0));

  CU_ASSERT(str == NULL);
  
  /* assert not 1 */
  str = ags_turtle_read_boolean(not_boolean_1,
				not_boolean_1 + strlen(not_boolean_1));

  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_integer()
{
  gchar *str, *tmp;
  gchar *no_integer = "!0\0";
  
  guint i;
  gboolean success;

  /* check random integer */
  success = TRUE;
  
  for(i = 0; i < AGS_TURTLE_TEST_READ_INTEGER_COUNT; i++){
    tmp = g_strdup_printf("%d\0", rand() % 4294967296 - 2147483648);
    str = ags_turtle_read_integer(tmp,
				  tmp + strlen(tmp));

    if(str == NULL ||
       g_ascii_strcasecmp(tmp,
			  str)){
      success = FALSE;

      break;
    }
    
    free(tmp);
  }

  CU_ASSERT(success == TRUE);

  /* check no */
  str = ags_turtle_read_integer(no_integer,
				no_integer + strlen(no_integer));
  
  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_decimal()
{
  gchar *str, *tmp;
  gchar *no_double = "!0\0";
  
  guint i;
  gboolean success;

  /* check random double */
  success = TRUE;
  
  for(i = 0; i < AGS_TURTLE_TEST_READ_DECIMAL_COUNT; i++){
    tmp = g_strdup_printf("%.5f\0", (rand() % 4294967296 - 2147483648) / 31.0);
    str = ags_turtle_read_decimal(tmp,
				  tmp + strlen(tmp));

    if(str == NULL ||
       g_ascii_strcasecmp(tmp,
			  str)){
      success = FALSE;

      break;
    }
    
    free(tmp);
  }

  CU_ASSERT(success == TRUE);

  /* check no */
  str = ags_turtle_read_decimal(no_double,
				no_double + strlen(no_double));
  
  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_double()
{
  gchar *str, *tmp;
  gchar *no_double = "!0E12\0";

  gdouble val_0;
  guint i;
  gboolean success;

  /* check random double */
  success = TRUE;
  
  for(i = 0; i < AGS_TURTLE_TEST_READ_DOUBLE_COUNT; i++){
    val_0 = (rand() % 4294967296 - 2147483648) / 31.0;
    
    tmp = g_strdup_printf("%01.5fE11\0", val_0);
    str = ags_turtle_read_double(tmp,
				 tmp + strlen(tmp));

    if(str == NULL ||
       g_ascii_strcasecmp(str,
			  tmp)){
      success = FALSE;

      break;
    }
    
    free(tmp);
  }

  CU_ASSERT(success == TRUE);

  /* check no */
  str = ags_turtle_read_double(no_double,
			       no_double + strlen(no_double));
  
  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_exponent()
{
  gchar *exponent_e10 = "e+10\0";
  gchar *exponent_E10 = "E-10\0";
  gchar *no_exponent = "e 10\0";
  gchar *str;
  
  /* assert e+10 */
  str = ags_turtle_read_exponent(exponent_e10,
				 exponent_e10 + strlen(exponent_e10));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(exponent_e10,
				 str,
				 5));

  /* assert E-10 */
  str = ags_turtle_read_exponent(exponent_E10,
				 exponent_E10 + strlen(exponent_E10));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(exponent_E10,
				 str,
				 5));
}

void
ags_turtle_test_read_string()
{
  gchar *string_literal_quote_MY_TURTLE = "\"MY Turtle\"\0";
  gchar *string_literal_single_quote_MY_TURTLE = "\'MY Turtle\'\0";
  gchar *string_literal_long_quote_MY_TURTLE = "\"\"\"MY Turtle\nis just looking at the mirror\nand is waiting to get feed\"\"\"\0";
  gchar *string_literal_long_single_quote_MY_TURTLE = "'''MY Turtle\nis just looking at the mirror\nand is waiting to get feed'''\0";
  gchar *str;

  /* assert my turtle quote */
  str = ags_turtle_read_string(string_literal_quote_MY_TURTLE,
			       string_literal_quote_MY_TURTLE + strlen(string_literal_quote_MY_TURTLE));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(string_literal_quote_MY_TURTLE,
				 str,
				 strlen(string_literal_quote_MY_TURTLE)));

  /* assert my turtle single quote */
  str = ags_turtle_read_string(string_literal_single_quote_MY_TURTLE,
			       string_literal_single_quote_MY_TURTLE + strlen(string_literal_single_quote_MY_TURTLE));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(string_literal_single_quote_MY_TURTLE,
				 str,
				 strlen(string_literal_single_quote_MY_TURTLE)));

  /* assert my turtle long quote */
  str = ags_turtle_read_string(string_literal_long_quote_MY_TURTLE,
			       string_literal_long_quote_MY_TURTLE + strlen(string_literal_long_quote_MY_TURTLE));
	    
  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(string_literal_long_quote_MY_TURTLE,
				 str,
				 strlen(string_literal_long_quote_MY_TURTLE)));

  /* assert my turtle long single quote */
  str = ags_turtle_read_string(string_literal_long_single_quote_MY_TURTLE,
			       string_literal_long_single_quote_MY_TURTLE + strlen(string_literal_long_single_quote_MY_TURTLE));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(string_literal_long_single_quote_MY_TURTLE,
				 str,
				 strlen(string_literal_long_single_quote_MY_TURTLE)));
}

void
ags_turtle_test_read_string_literal_quote()
{
  gchar *string_literal_quote_MY_TURTLE = "\"MY Turtle\"\0";
  gchar *str;

  /* assert my turtle quote */
  str = ags_turtle_read_string(string_literal_quote_MY_TURTLE,
			       string_literal_quote_MY_TURTLE + strlen(string_literal_quote_MY_TURTLE));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(string_literal_quote_MY_TURTLE,
				 str,
				 strlen(string_literal_quote_MY_TURTLE)));
}

void
ags_turtle_test_read_string_literal_single_quote()
{
  gchar *string_literal_single_quote_MY_TURTLE = "\'MY Turtle\'\0";
  gchar *str;

  /* assert my turtle single quote */
  str = ags_turtle_read_string(string_literal_single_quote_MY_TURTLE,
			       string_literal_single_quote_MY_TURTLE + strlen(string_literal_single_quote_MY_TURTLE));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(string_literal_single_quote_MY_TURTLE,
				 str,
				 strlen(string_literal_single_quote_MY_TURTLE)));
}

void
ags_turtle_test_read_string_literal_long_quote()
{
  gchar *string_literal_long_quote_MY_TURTLE = "\"\"\"MY Turtle\nis just looking at the mirror\nand is waiting to get feed\"\"\"\0";
  gchar *str;

  /* assert my turtle long quote */
  str = ags_turtle_read_string(string_literal_long_quote_MY_TURTLE,
			       string_literal_long_quote_MY_TURTLE + strlen(string_literal_long_quote_MY_TURTLE));
	    
  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(string_literal_long_quote_MY_TURTLE,
				 str,
				 strlen(string_literal_long_quote_MY_TURTLE)));
}

void
ags_turtle_test_read_string_literal_long_single_quote()
{
  gchar *string_literal_long_single_quote_MY_TURTLE = "'''MY Turtle\nis just looking at the mirror\nand is waiting to get feed'''\0";
  gchar *str;

  /* assert my turtle long single quote */
  str = ags_turtle_read_string(string_literal_long_single_quote_MY_TURTLE,
			       string_literal_long_single_quote_MY_TURTLE + strlen(string_literal_long_single_quote_MY_TURTLE));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(string_literal_long_single_quote_MY_TURTLE,
				 str,
				 strlen(string_literal_long_single_quote_MY_TURTLE)));
}

void
ags_turtle_test_read_uchar()
{
  gchar *uchar_2_A = "\\u0041\0";
  gchar *uchar_2_e_diaresis = "\\u00eb\0";
  gchar *uchar_4_A = "\\Uffef0041\0";
  gchar *uchar_4_e_diaresis = "\\Uffef00eb\0";
  gchar *str;

  /* assert 2 byte capital A */
  str = ags_turtle_read_uchar(uchar_2_A,
			      uchar_2_A + strlen(uchar_2_A));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(uchar_2_A,
				 str,
				 7));
  
  /* assert 2 byte e diaresis */
  str = ags_turtle_read_uchar(uchar_2_e_diaresis,
			      uchar_2_e_diaresis + strlen(uchar_2_e_diaresis));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(uchar_2_e_diaresis,
				 str,
				 7));

  /* assert 4 byte capital A */
  str = ags_turtle_read_uchar(uchar_4_A,
			      uchar_4_A + strlen(uchar_4_A));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(uchar_4_A,
				 str,
				 11));

  /* assert 4 byte e diaresis */
  str = ags_turtle_read_uchar(uchar_4_e_diaresis,
			      uchar_4_e_diaresis + strlen(uchar_4_e_diaresis));

  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(uchar_4_e_diaresis,
				 str,
				 11));
}

void
ags_turtle_test_read_echar()
{
  gchar current[3];
  gchar *echar = "tbnrf\"\0";
  gchar *str;
  
  guint i;
  gboolean success;

  /* assert escaped characters */
  current[0] = '\\';
  current[2] = '\0';

  success = TRUE;

  for(i = 0; i < 7; i++){
    current[1] = echar[i];

    str = ags_turtle_read_echar(current,
				current + 3);
    
    if(g_ascii_strncasecmp(current,
			   str,
			   3)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);

  /* assert not escaped */
  current[1] = 'x';

  str = ags_turtle_read_echar(current,
			      current + 3);
    
  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_ws()
{
  gchar current[2];
  gchar *ws = "\x20\x09\x0D\x0A\0";
  gchar *str;
  
  guint i;
  gboolean success;

  /* assert white-space */
  current[1] = '\0';
  
  success = TRUE;

  for(i = 0; i < 4; i++){
    current[0] = ws[i];

    str = ags_turtle_read_ws(current,
			     current + 2);
    
    if(g_ascii_strncasecmp(current,
			   str,
			   2)){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success == TRUE);

  /* assert no ws */
  current[0] = 'a';

  str = ags_turtle_read_ws(current,
			   current + 2);
    
  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_anon()
{
  gchar *anon_empty = "[]\0";
  gchar *anon_ws = "[ ]\0";
  gchar *anon_tab = "[\t\t\t]\0";
  gchar *anon_mixed = "[\t \t ]\0";
  gchar *str;
  
  /* assert anon empty */
  str = ags_turtle_read_anon(anon_empty,
			     anon_empty + 3);
  
  CU_ASSERT(str != NULL &&
	    !g_ascii_strcasecmp(str,
				anon_empty));

  /* assert anon ws */
  str = ags_turtle_read_anon(anon_ws,
			     anon_ws + 5);

  CU_ASSERT(str != NULL &&
	    !g_ascii_strcasecmp(str,
				anon_ws));

  /* assert anon tab */
  str = ags_turtle_read_anon(anon_tab,
			     anon_tab + 6);

  CU_ASSERT(str != NULL &&
	    !g_ascii_strcasecmp(str,
				anon_tab));

  /* assert anon mixed */
  str = ags_turtle_read_anon(anon_mixed,
			     anon_mixed + 7);

  CU_ASSERT(str != NULL &&
	    !g_ascii_strcasecmp(str,
				anon_mixed));
}

void
ags_turtle_test_read_pn_chars_base()
{
  gchar current[25];
  gchar *str;

  guint n_bytes, nth;
  guint i;
  gboolean success;
  
  /* test single char A-Z */
  success = TRUE;

  for(i = 0; i < 26; i++){
    current[0] = 'A' + i;
    current[1] = '\0';

    str = ags_turtle_read_pn_chars_base(current,
					current + 2);
    
    if(g_ascii_strncasecmp(current,
			   str,
			   2)){
      success = FALSE;
      
      break;
    }
    
    current[0] = 'a' + i;
    current[1] = '\0';

    str = ags_turtle_read_pn_chars_base(current,
					current + 2);
    
    if(g_ascii_strncasecmp(current,
			   str,
			   2)){
      success = FALSE;
      
      break;
    }
  }
  
  CU_ASSERT(success == TRUE);

  /* test random string */
  success = TRUE;

  for(i = 0; i < AGS_TURTLE_TEST_READ_PN_CHARS_BASE_ITERATION_COUNT; i++){
    guint range;
    
    nth = 0;
    n_bytes = 0;

    range = rand() % 13;
    
    switch(range){
    case 0:
      {
	if(rand() % 2 == 1){
	  current[nth] = 'a' + (rand() % 26);
	}else{
	  current[nth] = 'A' + (rand() % 26);
	}

	n_bytes = 1;
      }
      break;
    case 1:
      {
	current[nth] = 0xC3;
	current[nth + 1] = 0x80 + (rand() % 22);
	  
	n_bytes = 2;
      }
      break;
    case 2:
      {
	current[nth] = 0xC3;
	current[nth + 1] = 0x98 + (rand() % 30);
	  
	n_bytes = 2;
      }
      break;
    case 3:
      {
	guint sub;

	sub = rand() % 3;
	  
	if(sub == 0){
	  current[nth] = 0xC3;
	  current[nth + 1] = 0xB8 + (rand() % 8);
	}else if(sub == 1){
	  current[nth] = 0xC3 + (rand() % 7);
	  current[nth + 1] = 0x80 + (rand() % 64);
	}else if(sub == 2){
	  current[nth] = 0xCB;
	  current[nth + 1] = 0x80 + (rand() % 64);
	}
	  
	n_bytes = 2;
      }
      break;
    case 4:
      {
	current[nth] = 0xCD;
	current[nth + 1] = 0xB0 + (rand() % 13);
	  
	n_bytes = 2;
      }
      break;
    case 5:
      {
	guint sub;

	sub = rand() % 3;
	  
	if(sub == 0){
	  current[nth] = 0xCD;
	  current[nth + 1] = 0xBF + (rand() % 32);
	  
	  n_bytes = 2;
	}else if(sub == 1){
	  current[nth] = 0xCE + (rand() % 7);
	  current[nth + 1] = 0x80 + (rand() % 64);
	  
	  n_bytes = 2;
	}else if(sub == 2){
	  current[nth] = 0xE0 + (rand() % 2);
	  current[nth + 1] = 0x80 + (rand() % 64);
	  current[nth + 2] = 0x80 + (rand() % 64);
	  
	  n_bytes = 3;
	}
      }
      break;
    case 6:
      {
	current[nth] = 0xE2;
	current[nth + 1] = 0x80;
	current[nth + 2] = 0x8C + (rand() % 2);
	  
	n_bytes = 3;
      }
      break;
    case 7:
      {
	guint sub;

	sub = rand() % 3;
	  
	if(sub == 0){
	  current[nth] = 0xE2;
	  current[nth + 1] = 0x81;
	  current[nth + 2] = 0xB0 + (rand() % 16);
	}else if(sub == 1){
	  current[nth] = 0xE2;
	  current[nth + 1] = 0x81 + (rand() % 4);
	  current[nth + 2] = 0x80 + (rand() % 64);
	}else if(sub == 2){
	  current[nth] = 0xE2;
	  current[nth + 1] = 0x86;
	  current[nth + 2] = 0x80 + (rand() % 16);
	}
	  
	n_bytes = 3;
      }
      break;
    case 8:
      {
	guint sub;

	sub = rand() % 2;
	  
	if(sub == 0){
	  current[nth] = 0xE2;
	  current[nth + 1] = 0xB0 + (rand() % 15);
	  current[nth + 2] = 0x80 + (rand() % 64);
	}else if(sub == 1){
	  current[nth] = 0xE2;
	  current[nth + 1] = 0xBF;
	  current[nth + 2] = 0x80 + (rand() % 48);
	}
	  
	n_bytes = 3;
      }
      break;
    case 9:
      {
	guint sub;

	sub = rand() % 2;
	  
	if(sub == 0){
	  current[nth] = 0xE3;
	  current[nth + 1] = 0x80 + (rand() % 64);
	  current[nth + 2] = 0x81 + (rand() % 63);
	}else if(sub == 1){
	  current[nth] = 0xE4 + (rand() % 8);
	  current[nth + 1] = 0x80 + (rand() % 16);
	  current[nth + 2] = 0x80 + (rand() % 64);
	}
	  
	n_bytes = 3;
      }
      break;
    case 10:
      {
	guint sub;

	sub = rand() % 2;
	  
	if(sub == 0){
	  current[nth] = 0xEF;
	  current[nth + 1] = 0xA4 + (rand() % 18);
	  current[nth + 2] = 0x80 + (rand() % 64);
	}else if(sub == 1){
	  current[nth] = 0xEF;
	  current[nth + 1] = 0xBF;
	  current[nth + 2] = 0x80 + (rand() % 16);
	}
	  
	n_bytes = 3;
      }
      break;
    case 11:
      {
	guint sub;

	sub = rand() % 3;
	  
	if(sub == 0){
	  current[nth] = 0xEF;
	  current[nth + 1] = 0xB7;
	  current[nth + 2] = 0xB0 + (rand() % 16);
	}else if(sub == 1){
	  current[nth] = 0xEF;
	  current[nth + 1] = 0xB8 + (rand() % 6);
	  current[nth + 2] = 0x80 + (rand() % 64);
	}else if(sub == 2){
	  current[nth] = 0xEF;
	  current[nth + 1] = 0xBF;
	  current[nth + 2] = 0x80 + (rand() % 62);
	}
	  
	n_bytes = 3;
      }
      break;
    case 12:
      {
	current[nth] = 0xF0 + (rand() % 4);
	current[nth + 1] = 0x90 + (rand() % 32);
	current[nth + 2] = 0x80 + (rand() % 64);
	current[nth + 3] = 0x80 + (rand() % 64);

	n_bytes = 4;
      }
    }

    current[nth + n_bytes] = '\0';
    str = ags_turtle_read_pn_chars_base(current,
					current + n_bytes);
    
    if(str == NULL ||
       g_ascii_strncasecmp(current,
			   str,
			   n_bytes)){
      success = FALSE;
	
      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_turtle_test_read_pn_chars_u()
{
  gchar *underscore = "_\0";
  gchar *str;
  
  str = ags_turtle_read_pn_chars_u(underscore,
				   underscore + 2);
  
  CU_ASSERT(str != NULL ||
	    !g_ascii_strncasecmp(underscore,
				 str,
				 2));
}

void
ags_turtle_test_read_pn_chars()
{
  gchar *dash = "-\0";
  gchar *bullet = "\xC2\xB7\0";
  gchar *key_0 = "\xE2\x80\xBF\0";
  gchar *key_1 = "\xE2\x81\x80\0";
  gchar current[25];  
  gchar *str;
  
  guint n_bytes, nth;
  guint i;
  gboolean success;

  /* test single char 0-9 */
  success = TRUE;

  for(i = 0; i < 10; i++){
    current[0] = '0' + i;
    current[1] = '\0';

    str = ags_turtle_read_pn_chars(current,
				   current + 2);
    
    if(g_ascii_strncasecmp(current,
			   str,
			   2)){
      success = FALSE;
      
      break;
    }
  }
  
  CU_ASSERT(success == TRUE);

  /* test dash */  
  str = ags_turtle_read_pn_chars(dash,
				 dash + 2);
  
  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(dash,
				 str,
				 2));
  
  /* test bullet */  
  str = ags_turtle_read_pn_chars(bullet,
				 bullet + 2);
  
  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(bullet,
				 str,
				 2));

  
  /* test random string */
  success = TRUE;

  for(i = 0; i < AGS_TURTLE_TEST_READ_PN_CHARS_BASE_ITERATION_COUNT; i++){
    guint range;
    
    nth = 0;
    n_bytes = 0;

    range = rand() % 2;
    
    switch(range){
    case 0:
      {
	current[nth] = 0xCC;
	current[nth + 1] = 0xB0 + (rand() % 16);
	
	n_bytes = 2;
      }
      break;
    case 1:
      {
	current[nth] = 0xCD;
	current[nth + 1] = 0x80 + (rand() % 48);
	
	n_bytes = 2;
      }
      break;
    }

    current[nth + n_bytes] = '\0';
    str = ags_turtle_read_pn_chars(current,
				   current + n_bytes);
    
    if(str == NULL ||
       g_ascii_strncasecmp(current,
			   str,
			   n_bytes)){
      success = FALSE;
	
      break;
    }
  }
  
  CU_ASSERT(success == TRUE);

  /* test key_0 */  
  str = ags_turtle_read_pn_chars(key_0,
				 key_0 + 2);
  
  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(key_0,
				 str,
				 2));
  
  /* test key_1 */  
  str = ags_turtle_read_pn_chars(key_1,
				 key_1 + 2);
  
  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(key_1,
				 str,
				 2));
}

void
ags_turtle_test_read_pn_prefix()
{
  gchar *no_prefix = ".\0";
  gchar *str;

  static const gchar *prefix[] = {
    "aa.\0",
    "aaa...\0",
    "aaa.zzz\0",
    "aaa...zzz\0",
  };
  
  /* test prefix 0 */  
  str = ags_turtle_read_pn_prefix(prefix[0],
				  prefix[0] + strlen(prefix[0]));
  
  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(prefix[0],
				 str,
				 strlen(prefix[0])));
  
  /* test prefix 1 */  
  str = ags_turtle_read_pn_prefix(prefix[1],
				  prefix[1] + strlen(prefix[1]));
  
  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(prefix[1],
				 str,
				 strlen(prefix[1])));

  /* test prefix 2 */  
  str = ags_turtle_read_pn_prefix(prefix[2],
				  prefix[2] + strlen(prefix[2]));
  
  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(prefix[2],
				 str,
				 strlen(prefix[2])));

  /* test prefix 3 */  
  str = ags_turtle_read_pn_prefix(prefix[3],
				  prefix[3] + strlen(prefix[3]));
  
  CU_ASSERT(str != NULL &&
	    !g_ascii_strncasecmp(prefix[3],
				 str,
				 strlen(prefix[3])));

  /* test no prefix */  
  str = ags_turtle_read_pn_prefix(no_prefix,
				  no_prefix + strlen(no_prefix));
  
  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_pn_local()
{
  gchar *no_local = "%\0";
  gchar *str;

  static const gchar *local[] = {
    "::\0",
    "_:\0",
    ":_\0",
    "_a\0",
    ":%00:\0",
    ":%ff:\0",
    "zzz:\0",
    "000.999\0",
    "0xff\0",
  };

  guint i;
  
  /* test local 0 */
  for(i = 0; i < 9; i++){
    str = ags_turtle_read_pn_local(local[i],
				   local[i] + strlen(local[i]));
  
    CU_ASSERT(str != NULL &&
	      !g_ascii_strncasecmp(local[i],
				   str,
				   strlen(local[i])));
  }

  /* test no local */  
  str = ags_turtle_read_pn_local(no_local,
				 no_local + strlen(no_local));
  
  CU_ASSERT(str == NULL);
}

void
ags_turtle_test_read_plx()
{
  //TODO:JK: implement me
}

void
ags_turtle_test_read_percent()
{
  gchar percent[4];
  gchar *hex = "0123456789abcdef\0";
  gchar *str;
  
  guint i, j;
  gboolean success;
  
  percent[0] = '%';
  percent[3] = '\0';

  success = TRUE;
  
  for(i = 0; i < 16; i++){
    percent[1] = hex[i];
    
    for(j = 0; j < 16; j++){
      percent[2] = hex[j];

      str = ags_turtle_read_percent(percent,
				    percent + strlen(percent));
      
      if(str == NULL ||
	 g_ascii_strncasecmp(percent,
			     str,
			     4)){
	success = FALSE;

	break;
      }
    }
  }
     
  CU_ASSERT(success == TRUE);
}

void
ags_turtle_test_read_hex()
{
  gchar hex[2];
  //TODO:JK: implement me
}

void
ags_turtle_test_read_pn_local_esc()
{
  gchar local_esc[3];
  gchar *escapes = "\\_~.-!$&()*+,;=/?#@%\0";
  gchar *str;

  guint i;
  
  local_esc[0] = '\\';
  local_esc[2] = '\0';

  for(i = 0; i < strlen(escapes); i++){
    local_esc[1] = escapes[i];
    str = ags_turtle_read_pn_local_esc(local_esc,
				       local_esc + strlen(local_esc));
  
    CU_ASSERT(str != NULL &&
	      !g_ascii_strncasecmp(local_esc,
				   str,
				   3));
  }
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C\0");
  putenv("LANG=C\0");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsTurtleTest\0", ags_turtle_test_init_suite, ags_turtle_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsTurtle read iriref\0", ags_turtle_test_read_iriref) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read prefixed name namespace\0", ags_turtle_test_read_pname_ns) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read prefixed name localized name\0", ags_turtle_test_read_pname_ln) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read blank node label\0", ags_turtle_test_read_blank_node_label) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read language tag\0", ags_turtle_test_read_langtag) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read boolean\0", ags_turtle_test_read_boolean) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read integer\0", ags_turtle_test_read_integer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read decimal\0", ags_turtle_test_read_decimal) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read double\0", ags_turtle_test_read_double) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read exponent\0", ags_turtle_test_read_exponent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read string\0", ags_turtle_test_read_string) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read string literal quote\0", ags_turtle_test_read_string_literal_quote) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read string literal single quote\0", ags_turtle_test_read_string_literal_single_quote) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read string literal long quote\0", ags_turtle_test_read_string_literal_long_quote) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read string literal long single quote\0", ags_turtle_test_read_string_literal_long_single_quote) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read uchar\0", ags_turtle_test_read_uchar) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read echar\0", ags_turtle_test_read_echar) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read ws\0", ags_turtle_test_read_ws) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read anon\0", ags_turtle_test_read_anon) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-chars base\0", ags_turtle_test_read_pn_chars_base) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-chars u\0", ags_turtle_test_read_pn_chars_u) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-chars\0", ags_turtle_test_read_pn_chars) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-prefix\0", ags_turtle_test_read_pn_prefix) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-local\0", ags_turtle_test_read_pn_local) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read plx\0", ags_turtle_test_read_plx) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read percent\0", ags_turtle_test_read_percent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read hex\0", ags_turtle_test_read_hex) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTurtle read pn-local esc\0", ags_turtle_test_read_pn_local_esc) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
