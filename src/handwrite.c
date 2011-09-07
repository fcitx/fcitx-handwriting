//      handwrite.c
//
//      Copyright 2011 wolf <chfc2009@yeah.net>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkinput.h>
#include <gdk/gdkx.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-bindings.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput.h>
#include <zinnia.h>

#include "handwrite.h"
#include "service.h"

Point p;
gboolean PASS_FLAG = FALSE;

static void
set_window_background ( GtkWidget *window, KeyBoard *keyboard )
{
	GdkPixbuf *pixbuf;
	GdkBitmap *bitmap;
	GdkPixmap *pixmap;
	gchar filepath[256];

	sprintf ( filepath, "%s/beijing.png", keyboard->userdir );
	pixbuf = gdk_pixbuf_new_from_file ( filepath, NULL );
	gdk_pixbuf_render_pixmap_and_mask ( pixbuf, &pixmap, &bitmap, 128 );
	gtk_widget_shape_combine_mask ( window, bitmap, 0, 0 );
	GtkStyle *style = gtk_style_copy ( window->style );
	style->bg_pixmap[GTK_STATE_NORMAL] = g_object_ref ( pixmap );
	if ( style->bg_pixmap[GTK_STATE_NORMAL] )
		g_object_unref ( style->bg_pixmap[GTK_STATE_NORMAL] );
	gtk_widget_set_style ( window, style );
	g_object_unref ( pixbuf );
}

static gboolean
window_move_callback ( GtkWidget* widget, GdkEventButton *event, GdkWindowEdge edge )
{
	if ( event->type == GDK_BUTTON_PRESS )
	{
		if ( 3 == event->button )
		{
			gtk_window_begin_move_drag ( GTK_WINDOW ( gtk_widget_get_toplevel ( widget ) ),
			                             event->button,
			                             event->x_root,
			                             event->y_root,
			                             event->time );
		}
	}

	return FALSE;
}

static void
simulation_keyboard_input ( int key_code )
{
	Display* disp = XOpenDisplay ( NULL );
	if ( disp == NULL )
		return;
	XTestFakeKeyEvent ( disp,key_code, True,0 );
	XTestFakeKeyEvent ( disp,key_code, False,0 );
	XFlush ( disp );
	XCloseDisplay ( disp );
}

static void
set_button_font ( GtkWidget *bt )
{
	GtkWidget *font_label;
	PangoFontDescription* text_font = NULL;
	text_font = pango_font_description_from_string ( "Arial,bold 17" );
	font_label = gtk_bin_get_child ( GTK_BIN ( bt ) );
	gtk_widget_modify_font ( font_label, text_font );
}

static void
set_font_size ( GtkWidget *bt )
{
	GtkWidget *font_label;
	PangoFontDescription* text_font = NULL;
	//~ text_font = pango_font_description_from_string("Arial,bold 13");
	text_font = pango_font_description_from_string ( "WenQuanYi Zen Hei 13" );
	font_label = gtk_bin_get_child ( GTK_BIN ( bt ) );
	gtk_widget_modify_font ( font_label, text_font );
}

static gchar *
get_handwrite_config_dir ()
{
	gchar *configdir;
	configdir = g_strdup_printf ( DATADIR "/fcitx/handwrite" );

	return configdir;
}

static void
stroke_clean_draw ( KeyBoard *keyboard )
{
	stroke_clean ( keyboard->stk );
        gtk_widget_queue_draw ( keyboard->window );
}

static gboolean
create_dbus ( KeyBoard *keyboard )
{
	GError* error = NULL;
	guint   result;
	keyboard->bus = dbus_g_bus_get ( DBUS_BUS_SESSION, &error );
	if ( !keyboard->bus )
	{
		g_warning ( "Failed to connect to the D-BUS daemon: %s", error->message );
		return FALSE;
	}
	keyboard->proxy = dbus_g_proxy_new_for_name ( keyboard->bus,
	                  DBUS_SERVICE_DBUS,
	                  DBUS_PATH_DBUS,
	                  DBUS_INTERFACE_DBUS );
	org_freedesktop_DBus_request_name ( keyboard->proxy,
	                                    DBUS_HANDWRITING_SERVICE,
	                                    DBUS_NAME_FLAG_DO_NOT_QUEUE, &result, &error );

	HandWritingService *service = g_object_new ( TYPE_HANDWRITING_SERVICE, NULL );
	dbus_g_connection_register_g_object ( keyboard->bus, DBUS_HANDWRITING_SERVICE_PATH, G_OBJECT ( service ) );
	keyboard->service = service;

	return TRUE;
}

