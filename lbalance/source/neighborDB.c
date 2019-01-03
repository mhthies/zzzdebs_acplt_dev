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

#include "PostSys.h"
#include "PostSys_helper.h"
#include "acplt_simpleMsgHandling.h"
#include "cJSON.h"
#include "lbalance.h"
#include "lbalance_helper.h"
#include "libov/ov_macros.h"
#include "libov/ov_result.h"

OV_DLLFNCEXPORT OV_RESULT lbalance_neighborDB_reset_set(
    OV_INSTPTR_lbalance_neighborDB pinst, const OV_BOOL value) {
  OV_RESULT result = OV_ERR_OK;
  if(value && !pinst->v_reset) {
    result |= Ov_SetDynamicVectorLength(&pinst->v_loads, 0, UINT);
    result |= Ov_SetDynamicVectorLength(&pinst->v_caps, 0, UINT);
    result |= Ov_SetDynamicVectorLength(&pinst->v_IPs, 0, STRING);
    result |= Ov_SetDynamicVectorLength(&pinst->v_sysInfos, 0, STRING);
    pinst->v_syncTrigger = 0;
  }
  pinst->v_reset = value;
  return result;
}

OV_DLLFNCEXPORT void
lbalance_neighborDB_typemethod(OV_INSTPTR_fb_functionblock pfb, OV_TIME* pltc) {
  /*
   *   local variables
   */
  OV_INSTPTR_PostSys_Message pMsg = NULL;
  OV_STRING                  MsgBody = NULL;
  OV_STRING                  command = NULL;
  OV_STRING                  value = NULL;
  OV_STRING                  order = NULL;
  OV_RESULT                  result = OV_ERR_OK;
  OV_STRING                  service = NULL;
  OV_STRING_VEC              ids = {0, NULL};
  OV_STRING_VEC              values = {0, NULL};
  OV_STRING_VEC              types = {0, NULL};
  OV_STRING_VEC              units = {0, NULL};

  OV_INSTPTR_lbalance_neighborDB pinst =
      Ov_StaticPtrCast(lbalance_neighborDB, pfb);

  OV_UINT waitingMsgsLen = 0;
  OV_UINT nLen = 0;
  OV_UINT currentInd = pinst->v_loads.veclen;

  cJSON* jsMsg = NULL;
  cJSON* jsip = NULL;
  cJSON* jsload = NULL;
  cJSON* jscap = NULL;
  cJSON* jsinfo = NULL;

  Ov_ForEachChildEx(ov_containment, &pinst->p_inbox, pMsg, PostSys_Message) {
    waitingMsgsLen++;
  }
  if(!waitingMsgsLen) {
    return;
  }
  nLen = pinst->v_loads.veclen + waitingMsgsLen;

  result |= Ov_SetDynamicVectorLength(&pinst->v_loads, nLen, UINT);
  result |= Ov_SetDynamicVectorLength(&pinst->v_caps, nLen, UINT);
  result |= Ov_SetDynamicVectorLength(&pinst->v_IPs, nLen, STRING);
  result |= Ov_SetDynamicVectorLength(&pinst->v_sysInfos, nLen, STRING);
  if(Ov_Fail(result)) {
    ov_logfile_error("lbalance_neighborDB: %u: %s: failed to allocate vector",
                     result, ov_result_getresulttext(result));
    pinst->v_status = LB_INTERNALERROR;
    pinst->v_result = result;
    return;
  }

  ov_memstack_lock();
  Ov_ForEachChildEx(ov_containment, &pinst->p_inbox, pMsg, PostSys_Message) {
    pinst->v_syncTrigger = 1;
    /* parse and edit output */

    MsgBody = PostSys_Message_msgBody_get(pMsg);
    /*	DEBUG: print MSG-Body	*/
    /*	ov_logfile_debug("Msg-Body:\n\n%s\n\n", MsgBody);	*/
    result = acplt_simpleMsg_parseFlatBody(MsgBody, &service, &command, FALSE,
                                           NULL, &ids, &values, &units, &types);
    if(Ov_Fail(result)) {
      ov_logfile_warning("lbalance_neighborDB: parsing msg failed %s",
                         pinst->v_identifier);
      Ov_DeleteObject(pMsg); /*	Message is corrupted --> delete it
                              */
      continue;
    }
    /*	check message	*/
    if(ids.veclen > 2 || ids.veclen < 1) {
      ov_logfile_info("%s: malformed message --> deleting it",
                      pinst->v_identifier);
      Ov_DeleteObject(pMsg); /*	Message is corrupted --> delete it
                              */
      continue;
    }
    if(ids.veclen == 2) {
      if(ids.value[1] &&
         ov_string_compare(ids.value[1], "Value") != OV_STRCMP_EQUAL) {
        ov_logfile_info("%s: malformed message --> deleting it",
                        pinst->v_identifier);
        Ov_DeleteObject(pMsg); /*	Message is corrupted --> delete it
                                */
        continue;
      }
    }
    /*	get service	and Operation	*/
    if(ov_string_compare(service, "ProcessControl") != OV_STRCMP_EQUAL) {
      ov_logfile_info("%s: unknown service requested --> deleting message",
                      pinst->v_identifier);
      Ov_DeleteObject(pMsg); /*	Message is corrupted --> delete it
                              */
      continue;
    }

    if(ids.veclen == 2 && values.value[1]) { /*	value found	*/
      value = values.value[1];
    } else { /*	now value found, set zo 0	*/
      value = "0";
    }
    order = ov_memstack_alloc(strlen(values.value[0]) + strlen(command) +
                              strlen(value) +
                              3); /*	commander;command;value\0	*/
    if(!order) {
      ov_memstack_unlock();
      ov_logfile_info("%s: could not create order, heap is out of memory",
                      pinst->v_identifier);
      return;
    }

    jsMsg = cJSON_Parse(value);
    if(!jsMsg || !cJSON_IsArray(jsMsg) ||
       cJSON_GetArraySize(jsMsg) != LB_NBINFORMER_JSONLEN) {
      ov_logfile_warning("lbalance_neighborDB: msg: bad json format");
      Ov_DeleteObject(pMsg);
      continue;
    }
    jsip = cJSON_GetArrayItem(jsMsg, LB_NBINFORMER_IPPOS);
    if(!jsip || !cJSON_IsNumber(jsip)) {
      ov_logfile_warning("lbalance_neighborDB: msg: bad json format at %d",
                         LB_NBINFORMER_IPPOS);
      Ov_DeleteObject(pMsg);
      continue;
    }

    jsload = cJSON_GetArrayItem(jsMsg, LB_NBINFORMER_LOADPOS);
    if(!jsload || !cJSON_IsNumber(jsload)) {
      ov_logfile_warning("lbalance_neighborDB: msg: bad json format at %d",
                         LB_NBINFORMER_LOADPOS);
      Ov_DeleteObject(pMsg);
      continue;
    }

    jscap = cJSON_GetArrayItem(jsMsg, LB_NBINFORMER_CAPPOS);
    if(!jscap || !cJSON_IsNumber(jscap)) {
      ov_logfile_warning("lbalance_neighborDB: msg: bad json format at %d",
                         LB_NBINFORMER_CAPPOS);
      Ov_DeleteObject(pMsg);
      continue;
    }

    jsinfo = cJSON_GetArrayItem(jsMsg, LB_NBINFORMER_INFOPOS);
    if(!jsinfo || !cJSON_IsObject(jsinfo)) {
      ov_logfile_warning("lbalance_neighborDB: msg: bad json format at %d",
                         LB_NBINFORMER_INFOPOS);
      Ov_DeleteObject(pMsg);
      continue;
    }

    ov_string_setvalue(&pinst->v_IPs.value[currentInd], jsip->valuestring);
    pinst->v_caps.value[currentInd] = jscap->valueint;
    pinst->v_loads.value[currentInd] = jsload->valueint;

    currentInd++;

    cJSON_Delete(jsMsg);
    Ov_DeleteObject(pMsg);
  }
  ov_memstack_unlock();

  result |= Ov_SetDynamicVectorLength(&pinst->v_loads, currentInd, UINT);
  result |= Ov_SetDynamicVectorLength(&pinst->v_caps, currentInd, UINT);
  result |= Ov_SetDynamicVectorLength(&pinst->v_IPs, currentInd, STRING);
  result |= Ov_SetDynamicVectorLength(&pinst->v_sysInfos, currentInd, STRING);
  if(Ov_Fail(result)) {
    ov_logfile_error("lbalance_neighborDB: %u: %s: failed to allocate vector "
                     "with currentInd",
                     result, ov_result_getresulttext(result));
    pinst->v_status = LB_INTERNALERROR;
    pinst->v_result = result;
    return;
  }

  return;
}
