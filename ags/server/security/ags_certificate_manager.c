/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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
  static GType ags_type_certificate_manager = 0;

  if(!ags_type_certificate_manager){
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
							  "AgsCertificateManager\0",
							  &ags_certificate_manager_info,
							  0);
  }

  return (ags_type_certificate_manager);
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

GList*
ags_certificate_manager_get_certificate(AgsCertificateManager *certificate_manager)
{
  if(certificate_manager == NULL){
    certificate_manager = ags_certificate_manager_get_instance();
  }
  
  return(certificate_manager->certificate);
}

void
ags_certificate_manager_add_certificate(AgsCertificateManager *certificate_manager,
					GObject *certificate)
{
  if(certificate_manager == NULL){
    certificate_manager = ags_certificate_manager_get_instance();
  }

  certificate_manager->certificate = g_list_prepend(certificate_manager->certificate,
						    certificate);
}

void
ags_certificate_manager_remove_certificate(AgsCertificateManager *certificate_manager,
					   GObject *certificate)
{
  if(certificate_manager == NULL){
    certificate_manager = ags_certificate_manager_get_instance();
  }

  certificate_manager->certificate = g_list_remove(certificate_manager->certificate,
						   certificate);
}

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
 * Since: 1.0.0
 */
AgsCertificateManager*
ags_certificate_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_certificate_manager == NULL){
    ags_certificate_manager = ags_certificate_manager_new();

    pthread_mutex_unlock(&(mutex));
  }else{
    pthread_mutex_unlock(&(mutex));
  }

  return(ags_certificate_manager);
}

/**
 * ags_certificate_manager_new:
 *
 * Creates an #AgsCertificateManager
 *
 * Returns: a new #AgsCertificateManager
 *
 * Since: 1.0.0
 */
AgsCertificateManager*
ags_certificate_manager_new()
{
  AgsCertificateManager *certificate_manager;

  certificate_manager = (AgsCertificateManager *) g_object_new(AGS_TYPE_CERTIFICATE_MANAGER,
							       NULL);

  return(certificate_manager);
}
