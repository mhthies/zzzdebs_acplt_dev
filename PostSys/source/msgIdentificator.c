/******************************************************************************
 *
 *   FILE
 *   ----
 *   msgIdentificator.c
 *
 *   History
 *   -------
 *   2013-11-07   File created
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
#include "PostSys_helper.h"
#include "acplt_simpleMsgHandling.h"
#include "ks_logfile.h"
#include "libov/ov_macros.h"
#include "libov/ov_result.h"
#include <stdlib.h>

static const char msgIdentBegin[] = "<msg ";

OV_DLLFNCEXPORT OV_RESULT
                PostSys_msgIdentificator_constructor(OV_INSTPTR_ov_object pobj) {
  OV_RESULT result;
  OV_INSTPTR_PostSys_msgIdentificator this =
      Ov_StaticPtrCast(PostSys_msgIdentificator, pobj);

  /* do what the base class does first */
  result = ov_object_constructor(pobj);
  if(Ov_Fail(result))
    return result;

  /*	set protoclID	*/
  return ov_string_setvalue(&(this->v_protocolID), KSMSG_IDENTIFIER);
}

OV_DLLFNCEXPORT OV_BOOL
                PostSys_msgIdentificator_identify(OV_INSTPTR_ksbase_ProtocolIdentificator this,
                                                  OV_INSTPTR_ksbase_Channel pchannel) {
  OV_STRING tempstring = NULL;
  OV_STRING tempData = NULL;
  OV_UINT   i;
  KS_logfile_debug(("%s: identify called", this->v_identifier));
  /*	check if data is long enough for complete <msg>-tag (at least "<msg
   * hdrL=x bdyL=x>" --> 19 bytes	*/
  if(pchannel->v_inData.length < 19)
    return FALSE;
  /*
   * check if data begins with "<msg "
   */
  if(strncmp((char*)pchannel->v_inData.readPT, msgIdentBegin,
             sizeof(msgIdentBegin) - 1) ==
     0) { /*	begin is ok. check if hdrL and bdyL are set	*/
    /*	copy data to temporary buffer (msg is not necessarily NULL-terminated.
     * we have to do it	*/
    ov_memstack_lock();
    tempstring = ov_memstack_alloc(pchannel->v_inData.length + 1);
    if(!tempstring) {
      KS_logfile_error(
          ("%s: no heap for temporary string", this->v_identifier));
      ov_memstack_unlock();
      return FALSE;
    }
    /*	just copy until end of tag	*/
    for(i = 0; i < pchannel->v_inData.length &&
               pchannel->v_inData.readPT[i - 1] != '>';
        i++)
      tempstring[i] = pchannel->v_inData.readPT[i];
    tempstring[i] = '\0';
    if(Ov_Fail(acplt_simpleMsg_xml_getAttributeData(tempstring, "msg", "hdrL",
                                                    &tempData))) {
      ov_memstack_unlock();
      return FALSE;
    }
    if(!strtoul(tempData, NULL, 10)) {
      ov_memstack_unlock();
      return FALSE;
    }
    if(Ov_Fail(acplt_simpleMsg_xml_getAttributeData(tempstring, "msg", "bdyL",
                                                    &tempData))) {
      ov_memstack_unlock();
      return FALSE;
    }
    if(!strtoul(tempData, NULL, 10)) {
      ov_memstack_unlock();
      return FALSE;
    }
    ov_memstack_lock();
    return TRUE;
  } else
    return FALSE;
}

OV_DLLFNCEXPORT OV_RESULT PostSys_msgIdentificator_createClientHandler(
    OV_INSTPTR_ksbase_ProtocolIdentificator this,
    OV_INSTPTR_ksbase_Channel pchannel) {
  /*
   *   local variables
   */
  OV_INSTPTR_PostSys_msgHandler pMsgHandler = NULL;
  OV_UINT                       namecounter = 0;
  char      CHNameBuffer[22]; //"MsgHandler + length MAXINT + '\0'
  OV_RESULT result;

  // get first free "MsgHandler"-name
  do {
    pMsgHandler = NULL;
    namecounter++;
    sprintf(CHNameBuffer, "MsgHandler%" OV_PRINT_UINT, namecounter);
    pMsgHandler = (OV_INSTPTR_PostSys_msgHandler)Ov_SearchChild(
        ov_containment, Ov_StaticPtrCast(ov_domain, this), CHNameBuffer);
  } while(pMsgHandler);

  result = Ov_CreateObject(PostSys_msgHandler, pMsgHandler, this, CHNameBuffer);
  if(Ov_OK(result)) {
    KS_logfile_debug(("%s: MsgHandler created: %s", this->v_identifier,
                      pMsgHandler->v_identifier));
    result = Ov_Link(ksbase_AssocChannelClientHandler, pchannel, pMsgHandler);
    if(Ov_Fail(result)) {
      KS_logfile_error(("%s: could not link MsgHandler %s to Channel %s",
                        this->v_identifier, pMsgHandler->v_identifier,
                        pchannel->v_identifier));
      return result;
    } else {
      result =
          ov_string_setvalue(&(pMsgHandler->v_sourceAdr), pchannel->v_address);
      if(Ov_Fail(result)) {
        ov_memstack_lock();
        KS_logfile_error(("%s: could not set %s's sourceAdr. reason: %s",
                          this->v_identifier, pMsgHandler->v_identifier,
                          ov_result_getresulttext(result)));
        ov_memstack_unlock();
      }
      return OV_ERR_OK;
    }
  } else {
    ov_memstack_lock();
    KS_logfile_error(("%s: could not create MsgHandler: %s", this->v_identifier,
                      ov_result_getresulttext(result)));
    ov_memstack_unlock();
    return result;
  }
}

