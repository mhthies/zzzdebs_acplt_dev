#ifndef OV_COMPILE_LIBRARY_PostSys
#define OV_COMPILE_LIBRARY_PostSys
#endif

#include "PostSys.h"

//#include "ServiceProviderConfig.h"

#include "PostSys_helper.h"
#include "libov/ov_element.h"
#include "libov/ov_macros.h"
#include "libov/ov_ov.h"
#include "libov/ov_string.h"
#include "libov/ov_time.h"
#include <time.h>

static OV_INT LOCALMSGCOUNTER;

OV_DLLFNCEXPORT OV_ACCESS PostSys_Message_getaccess(OV_INSTPTR_ov_object pobj,
                                                    const OV_ELEMENT*    pelem,
                                                    const OV_TICKET* pticket) {
  /*
   *	local variables
   */
  /*
   *	switch based on the element's type
   */
  switch(pelem->elemtype) {
    case OV_ET_VARIABLE:
      if(pelem->elemunion.pvar->v_offset >=
         offsetof(OV_INST_ov_object, __classinfo)) {
        if(pelem->elemunion.pvar->v_vartype == OV_VT_CTYPE)
          return OV_AC_NONE;
        else
          return OV_AC_READWRITE;
      }
      break;
    default:
      break;
  }

  return ov_object_getaccess(pobj, pelem, pticket);
}

OV_DLLFNCEXPORT OV_STRING*
                PostSys_Message_pathAddress_get(OV_INSTPTR_PostSys_Message pobj,
                                                OV_UINT*                   pveclen) {
  *pveclen = pobj->v_pathAddress.veclen;
  return pobj->v_pathAddress.value;
}

OV_DLLFNCEXPORT OV_RESULT
                PostSys_Message_pathAddress_set(OV_INSTPTR_PostSys_Message pobj,
                                                const OV_STRING* value, const OV_UINT veclen) {
  return Ov_SetDynamicVectorValue(&pobj->v_pathAddress, value, veclen, STRING);
}

OV_DLLFNCEXPORT OV_STRING*
                PostSys_Message_pathName_get(OV_INSTPTR_PostSys_Message pobj,
                                             OV_UINT*                   pveclen) {
  *pveclen = pobj->v_pathName.veclen;
  return pobj->v_pathName.value;
}

OV_DLLFNCEXPORT OV_RESULT
                PostSys_Message_pathName_set(OV_INSTPTR_PostSys_Message pobj,
                                             const OV_STRING* value, const OV_UINT veclen) {
  return Ov_SetDynamicVectorValue(&pobj->v_pathName, value, veclen, STRING);
}

OV_DLLFNCEXPORT OV_STRING*
                PostSys_Message_pathComponent_get(OV_INSTPTR_PostSys_Message pobj,
                                                  OV_UINT*                   pveclen) {
  *pveclen = pobj->v_pathComponent.veclen;
  return pobj->v_pathComponent.value;
}

OV_DLLFNCEXPORT OV_RESULT PostSys_Message_pathComponent_set(
    OV_INSTPTR_PostSys_Message pobj, const OV_STRING* value,
    const OV_UINT veclen) {
  return Ov_SetDynamicVectorValue(&pobj->v_pathComponent, value, veclen,
                                  STRING);
}

OV_DLLFNCEXPORT OV_RESULT PostSys_Message_copy(
    OV_INSTPTR_PostSys_Message pMsgCpy, const OV_INSTPTR_PostSys_Message pMsg) {
  OV_RESULT result = OV_ERR_OK;
  result =
      Ov_SetDynamicVectorValue(&pMsgCpy->v_pathName, pMsg->v_pathName.value,
                               pMsg->v_pathName.veclen, STRING);
  result |= Ov_SetDynamicVectorValue(&pMsgCpy->v_pathComponent,
                                     pMsg->v_pathComponent.value,
                                     pMsg->v_pathComponent.veclen, STRING);
  result |= Ov_SetDynamicVectorValue(&pMsgCpy->v_pathAddress,
                                     pMsg->v_pathAddress.value,
                                     pMsg->v_pathAddress.veclen, STRING);
  result |= ov_string_setvalue(&pMsgCpy->v_msgBody, pMsg->v_msgBody);
  result |= ov_string_setvalue(&pMsgCpy->v_msgID, pMsg->v_msgID);
  pMsgCpy->v_expectAnswer = pMsg->v_expectAnswer;
  pMsgCpy->v_msgStatus = pMsg->v_msgStatus;
  pMsgCpy->v_sendBy = pMsg->v_sendBy;
  pMsgCpy->v_pathLen = pMsg->v_pathLen;
  return result;
}

