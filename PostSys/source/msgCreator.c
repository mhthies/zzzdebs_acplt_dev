/******************************************************************************
 *
 *   FILE
 *   ----
 *   msgCreator.c
 *
 *   History
 *   -------
 *   2013-06-11   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_PostSys
#define OV_COMPILE_LIBRARY_PostSys
#endif

#include "PostSys.h"
#include "libov/ov_macros.h"

#include "libov/ov_path.h"
#include "libov/ov_result.h"
#include "libov/ov_vendortree.h"

#include "PostSys_helper.h"
#include "acplt_simpleMsgHandling.h"
#include "ksbase_helper.h"

OV_DLLFNCEXPORT OV_RESULT
                PostSys_msgCreator_constructor(OV_INSTPTR_ov_object pobj) {
  OV_RESULT                     result = OV_ERR_OK;
  OV_INSTPTR_PostSys_msgCreator pinst =
      Ov_StaticPtrCast(PostSys_msgCreator, pobj);

  result = fb_functionblock_constructor(pobj);
  result |= PostSys_msgCreator_pathLen_set(pinst, 1);
  result |=
      PostSys_msgCreator_dst_set(pinst, "//localhost:7509/MANAGER/TechUnits");

  return result;
}

OV_DLLFNCEXPORT OV_UINT
                PostSys_msgCreator_msgsInQueue_get(OV_INSTPTR_PostSys_msgCreator pinst) {
  OV_UINT                    msgs = 0;
  OV_INSTPTR_PostSys_Message pMsg = NULL;
  Ov_ForEachChildEx(ov_containment, pinst, pMsg, PostSys_Message) { msgs++; }
  return msgs;
}

OV_DLLFNCEXPORT OV_RESULT PostSys_msgCreator_pathElem_push(
    OV_INSTPTR_PostSys_msgCreator pinst, OV_STRING dstKS) {
  OV_RESULT result = OV_ERR_OK;
  OV_UINT   currentLen = PostSys_msgCreator_pathLen_get(pinst);
  result |= PostSys_msgCreator_pathLen_set(pinst, currentLen + 1);
  result |= PostSys_msgCreator_pathElem_set(pinst, dstKS, currentLen);
  return result;
}

OV_DLLFNCEXPORT OV_RESULT PostSys_msgCreator_dst_set(
    OV_INSTPTR_PostSys_msgCreator pinst, OV_STRING dstKS) {
  OV_RESULT result = OV_ERR_OK;
  if(!PostSys_msgCreator_pathLen_get(pinst)) {
    result |= PostSys_msgCreator_pathLen_set(pinst, 1);
    ov_logfile_warning("PostSys_msgCreator: pathLen was 0");
  }
  result |= PostSys_msgCreator_pathElem_set(
      pinst, dstKS, PostSys_msgCreator_pathLen_get(pinst) - 1);
  return result;
}

OV_DLLFNCEXPORT OV_RESULT PostSys_msgCreator_pathElem_set(
    OV_INSTPTR_PostSys_msgCreator pinst, OV_STRING dstKS, OV_UINT index) {
  OV_RESULT result = OV_ERR_OK;
  OV_ANY    srvnameprops;
  OV_STRING dstHost = NULL;
  OV_STRING dstServer = NULL;
  OV_STRING dstInst = NULL;
  OV_STRING dstHostPort = NULL;
  OV_STRING dstServerPort = NULL;

  if(PostSys_msgCreator_pathLen_get(pinst) <= index) {
    return OV_ERR_BADPARAM;
  }

  ov_memstack_lock();
  result = ks_splitOneStringPath(dstKS, &dstHost, &dstHostPort, &dstServer,
                                 &dstServerPort, &dstInst);
  if(Ov_Fail(result)) {
    ov_logfile_error("%u: %s:", result, ov_result_getresulttext(result));
    ov_memstack_unlock();
    return result;
  }
  if(!dstHost) {
    result |= ov_string_setvalue(&dstHost, "localhost");
    result |= ov_vendortree_getservername(&srvnameprops, NULL);
    result |= ov_string_setvalue(&dstServer,
                                 srvnameprops.value.valueunion.val_string);
    if(Ov_Fail(result)) {
      ov_logfile_error("PostSys_msgCreator: %u: %s: ", result,
                       ov_result_getresulttext(result));
      ov_memstack_unlock();
      return result;
    }
  }
  result |= ov_string_setvalue(&pinst->v_receiverHost.value[index], dstHost);
  result |= ov_string_setvalue(&pinst->v_receiverName.value[index], dstServer);
  result |=
      ov_string_setvalue(&pinst->v_receiverInstance.value[index], dstInst);
  ov_memstack_unlock();
  return result;
}

OV_RESULT acplt_msgExtendWithPath(OV_STRING* msg, OV_UINT pathLen,
                                  OV_STRING* Host, OV_STRING* Name,
                                  OV_STRING* Inst) {
  // TODO: zzz: add Sa 08 Dez 2018 23:42:20 CET
  return 0;
}

OV_DLLFNCEXPORT OV_RESULT PostSys_msgCreator_pathLen_set(
    OV_INSTPTR_PostSys_msgCreator pobj, const OV_UINT value) {
  OV_STRING_VEC* tmp[] = {&pobj->v_receiverHost, &pobj->v_receiverName,
                          &pobj->v_receiverInstance};
  OV_UINT        len = PostSys_msgCreator_pathLen_get(pobj);
  for(OV_UINT i = 0; i < 3; i++) {
    if(len != value) {
      Ov_SetDynamicVectorLength(tmp[i], value, STRING);
      /* setting added values to NULL */
      if(len < value) {
        for(OV_UINT j = len; j < value; j++) {
          tmp[i]->value[j] = NULL;
        }
      }
    }
  }
  return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_UINT
                PostSys_msgCreator_pathLen_get(OV_INSTPTR_PostSys_msgCreator pinst) {
  return pinst->v_receiverHost.veclen;
}

