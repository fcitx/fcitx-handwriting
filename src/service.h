#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <glib.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <dbus/dbus-glib-lowlevel.h>

#define DBUS_HANDWRITING_SERVICE_PATH  "/handwriting"
#define DBUS_HANDWRITING_SERVICE       "org.fcitx.HandWriting"

#define TYPE_HANDWRITING_SERVICE       (handwriting_service_get_type ())
#define HANDWRITING_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), M_TYPE_TEST_SERVICE, HandWritingService))
#define HANDWRITING_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), M_TYPE_TEST_SERVICE, HandWritingServiceClass))
#define IS_HANDWRITING_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), M_TYPE_TEST_SERVICE))
#define IS_HANDWRITING_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), M_TYPE_TEST_SERVICE))
#define HANDWRITING_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), M_TYPE_TEST_SERVICE, HandWritingServiceClass))

G_BEGIN_DECLS

typedef enum {
    E_SIGNAL_SEND_WORD,
    E_SIGNAL_COUNT
} HandWritingServiceSignal ;

#define SIGNAL_SENDWORD "send_word"


typedef struct _HandWritingService HandWritingService;
typedef struct _HandWritingServiceClass HandWritingServiceClass;

struct _HandWritingService {
        GObject parent;
};

struct _HandWritingServiceClass {
    GObjectClass parent;
};

HandWritingService *handwriting_service_new (void);
GType handwriting_service_get_type (void);
void handwriting_serice_sendword(HandWritingService* service, const gchar* string);


G_END_DECLS

#endif