static void
send_word_callback ( GtkButton *button, gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;
	const gchar *label;

	label = gtk_button_get_label ( GTK_BUTTON ( button ) );
	dbg ( "%s \n", label );

	if ( !label )
		return ;

	handwriting_serice_sendword ( keyboard->service, label );

	stroke_clean_draw ( keyboard );

}

static void
keyboard_english_callback ( GtkButton *button, gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;
	int i;
	gchar *english_info[HAND_WORD_NUM] = {"a","b","c","d","e","f","g","h","i",
	                                      "j","k","l","m","n","o","p","q","r",
	                                      "s","t","u","v","w","x","y","z","A",
	                                      "B","C","D","E","F","G","H","I","J",
	                                      "K","L","M","N","O","P","Q","R","S",
	                                      "T","U","V","W","X","Y","Z","'","\""
	                                     };

	for ( i = 0; i < HAND_WORD_NUM; i++ )
	{
		keyboard->value[i] = english_info[i];
		if ( i < 9 )
		{
			gtk_button_set_label ( GTK_BUTTON ( keyboard->hand_button[i] ), english_info[i] );
			set_font_size ( keyboard->hand_button[i] );
		}
	}
	keyboard->pagenum = 0;
	stroke_clean_draw ( keyboard );
}

static void
keyboard_number_callback ( GtkButton *button, gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;
	int i;
	gchar *number_info[HAND_WORD_NUM] = {"1","2","3","4","5","6","7","8","9",
	                                     "0",",",".","?","!",":","\"","'","(",
	                                     ")","{","}","<",">","[","]","%","#",
	                                     "@","$","^","&","+","-","*","/","=",
	                                     "_","\\","`","~","0","9","8","7","6",
	                                     "5","4","3","2","1","0","10","100","500"
	                                    };

	for ( i = 0; i < HAND_WORD_NUM; i++ )
	{
		keyboard->value[i] = number_info[i];
		if ( i < 9 )
		{
			gtk_button_set_label ( GTK_BUTTON ( keyboard->hand_button[i] ), number_info[i] );
			set_font_size ( keyboard->hand_button[i] );
		}
	}
	keyboard->pagenum = 0;
	stroke_clean_draw ( keyboard );
}

static void
keyboard_prevpage_callback ( GtkButton *button, gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;
	int i, j;

	if ( keyboard->pagenum == 0 )
		return;
	keyboard->pagenum--;
	j = 9 * keyboard->pagenum;
	for ( i = 0; i < 9; i++ )
	{
		gtk_button_set_label ( GTK_BUTTON ( keyboard->hand_button[i] ), keyboard->value[j] );
		set_font_size ( keyboard->hand_button[i] );
		j++;
	}

}

static void
keyboard_nextpage_callback ( GtkButton *button, gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;
	int i, j;

	if ( keyboard->pagenum >= ( HAND_WORD_NUM / 9 )-1 )
		return;
	keyboard->pagenum++;
	j = 9 * keyboard->pagenum;
	for ( i = 0; i < 9; i++ )
	{
		gtk_button_set_label ( GTK_BUTTON ( keyboard->hand_button[i] ), keyboard->value[j] );
		set_font_size ( keyboard->hand_button[i] );
		j++;
	}

}

static void
keyboard_backspace_callback ( GtkButton *button, gpointer data )
{
	simulation_keyboard_input ( 22 );  /* 输入backspace键 */
}

static void
keyboard_spaces_callback ( GtkButton *button, gpointer data )
{
	simulation_keyboard_input ( 65 );		/* 输入空格键 */
}

