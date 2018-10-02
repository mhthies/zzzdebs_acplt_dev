/******************************************************************************
 *
 *   FILE
 *   ----
 *   dijkstraTest.c
 *
 *   History
 *   -------
 *   2018-07-18   File created
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

#include "CException.h"

#include "unity.h"
#include "unity_fixture.h"

#include "ovunity.h"
#include "ovunity_helper.h"

OV_INSTPTR_ovunity_main gpinst;
OV_INSTPTR_gtpf_dijkstra gpobj;
OV_TIME *gpltc;

TEST_GROUP(dijkstra);

TEST_SETUP(dijkstra) {
	ov_memstack_lock();
	//This is run before EACH TEST
	gpobj->v_EN = 0;
	ov_string_setvalue(&gpobj->v_start, "PE004");
	ov_string_setvalue(&gpobj->v_topologie, NULL);

	Ov_SetDynamicVectorLength(&gpobj->v_recipe, 0, STRING);
	gpobj->v_recipe.value = NULL;

	gpobj->v_EN = 0;
}

TEST_TEAR_DOWN(dijkstra) {
	ov_memstack_unlock();
}

TEST(dijkstra, dijkstra_default) {
	//loading env
	OV_STRING case_name = "case_schieber";
	OV_STRING case_path = ovunity_getCasePath(gpinst, case_name);
	OV_STRING env_path = NULL;
	ov_string_print(&env_path, "%s/%s", case_path, "env");
	

	OV_INSTPTR_ov_domain pcase;
	Ov_CreateObject(ov_domain, pcase, gpinst, case_name);

	OV_INSTPTR_gtpf_dijkstra pobj = NULL;
	Ov_CreateObject(gtpf_dijkstra, pobj, pcase, "obj");

	ovunity_loadEnv(gpinst, "schieber.json", env_path);
	
	//setting param
	ov_string_setvalue(&pobj->v_start, "SCHIEBER1");
	ov_string_setvalue(&pobj->v_topologie, env_path);
	OV_STRING recipe[1] = { "WAGON1" };
	Ov_SetDynamicVectorValue(&pobj->v_recipe, recipe, 1, STRING);

	//executing
	gtpf_dijkstra_typemethod(Ov_StaticPtrCast(fb_functionblock, pobj), gpltc);

	//checking
	TEST_ASSERT_EQUAL(pobj->v_result, 0);

	const OV_UINT pathDirLen = 2;

	OV_STRING pathDirValue[] = { "[-0.000003, -142.000031, 0.000000]",
			"[-141.421356, 0.578671, 0.000000]" };
	TEST_ASSERT_EQUAL(pobj->v_pathDir.veclen, pathDirLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirValue, pobj->v_pathDir.value,
		pathDirLen);

	OV_STRING pathDirStrValue[2] = { "DOWN", "LEFT" };
	TEST_ASSERT_EQUAL(pobj->v_pathDirStr.veclen, pathDirLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirStrValue, pobj->v_pathDirStr.value,
		pathDirLen);

	OV_UINT pathNodeLen = 2;
	OV_STRING pathNodeValue[] = { "SCHIEBER1_0", "SCHIEBER1_1" };
	TEST_ASSERT_EQUAL(pobj->v_pathNode.veclen, pathNodeLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathNodeValue, pobj->v_pathNode.value,
		pathNodeLen);

	OV_UINT paramLen = 2;
	OV_STRING paramValue[] = { NULL, NULL };
	TEST_ASSERT_EQUAL(pobj->v_parameter.veclen, paramLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(paramValue, pobj->v_parameter.value,
		paramLen);
}

TEST(dijkstra, dijkstra_badstart) {
	OV_STRING case_path = ovunity_getCasePath(
		Ov_StaticPtrCast(ovunity_main, gpinst), "case_badstart");
	OV_STRING env_path = NULL;
	ov_string_print(&env_path, "%s/%s", case_path, "env");

	OV_INSTPTR_ov_domain pcase;
	Ov_CreateObject(ov_domain, pcase, gpinst, "case_badstart");

	OV_INSTPTR_gtpf_dijkstra pobj = NULL;
	Ov_CreateObject(gtpf_dijkstra, pobj, pcase, "obj");

	ovunity_loadEnv(Ov_StaticPtrCast(ovunity_main, gpinst), "schieber.json",
		env_path);

	ov_string_setvalue(&pobj->v_start, "PE0033");
	gtpf_dijkstra_typemethod(Ov_StaticPtrCast(fb_functionblock, pobj), gpltc);
	TEST_ASSERT_EQUAL(pobj->v_result, 4);
}

//test2 bad top
TEST(dijkstra, dijkstra_badtopo) {
//	OV_STRING case_path = ovunity_getCasePath(Ov_StaticPtrCast(ovunity_main, gpinst),
//			"case_badtopo");
//		ovunity_loadEnv(Ov_StaticPtrCast(ovunity_main, gpinst), "default.json",
//			case_path);
//	ov_logfile_info("test: bad topo");
	OV_INSTPTR_ov_domain pcase;
	Ov_CreateObject(ov_domain, pcase, gpinst, "case_badtopo");

	OV_INSTPTR_gtpf_dijkstra pobj = NULL;
	Ov_CreateObject(gtpf_dijkstra, pobj, pcase, "obj");

	ov_string_setvalue(&pobj->v_topologie, "PE03");
	gtpf_dijkstra_typemethod(Ov_StaticPtrCast(fb_functionblock, pobj), gpltc);
	TEST_ASSERT_EQUAL(pobj->v_result, 4);
}

//test3 start==end
TEST(dijkstra, dijkstra_start_same_end) {
	OV_INSTPTR_gtpf_dijkstra pobj = NULL;
	/* env */
	OV_STRING case_path = ovunity_getCasePath(
		Ov_StaticPtrCast(ovunity_main, gpinst), "case_start_same_end");
	OV_STRING env_path = NULL;
	ov_string_print(&env_path, "%s/%s", case_path, "env");

	OV_INSTPTR_ov_domain pcase;
	Ov_CreateObject(ov_domain, pcase, gpinst, "case_start_same_end");

	Ov_CreateObject(gtpf_dijkstra, pobj, pcase, "obj");

	ovunity_loadEnv(Ov_StaticPtrCast(ovunity_main, gpinst), "schieber.json",
		env_path);
