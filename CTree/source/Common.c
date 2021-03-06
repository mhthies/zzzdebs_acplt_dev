
/******************************************************************************
 *
 *   FILE
 *   ----
 *   Common.c
 *
 *   History
 *   -------
 *   2018-04-05   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_CTree
#define OV_COMPILE_LIBRARY_CTree
#endif

#include "CTree.h"
#include "fb_namedef.h"
#include "libov/ov_macros.h"

OV_DLLFNCEXPORT OV_RESULT CTree_Common_trigger_set(OV_INSTPTR_CTree_Common pobj,
                                                   const OV_INT value) {
  OV_VTBLPTR_CTree_Common pVtblthis = NULL;

  if(value && (!pobj->v_trigger)) {
    Ov_GetVTablePtr(CTree_Common, pVtblthis, pobj);
    if(!pVtblthis)
      return OV_ERR_BADOBJTYPE;
    OV_TIME pltc = {0, 0};
    ov_time_gettime(&pltc);
    fb_functionblock_execTypeMethod((void*)pobj, &pltc);
    pobj->v_trigger = 1;
  }
  return pobj->v_result;

  return OV_ERR_OK;
}

