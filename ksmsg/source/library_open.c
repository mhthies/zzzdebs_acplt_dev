
/******************************************************************************
 *
 *   FILE
 *   ----
 *   library_open.c
 *
 *   Author: C. Hansen, D. Kampert
 *
 *   History
 *   -------
 *   2012-10-19   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_ksmsg
#define OV_COMPILE_LIBRARY_ksmsg
#endif

#include "ksmsg.h"
#include "libov/ov_library.h"
#include "libov/ov_logfile.h"
#include "libov/ov_macros.h"
#include "libov/ov_path.h"
#include "libov/ov_result.h"
#include "libov/ov_time.h"

#ifdef ov_library_open_ksmsg
#undef ov_library_open_ksmsg
#endif

/*
 * This function will be called, when the library is loaded.
 * It could generate components and initializes the startup procedure
 */
OV_RESULT ov_library_setglobalvars_ksmsg_new(void) {
  OV_RESULT result;
  /*
   *    set the global variables of the original version
   *    and if successful, load other libraries
   *    and create some objects
   */

  result = ov_library_setglobalvars_ksmsg();

  ov_memstack_lock();
  /* create command executer */
  OV_INSTPTR_ksmsg_executer pexec = NULL;
  OV_INSTPTR_ov_domain      pData =
      Ov_StaticPtrCast(ov_domain, ov_path_getobjectpointer("/data", 2));

  result = Ov_CreateObject(ksmsg_executer, pexec, pData, "exec");
  if(Ov_Fail(result)) {
    if(result == OV_ERR_ALREADYEXISTS) {
    } else {
      ov_logfile_error("%u: %s: ksmsg: creation of executer failed", result,
                       ov_result_getresulttext(result));
      ov_memstack_unlock();
      return result;
    }
  } else {
		pexec->v_actimode = 1;
  }

  ov_memstack_unlock();
  return OV_ERR_OK;
}

/*
 *       Replace the 'setglobalvars' function of a library with this
 *       previous one, which additionally creates instances.
 * 	This is called by the OV system upon library load.
 */
OV_DLLFNCEXPORT OV_LIBRARY_DEF* ov_library_open_ksmsg(void) {
  /* local variables */
  static OV_LIBRARY_DEF* OV_LIBRARY_DEF_ksmsg_new;

  /*
   *       replace the 'setglobalvars' function created by the code generator
   *       with a new one.
   */
  OV_LIBRARY_DEF_ksmsg_new = ov_library_open_ksmsg_old();
  OV_LIBRARY_DEF_ksmsg_new->setglobalvarsfnc =
      ov_library_setglobalvars_ksmsg_new;
  return OV_LIBRARY_DEF_ksmsg_new;
}