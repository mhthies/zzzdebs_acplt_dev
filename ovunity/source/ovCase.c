/******************************************************************************
 *
 *   FILE
 *   ----
 *   ovCase.c
 *
 *   History
 *   -------
 *   2018-10-19   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_ovunity
#define OV_COMPILE_LIBRARY_ovunity
#endif

#include "libov/ov_macros.h"

#include "CTree.h"
#include "cJSON.h"
#include "json_tools.h"
#include "ovunity.h"
#include "ovunity_helper.h"
//#include "CTree.ovt"

#define pathToSoll "/soll.json"
#define pathToResultIst "/result/ist.json"
#define pathToResultSoll "/result/soll.json"

/* compares ist with soll and writes result in result dir*/
OV_DLLFNCEXPORT OV_RESULT
                ovunity_compareIstSoll(const OV_INSTPTR_ovunity_ovCase pinst) {
  OV_RESULT    result = OV_ERR_OK;
  CEXCEPTION_T e;
  OV_STRING    sollPath = NULL;
  OV_STRING    sollJson = NULL;
  OV_STRING    sollIst = NULL;
  OV_STRING    istResultpath = NULL;
  OV_STRING    sollResultpath = NULL;
  ov_string_print(&sollPath, "%s%s", pinst->v_sysPath, pathToSoll);
  ov_string_print(&istResultpath, "%s%s", pinst->v_sysPath, pathToResultIst);
  ov_string_print(&sollResultpath, "%s%s", pinst->v_sysPath, pathToResultSoll);

  cJSON* soll = NULL;
  Try {
    sollJson = ovunity_helper_data2str(sollPath);
    soll = cJSON_Parse(sollJson);
    Ov_HeapFree(sollJson);

    if(!soll) return OV_ERR_BADVALUE;
  }
  Catch(e) { return e; }

  // getting ist stand as json
  OV_INSTPTR_CTree_Upload pobj = NULL;
  result = Ov_CreateIDedObject(CTree_Upload, pobj, pinst, "Upload");
  if(result) {
    ov_logfile_error("Error at attempt to create Upload");
    return result;
  }
  ov_string_setvalue(&pobj->v_path, pinst->v_path);
  result = CTree_Upload_execute(pobj);
  if(Ov_Fail(result)) {
    ov_logfile_error("Error at running Upload");
    return result;
  }
  cJSON* ist = ((CTreeUploadCache)pobj->v_cache).jsbase;
  // (CTreeUploadCache) pobj->v_cache;
  if(cJSON_isSame(soll, ist)) {
    result = 0;
  } else {
    cJSON_IsRaw(soll);
  }
  Ov_DeleteObject(pobj);

  OV_STRING tmp = cJSON_Print(soll);
  if(tmp) {
    result |= ovunity_helper_str2data(tmp, sollResultpath);
  } else {
    ov_logfile_error("soll file printed null");
    result |= OV_ERR_GENERIC;
  }
  free(tmp);
  tmp = cJSON_Print(ist);
  if(tmp) {
    result |= ovunity_helper_str2data(tmp, istResultpath);
  } else {
    ov_logfile_error("ist file printed null");
    result |= OV_ERR_GENERIC;
  }
  free(tmp);
  // freeing
  cJSON_Delete(ist);
  cJSON_Delete(soll);
  return result;
}

/* getting obj path */
OV_DLLFNCEXPORT OV_INSTPTR_ov_object
                ovunity_ovCase_getObjPath(OV_INSTPTR_ovunity_ovCase pcase) {
  OV_ELEMENT           parent = {0};
  OV_ELEMENT           child = {0};
  parent.elemtype = OV_ET_OBJECT;
  parent.pobj = pcase;
  child.elemtype = OV_ET_OBJECT;
  ov_element_searchchild(&parent, &child, "obj");
  return child.pobj;
}

OV_DLLFNCEXPORT void ovunity_ovCase_typemethod(OV_INSTPTR_fb_functionblock pfb,
                                               OV_TIME* pltc) {
  /*
   *   local variables
   */
  OV_INSTPTR_ovunity_ovCase pinst = Ov_StaticPtrCast(ovunity_ovCase, pfb);
  return;
}

