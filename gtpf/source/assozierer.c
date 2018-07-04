/******************************************************************************
 *
 *   FILE
 *   ----
 *   assozierer.c
 *
 *   History
 *   -------
 *   2018-06-17   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/
//
//#include <fb.h>
//#include <libpng12/pngconf.h>
//#include "libov/ov_config.h"
//#include "libov/ov_ov.h"
//#include <setjmp.h>
//
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>

#define PNG_DEBUG 3
#include <png.h>

#ifndef OV_COMPILE_LIBRARY_gtpf
#define OV_COMPILE_LIBRARY_gtpf
#endif

#include "gtpf.h"
#include "libov/ov_macros.h"
#include "wandelbareTopologie.h"

#include "libov/ov_path.h"
#include "libov/ov_result.h"
#include "list.h"
#include "geometry2d.h"

#define MAXGAP 30
#define DISCRETFACTOR 10
#define M_PI 3.14159265358979323846

//#define XRATE 5
//#define YRATE	5

int calculateXmax();
int calculateYmax();

typedef struct vector {

} vector_t;

vector_t* getTopsVector();

typedef struct Cell {
	OV_BOOL abnehmbar;
	OV_BOOL abgebbar;
	OV_BOOL dir[1];
} Cell_t;

typedef struct Gitter {
	int x;
	int y;
	int height;
	int width;
	OV_SINGLE step;
	Cell_t* A;
	OV_INSTPTR_wandelbareTopologie_Node pWagon;
} Gitter_t;
/*
 * Gitter Functions
 */
Gitter_t* constructGitter() {
	Gitter_t* w1Gitter = ov_memstack_alloc(sizeof(Gitter_t));
	w1Gitter->pWagon = NULL;
	w1Gitter->x = 0;
	w1Gitter->y = 0;
	w1Gitter->height = 1000;
	w1Gitter->width = 1000;
	w1Gitter->step = 2;
	w1Gitter->A = Ov_HeapMalloc(
		sizeof(Cell_t) * w1Gitter->height * w1Gitter->width);
	for (OV_UINT i = 0; i < w1Gitter->height * w1Gitter->width; ++i) {
		(w1Gitter->A + i)->abgebbar = 0;
		(w1Gitter->A + i)->abnehmbar = 0;
	}
	return w1Gitter;
}

void destructGitter(Gitter_t* gitter) {
	Ov_HeapFree(gitter->A);
}

Rectangular_t* createRectFromNode(OV_INSTPTR_wandelbareTopologie_Node w1) {
	Rectangular_t* rect = rectConstruct();
	rect->b = w1->v_Xlength;
	rect->h = w1->v_Ylength;
	rect->pos.pos.x = w1->v_x;
	rect->pos.pos.y = w1->v_y;
	rect->pos.dir = degToRad(w1->v_ThetaZ);
	return rect;
}

Gitter_t Global;

//void calculate(Netz_t* Netz, Wagen1, Wagen2) {
//	vector_t* tops = getTopsVector();
//	int numOfTops;
//	// = tops->len();
//
//	for (OV_UINT i = 0; i < numOfTops; ++i) {
//		int Factory[i][y][x] = alloc(size(uint16) * x * y)
//		for (OV_INT x = vector_at(tops,i)->; x < max;
//				x += XRATE
//			) {
//				for (OV_INT y = vector_at(tops,i)->; y < max; y+=YRATE) {
//					for theta in [tmin, tmax]
//					currentPos = [top.pos.x + x, top.pos.y + y, top.pos.v+theta]
//					canTake(currentPos)
//					if c[1] = [0,1]
//					reachable(Factory, currentPos, X);
//					if c[0] = [1,0]
//					reachable(Factory, currentPos, Y);
//
//				}
//			}
//		}
//
//		for (OV_UINT i = 0; i < numOfTops - 1; ++i) {
//			for (OV_UINT j = i + 1; j < numOfTops; ++j) {
//				OV_BOOL Filter[ynum][xnum] = filter(Factory[i], Factory[j], canTake)
//			for l in y:
//			for m in x:
//			if(Filter[l][m]):
//
//		}
//	}
//}

