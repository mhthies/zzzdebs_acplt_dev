
/******************************************************************************
*
*   FILE
*   ----
*   neighborDB.c
*
*   History
*   -------
*   2018-12-28   File created
*
*******************************************************************************
*
*   This file is generated by the 'acplt_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_lbalance
#define OV_COMPILE_LIBRARY_lbalance
#endif


#include "lbalance.h"
#include "libov/ov_macros.h"


OV_DLLFNCEXPORT OV_RESULT lbalance_neighborDB_reset_set(
    OV_INSTPTR_lbalance_neighborDB          pobj,
    const OV_BOOL  value
) {
    pobj->v_reset = value;
    return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_RESULT lbalance_neighborDB_constructor(
	OV_INSTPTR_ov_object 	pobj
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_lbalance_neighborDB pinst = Ov_StaticPtrCast(lbalance_neighborDB, pobj);
    OV_RESULT    result;

    /* do what the base class does first */
    result = fb_functionblock_constructor(pobj);
    if(Ov_Fail(result))
         return result;

    /* do what */


    return OV_ERR_OK;
}

OV_DLLFNCEXPORT void lbalance_neighborDB_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_lbalance_neighborDB pinst = Ov_StaticPtrCast(lbalance_neighborDB, pfb);

    return;
}