OV_DLLFNCEXPORT OV_RESULT PostSys_msgCreator_order_set(
    OV_INSTPTR_PostSys_msgCreator pobj, const OV_STRING value) {
  OV_RESULT                  result;
  OV_STRING                  msgBody = NULL;
  OV_STRING                  msgIdentifier = NULL;
  OV_STRING                  msgPrefix = "Message";
  OV_UINT                    i;
  OV_UINT                    tempctr = 0;
  OV_UINT                    pathLen = 0;
  OV_INSTPTR_PostSys_Message pMsg = NULL;
  OV_ANY                     tempAny;
  OV_STRING                  command = NULL;
  OV_STRING                  valueCopy = NULL;
  OV_STRING                  ids_vec[2] = {"cmdr", "Value"};
  OV_STRING                  values_vec[2] = {NULL, NULL};
  OV_STRING                  empty_vec[2] = {NULL, NULL};
  OV_STRING_VEC              ids = {2, ids_vec};
  OV_STRING_VEC              values = {2, values_vec};
  OV_STRING_VEC              types = {2, empty_vec};
  OV_STRING_VEC              units = {2, empty_vec};

  tempAny.value.vartype = OV_VT_VOID;
  tempAny.value.valueunion.val_double = 0.0;

  if(!value || !*value)
    return OV_ERR_OK;

  if(PostSys_msgCreator_msgsInQueue_get(pobj) >= pobj->v_queueLength)
    return OV_ERR_NOACCESS;

  for(i = 0; value[i] != '\0'; i++) {
    if(value[i] == ';')
      tempctr++;
  }
  if(tempctr != 2)
    return OV_ERR_BADVALUE; /*	an order has exactly 2 ";" in it
                             */

  // todo check lenght r same, and check if it is greater than 2
  pathLen = PostSys_msgCreator_pathLen_get(pobj);
  OV_STRING_VEC* a[3] = {&pobj->v_receiverHost, &pobj->v_receiverName,
                         &pobj->v_receiverInstance};
  for(OV_UINT i = 0; i < 3; i++) {
    for(OV_UINT j = 0; j < pathLen; j++) {
      if(!a[i]->value[j])
        return OV_ERR_BADVALUE;
    }
  }

  ov_memstack_lock();
  valueCopy = ov_memstack_alloc(i + 1); // here i holds the length of value
  if(!valueCopy) {
    ov_memstack_unlock();
    return OV_ERR_HEAPOUTOFMEMORY;
  }
  memcpy(valueCopy, value, i + 1);

  msgIdentifier = ov_memstack_alloc(sizeof(msgPrefix) + 12);
  if(!msgIdentifier) {
    ov_memstack_unlock();
    return OV_ERR_HEAPOUTOFMEMORY;
  }

  // snprintf(msgIdentifier, sizeof(msgPrefix) + 11, "%s%u", msgPrefix,
  // 	pobj->v_msgsInQueue);
  result =
      PostSys_createAnonymousMessage(Ov_StaticPtrCast(ov_domain, pobj),
                                     "Message", (OV_INSTPTR_ov_object*)(&pMsg));
  if(Ov_Fail(result)) {
    ov_logfile_error("Couldn't create Object 'Message' Reason: %s",
                     ov_result_getresulttext(result));
    ov_memstack_unlock();
    return OV_ERR_OK;
  }
  // result = Ov_CreateObject(PostSys_Message, pMsg, pobj, msgIdentifier);
  // if(Ov_Fail(result)) {
  // 	ov_memstack_unlock();
  // 	return result;
  // }

  OV_STRING_VEC tmpStrVec = {0};
  Ov_SetDynamicVectorLength(&tmpStrVec, pathLen + 1, STRING);

  /*setting Host path */
  result = ov_string_setvalue(&tmpStrVec.value[0], "none");
  if(Ov_Fail(result)) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return result;
  }
  result = ov_string_setvecvalue(tmpStrVec.value + 1,
                                 pobj->v_receiverHost.value, pathLen);
  if(Ov_Fail(result)) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return result;
  }
  result = PostSys_Message_pathAddress_set(pMsg, tmpStrVec.value, pathLen + 1);
  if(Ov_Fail(result)) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return result;
  }

  /* setting Servername path */
  ov_vendortree_getservername(&tempAny, NULL);
  result = ov_string_setvalue(&tmpStrVec.value[0],
                              tempAny.value.valueunion.val_string);
  if(Ov_Fail(result)) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return result;
  }
  result = ov_string_setvecvalue(tmpStrVec.value + 1,
                                 pobj->v_receiverName.value, pathLen);
  if(Ov_Fail(result)) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return result;
  }
  result = PostSys_Message_pathName_set(pMsg, tmpStrVec.value, pathLen + 1);
  if(Ov_Fail(result)) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return result;
  }
  /*setting Inst path */
  result = ov_string_setvalue(
      &tmpStrVec.value[0],
      ov_path_getcanonicalpath(Ov_StaticPtrCast(ov_object, pobj), 2));
  if(Ov_Fail(result)) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return result;
  }
  result = ov_string_setvecvalue(tmpStrVec.value + 1,
                                 pobj->v_receiverInstance.value, pathLen);
  if(Ov_Fail(result)) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return result;
  }
  result =
      PostSys_Message_pathComponent_set(pMsg, tmpStrVec.value, pathLen + 1);
  if(Ov_Fail(result)) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return result;
  }

  pMsg->v_sendBy = MSG_SEND_KSSETVAR; /*	send via ks-setvar	*/

  /*	generic part set, now build the body	*/

  values.value[0] = valueCopy;
  command = strchr(valueCopy, ';') + 1;
  *(command - 1) = '\0';
  values.value[1] = strchr(command, ';') + 1;
  *(values.value[1] - 1) = '\0';

  msgBody = acplt_simpleMsg_GenerateFlatBody(
      "ProcessControl", command, FALSE, NULL, &ids, &values, &units, &types);
  acplt_msgExtendWithPath(&msgBody, pathLen, pobj->v_receiverHost.value,
                          pobj->v_receiverName.value,
                          pobj->v_receiverInstance.value);

  if(!msgBody) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return OV_ERR_HEAPOUTOFMEMORY;
  }

  //    ov_logfile_debug("length: %lu\nmessageBody:\n%s\n", tempctr, msgBody);

  result = PostSys_Message_msgBody_set(pMsg, msgBody);
  if(Ov_Fail(result)) {
    Ov_DeleteObject(pMsg);
    ov_memstack_unlock();
    return result;
  }
  ov_memstack_unlock();

  return OV_ERR_OK;
}

