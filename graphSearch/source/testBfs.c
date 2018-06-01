/******************************************************************************
 *
 *   FILE
 *   ----
 *   testBfs.c
 *
 *   History
 *   -------
 *   2018-06-01   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#define OV_DEBUG

#ifndef OV_COMPILE_LIBRARY_graphSearch
#define OV_COMPILE_LIBRARY_graphSearch
#endif

#include <stdarg.h>

#include "graphSearch.h"
#include "libov/ov_macros.h"

typedef struct Input {
	OV_STRING start;
	OV_STRING_VEC topology;
	OV_STRING_VEC recipes;
	OV_UINT en; //"0: done & ready 1: run 3: clean";
} Input_t;

void initInput(Input_t* input) {
	ov_string_setvalue(&input->start, NULL);

	input->topology.veclen = 0;
	ov_string_setvalue(input->topology.value, NULL);

	input->recipes.veclen = 0;
	ov_string_setvalue(input->recipes.value, NULL);

	input->en = 0;
}

Input_t* getDefaultInput() {
	Input_t* def = Ov_MemStackAlloc(Input_t);
}

typedef struct Output {
	OV_STRING_VEC pathNode;	//"path from start through recipe in Nodes";
	OV_STRING_VEC pathdir;	//"path from start through recipe in Direction";
	OV_STRING_VEC parameter;		//"parameters";
	OV_INT result; //"0: succes, 1: error, 4: badparam";
} Output_t;

#define TEST_CHECK(condition) \
	Ov_WarnIfNot(condition)

//OV_RESULT Download_log(OV_INSTPTR_CTree_Download pinst, OV_MSG_TYPE msg_type,
//		OV_RESULT result, const OV_STRING format, ...) {
//	char msg[1024];
//
//	va_list args;
//
//	pinst->v_result = result;
//	va_start(args, format);
//
//#if OV_SYSTEM_UNIX && !OV_SYSTEM_SOLARIS
//	vsnprintf(msg, sizeof(msg), format, args);
//#else
//	vsprintf(msg, format, args);
//#endif
//	va_end(args);
//
//	ov_logfile_print(msg_type, msg);
//
//	if (msg_type == OV_MT_ERROR) {
//		ov_string_print(&pinst->v_ErrorMsg, "%s ; %s ;",
//				ov_result_getresulttext(result), msg);
//	}
//	return result;
//}

OV_RESULT ov_string_set_vecvalue_one(OV_STRING_VEC* vec, OV_UINT len, ...) {
	va_list args;
	va_start(args, len);
	OV_RESULT result = Ov_SetDynamicVectorLength(vec, len, STRING);
	for (OV_UINT i = 0; i < len; ++i) {
		ov_string_setvalue(&vec->value[i], va_arg(args, OV_STRING));
	}
	va_end(args);
	return 0;
}

OV_BOOL test_check_vector(OV_STRING_VEC vec, OV_UINT len, ...) {
	va_list args;
	if(vec.veclen != len) return 0;
	va_start(args, len);
	for (OV_UINT i = 0; i < len; ++i) {
		if(ov_string_compare(vec.value[i],
			va_arg(args, OV_STRING))!=OV_STRCMP_EQUAL) return 0;
	}
	va_end(args);
	return 1;
}

void bfsSetDefault(OV_INSTPTR_graphSearch_bfs pinst) {
	ov_string_setvalue(&pinst->v_start, "PE004");
	ov_string_setvalue(&pinst->v_topologie, "/TechUnits/Topology");
	ov_string_set_vecvalue_one(&pinst->v_recipe, 1, "PE005");
	pinst->v_EN = 0;
}

void bfsExpectDefault(OV_INSTPTR_graphSearch_bfs pinst) {
	TEST_CHECK(pinst->v_result == 0);
	TEST_CHECK(test_check_vector(pinst->v_pathDir, 2, "right", "right"));
	TEST_CHECK(test_check_vector(pinst->v_pathNode, 2, "PE034", "PE005"));
	TEST_CHECK(test_check_vector(pinst->v_parameter, 2, "", ""));
}

OV_DLLFNCEXPORT void graphSearch_testBfs_typemethod(
		OV_INSTPTR_fb_functionblock pfb, OV_TIME *pltc) {
	/*
	 *   local variables
	 */
