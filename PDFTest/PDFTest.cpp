//// PDFTest.cpp : Defines the entry point for the console application.
////
//
#include "stdafx.h"

#include "windows.h"
#include "include\hpdf.h"
//#include <hpdf.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#pragma comment(lib, "lib/libhpdf.lib")
#pragma comment(lib, "lib/zlib.lib")
#pragma comment(lib, "lib/libpng.lib")


jmp_buf env;

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler(HPDF_STATUS   error_no,
HPDF_STATUS   detail_no,
void         *user_data)
{
	printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
		(HPDF_UINT)detail_no);
	longjmp(env, 1);
}

void
print_page(HPDF_Page page, HPDF_Font font, int page_num)
{
	char buf[50];

	HPDF_Page_SetWidth(page, 200);
	HPDF_Page_SetHeight(page, 200);

	HPDF_Page_SetFontAndSize(page, font, 20);

	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, 50, 150);
//#ifdef __WIN32__
	_snprintf_s(buf, 50, "Page:%d", page_num);
//#else
//	_snprintf(buf, 50, "Page:%d", page_num);
//#endif
	HPDF_Page_ShowText(page, buf);
	HPDF_Page_EndText(page);
}

void
draw_image(HPDF_Doc     pdf,
const char  *filename,
float        x,
float        y,
const char  *text)
{
#ifdef __WIN32__
	const char* FILE_SEPARATOR = "\\";
#else
	const char* FILE_SEPARATOR = "/";
#endif
	char filename1[255];

	HPDF_Page page = HPDF_GetCurrentPage(pdf);
	HPDF_Image image;

	strcpy_s(filename1, "pngsuite");
	strcat_s(filename1, FILE_SEPARATOR);
	strcat_s(filename1, filename);

	image = HPDF_LoadPngImageFromFile(pdf, filename1);

	/* Draw image to the canvas. */
	HPDF_Page_DrawImage(page, image, x, y, HPDF_Image_GetWidth(image),
		HPDF_Image_GetHeight(image));

	/* Print the text. */
	HPDF_Page_BeginText(page);
	HPDF_Page_SetTextLeading(page, 16);
	HPDF_Page_MoveTextPos(page, x, y);
	HPDF_Page_ShowTextNextLine(page, filename);
	HPDF_Page_ShowTextNextLine(page, text);
	HPDF_Page_EndText(page);
}
int main(int argc, char **argv)
{
	HPDF_Doc  pdf;
	HPDF_Font font;
	HPDF_Page index_page;
	HPDF_Page page[9];
	HPDF_Destination dst;
	char fname[256];
	HPDF_Rect rect;
	HPDF_Point tp;
	HPDF_Annotation annot;
	HPDF_UINT i;
	const char *uri = "D://b.png";//"http://libharu.org";

	strcpy_s(fname, argv[0]);
	strcat_s(fname, ".pdf");

	pdf = HPDF_New(error_handler, NULL);
	if (!pdf) {
		printf("error: cannot create PdfDoc object\n");
		return 1;
	}

	if (setjmp(env)) {
		HPDF_Free(pdf);
		return 1;
	}

	/* create default-font */
	HPDF_UseCNSFonts(pdf);
	HPDF_UseCNTFonts(pdf);
	HPDF_UseCNTEncodings(pdf);
	HPDF_UseCNSEncodings(pdf);
	font = HPDF_GetFont(pdf, "SimSun", "GBK-EUC-H");
	//font = HPDF_GetFont(pdf, "Helvetica", "GBK-EUC-H");

	/* create index page */
	index_page = HPDF_AddPage(pdf);
	HPDF_Page_SetWidth(index_page, 300);
	HPDF_Page_SetHeight(index_page, 220);

	/* Add 7 pages to the document. */
	for (i = 0; i < 7; i++) {
		page[i] = HPDF_AddPage(pdf);
		print_page(page[i], font, i + 1);
	}

	HPDF_Page_BeginText(index_page);
	HPDF_Page_SetFontAndSize(index_page, font, 10);
	HPDF_Page_MoveTextPos(index_page, 15, 200);
	HPDF_Page_ShowText(index_page, "Link Annotation Demo");
	HPDF_Page_EndText(index_page);

	/*
	* Create Link-Annotation object on index page.
	*/
	HPDF_Page_BeginText(index_page);
	HPDF_Page_SetFontAndSize(index_page, font, 8);
	HPDF_Page_MoveTextPos(index_page, 20, 180);
	HPDF_Page_SetTextLeading(index_page, 23);

	/* page1 (HPDF_ANNOT_NO_HIGHTLIGHT) */
	tp = HPDF_Page_GetCurrentTextPos(index_page);

	HPDF_Page_ShowText(index_page, "asdf 我们是Jump to Page1 (HilightMode=HPDF_ANNOT_NO_HIGHTLIGHT)");
	rect.left = tp.x - 4;
	rect.bottom = tp.y - 4;
	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
	rect.top = tp.y + 10;

	HPDF_Page_MoveToNextLine(index_page);

	dst = HPDF_Page_CreateDestination(page[0]);

	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);

	HPDF_LinkAnnot_SetHighlightMode(annot, HPDF_ANNOT_NO_HIGHTLIGHT);


	/* page2 (HPDF_ANNOT_INVERT_BOX) */
	tp = HPDF_Page_GetCurrentTextPos(index_page);

	HPDF_Page_ShowText(index_page, "Jump to Page2 (HilightMode=HPDF_ANNOT_INVERT_BOX)");
	rect.left = tp.x - 4;
	rect.bottom = tp.y - 4;
	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
	rect.top = tp.y + 10;

	HPDF_Page_MoveToNextLine(index_page);

	dst = HPDF_Page_CreateDestination(page[1]);

	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);

	HPDF_LinkAnnot_SetHighlightMode(annot, HPDF_ANNOT_INVERT_BOX);


	/* page3 (HPDF_ANNOT_INVERT_BORDER) */
	tp = HPDF_Page_GetCurrentTextPos(index_page);

	HPDF_Page_ShowText(index_page, "Jump to Page3 (HilightMode=HPDF_ANNOT_INVERT_BORDER)");
	rect.left = tp.x - 4;
	rect.bottom = tp.y - 4;
	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
	rect.top = tp.y + 10;

	HPDF_Page_MoveToNextLine(index_page);

	dst = HPDF_Page_CreateDestination(page[2]);

	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);

	HPDF_LinkAnnot_SetHighlightMode(annot, HPDF_ANNOT_INVERT_BORDER);


	/* page4 (HPDF_ANNOT_DOWN_APPEARANCE) */
	tp = HPDF_Page_GetCurrentTextPos(index_page);

	HPDF_Page_ShowText(index_page, "Jump to Page4 (HilightMode=HPDF_ANNOT_DOWN_APPEARANCE)");
	rect.left = tp.x - 4;
	rect.bottom = tp.y - 4;
	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
	rect.top = tp.y + 10;

	HPDF_Page_MoveToNextLine(index_page);

	dst = HPDF_Page_CreateDestination(page[3]);

	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);

	HPDF_LinkAnnot_SetHighlightMode(annot, HPDF_ANNOT_DOWN_APPEARANCE);


	/* page5 (dash border) */
	tp = HPDF_Page_GetCurrentTextPos(index_page);

	HPDF_Page_ShowText(index_page, "Jump to Page5 (dash border)");
	rect.left = tp.x - 4;
	rect.bottom = tp.y - 4;
	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
	rect.top = tp.y + 10;

	HPDF_Page_MoveToNextLine(index_page);

	dst = HPDF_Page_CreateDestination(page[4]);

	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);

	HPDF_LinkAnnot_SetBorderStyle(annot, 1, 3, 2);


	/* page6 (no border) */
	tp = HPDF_Page_GetCurrentTextPos(index_page);

	HPDF_Page_ShowText(index_page, "Jump to Page6 (no border)");
	rect.left = tp.x - 4;
	rect.bottom = tp.y - 4;
	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
	rect.top = tp.y + 10;

	HPDF_Page_MoveToNextLine(index_page);

	dst = HPDF_Page_CreateDestination(page[5]);

	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);

	HPDF_LinkAnnot_SetBorderStyle(annot, 0, 0, 0);


	/* page7 (bold border) */
	tp = HPDF_Page_GetCurrentTextPos(index_page);

	HPDF_Page_ShowText(index_page, "Jump to Page7 (bold border)");
	rect.left = tp.x - 4;
	rect.bottom = tp.y - 4;
	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
	rect.top = tp.y + 10;

	HPDF_Page_MoveToNextLine(index_page);

	dst = HPDF_Page_CreateDestination(page[6]);

	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);

	HPDF_LinkAnnot_SetBorderStyle(annot, 2, 0, 0);


	/* URI link */
	tp = HPDF_Page_GetCurrentTextPos(index_page);

	HPDF_Page_ShowText(index_page, "URI (");
	HPDF_Page_ShowText(index_page, uri);
	HPDF_Page_ShowText(index_page, ")");

	rect.left = tp.x - 4;
	rect.bottom = tp.y - 4;
	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
	rect.top = tp.y + 10;

	HPDF_Page_CreateURILinkAnnot(index_page, rect, uri);

	HPDF_Page_EndText(index_page);

	/* save the document to a file */
	HPDF_SaveToFile(pdf, fname);

	/* clean up */
	HPDF_Free(pdf);

	return 0;
}

