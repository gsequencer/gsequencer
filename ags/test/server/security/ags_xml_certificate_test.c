/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2019 Joël Krähemann
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

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_xml_certificate_test_init_suite();
int ags_xml_certificate_test_clean_suite();

void ags_xml_certificate_test_get_cert_uuid();
void ags_xml_certificate_test_set_domain();
void ags_xml_certificate_test_get_domain();
void ags_xml_certificate_test_set_key_type();
void ags_xml_certificate_test_get_key_type();
void ags_xml_certificate_test_set_public_key_file();
void ags_xml_certificate_test_get_public_key_file();
void ags_xml_certificate_test_set_private_key_file();
void ags_xml_certificate_test_get_private_key_file();
void ags_xml_certificate_test_open_filename();

#define AGS_XML_CERTIFICATE_TEST_GET_CERT_UUID_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_CERTIFICATE_TEST_GET_CERT_UUID_DEFAULT_SECURITY_TOKEN "ags-test-security-token"

#define AGS_XML_CERTIFICATE_TEST_SET_DOMAIN_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_CERTIFICATE_TEST_SET_DOMAIN_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_CERTIFICATE_TEST_SET_DOMAIN_DEFAULT_CERT_UUID "ags-test-cert-uuid"
#define AGS_XML_CERTIFICATE_TEST_SET_DOMAIN_DEFAULT_DOMAIN "ags-test-domain"

#define AGS_XML_CERTIFICATE_TEST_GET_DOMAIN_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_CERTIFICATE_TEST_GET_DOMAIN_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_CERTIFICATE_TEST_GET_DOMAIN_DEFAULT_CERT_UUID "ags-test-cert-uuid"

#define AGS_XML_CERTIFICATE_TEST_SET_KEY_TYPE_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_CERTIFICATE_TEST_SET_KEY_TYPE_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_CERTIFICATE_TEST_SET_KEY_TYPE_DEFAULT_CERT_UUID "ags-test-cert-uuid"
#define AGS_XML_CERTIFICATE_TEST_SET_KEY_TYPE_DEFAULT_KEY_TYPE "ags-test-key-type"

#define AGS_XML_CERTIFICATE_TEST_GET_KEY_TYPE_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_CERTIFICATE_TEST_GET_KEY_TYPE_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_CERTIFICATE_TEST_GET_KEY_TYPE_DEFAULT_CERT_UUID "ags-test-cert-uuid"

#define AGS_XML_CERTIFICATE_TEST_SET_PUBLIC_KEY_FILE_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_CERTIFICATE_TEST_SET_PUBLIC_KEY_FILE_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_CERTIFICATE_TEST_SET_PUBLIC_KEY_FILE_DEFAULT_CERT_UUID "ags-test-cert-uuid"
#define AGS_XML_CERTIFICATE_TEST_SET_PUBLIC_KEY_FILE_DEFAULT_PUBLIC_KEY_FILE "./ags-test-public-key-file"

#define AGS_XML_CERTIFICATE_TEST_GET_PUBLIC_KEY_FILE_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_CERTIFICATE_TEST_GET_PUBLIC_KEY_FILE_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_CERTIFICATE_TEST_GET_PUBLIC_KEY_FILE_DEFAULT_CERT_UUID "ags-test-cert-uuid"

#define AGS_XML_CERTIFICATE_TEST_SET_PRIVATE_KEY_FILE_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_CERTIFICATE_TEST_SET_PRIVATE_KEY_FILE_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_CERTIFICATE_TEST_SET_PRIVATE_KEY_FILE_DEFAULT_CERT_UUID "ags-test-cert-uuid"
#define AGS_XML_CERTIFICATE_TEST_SET_PRIVATE_KEY_FILE_DEFAULT_PRIVATE_KEY_FILE "./ags-test-private-key-file"

#define AGS_XML_CERTIFICATE_TEST_GET_PRIVATE_KEY_FILE_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_CERTIFICATE_TEST_GET_PRIVATE_KEY_FILE_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_CERTIFICATE_TEST_GET_PRIVATE_KEY_FILE_DEFAULT_CERT_UUID "ags-test-cert-uuid"

#define AGS_XML_CERTIFICATE_TEST_OPEN_FILENAME AGS_SRC_DIR "/" "ags_certificate_test.xml"