Cell_t* cell_at_rel2global(Gitter_t* gitter, OV_INT x, OV_INT y) {
	OV_INT cellx = (x - gitter->x) / gitter->step;
	OV_INT celly = (y - gitter->y) / gitter->step;
	return gitter->A + celly * (gitter->width) + cellx;
}

Cell_t* cell_at(Gitter_t* gitter, OV_INT x, OV_INT y) {
	return gitter->A + y * (gitter->width) + x;
}

void reachable(Gitter_t* gitter, Point_t* pos, int dir, OV_SINGLE range) {
	Cell_t* cell = cell_at_rel2global(gitter, pos->x, pos->y);
	cell->abgebbar = 1;
}

static int gitter2png(Gitter_t* gitter, OV_STRING name) {
	FILE * fp;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	size_t x, y;
	png_byte ** row_pointers = NULL;
	/* "status" contains the return value of this function. At first
	 it is set to a value which means 'failure'. When the routine
	 has finished its work, it is set to a value which means
	 'success'. */
	int status = -1;
	/* The following number is set by trial and error only. I cannot
	 see where it it is documented in the libpng manual.
	 */
	int pixel_size = 1;
	int depth = 8;

	OV_STRING path = NULL;
	ov_string_print(&path, "%s.png", name);

	fp = fopen(path, "wb");
	if(!fp) {
		goto fopen_failed;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL) {
		goto png_create_write_struct_failed;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL) {
		goto png_create_info_struct_failed;
	}

	/* Set up error handling. */

	if(setjmp(png_jmpbuf (png_ptr))) {
		goto png_failure;
	}

	/* Set image attributes. */

	png_set_IHDR(png_ptr, info_ptr, gitter->width, gitter->height, depth,
	PNG_COLOR_TYPE_GRAY,
	PNG_INTERLACE_NONE,
	PNG_COMPRESSION_TYPE_DEFAULT,
	PNG_FILTER_TYPE_DEFAULT);

	/* Initialize rows of PNG. */

	row_pointers = png_malloc(png_ptr, gitter->height * sizeof(png_byte *));
	for (y = 0; y < gitter->height; y++) {
		png_byte *row = png_malloc(png_ptr,
			sizeof(uint8_t) * gitter->width * pixel_size);
		row_pointers[y] = row;
		for (x = 0; x < gitter->width; x++) {
			Cell_t * pixel = cell_at(gitter, x, y);
			*row++ = 255 * pixel->abnehmbar;
			//todo: init all abgebbar to zero
		}
	}

	/* Write the image data to "fp". */

	png_init_io(png_ptr, fp);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	/* The routine has successfully written the file, so we set
	 "status" to a value which indicates success. */

	status = 0;

	for (int y = 0; y < gitter->height; y++) {
		png_free(png_ptr, row_pointers[y]);
	}
	png_free(png_ptr, row_pointers);

	png_failure: png_create_info_struct_failed: png_destroy_write_struct(&png_ptr,
		&info_ptr);
	png_create_write_struct_failed: fclose(fp);
	fopen_failed: return status;
}

void canTakeAtPoint(Gitter_t* gitter, OV_SINGLE x, OV_SINGLE y) {
	Cell_t* cell = cell_at_rel2global(gitter, x, y);
	cell->abnehmbar = 1;
}

void canTakeBetweenPoints(Gitter_t* gitter, Point_t* pnt1, Point_t* pnt2) {
	OV_SINGLE dist = sqrt(
		pow((pnt2->x - pnt1->x), 2) + pow((pnt2->y - pnt1->y), 2));
	OV_INT numOfSteps = dist / gitter->step + 1;
	OV_SINGLE xrate = (pnt2->x - pnt1->x) / numOfSteps;
	OV_SINGLE yrate = (pnt2->y - pnt1->y) / numOfSteps;
	for (OV_UINT i = 0; i < numOfSteps; ++i) {
		canTakeAtPoint(gitter, pnt1->x + i * xrate, pnt1->y + i * yrate);
	}
	canTakeAtPoint(gitter, pnt2->x, pnt2->y);
}