OV_DLLFNCEXPORT OV_STRING
                PostSys_Message_msgID_get(OV_INSTPTR_PostSys_Message pobj) {
  return pobj->v_msgID;
}

OV_DLLFNCEXPORT OV_RESULT PostSys_Message_msgID_set(
    OV_INSTPTR_PostSys_Message pobj, const OV_STRING value) {
  return ov_string_setvalue(&pobj->v_msgID, value);
}

OV_DLLFNCEXPORT OV_INT
                PostSys_Message_msgStatus_get(OV_INSTPTR_PostSys_Message pobj) {
  return pobj->v_msgStatus;
}

OV_DLLFNCEXPORT OV_RESULT PostSys_Message_msgStatus_set(
    OV_INSTPTR_PostSys_Message pobj, const OV_INT value) {
  pobj->v_msgStatus = value;
  return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_STRING
                PostSys_Message_msgBody_get(OV_INSTPTR_PostSys_Message pobj) {
  return pobj->v_msgBody;
}

OV_DLLFNCEXPORT OV_RESULT PostSys_Message_msgBody_set(
    OV_INSTPTR_PostSys_Message pobj, const OV_STRING value) {
  return ov_string_setvalue(&pobj->v_msgBody, value);
}

OV_DLLFNCEXPORT OV_RESULT PostSys_Message_pathLen_set(
    OV_INSTPTR_PostSys_Message pobj, const OV_UINT value) {
  OV_RESULT  result = OV_ERR_OK;
  OV_STRING* tmp = Ov_HeapMalloc(value * sizeof(OV_STRING));
  for(OV_UINT i = 0; i < value; ++i) {
    tmp[i] = NULL;
  }

  result |= PostSys_Message_pathName_set(pobj, tmp, value);
  result |= PostSys_Message_pathAddress_set(pobj, tmp, value);
  result |= PostSys_Message_pathComponent_set(pobj, tmp, value);
  pobj->v_pathLen = value;
  Ov_HeapFree(tmp);
  return result;
}

OV_DLLFNCEXPORT OV_UINT
                PostSys_Message_pathLen_get(OV_INSTPTR_PostSys_Message pinst) {
  return pinst->v_pathLen;
}

OV_DLLFNCEXPORT OV_RESULT
                PostSys_Message_constructor(OV_INSTPTR_ov_object pobj) {
  OV_INSTPTR_PostSys_Message this = Ov_StaticPtrCast(PostSys_Message, pobj);
  OV_UINT   ID = 4294967295;
  OV_STRING servername = NULL;
  OV_ANY    srvnameprops;

  ov_object_constructor(pobj);

  srand(time(NULL));
  LOCALMSGCOUNTER = LOCALMSGCOUNTER + 1;
  while(ID >= 4294967295) {
    ID = (((rand() % 1625) + 1) * ((rand() % 1625) + 1) *
          ((rand() % 1625) + 1)) +
         LOCALMSGCOUNTER;
  }
  ov_string_print(&this->v_msgID, "%lu", ID);

  ov_vendortree_getservername(&srvnameprops, NULL);
  PostSys_Message_pathName_set(this, &srvnameprops.value.valueunion.val_string,
                               1);

  // Collect Garbage
  ov_string_setvalue(&servername, NULL);
  this->v_sendBy = MSG_SEND_KSSETVAR;

  return OV_ERR_OK;
}

OV_DLLFNCEXPORT void PostSys_Message_destructor(OV_INSTPTR_ov_object pobj) {
  /*
   *   local variables
   */
  OV_INSTPTR_PostSys_Message this = Ov_StaticPtrCast(PostSys_Message, pobj);

  /* do what */
  ov_string_setvalue(&this->v_msgBody, NULL);
  /*	delete an associated channel object	*/
  if(Ov_GetChild(PostSys_Message2Channel, this)) {
    Ov_DeleteObject(Ov_GetChild(PostSys_Message2Channel, this));
  }

  /* destroy object */
  ov_object_destructor(pobj);

  return;
}