AgsServerApplicationContext *server_application_context;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xml_certificate_test_init_suite()
{
  AgsConfig *config;

  ags_priority_load_defaults(ags_priority_get_instance());  

  config = ags_config_get_instance();

  server_application_context = (AgsApplicationContext *) ags_server_application_context_new();
  g_object_ref(server_application_context);

  ags_application_context_prepare(server_application_context);
  ags_application_context_setup(server_application_context);

  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xml_certificate_test_clean_suite()
{
  return(0);
}

void
ags_xml_certificate_test_get_cert_uuid()
{
  AgsXmlCertificate *xml_certificate;
  AgsSecurityContext *security_context;

  GError *error;
  
  xml_certificate = ags_xml_certificate_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  ags_certificate_get_cert_uuid(AGS_CERTIFICATE(xml_certificate),
				security_context,
				AGS_XML_CERTIFICATE_TEST_GET_CERT_UUID_DEFAULT_USER_UUID,
				AGS_XML_CERTIFICATE_TEST_GET_CERT_UUID_DEFAULT_SECURITY_TOKEN,
				&error);
  
  //TODO:JK: implement me
}

void
ags_xml_certificate_test_set_domain()
{
  AgsXmlCertificate *xml_certificate;
  AgsSecurityContext *security_context;

  GError *error;
  
  xml_certificate = ags_xml_certificate_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  ags_certificate_set_domain(AGS_CERTIFICATE(xml_certificate),
			     security_context,
			     AGS_XML_CERTIFICATE_TEST_SET_DOMAIN_DEFAULT_USER_UUID,
			     AGS_XML_CERTIFICATE_TEST_SET_DOMAIN_DEFAULT_SECURITY_TOKEN,
			     AGS_XML_CERTIFICATE_TEST_SET_DOMAIN_DEFAULT_CERT_UUID,
			     AGS_XML_CERTIFICATE_TEST_SET_DOMAIN_DEFAULT_DOMAIN,
			     &error);
  
  //TODO:JK: implement me
}

void
ags_xml_certificate_test_get_domain()
{
  AgsXmlCertificate *xml_certificate;
  AgsSecurityContext *security_context;

  gchar *domain;
  
  GError *error;
  
  xml_certificate = ags_xml_certificate_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  domain = ags_certificate_get_domain(AGS_CERTIFICATE(xml_certificate),
				      security_context,
				      AGS_XML_CERTIFICATE_TEST_GET_DOMAIN_DEFAULT_USER_UUID,
				      AGS_XML_CERTIFICATE_TEST_GET_DOMAIN_DEFAULT_SECURITY_TOKEN,
				      AGS_XML_CERTIFICATE_TEST_GET_DOMAIN_DEFAULT_CERT_UUID,
				      &error);

  CU_ASSERT(domain == NULL);
  
  //TODO:JK: implement me
}

void
ags_xml_certificate_test_set_key_type()
{
  AgsXmlCertificate *xml_certificate;
  AgsSecurityContext *security_context;

  GError *error;
  
  xml_certificate = ags_xml_certificate_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  ags_certificate_set_key_type(AGS_CERTIFICATE(xml_certificate),
			       security_context,
			       AGS_XML_CERTIFICATE_TEST_SET_KEY_TYPE_DEFAULT_USER_UUID,
			       AGS_XML_CERTIFICATE_TEST_SET_KEY_TYPE_DEFAULT_SECURITY_TOKEN,
			       AGS_XML_CERTIFICATE_TEST_SET_KEY_TYPE_DEFAULT_CERT_UUID,
			       AGS_XML_CERTIFICATE_TEST_SET_KEY_TYPE_DEFAULT_KEY_TYPE,
			       &error);
  
  //TODO:JK: implement me
}

void
ags_xml_certificate_test_get_key_type()
{
  AgsXmlCertificate *xml_certificate;
  AgsSecurityContext *security_context;

  gchar *key_type;
  
  GError *error;
  
  xml_certificate = ags_xml_certificate_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  key_type = ags_certificate_get_key_type(AGS_CERTIFICATE(xml_certificate),
					  security_context,
					  AGS_XML_CERTIFICATE_TEST_GET_KEY_TYPE_DEFAULT_USER_UUID,
					  AGS_XML_CERTIFICATE_TEST_GET_KEY_TYPE_DEFAULT_SECURITY_TOKEN,
					  AGS_XML_CERTIFICATE_TEST_GET_KEY_TYPE_DEFAULT_CERT_UUID,
					  &error);

  CU_ASSERT(key_type == NULL);
  
  //TODO:JK: implement me
}

void
ags_xml_certificate_test_set_public_key_file()
{
  AgsXmlCertificate *xml_certificate;
  AgsSecurityContext *security_context;

  GError *error;
  
  xml_certificate = ags_xml_certificate_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  ags_certificate_set_public_key_file(AGS_CERTIFICATE(xml_certificate),
				      security_context,
				      AGS_XML_CERTIFICATE_TEST_SET_PUBLIC_KEY_FILE_DEFAULT_USER_UUID,
				      AGS_XML_CERTIFICATE_TEST_SET_PUBLIC_KEY_FILE_DEFAULT_SECURITY_TOKEN,
				      AGS_XML_CERTIFICATE_TEST_SET_PUBLIC_KEY_FILE_DEFAULT_CERT_UUID,
				      AGS_XML_CERTIFICATE_TEST_SET_PUBLIC_KEY_FILE_DEFAULT_PUBLIC_KEY_FILE,
				      &error);
  
  //TODO:JK: implement me
}

void
ags_xml_certificate_test_get_public_key_file()
{
  AgsXmlCertificate *xml_certificate;
  AgsSecurityContext *security_context;

  gchar *public_key_file;
  
  GError *error;
  
  xml_certificate = ags_xml_certificate_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  public_key_file = ags_certificate_get_public_key_file(AGS_CERTIFICATE(xml_certificate),
							security_context,
							AGS_XML_CERTIFICATE_TEST_GET_PUBLIC_KEY_FILE_DEFAULT_USER_UUID,
							AGS_XML_CERTIFICATE_TEST_GET_PUBLIC_KEY_FILE_DEFAULT_SECURITY_TOKEN,
							AGS_XML_CERTIFICATE_TEST_GET_PUBLIC_KEY_FILE_DEFAULT_CERT_UUID,
							&error);

  CU_ASSERT(public_key_file == NULL);
  
  //TODO:JK: implement me
}

void
ags_xml_certificate_test_set_private_key_file()
{
  AgsXmlCertificate *xml_certificate;
  AgsSecurityContext *security_context;

  GError *error;
  
  xml_certificate = ags_xml_certificate_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  ags_certificate_set_private_key_file(AGS_CERTIFICATE(xml_certificate),
				       security_context,
				       AGS_XML_CERTIFICATE_TEST_SET_PRIVATE_KEY_FILE_DEFAULT_USER_UUID,
				       AGS_XML_CERTIFICATE_TEST_SET_PRIVATE_KEY_FILE_DEFAULT_SECURITY_TOKEN,
				       AGS_XML_CERTIFICATE_TEST_SET_PRIVATE_KEY_FILE_DEFAULT_CERT_UUID,
				       AGS_XML_CERTIFICATE_TEST_SET_PRIVATE_KEY_FILE_DEFAULT_PRIVATE_KEY_FILE,
				       &error);
  
  //TODO:JK: implement me
}

void
ags_xml_certificate_test_get_private_key_file()
{
  AgsXmlCertificate *xml_certificate;
  AgsSecurityContext *security_context;

  gchar *private_key_file;
  
  GError *error;
  
  xml_certificate = ags_xml_certificate_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  private_key_file = ags_certificate_get_private_key_file(AGS_CERTIFICATE(xml_certificate),
							  security_context,
							  AGS_XML_CERTIFICATE_TEST_GET_PRIVATE_KEY_FILE_DEFAULT_USER_UUID,
							  AGS_XML_CERTIFICATE_TEST_GET_PRIVATE_KEY_FILE_DEFAULT_SECURITY_TOKEN,
							  AGS_XML_CERTIFICATE_TEST_GET_PRIVATE_KEY_FILE_DEFAULT_CERT_UUID,
							  &error);

  CU_ASSERT(private_key_file == NULL);
  
  //TODO:JK: implement me
}

void
ags_xml_certificate_test_open_filename()
{
  AgsXmlPasswordStore *xml_certificate;
  
  xml_certificate = ags_xml_certificate_new();

  ags_xml_certificate_open_filename(xml_certificate,
				    AGS_XML_CERTIFICATE_TEST_OPEN_FILENAME);

  CU_ASSERT(xml_certificate->filename != NULL);
  CU_ASSERT(xml_certificate->doc != NULL);
  CU_ASSERT(xml_certificate->root_node != NULL);
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
  pSuite = CU_add_suite("AgsXmlCertificateTest", ags_xml_certificate_test_init_suite, ags_xml_certificate_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsXmlCertificate get cert UUID", ags_xml_certificate_test_get_cert_uuid) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlCertificate set domain", ags_xml_certificate_test_set_domain) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlCertificate get domain", ags_xml_certificate_test_get_domain) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlCertificate set key type", ags_xml_certificate_test_set_key_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlCertificate get key type", ags_xml_certificate_test_get_key_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlCertificate set public key file", ags_xml_certificate_test_set_public_key_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlCertificate get public key file", ags_xml_certificate_test_get_public_key_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlCertificate set private key file", ags_xml_certificate_test_set_private_key_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlCertificate get private key file", ags_xml_certificate_test_get_private_key_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlCertificate open filename", ags_xml_certificate_test_open_filename) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
