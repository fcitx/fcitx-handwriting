#include "service.h"
#include "handwriting-service.h"

static gint handwriting_service_signals[E_SIGNAL_COUNT] = { 0 };

G_DEFINE_TYPE ( HandWritingService, handwriting_service, G_TYPE_OBJECT )

static void
handwriting_service_finalize ( GObject *object )
{
	G_OBJECT_CLASS ( handwriting_service_parent_class )->finalize ( object );
}


static void
handwriting_service_class_init ( HandWritingServiceClass *klass )
{
	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS ( klass );
	object_class->finalize = handwriting_service_finalize;

	const gchar* signalNames [ E_SIGNAL_COUNT ] =
	{
		SIGNAL_SENDWORD
	};

	int i;
	for ( i = 0; i < E_SIGNAL_COUNT ; i++ )
	{
		guint signalId ;
		signalId =
		    g_signal_new ( signalNames [i],
		                   G_OBJECT_CLASS_TYPE ( klass ),
		                   G_SIGNAL_RUN_LAST ,
		                   0,
		                   NULL ,
		                   NULL ,
		                   g_cclosure_marshal_VOID__STRING ,
		                   G_TYPE_NONE ,
		                   1,
		                   G_TYPE_STRING );
		handwriting_service_signals [i] = signalId ;
	}
	dbus_g_object_type_install_info ( TYPE_HANDWRITING_SERVICE ,& dbus_glib_handwriting_service_object_info );

}

static void
handwriting_service_init ( HandWritingService *object )
{

}


HandWritingService *
handwriting_service_new ( void )
{
	return g_object_new ( TYPE_HANDWRITING_SERVICE, NULL );
}

gboolean handwriting_serice_sendword ( HandWritingService* service, const gchar* string )
{
	g_signal_emit ( service,
	                handwriting_service_signals[E_SIGNAL_SEND_WORD],
	                0,
	                string
	              );
}
