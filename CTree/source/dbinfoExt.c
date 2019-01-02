/******************************************************************************
 *
 *   FILE
 *   ----
 *   dbinfoExt.c
 *
 *   History
 *   -------
 *   2018-03-31   File created
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

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include "libov/ov_macros.h"
#include "libov/ov_vendortree.h"

void exitInfo(void) {
  ov_logfile_info("exitInfo");
  if(errno)
    perror("terminating␣with␣error␣condition");
  fputs("Good␣Bye\n", stderr);
}
OV_RESULT updateAddonlibs();

OV_DLLFNCEXPORT OV_RESULT
                CTree_dbinfoExt_constructor(OV_INSTPTR_ov_object pobj) {
  /*
   *   local variables
   */
  OV_INSTPTR_CTree_dbinfoExt pinst = Ov_StaticPtrCast(CTree_dbinfoExt, pobj);
  OV_RESULT                  result;

  /* do what the base class does first */
  result = ov_object_constructor(pobj);
  if(Ov_Fail(result))
    return result;

  atexit(exitInfo);

  /* do what */
  pinst->v_addonlibs.veclen = 0;
  pinst->v_addonlibs.value = NULL;
  /* architect */
  OV_UINT arch = 0;
  ov_string_setvalue(&pinst->v_serversystem, SERVER_SYSTEM);
#ifdef __x86_64
  arch = 64;
#else
  arch = 32;
#endif

  pinst->v_serversystemarch = arch;

  /* get acplt home */

  ov_string_setvalue(&pinst->v_acplthome, getenv("ACPLT_HOME"));

  updateAddonlibs(pobj);

  return OV_ERR_OK;
}

OV_DLLFNCEXPORT void CTree_dbinfoExt_destructor(OV_INSTPTR_ov_object pobj) {
  /*
   *   local variables
   */
  //  OV_INSTPTR_CTree_dbinfoExt pinst = Ov_StaticPtrCast(CTree_dbinfoExt,
  //  pobj);

  /* do what */

  /* destroy object */
  ov_object_destructor(pobj);

  return;
}

OV_DLLFNCEXPORT void CTree_dbinfoExt_startup(OV_INSTPTR_ov_object pobj) {
  /*
   *   local variables
   */
  //  OV_INSTPTR_CTree_dbinfoExt pinst = Ov_StaticPtrCast(CTree_dbinfoExt,
  //  pobj);

  /* do what the base class does first */
  ov_object_startup(pobj);

  /* do what */

  return;
}

OV_DLLFNCEXPORT void CTree_dbinfoExt_shutdown(OV_INSTPTR_ov_object pobj) {
  /*
   *   local variables
   */
  //  OV_INSTPTR_CTree_dbinfoExt pinst = Ov_StaticPtrCast(CTree_dbinfoExt,
  //  pobj);

  /* do what */

  /* set the object's state to "shut down" */
  ov_object_shutdown(pobj);

  return;
}

OV_DLLFNCEXPORT OV_STRING*
                CTree_dbinfoExt_addonlibs_get(OV_INSTPTR_CTree_dbinfoExt pobj,
                                              OV_UINT*                   pveclen) {
  ov_logfile_info("libs getter");
  *pveclen = pobj->v_addonlibs.veclen;
  return pobj->v_addonlibs.value;
}

OV_DLLFNCEXPORT OV_ACCESS CTree_dbinfoExt_getaccess(OV_INSTPTR_ov_object pobj,
                                                    const OV_ELEMENT*    pelem,
                                                    const OV_TICKET* pticket) {
  /*
   *   local variables
   */
  //  OV_INSTPTR_CTree_dbinfoExt pinst = Ov_StaticPtrCast(CTree_dbinfoExt,
  //  pobj);
  /*
   *	switch based on the element's type
   */
  return OV_AC_READ;
  /*switch(pelem->elemtype) {
   case OV_ET_VARIABLE:
   if(pelem->elemunion.pvar->v_offset >=
   offsetof(OV_INST_ov_object,__classinfo)) {

   //TODO: isFlagSet important?
   if(IsFlagSet(pelem->elemunion.pvar->v_flags, 'i')) {
   return OV_AC_READWRITE;
   }
   return OV_AC_READ;
   }
   break;
   case OV_ET_OBJECT:
   Gibt es schon FB-Instanzen?
   return (OV_AC_READWRITE | OV_AC_DELETEABLE);
   default:
   break;
   }*/
  return ov_object_getaccess(pobj, pelem, pticket);
}

OV_RESULT updateAddonlibs(OV_INSTPTR_CTree_dbinfoExt pinst) {
  OV_RESULT result = OV_ERR_OK;

  const OV_UINT libdirnum = 1;
  OV_STRING     libdirs[1] = {0};
  ov_string_print(&libdirs[0], "%s/system/addonlibs", getenv("ACPLT_HOME"));

  DIR*           daddonlibs = NULL;
  struct dirent* dir = NULL;
  OV_STRING_VEC  strvec = {0};

  for(OV_UINT k = 0; k < libdirnum; k++) {
    /*addonlibs*/
    daddonlibs = opendir(libdirs[k]);
    if(daddonlibs) {
      while((dir = readdir(daddonlibs)) != NULL) {
        //			ov_logfile_info(dir->d_name);
        if(!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
          continue;
        strvec.veclen++;
      }
      closedir(daddonlibs);
    } else {
      ov_logfile_error("could not open dir: %s", libdirs[k]);
    }
  }

  ov_logfile_info("   %u", strvec.veclen);
  ov_memstack_lock();
  strvec.value =
      (OV_STRING*)ov_memstack_alloc(strvec.veclen * sizeof(OV_STRING));
  ov_memstack_unlock();

  OV_UINT i = 0;
  for(OV_UINT k = 0; k < libdirnum; k++) {
    daddonlibs = opendir(libdirs[k]);
    if(daddonlibs) {
      OV_STRING tmpFilename = NULL;
      while((dir = readdir(daddonlibs)) != NULL) {
        if(!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
          continue;
        //					dir = readdir(daddonlibs);
        //			ov_logfile_info(dir->d_name);
        strvec.value[i] = NULL;
        OV_UINT extLen = 0;
        extLen = strlen(LIBEXTENSION);
        OV_RESULT tmpres = strget(&strvec.value[i], dir->d_name, 0, -extLen);
        if(tmpres)
          ov_logfile_error("cant open librarie %s", dir->d_name);
        i++;
      }
      ov_string_setvalue(&tmpFilename, NULL);
      closedir(daddonlibs);
    }
  }
  //	for (OV_UINT i = 0; i < strvec.veclen; i++)
  //		ov_logfile_info(strvec.value[i]);

  Ov_SetDynamicVectorValue(&pinst->v_addonlibs, strvec.value, strvec.veclen,
                           STRING);
  return result;
}
