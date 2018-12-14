/******************************************************************************
 *
 *   FILE
 *   ----
 *   sscBuffer.c
 *
 *   History
 *   -------
 *   2018-11-14   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_sync
#define OV_COMPILE_LIBRARY_sync
#endif

#include "sync.h"

#include "PostSys.h"
#include "libov/ov_macros.h"
#include "libov/ov_result.h"

OV_DLLFNCEXPORT void sync_sscBuffer_typemethod(OV_INSTPTR_ksbase_ComTask this) {
  /*
   *   local variables
   */
  OV_RESULT                      result = OV_ERR_OK;
  OV_INSTPTR_sync_sscBuffer      pinst = Ov_StaticPtrCast(sync_sscBuffer, this);
  OV_INSTPTR_PostSys_Message     pMsg = NULL;
  OV_INSTPTR_PostSys_Message     pMsgCpy = NULL;
  OV_INSTPTR_PostSys_MsgDelivery pMsgDelivery = NULL;

  pMsg = Ov_GetChild(ov_containment, pinst);
  Ov_ForEachChildEx(ov_containment, pinst, pMsg, PostSys_Message) { break; }
  if(!pMsg) return;

  /* buffering msg */
  OV_INSTPTR_sync_Buffer pbuf = NULL;
  /* getting buffer pointer */
  pbuf = Ov_SearchChildEx(ov_containment, pinst, "buffer", sync_Buffer);
  if(!pbuf) result = Ov_CreateObject(sync_Buffer, pbuf, pinst, "buffer");
  /* copy msg to buffer */
  result = Ov_CreateIDedObject(sync_bufferMessage, pMsgCpy, pbuf, "Message");
  if(result) {
    ov_logfile_error("sync_sscBuffer: %s", ov_result_getresulttext(result));
    return;
  }

  Ov_SetDynamicVectorValue(&pMsgCpy->v_pathName, pMsg->v_pathName.value,
                           pMsg->v_pathName.veclen, STRING);
  Ov_SetDynamicVectorValue(&pMsgCpy->v_pathComponent,
                           pMsg->v_pathComponent.value,
                           pMsg->v_pathComponent.veclen, STRING);
  Ov_SetDynamicVectorValue(&pMsgCpy->v_pathAddress, pMsg->v_pathAddress.value,
                           pMsg->v_pathAddress.veclen, STRING);
  ov_string_setvalue(&pMsgCpy->v_msgBody, pMsg->v_msgBody);
  pMsgCpy->v_expectAnswer = pMsg->v_expectAnswer;
  ov_string_setvalue(&pMsgCpy->v_msgID, pMsg->v_msgID);
  pMsgCpy->v_msgStatus = pMsg->v_msgStatus;
  pMsgCpy->v_sendBy = pMsg->v_sendBy;

  /* edit msg path */
  OV_UINT   pathLen = pMsgCpy->v_pathAddress.veclen;
  OV_STRING destAddr = NULL;
  OV_STRING destName = NULL;
  OV_STRING destComp = NULL;

  for(OV_UINT i = 0; i < pathLen; i++) {
    OV_UINT preLen = 0;
    preLen = ov_string_getlength(pinst->v_SourcePref);

    /* edit addr & name */
    // ov_string_setvalue(
    /* edit comp */
    if(strncmp(pMsgCpy->v_pathComponent.value[i], pinst->v_SourcePref,
               preLen)) {
      OV_STRING tmpStr = NULL;
      ov_string_setvalue(&tmpStr, pinst->v_SourcePref);
      ov_string_append(&pinst->v_SourcePref,
                       pMsgCpy->v_pathComponent.value[i] + preLen);
      ov_string_setvalue(&pMsgCpy->v_pathComponent.value[i], NULL);
      pMsgCpy->v_pathComponent.value[i] = tmpStr;
    }
  }

  /* sending msg */
  PostSys_node_typemethod(Ov_StaticPtrCast(PostSys_node, pinst));
  return;
}
