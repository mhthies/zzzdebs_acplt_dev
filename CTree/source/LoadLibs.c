/******************************************************************************
 *
 *   FILE
 *   ----
 *   LoadLibs.c
 *
 *   History
 *   -------
 *   2018-03-29   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_CTree
#define OV_COMPILE_LIBRARY_CTree
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "CTree.h"
#include "NoneTicketAuthenticator.h"
#include "libov/ov_debug.h"
#include "libov/ov_logfile.h"
#include "libov/ov_macros.h"
#include "libov/ov_result.h"
#include "libov/ov_string.h"

#include "ksapi.h"
#include "ksbase.h"
#include "ksbase_helper.h"

enum format { OS, ARCH, LIBS, ACPLTHOME, FORMATLEN };
enum state {
  INITIAL,
  INFO_REQUESTED,
  INFO_RECEIVED,
  DATA_SENT,
  DONE,
  INTERNAL_ERROR,
  EXTERNAL_ERROR,
  NUMOFSTATES
};

#define DBINFO_SYSTEM "/data/CTree/dbinfo.serversystem"
#define DBINFO_ARCH "/data/CTree/dbinfo.serversystemarch"
#define DBINFO_ADDONLIBS "/data/CTree/dbinfo.addonlibs"

OV_DLLFNCEXPORT OV_RESULT
                CTree_LoadLibs_constructor(OV_INSTPTR_ov_object pobj) {
  /*
   *   local variables
   */
  //	OV_INSTPTR_CTree_LoadLibs pinst = Ov_StaticPtrCast(CTree_LoadLibs,
  // pobj);
  OV_RESULT result;

  /* do what the base class does first */
  result = fb_functionblock_constructor(pobj);
  if(Ov_Fail(result)) return result;

  /* do what */

  return OV_ERR_OK;
}

OV_DLLFNCEXPORT void CTree_LoadLibs_destructor(OV_INSTPTR_ov_object pobj) {
  /*
   *   local variables
   */
  //	OV_INSTPTR_CTree_LoadLibs pinst = Ov_StaticPtrCast(CTree_LoadLibs,
  // pobj);
  /* do what */
  /* destroy object */
  fb_functionblock_destructor(pobj);

  return;
}

OV_DLLFNCEXPORT void CTree_LoadLibs_startup(OV_INSTPTR_ov_object pobj) {
  /*
   *   local variables
   */
  OV_INSTPTR_CTree_LoadLibs pinst = Ov_StaticPtrCast(CTree_LoadLibs, pobj);

  /* do what the base class does first */
  fb_functionblock_startup(pobj);

  /* do what */
  pinst->p_ks.v_actimode = 1;
  return;
}

OV_DLLFNCEXPORT void CTree_LoadLibs_shutdown(OV_INSTPTR_ov_object pobj) {
  /*
   *   local variables
   */
  //	OV_INSTPTR_CTree_LoadLibs pinst = Ov_StaticPtrCast(CTree_LoadLibs,
  // pobj);
  /* do what */
  /* set the object's state to "shut down" */
  fb_functionblock_shutdown(pobj);

  return;
}

OV_DLLFNCEXPORT OV_RESULT
                CTree_LoadLibs_reset_set(OV_INSTPTR_CTree_LoadLibs pobj, const OV_BOOL value) {
  OV_RESULT                    result = OV_ERR_OK;
  OV_INSTPTR_ksbase_ClientBase pClient = NULL;
  OV_VTBLPTR_ksbase_ClientBase pVtblClient = NULL;
  //  OV_VTBLPTR_CTree_SendFiles pVtblSendfiles = NULL;
  if(value && (!pobj->v_reset)) {
    pobj->v_status = INITIAL;
    pobj->v_result = OV_ERR_OK;

    ov_string_setvalue(&pobj->v_targetOS, NULL);
    ov_string_setvalue(&pobj->v_OS, NULL);
    pobj->v_targetArch = 0;
    pobj->v_Arch = 0;

    Ov_SetDynamicVectorLength(&pobj->v_targetLibs, 0, STRING);
    Ov_SetDynamicVectorLength(&pobj->v_libsToSend, 0, STRING);

    Ov_ForEachChildEx(
        ov_containment, pobj, pClient,
        ksbase_ClientBase) { /*	find the object in the containment which is
                                derived from ClientBase	*/
      break;
    }

    if(pClient) {
      Ov_GetVTablePtr(ksbase_ClientBase, pVtblClient, pClient);
      if(pVtblClient) {
        result = pVtblClient->m_reset(pClient);
      }
    }

    pobj->p_sendFiles.v_reset = 1;
  }
  pobj->v_reset = value;
  return result;
}

OV_DLLFNCEXPORT OV_RESULT
                CTree_LoadLibs_savelibs(OV_INSTPTR_CTree_LoadLibs pobj) {
  ov_logfile_info("in save");
  return 0;
}