//    OV_INSTPTR_graphSearch_testBfs pinst = Ov_StaticPtrCast(graphSearch_testBfs, pfb);
	OV_INSTPTR_graphSearch_bfs pinst = Ov_StaticPtrCast(graphSearch_bfs, pfb);
	ov_memstack_lock();
	//test0 default
	ov_logfile_info("test: default");
	bfsSetDefault(pinst);
	graphSearch_bfs_typemethod(Ov_StaticPtrCast(fb_functionblock, pinst), pltc);
	bfsExpectDefault(pinst);

	//test1 bad start
	ov_logfile_info("test: bad start");
	bfsSetDefault(pinst);
	ov_string_setvalue(&pinst->v_start, "PE0033");
	graphSearch_bfs_typemethod(Ov_StaticPtrCast(fb_functionblock, pinst), pltc);
	TEST_CHECK(pinst->v_result == 4);

	//test2 bad top
	ov_logfile_info("test: bad topo");
	bfsSetDefault(pinst);
	ov_string_setvalue(&pinst->v_topologie, "PE03");
	graphSearch_bfs_typemethod(Ov_StaticPtrCast(fb_functionblock, pinst), pltc);
	TEST_CHECK(pinst->v_result == 4);

	//test3 start==end
	ov_logfile_info("test: start==end");
	bfsSetDefault(pinst);
	ov_string_set_vecvalue_one(&pinst->v_recipe, 1, "PE004");
	graphSearch_bfs_typemethod(Ov_StaticPtrCast(fb_functionblock, pinst), pltc);
	TEST_CHECK(pinst->v_result == 0);
	TEST_CHECK(test_check_vector(pinst->v_pathDir, 0));
	TEST_CHECK(test_check_vector(pinst->v_pathNode, 0));
	TEST_CHECK(test_check_vector(pinst->v_parameter, 0));

	//test4
	ov_logfile_info("test: heat & turn");
	bfsSetDefault(pinst);
	ov_string_set_vecvalue_one(&pinst->v_recipe, 3, "PE009 Heat", "PE025 Turn",
		"PE033");
	graphSearch_bfs_typemethod(Ov_StaticPtrCast(fb_functionblock, pinst), pltc);
	TEST_CHECK(pinst->v_result == 0);
	TEST_CHECK(
		test_check_vector(pinst->v_pathNode, 16, "PE034", "PE005", "PE008", "PE009",
			"PE009", "PE008", "PE005", "PE034", "PE034", "PE031", "PE030", "PE025",
			"PE025", "PE028", "PE027", "PE033"));
	TEST_CHECK(
		test_check_vector(pinst->v_pathDir, 16, "right", "right", "right", "right",
			"heat", "left", "left", "left", "down", "right", "right", "right", "turn",
			"right", "right", "right"));
	TEST_CHECK(
		test_check_vector(pinst->v_parameter, 16, "", "", "", "", "150", "", "", "",
			"", "", "", "", "180", "", "", ""));

	//test5
	ov_logfile_info("test: (heat&heat) & (turn&turn)");
	bfsSetDefault(pinst);
	ov_string_set_vecvalue_one(&pinst->v_recipe, 3, "PE009 Heat", "PE009 Heat",
		"PE025 Turn", "PE025 Turn");
	graphSearch_bfs_typemethod(Ov_StaticPtrCast(fb_functionblock, pinst), pltc);
	TEST_CHECK(
		test_check_vector(pinst->v_pathNode, 13, "PE034", "PE005", "PE008", "PE009",
			"PE009", "PE008", "PE005", "PE034", "PE034", "PE031", "PE030", "PE025",
			"PE025"));
	TEST_CHECK(
		test_check_vector(pinst->v_pathDir, 13, "right", "right", "right", "right",
			"heat", "left", "left", "left", "down", "right", "right", "right",
			"turn"));
	TEST_CHECK(
		test_check_vector(pinst->v_parameter, 13, "", "", "", "", "150", "", "", "",
			"", "", "", "", "180"));

//    ov_string_setvalue(&pinst->v_start, "PE004");
////    ov_string_setvalue(&pinst->)
//    graphSearch_bfs_typemethod(pinst, pltc);
//    TEST_CHECK(pinst->v_result!=4);
//
//    //test1
//    ov_string_setvalue(&pinst->v_start, "PE0033");
//    graphSearch_bfs_typemethod(pinst, pltc);
//    TEST_CHECK(pinst->v_result!=4);
//    //test2
//    ov_string_setvalue(&pinst->v_start, "PE004");
//    graphSearch_bfs_typemethod(pinst, pltc);
//    TEST_CHECK(pinst->v_result!=4);
//    //test3
//    ov_string_setvalue(&pinst->v_start, "PE004");
//    graphSearch_bfs_typemethod(pinst, pltc);
//    TEST_CHECK(pinst->v_result!=4);

//    TEST_CHECK(pinst->v_result!=4);
//    TEST_CHECK(pinst->v_result!=4);
//    ov_string_setvalue(&pinst->v_topologie, "a");

	ov_memstack_unlock();
	return;
}