static void
keyboard_close_callback ( GtkButton *button, gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;

#if 0
	gtk_widget_hide ( keyboard->window );
#else
	zinnia_character_destroy ( keyboard->stk->charactera );
	zinnia_recognizer_destroy ( keyboard->stk->recognizer );
	gtk_main_quit ();
#endif
}

static void
keyboard_enter_callback ( GtkButton *button, gpointer data )
{
	simulation_keyboard_input ( 36 );		/* 输入空格键 */
}

static void
get_word_from_charlib ( KeyBoard *keyboard )
{
	zinnia_result_t *result;
	size_t i;
	const char *value;

	result = zinnia_recognizer_classify ( keyboard->stk->recognizer, keyboard->stk->charactera, HAND_WORD_NUM );
	if ( result == NULL )
	{
		fprintf ( stderr, "%s\n", zinnia_recognizer_strerror ( keyboard->stk->recognizer ) );
		return;
	}

	for ( i = 0; i < zinnia_result_size ( result ); ++i )
	{
		value = zinnia_result_value ( result, i );
		keyboard->value[i] = ( gchar * ) value;
		if ( i < 9 )
		{
			gtk_button_set_label ( GTK_BUTTON ( keyboard->hand_button[i] ), value );
			set_font_size ( keyboard->hand_button[i] );
		}
	}
	keyboard->pagenum = 0;

	zinnia_result_destroy ( result );
}

static gboolean
handwriter_press_filter ( GtkWidget *widget, GdkEventButton *event, gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;

	if ( 1 == event->button )
	{
		if ( keyboard->stk->timer > 0 )
			g_source_remove ( keyboard->stk->timer );
		stroke_start ( keyboard->stk );

		PASS_FLAG = TRUE;
		//~ zinnia_character_add (keyboard->stk->charactera, keyboard->stk->num, event->x, event->y);

		gtk_widget_queue_draw ( widget );
	}
	return FALSE;
}

static gboolean
handwriter_timeout ( gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;

	p.x = -1 ;
	p.y = -1 ;

	stroke_stop ( keyboard->stk );

	get_word_from_charlib ( keyboard );		/* 从字库中获取字 */
	stroke_clean ( keyboard->stk );
	//~ gtk_widget_queue_draw (keyboard->window);

	keyboard->stk->num = 0;
	zinnia_character_clear ( keyboard->stk->charactera );

	return FALSE;
}

static gboolean
handwriter_release_filter ( GtkWidget *widget, GdkEventButton *event, gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;
	Point p;

	if ( 1 == event->button )
	{
		p.x = -1;
		p.y = -1;
		add_point_start ( keyboard->stk,p );

		PASS_FLAG = FALSE;
		//~ zinnia_character_add (keyboard->stk->charactera, keyboard->stk->num, event->x, event->y);

		gtk_widget_queue_draw ( widget );
		keyboard->stk->num = keyboard->stk->num + 1;
		keyboard->stk->timer = g_timeout_add ( 1000, handwriter_timeout, keyboard );
	}
	return FALSE;
}

static gboolean handwriter_motion_filter ( GtkWidget *widget, GdkEventMotion *event, gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;
	GdkModifierType state;
	Point p;
	int x,y;

	if ( PASS_FLAG )
	{
		gdk_window_get_pointer ( event->window, &x, &y, &state );
		//~ if (x%5)
		zinnia_character_add ( keyboard->stk->charactera, keyboard->stk->num, x, y );
	}
	else
		return FALSE;

	if ( ( state & GDK_BUTTON1_MASK ) )
	{
		p.x = ( short ) x;
		p.y = ( short ) y;
		stroke_store ( keyboard->stk, p );
		gtk_widget_queue_draw ( widget );
		return FALSE;
	}

	return FALSE;
}