/* checks if str in vec is. */
OV_BOOL strvec_contains(const OV_STRING_VEC* vec, const OV_STRING str) {
  if(!vec || !str) return 0;

  for(OV_UINT i = 0; i < vec->veclen; i++) {
    if(!ov_string_compare(vec->value[i], str)) return 1;
  }
  return 0;
}

OV_DLLFNCEXPORT void infoRequest_callback(const OV_INSTPTR_ov_domain this,
                                          const OV_INSTPTR_ov_domain that) {
  OV_INSTPTR_CTree_LoadLibs    pinst = Ov_StaticPtrCast(CTree_LoadLibs, this);
  OV_INSTPTR_ksbase_ClientBase pClient =
      Ov_StaticPtrCast(ksbase_ClientBase, that);
  OV_VTBLPTR_ksbase_ClientBase pVtblClient = NULL;
  OV_UINT                      itemsLength = 0;
  OV_GETVAR_ITEM*              itemsVals = NULL;
  OV_RESULT                    result = 0;

  if(!this || !that) {
    ov_logfile_error("callback issued with NULL pointers. aborting.");
    return;
  }

  Ov_GetVTablePtr(ksbase_ClientBase, pVtblClient, pClient);

  if(!pVtblClient) {
    ov_logfile_error(
        "%s callback: could not determine Vtable of Client %s. aborting",
        this->v_identifier, that->v_identifier);
    pinst->v_status = INTERNAL_ERROR;
    pinst->v_result = OV_ERR_BADOBJTYPE;
    return;
  }
  ov_memstack_lock();
  result = pVtblClient->m_processGetVar(
      pClient, NULL, (OV_RESULT*)&(pinst->v_result), &itemsLength, &itemsVals);
  if(Ov_Fail(result)) {
    pinst->v_status = INTERNAL_ERROR;
    pinst->v_result = result;
    ov_memstack_unlock();
    return;
  }

  pinst->v_status = INFO_RECEIVED;

  ov_memstack_unlock();
  if(!itemsVals[ARCH].result)
    pinst->v_targetArch =
        itemsVals[ARCH].var_current_props.value.valueunion.val_uint;
  if(!itemsVals[OS].result)
    ov_string_setvalue(
        &pinst->v_targetOS,
        itemsVals[OS].var_current_props.value.valueunion.val_string);
  if(!itemsVals[LIBS].result) {
    Ov_SetDynamicVectorValue(
        &pinst->v_targetLibs,
        itemsVals[LIBS].var_current_props.value.valueunion.val_string_vec.value,
        itemsVals[LIBS]
            .var_current_props.value.valueunion.val_string_vec.veclen,
        STRING);
  } else {
    ov_logfile_warning("couldnt get target addonlibs: %u",
                       itemsVals[LIBS].result);
  }
  //	char sep = '/';
  //	if(!itemsVals[ACPLTHOME].result)
  //		ov_string_print(&pinst->v_targetAddonlibsDir, "%s%s%c%s%c",
  // itemsVals[ACPLTHOME].var_current_props.value.valueunion.val_string,
  //"system", sep, "addonlibs", sep);;
  CTree_LoadLibs_execute(pinst);
  return;
}

OV_DLLFNCEXPORT void dataSend_callback(const OV_INSTPTR_ov_domain this,
                                       const OV_INSTPTR_ov_domain that) {
  ov_logfile_info("aaa");
  OV_INSTPTR_CTree_LoadLibs    pinst = Ov_StaticPtrCast(CTree_LoadLibs, this);
  OV_INSTPTR_ksbase_ClientBase pClient =
      Ov_StaticPtrCast(ksbase_ClientBase, that);
  OV_VTBLPTR_ksbase_ClientBase pVtblClient = NULL;
  OV_UINT                      itemsLength;
  OV_RESULT*                   itemsResults = NULL;
  OV_RESULT                    result;

  if(!this || !that) {
    ov_logfile_error("callback issued with NULL pointers. aborting.");
    return;
  }

  Ov_GetVTablePtr(ksbase_ClientBase, pVtblClient, pClient);

  if(!pVtblClient) {
    ov_logfile_error(
        "%s callback: could not determine Vtable of Client %s. aborting",
        this->v_identifier, that->v_identifier);
    pinst->v_status = CTREE_COMMON_INTERNALERROR;
    pinst->v_result = OV_ERR_BADOBJTYPE;
    return;
  }
  ov_memstack_lock();

  result = pVtblClient->m_processSetVar(pClient, NULL,
                                        (OV_RESULT*)&(pinst->v_result),
                                        &itemsLength, &itemsResults);
  if(Ov_Fail(result)) {
    pinst->v_status = CTREE_COMMON_INTERNALERROR;
    pinst->v_result = result;
    ov_memstack_unlock();
    return;
  }
  pinst->v_status = DONE;
  ov_logfile_info("Done.");

  if(Ov_Fail(pinst->v_result)) {
    pinst->v_status = CTREE_COMMON_EXTERNALERROR;
    ov_memstack_unlock();
    return;
  }

  for(OV_UINT i = 0; i < itemsLength; i++) {
    ov_logfile_info("%u: %s", itemsResults[i],
                    ov_result_getresulttext(itemsResults[i]));
  }
  ov_memstack_unlock();
  /*calling outer object*/
  if(pinst->v_postCallback.callbackFunction)
    (*pinst->v_postCallback.callbackFunction)(
        pinst->v_postCallback.instanceCalled,
        Ov_StaticPtrCast(ov_domain, pinst));
  return;
}