OV_DLLFNCEXPORT void
PostSys_msgCreator_typemethod(OV_INSTPTR_fb_functionblock pfb, OV_TIME* pltc) {
  /*
   *   local variables
   */

  OV_INSTPTR_PostSys_msgCreator pinst =
      Ov_StaticPtrCast(PostSys_msgCreator, pfb);
  OV_INSTPTR_PostSys_Message     pMsg = NULL;
  OV_INSTPTR_PostSys_MsgDelivery pMsgDelivery = NULL;
  OV_BOOL                        isOk = 0;

  Ov_ForEachChildEx(ov_containment, pinst, pMsg, PostSys_Message) { break; }

  if(pMsg) {
    if(pMsg->v_msgStatus == MSGNEW)
      pMsg->v_msgStatus = MSGREADYFORSENDING;

    if(pMsg->v_msgStatus == MSGREADYFORSENDING) {
      pMsgDelivery = Ov_StaticPtrCast(
          PostSys_MsgDelivery,
          Ov_GetFirstChild(ov_instantiation, pclass_PostSys_MsgDelivery));
      if(pMsgDelivery) {
        isOk = PostSys_MsgDelivery_sendMessage(pMsgDelivery, pMsg);
        pinst->v_tries = 0;
        if(!isOk) {
          // TODO: zzz: work on in this msg Mi 19 Dez 2018 22:03:41 CET
          Ov_DeleteObject(pMsg);
          return;
        }
      }
    } else if(pMsg->v_msgStatus == MSGWAITING) {
      pinst->v_tries++;
      if(pinst->v_tries > 3) {
        Ov_DeleteObject(pMsg);
        Ov_ForEachChildEx(ov_containment, pinst, pMsg, PostSys_Message) {
          break;
        }
        if(pMsg) {
          if(pMsg->v_msgStatus == MSGNEW)
            pMsg->v_msgStatus = MSGREADYFORSENDING;
          isOk = PostSys_MsgDelivery_sendMessage(pMsgDelivery, pMsg);
          pinst->v_tries = 0;
          if(!isOk) {
            // TODO: zzz: work on in this msg Mi 19 Dez 2018 22:03:41 CET
            Ov_DeleteObject(pMsg);
            return;
          }
        }
      }
    }
  }
  return;
}