static gboolean
handwriter_expose_event ( GtkWidget * widget, GdkEventExpose * event, gpointer data )
{
	KeyBoard *keyboard = ( KeyBoard * ) data;

	if ( keyboard->stk == NULL || keyboard->stk->pixmap == NULL )
		return FALSE;

	if ( keyboard->stk->status == STROKE_ERROR )
		return FALSE;

	gdk_draw_drawable ( widget->window,
	                    widget->style->fg_gc[GTK_WIDGET_STATE ( widget ) ],
	                    keyboard->stk->pixmap, event->area.x, event->area.y, event->area.x,
	                    event->area.y, event->area.width, event->area.height );
	return TRUE;
}

static gboolean
handwriter_notify_event ( GtkWidget * widget, GdkEventExpose * event, gpointer data )
{
	GdkCursorType ct;
	
	ct = GDK_PENCIL;
	gdk_window_set_cursor ( widget->window , gdk_cursor_new ( ct ) );
}

static GtkWidget *
create_handwriter_canvas ( KeyBoard *keyboard )
{
	gchar filepath[256];
	GtkWidget *ebox, *img;
	ebox = gtk_event_box_new();

	gtk_widget_set_events ( ebox, gtk_widget_get_events ( ebox )
	                        | GDK_LEAVE_NOTIFY_MASK
	                        | GDK_BUTTON_PRESS_MASK
	                        | GDK_BUTTON_RELEASE_MASK
	                        | GDK_POINTER_MOTION_MASK
	                        | GDK_POINTER_MOTION_HINT_MASK
				| GDK_ENTER_NOTIFY_MASK
				| GDK_LEAVE_NOTIFY_MASK );

	sprintf ( filepath, "%s/writer.png", keyboard->userdir );
	img = gtk_image_new_from_file ( filepath );
	gtk_container_add ( GTK_CONTAINER ( ebox ), img );
	gtk_widget_show ( img );

	g_signal_connect ( ebox, "button_press_event",
	                   G_CALLBACK ( handwriter_press_filter ), keyboard );

	g_signal_connect ( ebox, "button_release_event",
	                   G_CALLBACK ( handwriter_release_filter ), keyboard );

	g_signal_connect ( ebox, "motion_notify_event",
	                   G_CALLBACK ( handwriter_motion_filter ), keyboard );

	g_signal_connect ( ebox, "expose-event",
	                   G_CALLBACK ( handwriter_expose_event ), keyboard );

	g_signal_connect ( ebox, "enter-notify-event",
	                   G_CALLBACK ( handwriter_notify_event ), NULL );

	return ebox;
}

static GtkWidget *
create_hand_button ( KeyBoard *keyboard )
{
	GtkWidget *fixed;
	GtkWidget *drawing;
	GtkWidget *hand_change_bt[8];
	gchar *hand_button_info[] = {"ABC","123.,","上页","下页","删除","空格","回车","关闭"};
	int btx[] = {8,8,8,8,705,705,705,705};
	int bty[] = {8,58,108,158,8,58,108,158};
	int btx1[] = {111,182,253,111,182,253,111,182,253};
	int bty1[] = {11,11,11,75,75,75,139,139,139};
	int i;

	fixed = gtk_fixed_new ();
	gtk_widget_show ( fixed );

	/* 创建手写字显示按钮 */
	for ( i = 0 ;i < HAND_BUTTON_NUM ; i++ )
	{
		keyboard->hand_button[i] = gtk_button_new ();
		gtk_widget_set_size_request ( GTK_WIDGET ( keyboard->hand_button[i] ), 65, 58 );
		gtk_fixed_put ( GTK_FIXED ( fixed ), keyboard->hand_button[i], btx1[i], bty1[i] );
		g_signal_connect ( keyboard->hand_button[i], "clicked", G_CALLBACK ( send_word_callback ), keyboard );
		gtk_widget_set_name ( keyboard->hand_button[i], "button1" );
		gtk_widget_show ( keyboard->hand_button[i] );
	}

	/* 创建功能控制按钮 */
	for ( i = 0 ;i < 8 ; i++ )
	{
		hand_change_bt[i] = gtk_button_new_with_label ( hand_button_info[i] );
		gtk_widget_set_size_request ( hand_change_bt[i], 86, 45 );
		gtk_fixed_put ( GTK_FIXED ( fixed ), hand_change_bt[i], btx[i], bty[i] );
		gtk_widget_set_name ( hand_change_bt[i], "button0" );
		set_button_font ( hand_change_bt[i] );
		gtk_widget_show ( hand_change_bt[i] );
	}
	g_signal_connect ( hand_change_bt[0], "clicked", G_CALLBACK ( keyboard_english_callback ), keyboard );
	g_signal_connect ( hand_change_bt[1], "clicked", G_CALLBACK ( keyboard_number_callback ), keyboard );
	g_signal_connect ( hand_change_bt[2], "clicked", G_CALLBACK ( keyboard_prevpage_callback ), keyboard );
	g_signal_connect ( hand_change_bt[3], "clicked", G_CALLBACK ( keyboard_nextpage_callback ), keyboard );
	g_signal_connect ( hand_change_bt[4], "clicked", G_CALLBACK ( keyboard_backspace_callback ), NULL );
	g_signal_connect ( hand_change_bt[5], "clicked", G_CALLBACK ( keyboard_spaces_callback ), NULL );
	g_signal_connect ( hand_change_bt[6], "clicked", G_CALLBACK ( keyboard_enter_callback ), NULL );
	g_signal_connect ( hand_change_bt[7], "clicked", G_CALLBACK ( keyboard_close_callback ), keyboard );

	/* 创建手写的画图区域 */
	drawing = create_handwriter_canvas ( keyboard );
	gtk_fixed_put ( GTK_FIXED ( fixed ), drawing, 333, 8 );
	gtk_widget_show ( drawing );

	return fixed;
}

