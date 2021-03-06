/******************************************************************************
 *
 *   FILE
 *   ----
 *   neighborInformer.c
 *
 *   History
 *   -------
 *   Fr 28 Dez 2018 12:58:11 CET   File created
 *
 ******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_lbalance
#define OV_COMPILE_LIBRARY_lbalance
#endif

#include "PostSys_helper.h"
#include "cJSON.h"
#include "lbalance.h"
#include "lbalance_helper.h"
#include "libov/ov_path.h"
#include "libov/ov_result.h"

#include "libov/ov_macros.h"

OV_DLLFNCEXPORT OV_RESULT
                lbalance_neighborInformer_constructor(OV_INSTPTR_ov_object pobj) {
  OV_RESULT                            result = OV_ERR_OK;
  OV_INSTPTR_lbalance_neighborInformer pinst =
      Ov_StaticPtrCast(lbalance_neighborInformer, pobj);

  result = fb_functionblock_constructor(pobj);
  result |= Ov_Link(fb_tasklist, pinst, &pinst->p_broadcaster);
  pinst->p_broadcaster.v_actimode = 1;
  pinst->p_broadcaster.v_iexreq = 1;

  return result;
}

OV_DLLFNCEXPORT OV_RESULT lbalance_neighborInformer_B_set(
    OV_INSTPTR_lbalance_neighborInformer pinst, const OV_BOOL value) {
  pinst->v_B = value;
  if(!value) {
    pinst->v_status = LB_NBINFORMER_INIT;
  }
  return 0;
}

OV_DLLFNCEXPORT OV_UINT
                lbalance_neighborInformer_cap_get(OV_INSTPTR_lbalance_neighborInformer pinst) {
  return 100 - pinst->v_sum;
}

OV_DLLFNCEXPORT OV_RESULT lbalance_neighborInformer_reset_set(
    OV_INSTPTR_lbalance_neighborInformer pinst, const OV_BOOL value) {
  pinst->v_reset = value;
  return 0;
}

OV_DLLFNCEXPORT void
lbalance_neighborInformer_typemethod(OV_INSTPTR_fb_functionblock pfb,
                                     OV_TIME*                    pltc) {
  /*
   *   local variables
   */
  OV_RESULT                            result = OV_ERR_OK;
  OV_INSTPTR_lbalance_neighborInformer pinst =
      Ov_StaticPtrCast(lbalance_neighborInformer, pfb);
  OV_INSTPTR_PostSys_msgCreator pbcaster = &pinst->p_broadcaster;
  OV_STRING                     order = NULL;
  OV_STRING                     myIP = pinst->v_myIP;
  /* OV_STRING                     infoStr = NULL; */
  OV_STRING dstKS = NULL;

  OV_UINT nlen = 0;

  switch(pinst->v_status) {
    case LB_NBINFORMER_INIT:
      if(pinst->v_B) {
        nlen = pinst->v_neighborIPs.veclen;
        Ov_AbortIf(nlen != pinst->v_serverNames.veclen);
        /* paramcheck */
        {
          if(!nlen) {
            ov_logfile_warning(
                "lbalance_neighborInformer: no server to inform");
            pinst->v_status = LB_NBINFORMER_SENT;
            return;
          }
        }

        for(OV_UINT i = 0; i < nlen; ++i) {
          /* create neighbor ks*/
          result |= ov_string_print(&dstKS, "//%s/%s%s",
                                    pinst->v_neighborIPs.value[i],
                                    pinst->v_serverNames.value[i], LB_NDB_PATH);

          /* ov_string_print(&dstKS, "//%s/%s%s",
             pinst->v_neigborIPs.value[i], */
          /* pinst->v_serverNames.value[i], LB_NDB_PATH); */
          /* send to neighbor */
          result |= PostSys_msgCreator_dst_set(pbcaster, dstKS);
          result |= ov_string_append(&order, "a;a;");
          /* result |= ov_string_append(&order, ";"); */
          cJSON* jsMsg = cJSON_CreateArray();
          /* order is important. see helper */
          cJSON_AddItemToArray(jsMsg, cJSON_CreateString(myIP));
          OV_ANY servername = {0};
          servername.value.vartype = OV_VT_VOID;
          servername.value.valueunion.val_string = NULL;
          ov_vendortree_getservername(&servername, NULL);
          cJSON_AddItemToArray(
              jsMsg,
              cJSON_CreateString(servername.value.valueunion.val_string));
          cJSON_AddItemToArray(jsMsg, cJSON_CreateNumber(pinst->v_sum));
          cJSON_AddItemToArray(
              jsMsg,
              cJSON_CreateNumber(lbalance_neighborInformer_cap_get(pinst)));
          cJSON_AddItemToArray(jsMsg, cJSON_CreateArray());
          OV_STRING tmpStr = cJSON_Print(jsMsg);
          if(tmpStr) {
            result |= ov_string_append(&order, tmpStr);
            free(tmpStr);
          }
          /* result |= ov_string_setvalue(&order, myIP); */
          /* result |= ov_string_print(&tmpStr, "%u%s%u", pinst->v_sum, */
          /* LBALANCE_SEP, pinst->v_cap); */
          /* result |= ov_string_append(&order, tmpStr); */
          /* result |= ov_string_append(&order, infoStr); */
          result |= PostSys_msgCreator_order_set(pbcaster, order);
          ov_logfile_debug(
              "lbalance_neighborInformer: informing neighbor %s with order: %s",
              dstKS, order);
          if(Ov_Fail(result)) {
            ov_logfile_error("lbalance_neighborInformer %u: %s: ", result,
                             ov_result_getresulttext(result));
            pinst->v_actimode = 0;
            pinst->v_status = LB_INTERNALERROR;
            return;
          }
        }
        pinst->v_status = LB_NBINFORMER_SENT;
      }
      break;
    case LB_NBINFORMER_SENT:
      break;

      LB_CASEERROR
  }
  return;
}