void canTakeRect(Gitter_t* gitter, Rectangular_t* rect) {
	Point_t* c1 = pointConstruct();
	Point_t* c2 = pointConstruct();
	Point_t* c3 = pointConstruct();
	Point_t* c4 = pointConstruct();
	rectGetCorners(rect, c1, c2, c3, c4);
	canTakeBetweenPoints(gitter, c1, c2);
	canTakeBetweenPoints(gitter, c2, c3);
	canTakeBetweenPoints(gitter, c3, c4);
	canTakeBetweenPoints(gitter, c4, c1);
}

Gitter_t* createPics(OV_INSTPTR_wandelbareTopologie_Node w1) {

//		Gitter_t _
//		int Factory[i][y][x] = alloc(size(uint16) * x * y)

	Gitter_t* w1Gitter = constructGitter();

	w1Gitter->pWagon = w1;

	int XRATE = 1;
	int YRATE = 1;
	int VRATE = 1;

	Rectangular_t* rect = rectConstruct();
	rect->b = w1->v_Xlength;
	rect->h = w1->v_Ylength;
	for (OV_INT x = w1->v_TTPSVM.value[0]; x <= w1->v_TTPSVP.value[0]; x +=
			XRATE) {
		for (OV_INT y = w1->v_TTPSVM.value[1]; y <= w1->v_TTPSVP.value[1]; y +=
				YRATE) {
			for (OV_INT v = w1->v_TCSVM.value[2]; v <= w1->v_TCSVP.value[2]; v +=
					VRATE) {
				//todo: type conversion correction
				rect->pos.pos.x = w1->v_x + x;
				rect->pos.pos.y = w1->v_y + y;
				rect->pos.dir = degToRad(w1->v_ThetaZ + v);

				canTakeRect(w1Gitter, rect);
//				if(w1->v_CSV.value[0])
//					reachable(w1Gitter, &currentPos.pos, 0, w1->v_Xlength);
//				if(w1->v_CSV.value[1])
//					reachable(w1Gitter, &currentPos.pos, 90, w1->v_Ylength);
			}
		}
	}

	gitter2png(w1Gitter, w1->v_identifier);

//		for (OV_UINT i = 0; i < numOfTops - 1; ++i) {
//			for (OV_UINT j = i + 1; j < numOfTops; ++j) {
//				OV_BOOL Filter[ynum][xnum] = filter(Factory[i], Factory[j], canTake)
//			for l in y:
//			for m in x:
//			if(Filter[l][m]):
//
//		}
	return w1Gitter;
}