static GtkWidget *
create_window ()
{
	GtkWidget *fixed;
	KeyBoard *keyboard;
	gchar filepath[256];

	keyboard = g_new0 ( KeyBoard,1 );
	if ( !create_dbus ( keyboard ) )							/* 创建D-BUS */
	{
		fprintf ( stderr, "create dbus error. \n" );
		return NULL;
	}
	keyboard->userdir = get_handwrite_config_dir ();

	gchar *rcpath = g_strdup_printf ( "%s/gtkrc", keyboard->userdir );
	gtk_rc_parse ( rcpath );	/* 设置GTKRC */
	g_free ( rcpath );

	sprintf ( filepath, "%s/writer.png", keyboard->userdir );
	keyboard->stk = stroke_create ( filepath );		/* 手写初始化 */

	/* 创建键盘窗口 */
	keyboard->window = gtk_window_new ( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_default_size ( GTK_WINDOW ( keyboard->window ), 800, 210 );
	gtk_window_set_keep_above ( GTK_WINDOW ( keyboard->window ), TRUE );
	gtk_window_set_accept_focus ( GTK_WINDOW ( keyboard->window ), FALSE );
	gtk_window_set_skip_taskbar_hint ( GTK_WINDOW ( keyboard->window ), FALSE );
	gtk_window_set_decorated ( GTK_WINDOW ( keyboard->window ), FALSE );
	gtk_window_set_position ( GTK_WINDOW ( keyboard->window ), GTK_WIN_POS_CENTER );
	gtk_widget_add_events ( keyboard->window, GDK_BUTTON_PRESS_MASK );
	gtk_widget_show ( keyboard->window );

	/* 按下鼠标右键可移动窗口 */
	g_signal_connect ( keyboard->window, "button-press-event",
	                   G_CALLBACK ( window_move_callback ), NULL );

	set_window_background ( keyboard->window, keyboard );		/*设置窗口背景*/

	fixed = gtk_fixed_new ();
	gtk_container_add ( GTK_CONTAINER ( keyboard->window ), fixed );
	gtk_widget_show ( fixed );

	/* 创建手写界面部件 */
	keyboard->hand_fixed = create_hand_button ( keyboard );
	gtk_fixed_put ( GTK_FIXED ( fixed ), keyboard->hand_fixed, 0, 0 );

	return keyboard->window;
}

int main ( int argc, char **argv )
{
	GtkWidget *window = NULL;

	gtk_init ( &argc, &argv );

	window = create_window ();
	if ( !window )
		return 1;

	gtk_main ();
	return 0;
}

