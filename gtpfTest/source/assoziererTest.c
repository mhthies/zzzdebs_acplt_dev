/******************************************************************************
 *
 *   FILE
 *   ----
 *   assozierer.c
 *
 *   History
 *   -------
 *   2018-06-20   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_gtpfTest
#define OV_COMPILE_LIBRARY_gtpfTest
#endif

#include "gtpfTest.h"
#include "gtpf.h"
#include "libov/ov_macros.h"
#include "libov/ov_result.h"
#include "libov/ov_path.h"

#include "gitter.h"

#include "unity.h"
#include "unity_fixture.h"

#include "ovunity.h"
#include "ovunity_helper.h"

#include "CException.h"


OV_INSTPTR_ovunity_main gpinst;
OV_INSTPTR_gtpf_assozierer gpobj;
OV_TIME *gpltc;

TEST_GROUP(assozierer);

TEST_SETUP(assozierer) {
	ov_memstack_lock();
	//This is run before EACH TEST
//	ov_string_setvalue(&gpinst->v_start, "PE004");
//	ov_string_set_vecvalue_one(&gpinst->v_recipe, 1, "PE005");
//	gpinst->v_EN = 0;
}

TEST_TEAR_DOWN(assozierer) {
	ov_memstack_unlock();
}

TEST(assozierer, assozierer_default) {
	OV_STRING case_path = ovunity_getCasePath(gpinst, "case_default");
	ovunity_loadEnv(gpinst, "default.json", case_path);
	ov_string_setvalue(&gpobj->v_Path, case_path);
	gtpf_assozierer_typemethod(Ov_StaticPtrCast(fb_functionblock, gpobj), gpltc);

	//image
	Gitter_t* gitter = gitterConstruct();
	OV_INSTPTR_ov_domain ptop = Ov_StaticPtrCast(ov_domain,
		ov_path_getobjectpointer(case_path, 2));
	draw_top(gitter, ptop);
	gitter2png(gitter, "assoziererTest/case_default_vis");

	TEST_ASSERT_EQUAL(gpobj->v_result, 0);
}

TEST(assozierer, assozierer_2neighbour) {
	OV_STRING case_path = ovunity_getCasePath(gpinst, "case_2neighbour_fix");
	ovunity_loadEnv(gpinst, "2Neighbour.json", case_path);
	ov_string_setvalue(&gpobj->v_Path, case_path);
	gtpf_assozierer_typemethod(Ov_StaticPtrCast(fb_functionblock, gpobj), gpltc);

	//image
	Gitter_t* gitter = gitterConstruct();
	OV_INSTPTR_ov_domain ptop = Ov_StaticPtrCast(ov_domain,
		ov_path_getobjectpointer(case_path, 2));
	draw_top(gitter, ptop);
	gitter2png(gitter, "assoziererTest/case_2neighbour_fix_vis");

	TEST_ASSERT_EQUAL(gpobj->v_result, 0);
}

TEST(assozierer, assozierer_schieber) {
	OV_STRING case_path = ovunity_getCasePath(gpinst, "case_schieber");
	ovunity_loadEnv(gpinst, "schieber.json", case_path);
	gpobj->v_MAXGAP = 20;
	ov_string_setvalue(&gpobj->v_Path, case_path);
	gtpf_assozierer_typemethod(Ov_StaticPtrCast(fb_functionblock, gpobj), gpltc);

	//image
	Gitter_t* gitter = gitterConstruct();
	OV_INSTPTR_ov_domain ptop = Ov_StaticPtrCast(ov_domain,
		ov_path_getobjectpointer(case_path, 2));
	draw_top(gitter, ptop);
	gitter2png(gitter, "assoziererTest/case_schieber_vis");

	TEST_ASSERT_EQUAL(gpobj->v_result, 0);
}
//
//TEST(assozierer, assozierer_badstart){
//	ov_string_setvalue(&gpinst->v_start, "PE0033");
//	gtpf_assozierer_typemethod(Ov_StaticPtrCast(fb_functionblock, gpinst), gpltc);
//	TEST_ASSERT_EQUAL(gpinst->v_result, 4);
//}
//	//test2 bad top
//TEST(assozierer, assozierer_badtopo){
////	ov_logfile_info("test: bad topo");
//	ov_string_setvalue(&gpinst->v_topologie, "PE03");
//	gtpf_assozierer_typemethod(Ov_StaticPtrCast(fb_functionblock, gpinst), gpltc);
//	TEST_ASSERT_EQUAL(gpinst->v_result, 4);
//}
//
////test3 start==end
//TEST(assozierer, assozierer_start_same_end){
////	ov_logfile_info("test: start==end");
//	ov_string_set_vecvalue_one(&gpinst->v_recipe, 1, "PE004");
//	gtpf_assozierer_typemethod(Ov_StaticPtrCast(fb_functionblock, gpinst), gpltc);
//	TEST_ASSERT_EQUAL(gpinst->v_result,0);
//	OV_UINT pathDirLen = 0;
//	TEST_ASSERT_EQUAL(gpinst->v_pathDir.veclen, pathDirLen);
//
//	OV_UINT pathNodeLen = 0;
//	TEST_ASSERT_EQUAL(gpinst->v_pathNode.veclen, pathNodeLen);
//
//	OV_UINT paramLen = 0;
//	TEST_ASSERT_EQUAL(gpinst->v_parameter.veclen, paramLen);
//}
//
//TEST(assozierer, assozierer_heat_turn){
//	//test4
//	ov_logfile_info("test: heat & turn");
//	ov_string_set_vecvalue_one(&gpinst->v_recipe, 3, "PE009 Heat", "PE025 Turn",
//		"PE033");
//	gtpf_assozierer_typemethod(Ov_StaticPtrCast(fb_functionblock, gpinst), gpltc);
//	TEST_ASSERT_EQUAL(gpinst->v_result, 0);
//
//	OV_UINT pathDirLen = 16;
//	OV_STRING pathDirValue[] = { "right", "right", "right", "right",
//			"heat", "left", "left", "left", "down", "right", "right", "right", "turn",
//			"right", "right", "right" };
//	TEST_ASSERT_EQUAL(gpinst->v_pathDir.veclen, pathDirLen);
//	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirValue, gpinst->v_pathDir.value,
//		pathDirLen);
//
//	OV_UINT pathNodeLen = 16;
//	OV_STRING pathNodeValue[] = { "PE034", "PE005", "PE008", "PE009",
//			"PE009", "PE008", "PE005", "PE034", "PE034", "PE031", "PE030", "PE025",
//			"PE025", "PE028", "PE027", "PE033" };
//	TEST_ASSERT_EQUAL(gpinst->v_pathNode.veclen, pathNodeLen);
//	TEST_ASSERT_EQUAL_STRING_ARRAY(pathNodeValue, gpinst->v_pathNode.value,
//		pathNodeLen);
//
//	OV_UINT paramLen = 16;
//	OV_STRING paramValue[] = {NULL, NULL, NULL, NULL, "150", NULL, NULL, NULL,
//			NULL, NULL, NULL, NULL, "180", NULL, NULL, NULL };
//	TEST_ASSERT_EQUAL(gpinst->v_parameter.veclen, paramLen);
//	TEST_ASSERT_EQUAL_STRING_ARRAY(paramValue, gpinst->v_parameter.value, paramLen);
//}
//
//TEST(assozierer, assozierer_heat2_turn2){
//	//test5
////	ov_logfile_info("test: (heat&heat) & (turn&turn)");
//	ov_string_set_vecvalue_one(&gpinst->v_recipe, 3, "PE009 Heat", "PE009 Heat",
//		"PE025 Turn", "PE025 Turn");
//	gtpf_assozierer_typemethod(Ov_StaticPtrCast(fb_functionblock, gpinst), gpltc);
//
//	OV_UINT pathDirLen = 13;
//	OV_STRING pathDirValue[] = { "right", "right", "right", "right",
//			"heat", "left", "left", "left", "down", "right", "right", "right",
//			"turn" };
//	TEST_ASSERT_EQUAL(gpinst->v_pathDir.veclen, pathDirLen);
//	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirValue, gpinst->v_pathDir.value,
//		pathDirLen);
//
//	OV_UINT pathNodeLen = 13;
//	OV_STRING pathNodeValue[] = { "PE034", "PE005", "PE008", "PE009",
//			"PE009", "PE008", "PE005", "PE034", "PE034", "PE031", "PE030", "PE025",
//			"PE025" };
//	TEST_ASSERT_EQUAL(gpinst->v_pathNode.veclen, pathNodeLen);
//	TEST_ASSERT_EQUAL_STRING_ARRAY(pathNodeValue, gpinst->v_pathNode.value,
//		pathNodeLen);
//
//	OV_UINT paramLen = 13;
//	OV_STRING paramValue[] = {NULL, NULL, NULL, NULL, "150", NULL, NULL, NULL,
//			NULL, NULL, NULL, NULL, "180" };
//	TEST_ASSERT_EQUAL(gpinst->v_parameter.veclen, paramLen);
//	TEST_ASSERT_EQUAL_STRING_ARRAY(paramValue, gpinst->v_parameter.value, paramLen);
//}
//
TEST_GROUP_RUNNER(assozierer) {
	RUN_TEST_CASE(assozierer, assozierer_default);
	RUN_TEST_CASE(assozierer, assozierer_2neighbour);
	RUN_TEST_CASE(assozierer, assozierer_schieber);
//  RUN_TEST_CASE(assozierer, assozierer_badstart);
//  RUN_TEST_CASE(assozierer, assozierer_badtopo);
//  RUN_TEST_CASE(assozierer, assozierer_start_same_end);
//  RUN_TEST_CASE(assozierer, assozierer_heat_turn);
//  RUN_TEST_CASE(assozierer, assozierer_heat2_turn2);
//  RUN_TEST_CASE(assozierer, AnotherIgnoredTest);
//  RUN_TEST_CASE(assozierer, ThisFunctionHasNotBeenTested_NeedsToBeImplemented);
}

static void RunAllTests(void) {
	RUN_TEST_GROUP(assozierer);
}

OV_DLLFNCEXPORT void gtpfTest_assoziererTest_typemethod(

OV_INSTPTR_fb_functionblock pfb,

OV_TIME *pltc) {
	/*
	 *   local variables
	 */
	gpinst = Ov_StaticPtrCast(ovunity_main, pfb);
	gpobj = &Ov_StaticPtrCast(gtpfTest_assoziererTest,gpinst)->p_obj;
	gpltc = pltc;

	const char* argv[] = { "assozierer", "-v" };
	CEXCEPTION_T e;
	Try
			{
				UnityMain(2, argv, RunAllTests);
			}
				Catch(e)
	{
		ov_logfile_error("assoziererTest fail");
	}
	return;
}

