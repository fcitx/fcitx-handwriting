#ifndef _HANDWRITE_H
#define _HANDWRITE_H

#include "stroke.h"
#include <dbus/dbus-glib.h>
#include "service.h"

#define HAND_BUTTON_NUM	9
#define HAND_WORD_NUM 54

#define DEBUG  1
#ifdef DEBUG
#define dbg(txt, args... ) fprintf(stderr, "HANDWRITE-DEBUG: " txt , ##args )
#else
#define dbg(txt, args... ) /* nothing */
#endif


typedef struct _KeyBoard
{
	GtkWidget *window;
	GtkWidget *hand_fixed;
	GtkWidget *hand_button[HAND_BUTTON_NUM];	/* 显示字的按钮 */
	DBusGConnection *bus;						/* D-BUS */
	gchar *userdir;								/* 用户目录 $HOME/.fcitx/handwrite */

	Stroke *stk;

	int pagenum;								/* 当前字的页数 */
	gchar *value[HAND_WORD_NUM-1];				/* 当前的字 */
    DBusGProxy* proxy;
    HandWritingService* service;
}KeyBoard;

#endif