//	ov_logfile_info("test: start==end");
	/* setting param */
	ov_string_setvalue(&pobj->v_start, "SCHIEBER1");
	ov_string_setvalue(&pobj->v_topologie, env_path);
	OV_STRING recipe[1] = { "SCHIEBER1" };
	Ov_SetDynamicVectorValue(&pobj->v_recipe, recipe, 1, STRING);

	/* execute */
	gtpf_dijkstra_typemethod(Ov_StaticPtrCast(fb_functionblock, pobj), gpltc);

	/* testing */
	TEST_ASSERT_EQUAL(pobj->v_result, 0);
	OV_UINT pathDirLen = 0;
	TEST_ASSERT_EQUAL(pobj->v_pathDir.veclen, pathDirLen);

	OV_UINT pathNodeLen = 0;
	TEST_ASSERT_EQUAL(pobj->v_pathNode.veclen, pathNodeLen);

	OV_UINT paramLen = 0;
	TEST_ASSERT_EQUAL(pobj->v_parameter.veclen, paramLen);
}

TEST(dijkstra, dijkstra_heat) {
	OV_INSTPTR_gtpf_dijkstra pobj = NULL;
	/* env */
	OV_STRING case_path = ovunity_getCasePath(
		Ov_StaticPtrCast(ovunity_main, gpinst), "case_heat");
	OV_STRING env_path = NULL;
	ov_string_print(&env_path, "%s/env", case_path);

	OV_INSTPTR_ov_domain pcase;
	Ov_CreateObject(ov_domain, pcase, gpinst, "case_heat");

	Ov_CreateObject(gtpf_dijkstra, pobj, pcase, "obj");

	ovunity_loadEnv(Ov_StaticPtrCast(ovunity_main, gpinst), "3schichten.json",
		env_path);

	//test4
	ov_string_setvalue(&pobj->v_topologie, env_path);
	OV_STRING recipe[2] = { "PE019&HEAT", "SCHIEBER2" };
	Ov_SetDynamicVectorValue(&pobj->v_recipe, recipe, 2, STRING);

	gtpf_dijkstra_typemethod(Ov_StaticPtrCast(fb_functionblock, pobj), gpltc);
	TEST_ASSERT_EQUAL(pobj->v_result, 0);

	OV_UINT pathDirLen = 10;
	OV_STRING pathDirValue[] = { "[100.000000, 0.000000, 0.000000]",
			"[0.000000, -400.000000, 0.000000]", "[100.000000, 0.000000, 0.000000]",
			"[100.000000, 0.000000, 0.000000]", "[100.000000, 0.000000, 0.000000]",
			"HEAT", "[100.000000, 0.000000, 0.000000]",
			"[100.000000, 0.000000, 0.000000]", "[100.000000, 0.000000, 0.000000]",
			"[0.000000, 400.000000, 0.000000]" };
	TEST_ASSERT_EQUAL(pobj->v_pathDir.veclen, pathDirLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirValue, pobj->v_pathDir.value,
		pathDirLen);

	OV_STRING pathDirStrValue[10] = { "RIGHT", "DOWN", "RIGHT", "RIGHT", "RIGHT",
			"HEAT", "RIGHT", "RIGHT", "RIGHT", "UP" };
	TEST_ASSERT_EQUAL(pobj->v_pathDirStr.veclen, pathDirLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirStrValue, pobj->v_pathDirStr.value,
		pathDirLen);

	OV_UINT pathNodeLen = 10;
	OV_STRING pathNodeValue[] = { "PE004_0", "SCHIEBER1_0", "SCHIEBER1_1",
			"PE013_0", "PE016_0", "PE019", "PE019_0", "PE021_0", "PE026_0",
			"SCHIEBER2_1" };
	TEST_ASSERT_EQUAL(pobj->v_pathNode.veclen, pathNodeLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathNodeValue, pobj->v_pathNode.value,
		pathNodeLen);

	OV_UINT paramLen = 10;
	OV_STRING paramValue[] = { NULL, NULL, NULL, NULL, NULL, "180", NULL, NULL,
	NULL, NULL };

	TEST_ASSERT_EQUAL(pobj->v_parameter.veclen, paramLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(paramValue, pobj->v_parameter.value,
		paramLen);
}

TEST(dijkstra, dijkstra_heat_false) {
	OV_INSTPTR_gtpf_dijkstra pobj = NULL;
	/* env */
	OV_STRING case_path = ovunity_getCasePath(
		Ov_StaticPtrCast(ovunity_main, gpinst), "case_heat_false");
	OV_STRING env_path = NULL;
	ov_string_print(&env_path, "%s/%s", case_path, "env");
	
	OV_INSTPTR_ov_domain pcase;
	Ov_CreateObject(ov_domain, pcase, gpinst, "case_heat_false");

	Ov_CreateObject(gtpf_dijkstra, pobj, pcase, "obj");

	ovunity_loadEnv(Ov_StaticPtrCast(ovunity_main, gpinst), "3schichten.json",
		env_path);

	//test4
	ov_string_setvalue(&pobj->v_topologie, env_path);
	OV_STRING recipe[2] = { "PE021&HEAT", "SCHIEBER2" };
	Ov_SetDynamicVectorValue(&pobj->v_recipe, recipe, 2, STRING);

	gtpf_dijkstra_typemethod(Ov_StaticPtrCast(fb_functionblock, pobj), gpltc);
	TEST_ASSERT_EQUAL(pobj->v_result, 4);

	OV_UINT pathDirLen = 0;
	TEST_ASSERT_EQUAL(pobj->v_pathDir.veclen, pathDirLen);

	OV_UINT pathNodeLen = 0;
	TEST_ASSERT_EQUAL(pobj->v_pathNode.veclen, pathNodeLen);

	OV_UINT paramLen = 0;
	TEST_ASSERT_EQUAL(pobj->v_parameter.veclen, paramLen);
}

TEST(dijkstra, dijkstra_turn) {
	OV_INSTPTR_gtpf_dijkstra pobj = NULL;
	/* env */
	OV_STRING case_name = "case_turn";
	OV_STRING case_path = ovunity_getCasePath(
		Ov_StaticPtrCast(ovunity_main, gpinst), case_name);
	OV_STRING env_path = NULL;
	ov_string_print(&env_path, "%s/%s", case_path, "env");

	OV_INSTPTR_ov_domain pcase;
	Ov_CreateObject(ov_domain, pcase, gpinst, case_name);

	Ov_CreateObject(gtpf_dijkstra, pobj, pcase, "obj");

	ovunity_loadEnv(Ov_StaticPtrCast(ovunity_main, gpinst), "turn.json",
		env_path);

	//test4
	ov_string_setvalue(&pobj->v_start, "PE005");
	ov_string_setvalue(&pobj->v_topologie, env_path);
	OV_UINT recipeLen = 1;
	OV_STRING recipe[1] = { "PE027" };
	Ov_SetDynamicVectorValue(&pobj->v_recipe, recipe, recipeLen, STRING);

	gtpf_dijkstra_typemethod(Ov_StaticPtrCast(fb_functionblock, pobj), gpltc);
	TEST_ASSERT_EQUAL(pobj->v_result, 0);

	OV_UINT pathDirLen = 7;
	OV_STRING pathDirValue[7] = { "[100.000000, 0.000000, 0.000000]",
			"[100.000000, 0.000000, 0.000000]", "[0.000000, 0.000000, 88.000000]",
			"[-99.939079, -3.489949, 0.000000]", "[0.000000, 0.000000, 89.000000]",
			"[100.000000, 0.000000, 0.000000]", "[100.000000, 0.000000, 0.000000]" };
	TEST_ASSERT_EQUAL(pobj->v_pathDir.veclen, pathDirLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirValue, pobj->v_pathDir.value,
		pathDirLen);

	OV_STRING pathDirStrValue[7] = { "RIGHT", "RIGHT", "TURN", "DOWN", "TURN",
			"RIGHT", "RIGHT" };
	TEST_ASSERT_EQUAL(pobj->v_pathDirStr.veclen, pathDirLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirStrValue, pobj->v_pathDirStr.value,
		pathDirLen);

	OV_UINT pathNodeLen = 7;
	OV_STRING pathNodeValue[7] = { "PE005_0", "PE008_0", "PE009_0", "PE009_3",
			"PE025_4", "PE025_0", "PE028_0" };
	TEST_ASSERT_EQUAL(pobj->v_pathNode.veclen, pathNodeLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathNodeValue, pobj->v_pathNode.value,
		pathNodeLen);

	OV_UINT paramLen = 7;
	OV_STRING paramValue[7] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	TEST_ASSERT_EQUAL(pobj->v_parameter.veclen, paramLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(paramValue, pobj->v_parameter.value,
		paramLen);
}

TEST(dijkstra, dijkstra_heat2_turn2) {
	OV_INSTPTR_gtpf_dijkstra pobj = NULL;
	/* env */
	OV_STRING case_path = ovunity_getCasePath(
		Ov_StaticPtrCast(ovunity_main, gpinst), "case_heat2_turn2");
	OV_STRING env_path = NULL;
	ov_string_print(&env_path, "%s/%s", case_path, "env");
//!!!! name
	ovunity_loadEnv(Ov_StaticPtrCast(ovunity_main, gpinst), "default.json",
		env_path);
	//test5
	ov_logfile_info("test: (heat&heat) & (turn&turn)");
	ov_string_setvalue(&pobj->v_topologie, env_path);
	OV_STRING recipe[4] =
			{ "PE009 Heat", "PE009 Heat", "PE025 Turn", "PE025 Turn" };
	Ov_SetDynamicVectorValue(&pobj->v_recipe, recipe, 4, STRING);

	gtpf_dijkstra_typemethod(Ov_StaticPtrCast(fb_functionblock, pobj), gpltc);

	OV_UINT pathDirLen = 13;
	OV_STRING pathDirValue[] = { "right", "right", "right", "right", "heat",
			"left", "left", "left", "down", "right", "right", "right", "turn" };
	TEST_ASSERT_EQUAL(pobj->v_pathDir.veclen, pathDirLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirValue, pobj->v_pathDir.value,
		pathDirLen);

	OV_UINT pathNodeLen = 13;
	OV_STRING pathNodeValue[] = { "PE034", "PE005", "PE008", "PE009", "PE009",
			"PE008", "PE005", "PE034", "PE034", "PE031", "PE030", "PE025", "PE025" };
	TEST_ASSERT_EQUAL(pobj->v_pathNode.veclen, pathNodeLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathNodeValue, pobj->v_pathNode.value,
		pathNodeLen);

	OV_UINT paramLen = 13;
	OV_STRING paramValue[] = { NULL, NULL, NULL, NULL, "150", NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, "180" };
	TEST_ASSERT_EQUAL(pobj->v_parameter.veclen, paramLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(paramValue, pobj->v_parameter.value,
		paramLen);
}

TEST(dijkstra, dijkstra_action) {
	//loading env
	OV_STRING case_name = "case_action";
	OV_STRING case_path = ovunity_getCasePath(gpinst, case_name);
	OV_STRING env_path = NULL;
	ov_string_print(&env_path, "%s/%s", case_path, "env");

	OV_INSTPTR_ov_domain pcase;
	Ov_CreateObject(ov_domain, pcase, gpinst, case_name);

	OV_INSTPTR_gtpf_dijkstra pobj = NULL;
	Ov_CreateObject(gtpf_dijkstra, pobj, pcase, "obj");

	ovunity_loadEnv(gpinst, "3schichten.json", env_path);

	//setting param
	ov_string_setvalue(&pobj->v_start, "PE013");
	ov_string_setvalue(&pobj->v_topologie, env_path);
	OV_STRING recipe[2] = { "&HEAT", "PE013" };
	Ov_SetDynamicVectorValue(&pobj->v_recipe, recipe, 2, STRING);

	//executing
	gtpf_dijkstra_typemethod(Ov_StaticPtrCast(fb_functionblock, pobj), gpltc);

	//checking
	TEST_ASSERT_EQUAL(pobj->v_result, 0);

	const OV_UINT pathDirLen = 5;

	OV_STRING pathDirValue[] = { "[100.000000, 0.000000, 0.000000]",
		"[100.000000, 0.000000, 0.000000]","HEAT","[-100.000000, 0.000000, 0.000000]","[-100.000000, 0.000000, 0.000000]"
	};
	TEST_ASSERT_EQUAL(pobj->v_pathDir.veclen, pathDirLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirValue, pobj->v_pathDir.value,
		pathDirLen);

	OV_STRING pathDirStrValue[5] = { "RIGHT", "RIGHT", "HEAT", "LEFT",
			"LEFT" };
	TEST_ASSERT_EQUAL(pobj->v_pathDirStr.veclen, pathDirLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathDirStrValue, pobj->v_pathDirStr.value,
		pathDirLen);

	OV_UINT pathNodeLen = 5;
	OV_STRING pathNodeValue[] =
			{ "PE013_0", "PE016_0", "PE019", "PE019_0",
			"PE016_0" };
	TEST_ASSERT_EQUAL(pobj->v_pathNode.veclen, pathNodeLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(pathNodeValue, pobj->v_pathNode.value,
		pathNodeLen);

	OV_UINT paramLen = 5;
	OV_STRING paramValue[] = { NULL, NULL, "180", NULL, NULL };
	TEST_ASSERT_EQUAL(pobj->v_parameter.veclen, paramLen);
	TEST_ASSERT_EQUAL_STRING_ARRAY(paramValue, pobj->v_parameter.value, paramLen);
}


TEST_GROUP_RUNNER(dijkstra) {
	RUN_TEST_CASE(dijkstra, dijkstra_default);
	RUN_TEST_CASE(dijkstra, dijkstra_badstart);
	RUN_TEST_CASE(dijkstra, dijkstra_badtopo);
	RUN_TEST_CASE(dijkstra, dijkstra_start_same_end);
	RUN_TEST_CASE(dijkstra, dijkstra_heat);
	RUN_TEST_CASE(dijkstra, dijkstra_heat_false);
	RUN_TEST_CASE(dijkstra, dijkstra_turn);
	RUN_TEST_CASE(dijkstra, dijkstra_action);
//	RUN_TEST_CASE(dijkstra, dijkstra_heat2_turn2);
//  RUN_TEST_CASE(dijkstra, AnotherIgnoredTest);
//  RUN_TEST_CASE(dijkstra, ThisFunctionHasNotBeenTested_NeedsToBeImplemented);
}

static void RunAllTests(void) {
	RUN_TEST_GROUP(dijkstra);
}

OV_DLLFNCEXPORT void gtpfTest_dijkstraTest_typemethod(
		OV_INSTPTR_fb_functionblock pfb, OV_TIME *pltc) {
	/*
	 *   local variables
	 */

	gpinst = Ov_StaticPtrCast(ovunity_main, pfb);
	gpobj = &Ov_StaticPtrCast(gtpfTest_dijkstraTest, gpinst)->p_obj;
	gpltc = pltc;

	const char* argv[] = { "dijkstra", "-v" };
	CEXCEPTION_T e;
	Try
			{
				UnityMain(2, argv, RunAllTests);
			}
				Catch(e)
	{
		ov_logfile_error("dijkstraTest cexception: %s", ov_result_getresulttext(e));
	}

	return;
}

