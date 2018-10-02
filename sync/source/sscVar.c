/******************************************************************************
 *
 *   FILE
 *   ----
 *   sscVar.c
 *
 *   History
 *   -------
 *   2018-08-03   File created
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
#include "libov/ov_macros.h"
#include "libov/ov_string.h"

OV_DLLFNCEXPORT OV_RESULT sync_sscVar_port_set(OV_INSTPTR_sync_sscVar pobj,
		const OV_ANY* value) {
	ov_logfile_info("port set");
	if(value->value.vartype == OV_VT_STRING) {
		if(ov_string_compare(value->value.valueunion.val_string,
			pobj->v_value.value.valueunion.val_string)) pobj->v_changed = 1;
	}
	return ov_variable_setanyvalue(&pobj->v_value, value);
}