/******************************************************************************
 *
 *   FILE
 *   ----
 *   bufferer.c
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
#include "PostSys_helpers.h"
#include "libov/ov_macros.h"
#include "libov/ov_result.h"

OV_DLLFNCEXPORT void sync_bufferer_typemethod(OV_INSTPTR_ksbase_ComTask this) {
  /*
   *   local variables
   */
  OV_RESULT                result = OV_ERR_OK;
  OV_INSTPTR_sync_bufferer pinst = Ov_StaticPtrCast(sync_bufferer, this);
  OV_INSTPTR_PostSys_inbox pInb =
      Ov_SearchChildEx(ov_containment, pinst, "inbox", PostSys_inbox);
  if(!pInb) {
    ov_logfile_debug("sync_bufferer: no inbox");
    return;
  }

  OV_INSTPTR_PostSys_Message pMsg = NULL;
  OV_INSTPTR_PostSys_Message pMsgCpy = NULL;
  /*OV_INSTPTR_PostSys_MsgDelivery pMsgDelivery = NULL;*/

  pMsg = Ov_StaticPtrCast(PostSys_Message, Ov_GetChild(ov_containment, pInb));
  OV_BOOL lastFound = 0;
  Ov_ForEachChildEx(ov_containment, pInb, pMsg, PostSys_Message) {
    if(lastFound) {
      break;
    }
    if(ov_string_compare(pMsg->v_msgID, pinst->v_lastBufferedMsg) ==
       OV_STRCMP_EQUAL) {
      lastFound = 1;
    }
    continue;
  }
  if(!lastFound) {
    Ov_ForEachChildEx(ov_containment, pInb, pMsg, PostSys_Message) { break; }
  }
  if(!pMsg)
    return;

  /* buffering msg */
  OV_INSTPTR_sync_Buffer pbuf = &pinst->p_buffer;
  /* copy msg to buffer */
  result = Ov_CreateIDedObject(PostSys_Message, pMsgCpy, pbuf, "Message");
  if(result) {
    ov_logfile_error("sync_bufferer: %s", ov_result_getresulttext(result));
    return;
  }
  result = PostSys_Message_copy(pMsgCpy, pMsg);
  if(Ov_Fail(result)) {
    ov_logfile_error("%u: %s: could not create copy of msg", result,
                     ov_result_getresulttext(result));
    return;
  }
  /* sending msg */
  PostSys_node_typemethod(Ov_StaticPtrCast(ksbase_ComTask, pinst));
  ov_string_setvalue(&pinst->v_lastBufferedMsg, pMsg->v_msgID);
  return;
}
