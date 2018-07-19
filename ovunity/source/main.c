
/******************************************************************************
*
*   FILE
*   ----
*   main.c
*
*   History
*   -------
*   2018-07-13   File created
*
*******************************************************************************
*
*   This file is generated by the 'acplt_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_ovunity
#define OV_COMPILE_LIBRARY_ovunity
#endif


#include "ovunity.h"
#include "libov/ov_ov.h"
#include "libov/ov_macros.h"
#include "libov/ov_path.h"
#include "libov/ov_class.h"
#include "unity.h"
#include "unity_fixture.h"
#include "unity_internals.h"
#include "unity_fixture_internals.h"
#include "ovunity_helper.h"

OV_DLLFNCEXPORT OV_STRING ovunity_helper_data2str(OV_STRING filename) {
	OV_STRING buffer = NULL;
	long int string_size, read_size;
	FILE *handler = fopen(filename, "rb");

	if(handler) {
		// Seek the last byte of the file
		fseek(handler, 0, SEEK_END);
		// Offset from the first to the last byte, or in other words, filesize
		string_size = ftell(handler);
		// go back to the start of the file
		rewind(handler);

		// Allocate a string that can hold it all
		buffer = (OV_STRING) ov_database_malloc(sizeof(char) * (string_size + 1));


		// Read it all in one operation
		read_size = fread(buffer, sizeof(char), string_size, handler);

		// fread doesn't set it so put a \0 in the last position
		// and buffer is now officially a string
		buffer[string_size] = '\0';

		if(string_size != read_size) {
			// Something went wrong, throw away the memory and set
			// the buffer to NULL
			ov_string_setvalue(&buffer, NULL);
			fclose(handler);
			Throw(OV_ERR_GENERIC);
		}

		// Always remember to close the file.
		fclose(handler);
	}

	return buffer;
}

OV_DLLFNCEXPORT void ovunity_loadEnv(const OV_INSTPTR_ovunity_main pinst, const OV_STRING what, const OV_STRING where){
	/* getting libname and classname */
	OV_INSTPTR_ov_domain pclass = Ov_GetParent(ov_instantiation, pinst);
	OV_STRING classname = ov_strdup(pclass->v_identifier);
	OV_INSTPTR_ov_object plib = Ov_GetParent(ov_containment, pclass);
	OV_STRING projname = ov_strdup(plib->v_identifier);

//	OV_RESULT res = 0;
	OV_SETVAR_PAR params = { 0 };
	OV_SETVAR_RES result = { 0 };

	OV_TICKET* pticket = NULL;

	OV_UINT number_of_variables = 3;
//TODO: check json value

	ov_memstack_lock();

//create NONE-ticket
	pticket = ksbase_NoneAuth->v_ticket.vtbl->createticket(NULL, OV_TT_NONE);

//#####################################################################
//process multiple variables at once
	OV_STRING dataPath = NULL;
	char* ahome = getenv("ACPLT_HOME");
	ov_string_print(&dataPath, "%s/dev/%s/test/%s/%s", ahome, projname, classname, what);

	OV_SETVAR_ITEM items[3];
	OV_STRING uploadPath = "/data/CTree/Download";

	items[0].path_and_name = NULL;
	ov_string_print(&items[0].path_and_name, "%s.%s", uploadPath, "json"); /*	see comment below	*/
	items[0].var_current_props.value.vartype = KS_VT_STRING;
	items[0].var_current_props.value.valueunion.val_string = ovunity_helper_data2str(dataPath);

	items[1].path_and_name = NULL;
	ov_string_print(&items[1].path_and_name, "%s.%s", uploadPath, "path");
	items[1].var_current_props.value.vartype = KS_VT_STRING;
	items[1].var_current_props.value.valueunion.val_string = where;

	items[2].path_and_name = NULL;
	ov_string_print(&items[2].path_and_name, "%s.%s", uploadPath, "trigger");
	items[2].var_current_props.value.vartype = KS_VT_INT;
	items[2].var_current_props.value.valueunion.val_int = 1;

	params.items_val = items;
	params.items_len = number_of_variables;
	ov_ksserver_setvar(2, pticket, &params, &result);
	for (OV_UINT i = 0; i < number_of_variables; i++) {
		ov_string_setvalue(&params.items_val[i].path_and_name, NULL);
	}
	//TODO: free params.items_val[i].valuestr

	/*	delete Ticket	*/
	pticket->vtbl->deleteticket(pticket);

	/**
	 * Parse result from KS function
	 */

	if(Ov_Fail(result.result)) {
		//memory problem or NOACCESS
		ov_logfile_error("%s : NOACCESS or memory problem",
			ov_result_getresulttext(result.result));
		ov_memstack_unlock();
		return;
//		return result.result;
	}
	for (int i = 0; i < result.results_len; i++) {
//		OV_STRING resstring = NULL;
//		ov_string_setvalue(&resstring, ov_result_getresulttext(result.results_val[i]));
		if((result.results_val[i] != OV_ERR_OK)
				&& (result.results_val[i] != OV_ERR_NOACCESS)) return;
//			Download_log(pinst, OV_MT_WARNING, result.results_val[i],
//				"%s returns OV_RESULT: %i", params.items_val[i].path_and_name,
//				result.results_val[i]);
	}
//		fr = kshttp_print_result_array(&response->contentString, request.response_format, result.results_val, result.results_len, "");

	ov_memstack_unlock();
	ov_free(classname);
	ov_free(projname);
	return;
}

/* getting case path */
/*
 * gives path under the object to simulate environment
 */
OV_DLLFNCEXPORT OV_STRING ovunity_getCasePath(const OV_INSTPTR_ovunity_main pinst,
		const OV_STRING case_name) {
	OV_STRING path = NULL;
	OV_STRING tmp = ov_path_getcanonicalpath((void*) pinst, 2);
	ov_string_print(&path, "%s/%s", tmp, case_name);
	return path;
}

OV_DLLFNCEXPORT void ovunity_main_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
    /*
    *   local variables
    */
    return;
}
