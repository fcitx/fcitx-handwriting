#ifndef _STROKE_H
#define _STROKE_H

#include <cairo.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <zinnia.h>

#define STROKE_MAX_POINT 2096

/* 手写引擎 */
#define ZINNIA_RECOGNIZER_CH "/usr/lib/zinnia/model/tomoe/handwriting-zh_CN.model"
#define ZINNIA_RECOGNIZER_JA "/usr/lib/zinnia/model/tomoe/handwriting-ja.model"

typedef enum _StrokeStatus{
	STROKE_READY,
	STROKE_START,
	STROKE_END,
	STROKE_ERROR
}StrokeStatus;

typedef struct _Point
{
	short x;
	short y;
}Point;

typedef struct _Stroke
{
	GdkPixbuf *pixbuf;
	GdkPixmap *pixmap ;
	cairo_t *cairo ;

	StrokeStatus status;
	guint timer;
	int i_point;
	Point last;
	Point stroke[STROKE_MAX_POINT];
	int ncstroke;
	char cstroke[STROKE_MAX_POINT*4];
	short pPoints[STROKE_MAX_POINT*4];

	int num;							/* 笔画数 */
	zinnia_recognizer_t *recognizer;	/* 手写识别器 */
	zinnia_character_t  *charactera;	/* 记录手写 */
}Stroke;

Stroke * stroke_create (const gchar *bg_path);
int stroke_start( Stroke *stk);
int stroke_stop( Stroke *stk);
int stroke_clean( Stroke *stk);
int stroke_store( Stroke *stk,Point p);
Point * stroke_extract( Stroke *stk,int *count);
void add_last_point(Stroke *stk,Point p);
void add_point_start(Stroke *stk,Point p);

#endif
