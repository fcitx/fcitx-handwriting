#include <string.h>
#include <stdlib.h>
#include <zinnia.h>

#include "stroke.h"

static void add_point (Stroke *stk, Point p)
{
	gchar *buf, *s;	
	
	buf = g_strdup_printf ("%d,%d,0", p.x, p.y);	
	
	s = stk->cstroke;
	s+=stk->ncstroke;
	stk->ncstroke+=strlen(buf);		
	strcpy (s,buf);		
	g_free (buf);
	
	stk->stroke[stk->i_point].x = stk->last.x = p.x;
	stk->stroke[stk->i_point].y = stk->last.y = p.y;
	stk->pPoints[stk->i_point*2] = p.x ;
  	stk->pPoints[stk->i_point*2+1] = p.y ;
	stk->i_point++;
}

void add_point_start(Stroke *stk,Point p)
{
	gchar *buf,*s;

	buf=g_strdup_printf("%d,%d,0",p.x,p.y);

	s=stk->cstroke;
	s+=stk->ncstroke;
	stk->ncstroke+=strlen(buf);
	strcpy(s,buf);
	g_free(buf);
	
	stk->stroke[stk->i_point].x=stk->last.x=p.x;
	stk->stroke[stk->i_point].y=stk->last.y=p.y;
//	stk->i_point++; //here button release can't ++ ??? why!!
}

/*
void add_last_point(Stroke *stk,Point p)
{
  gchar *buf,*s;

  buf=g_strdup_printf("%d,%d,0",p.x,p.y);

  s=stk->cstroke;
  s+=stk->ncstroke;
  stk->ncstroke+=strlen(buf);
  strcpy(s,buf);
  g_free(buf);
  //DBG("called func %s cstroke = [%s]\n" ,__FUNCTION__ ,stk->cstroke);

  stk->stroke[stk->i_point].x=stk->last.x=p.x;
  stk->stroke[stk->i_point].y=stk->last.y=p.y;
  stk->pPoints[stk->i_point*2] = p.x ;
  stk->pPoints[stk->i_point*2+1] = p.y ;
  printf("all:%d",stk->i_point);
  stk->i_point++;

}
*/

Stroke * stroke_create (const gchar *bg_path)
{
	Stroke * stk;
	stk = g_new0 (Stroke, 1);
		
	stk->pixbuf = gdk_pixbuf_new_from_file (bg_path, NULL);
	if(!stk->pixbuf)
	{
		free(stk);
		return NULL;
	}
	stk->pixmap = gdk_pixmap_new (gdk_get_default_root_window (),
							gdk_pixbuf_get_width(stk->pixbuf),
							gdk_pixbuf_get_height(stk->pixbuf),
							-1);
	gdk_pixbuf_render_pixmap_and_mask (stk->pixbuf, &stk->pixmap, NULL, 255);
	stk->cairo = gdk_cairo_create (GDK_DRAWABLE (stk->pixmap));
	
	stk->i_point = 0;
	stk->last.x = -1;
	stk->last.y = -1;
	
	stk->ncstroke = 0;
	memset (stk->cstroke, 0, STROKE_MAX_POINT*4);
	stk->status = STROKE_READY;
	
	/* 初始化手写引擎 */
	stk->recognizer = zinnia_recognizer_new();
	if (!zinnia_recognizer_open(stk->recognizer, ZINNIA_RECOGNIZER_CH)) {
		fprintf(stderr, "ERROR: %s\n", zinnia_recognizer_strerror(stk->recognizer));
		return NULL;
	}
	stk->charactera = zinnia_character_new();
	zinnia_character_clear(stk->charactera);
	zinnia_character_set_width(stk->charactera, 300);
	zinnia_character_set_height(stk->charactera, 300);
	
	return stk;
}

int stroke_start (Stroke *stk)
{
	if (stk->status != STROKE_READY)		
		return -1;
	stk->status = STROKE_START;	
	return 0;
}

int stroke_stop( Stroke *stk)
{
	if(stk->status!=STROKE_START)		
		return -1;
	stk->status=STROKE_END;	
	return 0;
}

int stroke_clean( Stroke *stk)
{

	if(stk->status!=STROKE_END)	
		return -1;

	cairo_destroy(stk->cairo);	

	gdk_pixbuf_render_pixmap_and_mask(stk->pixbuf,&stk->pixmap,NULL,255);
	stk->cairo = gdk_cairo_create(GDK_DRAWABLE(stk->pixmap));

	stk->i_point=0;
	stk->last.x=-1;
	stk->last.y=-1;

	stk->ncstroke=0;
	//memset(stk->cstroke,0,STROKE_MAX_POINT*4);

	stk->status=STROKE_READY;	
	return 0;
}

int stroke_store (Stroke *stk, Point p)
{
	if( stk->status != STROKE_START) {
		return -1;
	}
	if (stk->i_point >= STROKE_MAX_POINT) {
		return -1;
	}
	if (p.x == -1 && p.y == -1) {
		//add_point(stk,p);		
		add_point_start(stk,p);
		return 0;		
	}
	
	if(stk->last.x == -1 && stk->last.y == -1) {
		cairo_move_to (stk->cairo, p.x, p.y);		
		cairo_line_to (stk->cairo, p.x+1, p.y+1);	
		cairo_set_line_width (stk->cairo, 3.0);
		cairo_set_line_cap (stk->cairo, CAIRO_LINE_CAP_ROUND);	
		cairo_stroke  (stk->cairo);	

		add_point (stk,p);		
		return 0;	
	}
	 


	if(abs(stk->last.x-p.x)<5&&abs(stk->last.y-p.y)<5)	
	{
		return 0;	
	}

	cairo_move_to (stk->cairo, stk->last.x, stk->last.y);
	cairo_line_to (stk->cairo, p.x, p.y);
	cairo_set_line_width (stk->cairo, 3.0);
	cairo_set_line_cap (stk->cairo, CAIRO_LINE_CAP_ROUND);
	cairo_stroke  (stk->cairo);
	add_point(stk,p);		
	
	return 0;
}

Point * stroke_extract( Stroke *stk,int *count)
{
	if(stk->status!=STROKE_END)
		return NULL;
	*count=stk->i_point;
	return stk->stroke;
}

