AgsApplicationContext *application_context;
AgsFile *file;

GError *error;

static const gchar *filename = "my_file.xml\0";

application_context = ags_application_context_get_instance();

file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				"application-context\0", application_context,
				"filename\0", filename,
				NULL);

error = NULL;
ags_file_rw_open(file,
		 TRUE,
		 &error);
ags_file_write(file);
ags_file_close(file);
