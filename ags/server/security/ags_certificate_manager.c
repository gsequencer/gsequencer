/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/server/security/ags_certificate_manager.h>

void ags_certificate_manager_class_init(AgsCertificateManagerClass *certificate_manager);
void ags_certificate_manager_init (AgsCertificateManager *certificate_manager);
void ags_certificate_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_certificate_manager
 * @short_description: Singleton pattern to organize certificate
 * @title: AgsCertificateManager
 * @section_id:
 * @include: ags/server/security/ags_certificate_manager.h
 *
 * The #AgsCertificateManager manages your certificate.
 */

static gpointer ags_certificate_manager_parent_class = NULL;

AgsCertificateManager *ags_certificate_manager = NULL;

GType
ags_certificate_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_certificate_manager = 0;

    static const GTypeInfo ags_certificate_manager_info = {
      sizeof (AgsCertificateManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_certificate_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCertificateManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_certificate_manager_init,
    };

    ags_type_certificate_manager = g_type_register_static(G_TYPE_OBJECT,
							  "AgsCertificateManager",
							  &ags_certificate_manager_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_certificate_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_certificate_manager_class_init(AgsCertificateManagerClass *certificate_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_certificate_manager_parent_class = g_type_class_peek_parent(certificate_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) certificate_manager;

  gobject->finalize = ags_certificate_manager_finalize;
}

void
ags_certificate_manager_init(AgsCertificateManager *certificate_manager)
{
  g_rec_mutex_init(&(certificate_manager->obj_mutex));

  certificate_manager->certificate = NULL;
}

void
ags_certificate_manager_finalize(GObject *gobject)
{
  AgsCertificateManager *certificate_manager;

  certificate_manager = AGS_CERTIFICATE_MANAGER(gobject);

  if(certificate_manager->certificate != NULL){
    g_list_free_full(certificate_manager->certificate,
		     g_object_unref);
  }

  G_OBJECT_CLASS(ags_certificate_manager_parent_class)->finalize(gobject);
}

/**
 * ags_certificate_manager_get_certificate:
 * @certificate_manager: the #AgsCertificateManager
 * 
 * Get certificate.
 * 
 * Returns: the #GList-struct containing #GObject implementing #AgsCertificate
 * 
 * Since: 3.0.0
 */
GList*
ags_certificate_manager_get_certificate(AgsCertificateManager *certificate_manager)
{
  GList *certificate;

  GRecMutex *certificate_manager_mutex;
  
  if(!AGS_IS_CERTIFICATE_MANAGER(certificate_manager)){
    return(NULL);
  }

  /* get certificate manager mutex */
  certificate_manager_mutex = AGS_CERTIFICATE_MANAGER_GET_OBJ_MUTEX(certificate_manager);

  /* get certificate */
  g_rec_mutex_lock(certificate_manager_mutex);

  certificate = g_list_copy_deep(certificate_manager->certificate,
				 g_object_ref,
				 NULL);

  g_rec_mutex_unlock(certificate_manager_mutex);
  
  return(certificate);
}

/**
 * ags_certificate_manager_add_certificate:
 * @certificate_manager: the #AgsCertificateManager
 * @certificate: the #GObject implementing #AgsCertificate
 * 
 * Add @certificate to @certificate_manager.
 * 
 * Since: 3.0.0
 */
void
ags_certificate_manager_add_certificate(AgsCertificateManager *certificate_manager,
					GObject *certificate)
{
  GRecMutex *certificate_manager_mutex;
  
  if(!AGS_IS_CERTIFICATE_MANAGER(certificate_manager) ||
     !AGS_IS_CERTIFICATE(certificate)){
    return;
  }

  /* get certificate manager mutex */
  certificate_manager_mutex = AGS_CERTIFICATE_MANAGER_GET_OBJ_MUTEX(certificate_manager);

  /* add certificate */
  g_rec_mutex_lock(certificate_manager_mutex);

  if(g_list_find(certificate_manager->certificate, certificate) == NULL){
    certificate_manager->certificate = g_list_prepend(certificate_manager->certificate,
						      certificate);
    g_object_ref(certificate);
  }

  g_rec_mutex_unlock(certificate_manager_mutex);
}

/**
 * ags_certificate_manager_remove_certificate:
 * @certificate_manager: the #AgsCertificateManager
 * @certificate: the #GObject implementing #AgsCertificate
 * 
 * Remove @certificate from @certificate_manager.
 * 
 * Since: 3.0.0
 */
void
ags_certificate_manager_remove_certificate(AgsCertificateManager *certificate_manager,
					   GObject *certificate)
{
  GRecMutex *certificate_manager_mutex;
  
  if(!AGS_IS_CERTIFICATE_MANAGER(certificate_manager) ||
     !AGS_IS_CERTIFICATE(certificate)){
    return;
  }

  /* get certificate manager mutex */
  certificate_manager_mutex = AGS_CERTIFICATE_MANAGER_GET_OBJ_MUTEX(certificate_manager);

  /* remove certificate */
  g_rec_mutex_lock(certificate_manager_mutex);

  if(g_list_find(certificate_manager->certificate, certificate) != NULL){
    certificate_manager->certificate = g_list_remove(certificate_manager->certificate,
						     certificate);
    g_object_unref(certificate);
  }

  g_rec_mutex_unlock(certificate_manager_mutex);
}

/**
 * ags_certificate_manager_verify_certificate:
 * @certificate_manager: the #AgsCertificateManager
 * @certs: the certs
 * 
 * Verify @certs.
 * 
 * Returns: %TRUE if valid, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_certificate_manager_verify_certificate(AgsCertificateManager *certificate_manager,
					   gchar *certs)
{
  //TODO:JK: implement me
  
  return(FALSE);
}

/**
 * ags_certificate_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsCertificateManager
 *
 * Since: 3.0.0
 */
AgsCertificateManager*
ags_certificate_manager_get_instance()
{
  static GRecMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_certificate_manager == NULL){
    ags_certificate_manager = ags_certificate_manager_new();
  }
  
  g_mutex_unlock(&mutex);

  return(ags_certificate_manager);
}

/**
 * ags_certificate_manager_new:
 *
 * Creates an #AgsCertificateManager
 *
 * Returns: a new #AgsCertificateManager
 *
 * Since: 3.0.0
 */
AgsCertificateManager*
ags_certificate_manager_new()
{
  AgsCertificateManager *certificate_manager;

  certificate_manager = (AgsCertificateManager *) g_object_new(AGS_TYPE_CERTIFICATE_MANAGER,
							       NULL);

  return(certificate_manager);
}
