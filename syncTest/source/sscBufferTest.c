
/******************************************************************************
*
*   FILE
*   ----
*   sscBufferTest.c
*
*   History
*   -------
*   2018-11-18   File created
*
*******************************************************************************
*
*   This file is generated by the 'acplt_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_syncTest
#define OV_COMPILE_LIBRARY_syncTest
#endif


#include "syncTest.h"
#include "libov/ov_macros.h"
#include "sync.h"

#include "unity.h"
#include "unity_fixture.h"

#include "ovunity.h"
#include "ovunity_helper.h"

#include "CException.h"

OV_INSTPTR_ovunity_main gpinst;

OV_TIME *gpltc;

TEST_GROUP(sscBuffer);

TEST_SETUP( sscBuffer) {
	ov_memstack_lock();
}

TEST_TEAR_DOWN( sscBuffer) {
	ov_memstack_unlock();
}

TEST( sscBuffer, sscBuffer_default) {
	OV_RESULT result = OV_ERR_OK;
	OV_INSTPTR_sync_sscBuffer pobj = NULL;
	OV_STRING case_name = "case_self2";
	OV_STRING case_path = ovunity_getCasePath(gpinst, case_name);

	/* create case */
	OV_INSTPTR_ovunity_ovCase pcase = ovunity_createCase(gpinst, case_name);
	if(!pcase) Throw(OV_ERR_GENERIC);

	ovunity_ovCase_build1(pcase);
	pobj = Ov_StaticPtrCast(sync_sscBuffer,
		ovunity_ovCase_getObjPath(pcase));

	result = ovunity_compareIstSoll(pcase);

	TEST_ASSERT_EQUAL(result, 0);
}

TEST_GROUP_RUNNER( sscBuffer) {
	RUN_TEST_CASE(sscBuffer, sscBuffer_default);
}

static void RunAllTests(void) {
	RUN_TEST_GROUP(sscBuffer);
}

OV_DLLFNCEXPORT void syncTest_sscBufferTest_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
    /*    
    *   local variables
    */
	OV_INSTPTR_syncTest_sscBufferTest pinst = Ov_StaticPtrCast(
		syncTest_sscBufferTest, pfb);
	gpinst = Ov_StaticPtrCast(ovunity_main, pfb);
	//	gpobj = &Ov_StaticPtrCast(syncTest_sscBufferTest,gpinst)->p_obj;
	gpltc = pltc;

	//todo
	char* ahome = getenv("ACPLT_HOME");
	ov_string_print(&pinst->v_sysPath, "%s/dev/syncTest/test/sscBufferTest",
		ahome);

	const char* argv[] = { "sscBuffer", "-v" };
	CEXCEPTION_T e;
	Try
			{
				UnityMain(2, argv, RunAllTests);
			}
				Catch(e)
	{
		ov_logfile_error("sscBufferTest fail");
	}
    return;
}