//jmp_buf env;
//
//#ifdef HPDF_DLL
//void  __stdcall
//#else
//void
//#endif
//error_handler(HPDF_STATUS   error_no,
//HPDF_STATUS   detail_no,
//void         *user_data)
//{
//	printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
//		(HPDF_UINT)detail_no);
//	longjmp(env, 1);
//}
//
//
//void
//print_page(HPDF_Page page, HPDF_Font font, int page_num)
//{
//	char buf[50];
//
//	HPDF_Page_SetWidth(page, 200);
//	HPDF_Page_SetHeight(page, 200);
//
//	HPDF_Page_SetFontAndSize(page, font, 20);
//
//	HPDF_Page_BeginText(page);
//	HPDF_Page_MoveTextPos(page, 50, 150);
////#ifdef __WIN32__
//	_snprintf_s(buf, 50, "Page:%d", page_num);
////#else
////	_snprintf(buf, 50, "Page:%d", page_num);
////#endif
//	HPDF_Page_ShowText(page, buf);
//	HPDF_Page_EndText(page);
//}
//
//void
//draw_image(HPDF_Doc     pdf,
//const char  *filename,
//float        x,
//float        y,
//const char  *text)
//{
//#ifdef __WIN32__
//	const char* FILE_SEPARATOR = "\\";
//#else
//	const char* FILE_SEPARATOR = "/";
//#endif
//	char filename1[255];
//
//	HPDF_Page page = HPDF_GetCurrentPage(pdf);
//	HPDF_Image image;
//
//	strcpy_s(filename1, "pngsuite");
//	strcat_s(filename1, FILE_SEPARATOR);
//	strcat_s(filename1, filename);
//
//	image = HPDF_LoadPngImageFromFile(pdf, filename1);
//
//	/* Draw image to the canvas. */
//	HPDF_Page_DrawImage(page, image, x, y, HPDF_Image_GetWidth(image),
//		HPDF_Image_GetHeight(image));
//
//	/* Print the text. */
//	HPDF_Page_BeginText(page);
//	HPDF_Page_SetTextLeading(page, 16);
//	HPDF_Page_MoveTextPos(page, x, y);
//	HPDF_Page_ShowTextNextLine(page, filename);
//	HPDF_Page_ShowTextNextLine(page, text);
//	HPDF_Page_EndText(page);
//}
//
//int main(int argc, char **argv)
//{
//	HPDF_Doc  pdf;
//	HPDF_Font font;
//	HPDF_Page index_page;
//	HPDF_Page page[9];
//	HPDF_Destination dst;
//	char fname[256];
//	HPDF_Rect rect;
//	HPDF_Point tp;
//	HPDF_Annotation annot;
//	HPDF_UINT i;
//	HPDF_Image hImage;
//
//	const char *uri = "bin/CSC.doc";//"http://libharu.org";
//
//	strcpy_s(fname, argv[0]);
//	strcat_s(fname, ".pdf");
//
//	pdf = HPDF_New(error_handler, NULL);
//	if (!pdf) {
//		printf("error: cannot create PdfDoc object\n");
//		return 1;
//	}
//
//	if (setjmp(env)) {
//		HPDF_Free(pdf);
//		return 1;
//	}
//
//	/* create default-font */
//	font = HPDF_GetFont(pdf, "Helvetica", NULL);
//	
//	hImage = HPDF_LoadPngImageFromFile(pdf, "d:/b.png");
//
//	/* add a new page object. */
//	HPDF_Page pageNew = HPDF_AddPage(pdf);
//
//	dst = HPDF_Page_CreateDestination(pageNew);
//	HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(pageNew), 1);
//	HPDF_SetOpenAction(pdf, dst);
//
//	HPDF_Page_SetHeight(pageNew, HPDF_Image_GetHeight(hImage));
//	HPDF_Page_SetWidth(pageNew, HPDF_Image_GetWidth(hImage));
//
//	HPDF_Page_SetFontAndSize(pageNew, font, 12);
//
//	unsigned int oldWidth = HPDF_Image_GetWidth(hImage);
//
//	hImage = HPDF_LoadPngImageFromFile(pdf, "d:/b.png");
//	if (hImage)
//	{
//		int x = 0;
//		int y = 0;
//		HPDF_Page_DrawImage(pageNew,
//			hImage,
//			x, y,
//			HPDF_Image_GetWidth(hImage),
//			HPDF_Image_GetHeight(hImage));
//	}
//	tp = HPDF_Page_GetCurrentTextPos(pageNew);
//	rect.left = tp.x+1;
//	rect.bottom = tp.y + HPDF_Image_GetHeight(hImage);
//	rect.right = HPDF_Image_GetWidth(hImage) + 4;
//	rect.top =tp.y + 1;
//
//	HPDF_Page_CreateURILinkAnnot(pageNew, rect, uri);
//	//HPDF_Page_EndText(pageNew);
//
//	HPDF_SaveToFile(pdf, "d://test.pdf");
//	HPDF_Free(pdf);
//	////////////////////////////////////////////////////////////////////////
//
//	/* create index page */
//	index_page = HPDF_AddPage(pdf);
//	HPDF_Page_SetWidth(index_page, 300);
//	HPDF_Page_SetHeight(index_page, 220);
//
//	/* Add 7 pages to the document. */
//	for (i = 0; i < 7; i++) {
//		page[i] = HPDF_AddPage(pdf);
//		print_page(page[i], font, i + 1);
//	}
//
//	HPDF_Page_BeginText(index_page);
//	HPDF_Page_SetFontAndSize(index_page, font, 10);
//	HPDF_Page_MoveTextPos(index_page, 15, 200);
//	HPDF_Page_ShowText(index_page, "Link Annotation Demo");
//	HPDF_Page_EndText(index_page);
//
//	/*
//	* Create Link-Annotation object on index page.
//	*/
//	HPDF_Page_BeginText(index_page);
//	HPDF_Page_SetFontAndSize(index_page, font, 8);
//	HPDF_Page_MoveTextPos(index_page, 20, 180);
//	HPDF_Page_SetTextLeading(index_page, 23);
//
//	/* page1 (HPDF_ANNOT_NO_HIGHTLIGHT) */
//	tp = HPDF_Page_GetCurrentTextPos(index_page);
//
//	HPDF_Page_ShowText(index_page, "Jump to Page1 (HilightMode=HPDF_ANNOT_NO_HIGHTLIGHT)");
//	rect.left = tp.x - 4;
//	rect.bottom = tp.y - 4;
//	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
//	rect.top = tp.y + 10;
//
//	HPDF_Page_MoveToNextLine(index_page);
//
//	dst = HPDF_Page_CreateDestination(page[0]);
//
//	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);
//
//	HPDF_LinkAnnot_SetHighlightMode(annot, HPDF_ANNOT_NO_HIGHTLIGHT);
//
//
//	/* page2 (HPDF_ANNOT_INVERT_BOX) */
//	tp = HPDF_Page_GetCurrentTextPos(index_page);
//
//	HPDF_Page_ShowText(index_page, "Jump to Page2 (HilightMode=HPDF_ANNOT_INVERT_BOX)");
//	rect.left = tp.x - 4;
//	rect.bottom = tp.y - 4;
//	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
//	rect.top = tp.y + 10;
//
//	HPDF_Page_MoveToNextLine(index_page);
//
//	dst = HPDF_Page_CreateDestination(page[1]);
//
//	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);
//
//	HPDF_LinkAnnot_SetHighlightMode(annot, HPDF_ANNOT_INVERT_BOX);
//
//
//	/* page3 (HPDF_ANNOT_INVERT_BORDER) */
//	tp = HPDF_Page_GetCurrentTextPos(index_page);
//
//	HPDF_Page_ShowText(index_page, "Jump to Page3 (HilightMode=HPDF_ANNOT_INVERT_BORDER)");
//	rect.left = tp.x - 4;
//	rect.bottom = tp.y - 4;
//	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
//	rect.top = tp.y + 10;
//
//	HPDF_Page_MoveToNextLine(index_page);
//
//	dst = HPDF_Page_CreateDestination(page[2]);
//
//	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);
//
//	HPDF_LinkAnnot_SetHighlightMode(annot, HPDF_ANNOT_INVERT_BORDER);
//
//
//	/* page4 (HPDF_ANNOT_DOWN_APPEARANCE) */
//	tp = HPDF_Page_GetCurrentTextPos(index_page);
//
//	HPDF_Page_ShowText(index_page, "Jump to Page4 (HilightMode=HPDF_ANNOT_DOWN_APPEARANCE)");
//	rect.left = tp.x - 4;
//	rect.bottom = tp.y - 4;
//	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
//	rect.top = tp.y + 10;
//
//	HPDF_Page_MoveToNextLine(index_page);
//
//	dst = HPDF_Page_CreateDestination(page[3]);
//
//	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);
//
//	HPDF_LinkAnnot_SetHighlightMode(annot, HPDF_ANNOT_DOWN_APPEARANCE);
//
//
//	/* page5 (dash border) */
//	tp = HPDF_Page_GetCurrentTextPos(index_page);
//
//	HPDF_Page_ShowText(index_page, "Jump to Page5 (dash border)");
//	rect.left = tp.x - 4;
//	rect.bottom = tp.y - 4;
//	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
//	rect.top = tp.y + 10;
//
//	HPDF_Page_MoveToNextLine(index_page);
//
//	dst = HPDF_Page_CreateDestination(page[4]);
//
//	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);
//
//	HPDF_LinkAnnot_SetBorderStyle(annot, 1, 3, 2);
//
//
//	/* page6 (no border) */
//	tp = HPDF_Page_GetCurrentTextPos(index_page);
//
//	HPDF_Page_ShowText(index_page, "阿斯蒂芬Jump to Page6 (no border)");
//	rect.left = tp.x - 4;
//	rect.bottom = tp.y - 4;
//	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
//	rect.top = tp.y + 10;
//
//	HPDF_Page_MoveToNextLine(index_page);
//
//	dst = HPDF_Page_CreateDestination(page[5]);
//
//	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);
//
//	HPDF_LinkAnnot_SetBorderStyle(annot, 0, 0, 0);
//
//
//	/* page7 (bold border) */
//	tp = HPDF_Page_GetCurrentTextPos(index_page);
//
//	HPDF_Page_ShowText(index_page, "Jump to Page7 (bold border)");
//	rect.left = tp.x - 4;
//	rect.bottom = tp.y - 4;
//	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
//	rect.top = tp.y + 10;
//
//	HPDF_Page_MoveToNextLine(index_page);
//
//	dst = HPDF_Page_CreateDestination(page[6]);
//
//	annot = HPDF_Page_CreateLinkAnnot(index_page, rect, dst);
//
//	HPDF_LinkAnnot_SetBorderStyle(annot, 2, 0, 0);
//
//	/* URI link */
//#if 0
//	//HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL); 
//	//HPDF_Page_BeginText(index_page);
//	tp = HPDF_Page_GetCurrentTextPos(index_page);
//
//	hImage = HPDF_LoadPngImageFromFile(pdf, "d:/b.png");
//	HPDF_Page_SetWidth(index_page, HPDF_Image_GetWidth(hImage));
//	HPDF_Page_SetHeight(index_page, HPDF_Image_GetHeight(hImage));
//
//	dst = HPDF_Page_CreateDestination(index_page);
//	HPDF_Destination_SetXYZ(dst, tp.y + 4, HPDF_Page_GetHeight(index_page), 1);
//	HPDF_SetOpenAction(pdf, dst);
//
//	//HPDF_Page_BeginText(index_page);
//	HPDF_Page_MoveTextPos(index_page, 220, HPDF_Page_GetHeight(index_page) - 70);
//	draw_image(pdf, "d:/b.png", tp.x, tp.y, "CSC.doc");
//	rect.left = tp.x - 4;
//	rect.bottom = tp.y - HPDF_Image_GetHeight(hImage);
//	rect.right = HPDF_Image_GetWidth(hImage) + 4;
//	rect.top = tp.y + 10;
//
//#else
//	tp = HPDF_Page_GetCurrentTextPos(index_page);
//
//	HPDF_Page_ShowText(index_page, "URI (");
//	HPDF_Page_ShowText(index_page, /*uri*/"hello");
//	HPDF_Page_ShowText(index_page, ")");
//
//	rect.left = tp.x - 4;
//	rect.bottom = tp.y - 4;
//	rect.right = HPDF_Page_GetCurrentTextPos(index_page).x + 4;
//	rect.top = tp.y + 10;
//#endif
//
//	HPDF_Page_CreateURILinkAnnot(index_page, rect, uri);
//
//	HPDF_Page_EndText(index_page);
//
//	/* save the document to a file */
//	HPDF_SaveToFile(pdf, fname);
//
//	/* clean up */
//	HPDF_Free(pdf);
//
//	return 0;
//}
//
////void
////error_handler(HPDF_STATUS   error_no,
////HPDF_STATUS   detail_no,
////void         *user_data)
////{
////	printf("ERROR: error_no=%04X, detail_no=%u/n", (HPDF_UINT)error_no,
////		(HPDF_UINT)detail_no);
////	longjmp(env, 1);
////}
////
////int main(int nArgc, char** pArgv)
////{
////	HPDF_Doc pdf;
////	HPDF_Page page;
////	HPDF_Destination dst;
////	HPDF_Font font;
////	HPDF_Image hpdfImage;
////
////	pdf = HPDF_New(error_handler, NULL);
////	if (!pdf) {
////		printf("ERROR: cannot create pdf object./n");
////		return 1;
////	}
////
////if (setjmp(env)) {
////	HPDF_Free(pdf);
////	return 1;
////}
////
////	//    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);
////
////	font = HPDF_GetFont(pdf, "Helvetica", NULL);
////
////	char * pstrPath[2] = {
////		"d://a.png",
////		"d://b.png"
////	};
////
////	hpdfImage = HPDF_LoadPngImageFromFile(pdf, pstrPath[0]);
////
////	/* add a new page object. */
////	page = HPDF_AddPage(pdf);
////
////	dst = HPDF_Page_CreateDestination(page);
////	HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page), 1);
////	HPDF_SetOpenAction(pdf, dst);
////
////	HPDF_Page_SetHeight(page, HPDF_Image_GetHeight(hpdfImage));
////	HPDF_Page_SetWidth(page, HPDF_Image_GetWidth(hpdfImage));
////
////	HPDF_Page_SetFontAndSize(page, font, 12);
////
////	unsigned int oldWidth = HPDF_Image_GetWidth(hpdfImage);
////
////	for (int i = 0; i < 2; i++)
////	{
////		/*
////
////		HPDF_Page_SetRGBFill (page, 1.0, 0, 0);
////		HPDF_Page_MoveTo (page, 100, 100);
////		HPDF_Page_LineTo (page, 100, 180);
////		*/
////
////		//    HPDF_SetPageMode(pdf, HPDF_PAGE_MODE_FULL_SCREEN);
////		hpdfImage = HPDF_LoadPngImageFromFile(pdf, pstrPath[i]);
////
////		if (hpdfImage)
////		{
////			int x = 0;
////			int y = 0;
////			if (i == 1)
////			{
////				x = oldWidth - HPDF_Image_GetWidth(hpdfImage) - 3;
////			}
////
////			HPDF_Page_DrawImage(page,
////				hpdfImage,
////				x, y,
////				HPDF_Image_GetWidth(hpdfImage),
////				HPDF_Image_GetHeight(hpdfImage));
////		}
////	}
////
////	/* save the document to a file */
////	HPDF_SaveToFile(pdf, "d://test.pdf");
////	/* clean up */
////	HPDF_Free(pdf);
////
////	return 0;
////}