/* calls callback with that object when done. */
OV_RESULT CTree_LoadLibs_execute_withCallback(
    OV_INSTPTR_CTree_LoadLibs pinst, OV_INSTPTR_ov_domain that,
    void (*callback)(OV_INSTPTR_ov_domain, OV_INSTPTR_ov_domain)) {
  pinst->v_postCallback.callbackFunction = callback;
  pinst->v_postCallback.instanceCalled = that;
  return CTree_LoadLibs_execute(pinst);
}

OV_RESULT CTree_LoadLibs_execute(OV_INSTPTR_CTree_LoadLibs pinst) {
  OV_RESULT result = OV_ERR_OK;

  OV_STRING_VEC libsToSend = {0};
  //  OV_STRING_VEC libs = {0};

  //  OV_STRING_VEC *ptarget_libs = NULL;

  OV_BYTE_VEC  blibs = {0, NULL};
  OV_BYTE_VEC* paddonlibs = {0, NULL};
  //  OV_UINT_VEC libPositions = {0, NULL};

  OV_INSTPTR_ksbase_ClientBase pClient =
      Ov_StaticPtrCast(ksbase_ClientBase, &pinst->p_ks);
  OV_VTBLPTR_ksbase_ClientBase pVtblClient = NULL;

  /*for local info*/
  OV_GETVAR_PAR gvParam = {0};
  OV_GETVAR_RES gvResult = {0};
  OV_TICKET*    pticket = NULL;

  switch(pinst->v_status) {
    case DONE:
    case INITIAL:
      /*
       * 1. get own os and arch
       */

      ov_memstack_lock();
      pticket = ksbase_NoneAuth->v_ticket.vtbl->createticket(NULL, OV_TT_NONE);
      gvParam.identifiers_len = 2;
      OV_STRING paths[2] = {
          "/data/CTree/dbinfo.serversystem",
          "/data/CTree/dbinfo.serversystemarch",
      };
      gvParam.identifiers_val = paths;

      ov_ksserver_getvar(2, pticket, &gvParam, &gvResult);

      Ov_WarnIf(gvResult.result);
      Ov_WarnIf(gvResult.items_val[0].result);
      Ov_WarnIf(gvResult.items_val[1].result);

      ov_string_setvalue(
          &pinst->v_OS,
          gvResult.items_val[OS].var_current_props.value.valueunion.val_string);
      pinst->v_Arch =
          gvResult.items_val[ARCH].var_current_props.value.valueunion.val_uint;

      /*
       * 2. get target info os and architect and libs
       */
      /* preparing client */
      OV_STRING serverHost = NULL;
      OV_STRING serverName = NULL;
      OV_STRING serverPort = NULL;
      parse_kspath(pinst->v_targetKS, &serverHost, &serverPort, &serverName,
                   NULL);
      if(!serverHost) {
        ov_logfile_error("%s: no serverHost set. aborting",
                         pinst->p_ks.v_identifier);
        return OV_ERR_BADPARAM;
      }

      result = ksbase_ClientBase_serverHost_set(
          (OV_INSTPTR_ksbase_ClientBase)&pinst->p_ks, serverHost);
      if(Ov_Fail(result)) {
        ov_logfile_error("%s: submit: could not set serverHost at Client",
                         pinst->p_ks.v_identifier);
        return result;
      }
      if(serverName) {
        result = ksbase_ClientBase_serverName_set(
            (OV_INSTPTR_ksbase_ClientBase)&pinst->p_ks, serverName);
        if(Ov_Fail(result)) {
          ov_logfile_error("%s: submit: could not set serverName at Client",
                           pinst->p_ks.v_identifier);
          return result;
        }
      }

      Ov_GetVTablePtr(ksbase_ClientBase, pVtblClient, pClient);
      if(!pVtblClient) {
        //			ov_logfile_error(
        //					"%s callback: could not
        // determine Vtable of Client %s. aborting",
        // pinst->v_identifier, that->v_identifier);
        pinst->v_status = INTERNAL_ERROR;
        pinst->v_result = OV_ERR_BADOBJTYPE;
        ov_memstack_unlock();
        return OV_ERR_GENERIC;
      }
      OV_STRING targetpaths[FORMATLEN] = {
          [OS] = "/data/CTree/dbinfo.serversystem",
          [ARCH] = "/data/CTree/dbinfo.serversystemarch",
          [LIBS] = "/data/CTree/dbinfo.addonlibs"};

      result = pVtblClient->m_requestGetVar(
          pClient, NULL, FORMATLEN, targetpaths, (OV_INSTPTR_ov_domain)pinst,
          &infoRequest_callback);

      if(!(pClient->v_state & KSBASE_CLST_ERROR))
        pinst->v_status = INFO_REQUESTED;
      else
        pinst->v_status = INTERNAL_ERROR;
      ov_memstack_unlock();
      break;
    case INFO_REQUESTED:
      ov_logfile_error("unexpected status %u", INFO_REQUESTED);
      break;
    case INFO_RECEIVED:

      /*
       * 3. compare os and arch
       */
      if(pinst->v_Arch == pinst->v_targetArch ||
         ov_string_compare(pinst->v_OS, pinst->v_targetOS) == OV_STRCMP_EQUAL) {
        ov_logfile_info("OS and Arch are matching with target system");
      } else {
        ov_logfile_error("Target os or arch is different from local");
        return OV_ERR_GENERIC;
      }
      /*
       * 4. Find out libs to transport
       */
      ov_memstack_lock();

      ov_logfile_info("%u", pinst->v_libsToSend.veclen);
      Ov_SetDynamicVectorLength(&libsToSend, pinst->v_libsToSend.veclen,
                                STRING);
      OV_UINT j = 0;
      for(OV_UINT i = 0; i < pinst->v_libsToSend.veclen; i++) {
        if(strvec_contains(&pinst->v_targetLibs,
                           pinst->v_libsToSend.value[i])) {
          ov_logfile_info("Library %s exists", pinst->v_libsToSend.value[i]);
          libsToSend.veclen--;
        } else {
          ov_logfile_info("Library %s will be sent",
                          pinst->v_libsToSend.value[i]);
          ov_string_setvalue(&libsToSend.value[j],
                             pinst->v_libsToSend.value[i]);
          j++;
        }
      }
      /*
       * 5. convert libs to OV_BYTE_VEC
       */
      ov_memstack_lock();

      Ov_SetDynamicVectorLength(&pinst->p_sendFiles.v_filesToSend,
                                libsToSend.veclen, STRING);
      for(OV_UINT i = 0; i < libsToSend.veclen; ++i) {
        ov_string_print(&pinst->p_sendFiles.v_filesToSend.value[i],
                        "%s/system/addonlibs/%s%s", PACPLTHOME,
                        libsToSend.value[i], LIBEXTENSION);
        ov_logfile_info(pinst->p_sendFiles.v_filesToSend.value[i]);
      }
      Ov_SetDynamicVectorLength(&pinst->p_sendFiles.v_whereToSend,
                                libsToSend.veclen, STRING);
      for(OV_UINT i = 0; i < libsToSend.veclen; ++i) {
        ov_string_print(&pinst->p_sendFiles.v_whereToSend.value[i],
                        "%s/system/addonlibs/%s%s", PACPLTHOME,
                        libsToSend.value[i], LIBEXTENSION);
      }
      /*
       * 6. send libs
       */
      ov_string_setvalue(&pinst->p_sendFiles.v_targetKS, pinst->v_targetKS);
      result = CTree_SendFiles_execute_withCallback(
          &pinst->p_sendFiles, (OV_INSTPTR_ov_domain)pinst, &dataSend_callback);
      if(Ov_OK(result))
        pinst->v_status = DATA_SENT;
      else
        ov_logfile_error("%d", result);

      Ov_HeapFree(paddonlibs);
      Ov_HeapFree(blibs.value);
      break;
    default: ov_logfile_error("%u : unexpected State", pinst->v_status);
  }

  return result;
}

OV_DLLFNCEXPORT void CTree_LoadLibs_typemethod(OV_INSTPTR_fb_functionblock pfb,
                                               OV_TIME* pltc) {
  /*
   *   local variables
   */
  OV_INSTPTR_CTree_LoadLibs pinst = Ov_StaticPtrCast(CTree_LoadLibs, pfb);
  pinst->v_result = -1;
  OV_RESULT result = CTree_LoadLibs_execute(pinst);
  switch(result) {
    case OV_ERR_OK:
      ov_logfile_info("SendFiles: sent. waiting for response.");
      break;
    case OV_ERR_BADPARAM:
      pinst->v_result = result;
      ov_logfile_error("LoadLibs failed.");
      break;
    default: pinst->v_result = result; return;
  }

  return;
}
