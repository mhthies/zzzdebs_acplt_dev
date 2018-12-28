
/******************************************************************************
*
*   FILE
*   ----
*   reqReceiver.c
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


OV_DLLFNCEXPORT OV_RESULT lbalance_reqReceiver_D_set(
    OV_INSTPTR_lbalance_reqReceiver          pobj,
    const OV_BOOL  value
) {
    pobj->v_D = value;
    return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_RESULT lbalance_reqReceiver_constructor(
	OV_INSTPTR_ov_object 	pobj
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_lbalance_reqReceiver pinst = Ov_StaticPtrCast(lbalance_reqReceiver, pobj);
    OV_RESULT    result;

    /* do what the base class does first */
    result = fb_functionblock_constructor(pobj);
    if(Ov_Fail(result))
         return result;

    /* do what */


    return OV_ERR_OK;
}

OV_DLLFNCEXPORT void lbalance_reqReceiver_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_lbalance_reqReceiver pinst = Ov_StaticPtrCast(lbalance_reqReceiver, pfb);

    return;
}