void createAssoc(Gitter_t* g1, Gitter_t* g2) {
	int XRATE = 1;
	int YRATE = 1;
	int VRATE = 1;
	OV_INSTPTR_wandelbareTopologie_Node w1 = g1->pWagon;
	OV_INSTPTR_wandelbareTopologie_Node w2 = g2->pWagon;

	Rectangular_t* rect1 = createRectFromNode(w1);
	Rectangular_t* rect2 = createRectFromNode(w2);

	for (OV_INT x = w1->v_TTPSVM.value[0]; x <= w1->v_TTPSVP.value[0]; x +=
			XRATE) {
		for (OV_INT y = w1->v_TTPSVM.value[1]; y <= w1->v_TTPSVP.value[1]; y +=
				YRATE) {
			for (OV_INT v = w1->v_TCSVM.value[2]; v <= w1->v_TCSVP.value[2]; v +=
					VRATE) {
				//todo: type conversion correction

				rect1->pos.pos.x = w1->v_x + x;
				rect1->pos.pos.y = w1->v_y + y;
				rect1->pos.dir = degToRad(w1->v_ThetaZ + v);

				Point_t* c1 = pointConstruct();
				Point_t* c2 = pointConstruct();
				Point_t* c3 = pointConstruct();
				Point_t* c4 = pointConstruct();
				rectGetCorners(rect1, c1, c2, c3, c4);

				for (OV_INT x2 = w2->v_TTPSVM.value[0]; x2 <= w2->v_TTPSVP.value[0];
						x2 += XRATE) {
					for (OV_INT y2 = w2->v_TTPSVM.value[1]; y2 <= w2->v_TTPSVP.value[1];
							y2 += YRATE) {
						for (OV_INT v2 = w2->v_TCSVM.value[2]; v2 <= w2->v_TCSVP.value[2];
								v2 += VRATE) {

							rect2->pos.pos.x = w2->v_x + x2;
							rect2->pos.pos.y = w2->v_y + y2;
							rect2->pos.dir = degToRad(w2->v_ThetaZ + v2);

							Degree_t dirs[4] = { 0, 90, 180, 270 };
							OV_SINGLE dist[4] = { w2->v_Xlength / 2, w2->v_Ylength / 2,
									w1->v_Xlength / 2, w1->v_Ylength / 2 };
							Point_t* corners[4] = { c1, c2, c3, c4 };

							Rectangular_t* abnehmRect = rectConstruct();

							//0
							for (OV_UINT i = 0; i < 4; ++i) {
								abnehmRect->h = dist[i];
								abnehmRect->b = MAXGAP;
								abnehmRect->pos.dir = rect2->pos.dir + degToRad(dirs[i]);

								Point_t* gapVector = pointConstruct();
								gapVector->x = MAXGAP / 2 + dist[i];
								pointRotate(gapVector, abnehmRect->pos.dir);
								abnehmRect->pos.pos = *pointAdd(&rect2->pos.pos, gapVector);

								if(isPointInRect(abnehmRect, corners[i])) {
									if(isPointInRect(abnehmRect, corners[(i + 1) % 4])) {
										//seite1
										Ov_Link(wandelbareTopologie_Neighbour, w1, w2);
										ov_logfile_info("linking %s %s", w1->v_identifier,
											w2->v_identifier);
									} else if(isPointInRect(abnehmRect, corners[(i + 3) % 4])) {
										//seite4
										Ov_Link(wandelbareTopologie_Neighbour, w1, w2);
										ov_logfile_info("linking %s %s", w1->v_identifier,
											w2->v_identifier);
									} else {
										continue;
									}
								} else {
									if(isPointInRect(abnehmRect, corners[(i + 2) % 4]))
										if(isPointInRect(abnehmRect, corners[(i + 1) % 4])) {
											//seite2
											Ov_Link(wandelbareTopologie_Neighbour, w1, w2);
											ov_logfile_info("linking %s %s", w1->v_identifier,
												w2->v_identifier);
										} else if(isPointInRect(abnehmRect, corners[(i + 3) % 4])) {
											//seite3
											Ov_Link(wandelbareTopologie_Neighbour, w1, w2);
											ov_logfile_info("linking %s %s", w1->v_identifier,
												w2->v_identifier);
										} else {
											continue;
										}
									else {
										continue;
									}
								}
							}

						}
					}
				}

			}
		}
	}
}

void drawRect(Gitter_t* gitter, Rectangular_t* rect) {
	Point_t* c1 = pointConstruct();
	Point_t* c2 = pointConstruct();
	Point_t* c3 = pointConstruct();
	Point_t* c4 = pointConstruct();
	rectGetCorners(rect, c1, c2, c3, c4);

	canTakeBetweenPoints(gitter, c1, c2);
	canTakeBetweenPoints(gitter, c2, c3);
	canTakeBetweenPoints(gitter, c3, c4);
	canTakeBetweenPoints(gitter, c4, c1);
}

void drawAssoc(Gitter_t* gitter, OV_INSTPTR_wandelbareTopologie_Node wagon1,
		OV_INSTPTR_wandelbareTopologie_Node wagon2) {
	Rectangular_t* rect1 = createRectFromNode(wagon1);
	Rectangular_t* rect2 = createRectFromNode(wagon2);
	canTakeBetweenPoints(gitter, &rect1->pos.pos, &rect2->pos.pos);
}

void visualize_topologie(OV_INSTPTR_ov_domain ptop) {
	Gitter_t* gitter = constructGitter();

	OV_INSTPTR_wandelbareTopologie_Node pchild = NULL;
	Ov_ForEachChild(ov_containment, ptop, pchild)
	{
		Rectangular_t* rect = rectConstruct();
		rect->b = pchild->v_Xlength;
		rect->h = pchild->v_Ylength;

		for (OV_INT x = pchild->v_TTPSVM.value[0]; x <= pchild->v_TTPSVP.value[0];
				x += 1) {
			for (OV_INT y = pchild->v_TTPSVM.value[1]; y <= pchild->v_TTPSVP.value[1];
					y += 1) {
				for (OV_INT v = pchild->v_TCSVM.value[2]; v <= pchild->v_TCSVP.value[2];
						v += 1) {
					//todo: type conversion correction
					rect->pos.pos.x = pchild->v_x + x;
					rect->pos.pos.y = pchild->v_y + y;
					rect->pos.dir = degToRad(pchild->v_ThetaZ + v);

					drawRect(gitter, rect);
				}
			}
		}
//	wandelbareTopologie_Neighbour,)
//
//	OV_INSTPTR_wandelbareTopologie_Node
//	neighbour =
//Ov_GetChild(
//		;
//		if(neighbour) {
//			drawAssoc(gitter, pchild, neighbour);
//		}
//	}
		gitter2png(gitter, "visualization");
	}
}

//
OV_RESULT gtpf_assozierer_execute(OV_INSTPTR_gtpf_assozierer pinst) {
// variables
	OV_RESULT result = OV_ERR_OK;
//	Data_t* recipes = NULL;

//	int xmax = calculateXmax();
//	int ymax = calculateYmax();
//	int xnum = xmax / DISCRETFACTOR;
//	int ynum = ymax / DISCRETFACTOR;
//
// param check
	ov_memstack_lock();
	OV_INSTPTR_ov_domain ptop = Ov_StaticPtrCast(ov_domain,
		ov_path_getobjectpointer(pinst->v_Path, 2));
	OV_INSTPTR_wandelbareTopologie_Node pchild = NULL;
	if(!ptop) {
		ov_logfile_error("topology could not be found");
		return OV_ERR_BADPARAM;
	}

	list_t* picList = constructList(sizeof(Gitter_t));
	Ov_ForEachChild(ov_containment, ptop, pchild)
	{
		insertFirst(picList, createPics(pchild));
	}
//	listSort(picList, relation);
	listNode_t* elem1 = NULL;
	listNode_t* elem2 = NULL;
	listIterate(picList, elem1)
	{
		listIterate(picList, elem2)
		{
//			canGive(elem1, elem2){
			if(((Gitter_t*) elem1->data)->pWagon != ((Gitter_t*) elem2->data)->pWagon)
				createAssoc((Gitter_t*) elem1->data, (Gitter_t*) elem2->data);
		}
	}
	visualize_topologie(ptop);
	destructList(picList);

	ov_memstack_unlock();
	return result;
}

OV_DLLFNCEXPORT void gtpf_assozierer_typemethod(OV_INSTPTR_fb_functionblock pfb,
		OV_TIME *pltc) {
	/*
	 *   local variables
	 */
	OV_INSTPTR_gtpf_assozierer pinst = Ov_StaticPtrCast(gtpf_assozierer, pfb);

	OV_RESULT result = OV_ERR_OK;
	result = gtpf_assozierer_execute(pinst);
	switch (result) {
		case OV_ERR_OK:
			ov_logfile_info("gtpf: done");
			pinst->v_result = result;
			break;
		case OV_ERR_BADPARAM:
			ov_logfile_error("gtpf: bad param");
			pinst->v_result = result;
			break;
		default:
			ov_logfile_error("gtpf: failed \n error: %s",
				ov_result_getresulttext(result));
			pinst->v_result = 1;
	}

	return;
}
