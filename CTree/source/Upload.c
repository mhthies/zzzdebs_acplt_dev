/******************************************************************************
 *
 *   FILE
 *   ----
 *   Upload.c
 *
 *   History
 *   -------
 *   2018-02-23   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/

#define OV_DEBUG

#ifndef OV_COMPILE_LIBRARY_CTree
#define OV_COMPILE_LIBRARY_CTree
#endif

#include "libov/ov_ov.h"
#include "CTree.h"

#include "libov/ov_macros.h"
#include "libov/ov_string.h"
#include "libov/ov_class.h"
#include "libov/ov_library.h"
#include "libov/ov_path.h"
#include "libov/ov_association.h"
#include "libov/ov_result.h"

#include "ksbase.h"
#include "NoneTicketAuthenticator.h"

#include "cJSON.h"

#include "libov/ov_debug.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/*
 * Helpers
 */

/*
 * from path=~/b, pre=/a gives /a/b
 */
OV_STRING inverse_path2(const OV_STRING pre, const OV_STRING path) {
	OV_STRING resstr = NULL;

	if (pre == NULL)
		return resstr;

	ov_string_setvalue(&resstr, pre);
	if (path == NULL)
		return resstr;
	ov_string_append(&resstr, path + 1);
	return resstr;
}
/*
 * from a/b searches for class b in library a
 */
OV_INSTPTR_ov_class inverse_neutralpath(const OV_STRING neutralpath) {
	OV_STRING path = "/acplt/";
	ov_string_append(&path, neutralpath);
	OV_INSTPTR_ov_class pclass = NULL;
	pclass = Ov_StaticPtrCast(ov_class,
			ov_path_getobjectpointer(path, VERSION_FOR_CTREE));
	if (pclass != NULL)
		return pclass;

	ov_string_setvalue(&path, "/Libraries/");
	ov_string_append(&path, neutralpath);
	pclass = Ov_StaticPtrCast(ov_class,
			ov_path_getobjectpointer(path, VERSION_FOR_CTREE));
	if (pclass != NULL)
		return pclass;

	return NULL;
}

OV_RESULT Upload_log(OV_INSTPTR_CTree_Upload pinst, OV_MSG_TYPE msg_type,
		OV_RESULT result, const OV_STRING format, ...) {
	char msg[1024];

	va_list args;

	pinst->v_result = result;
	va_start(args, format);

#if OV_SYSTEM_UNIX && !OV_SYSTEM_SOLARIS
	vsnprintf(msg, sizeof(msg), format, args);
#else
	vsprintf(msg, format, args);
#endif
	va_end(args);

	ov_logfile_print(msg_type, msg);

	if (msg_type == OV_MT_ERROR) {
		ov_string_print(&pinst->v_ErrorMsg, "%s ; %s ;", ov_result_getresulttext(result), msg);
	}
	return result;
}

OV_ANY get_value_from_str(cJSON* jsvar) {
	OV_ANY value = { 0 };
	value.state = OV_ST_GOOD;
	CTree_helper_strToOVType(&value.value.vartype,
			cJSON_GetArrayItem(jsvar, VARTYPE_POS)->valuestring);
	CTree_helper_strToValue(&value.value,
			cJSON_GetArrayItem(jsvar, VARVAL_POS)->valuestring);
	return value;
}

OV_RESULT set_variable_values(OV_INSTPTR_CTree_Upload pinst, cJSON* jsvariables,
		OV_INSTPTR_ov_object pobj) {
	/*
	 *	parameter and result objects
	 */
//	OV_INSTPTR_ksapi_setVar papiSet = &(pupload->pinst->p_apiSet);
	OV_RESULT res = OV_ERR_OK;

	cJSON* jsvariable = NULL;
	OV_UINT number_of_variables = cJSON_GetArraySize(jsvariables);
	OV_STRING objpathwithpunct = NULL;

	OV_SETVAR_PAR params = {0};
	OV_SETVAR_RES result = {0};
	OV_SETVAR_ITEM *addrp = NULL;

	OV_TICKET* pticket = NULL;

	//object path
	ov_memstack_lock();
	ov_string_setvalue(&objpathwithpunct,
			ov_path_getcanonicalpath(pobj, VERSION_FOR_CTREE));
	ov_string_append(&objpathwithpunct, ".");
	ov_memstack_unlock();

	//TODO: check json value

	ov_memstack_lock();
	addrp = (OV_SETVAR_ITEM*) ov_memstack_alloc(
			number_of_variables * sizeof(OV_SETVAR_ITEM));

	if (!addrp) {
		ov_memstack_unlock();
		res = OV_ERR_TARGETGENERIC;
		Upload_log(pinst, res, OV_MT_ERROR, "%s", ": internal memory problem");
		return res;
	}

	params.items_val = addrp;
	params.items_len = number_of_variables;

	//create NONE-ticket
	pticket = ksbase_NoneAuth->v_ticket.vtbl->createticket(NULL, OV_TT_NONE);

	//#####################################################################
	//process multiple variables at once
	cJSON_ArrayForEach(jsvariable, jsvariables)
	{
		addrp->var_current_props.value = get_value_from_str(jsvariable).value;
		//TODO:check for vartype and value
		OV_STRING tmp = NULL;
		ov_string_print(&tmp, "%s%s", objpathwithpunct, jsvariable->string);
		addrp->path_and_name = tmp;

		//add one size of a pointer
		addrp++;
	}

	ov_ksserver_setvar(2, pticket, &params, &result);

	/*	delete Ticket	*/
	pticket->vtbl->deleteticket(pticket);

	/**
	 * Parse result from KS function
	 */

	if (Ov_Fail(result.result)) {
		//memory problem or NOACCESS
//			kshttp_print_result_array(&response->contentString, request.response_format, &result.result, 1, ": NOACCESS or memory problem");
		ov_memstack_unlock();
		return result.result;
	}
	for (int i = 0; i < result.results_len; i++) {
//		OV_STRING resstring = NULL;
//		ov_string_setvalue(&resstring, ov_result_getresulttext(result.results_val[i]));
		if ((result.results_val[i] != OV_ERR_OK)
				&& (result.results_val[i] != OV_ERR_NOACCESS))
			Upload_log(pinst, OV_MT_WARNING, result.results_val[i],
					"%s returns OV_RESULT: %i",
					params.items_val[i].path_and_name, result.results_val[i]);
	}
//		fr = kshttp_print_result_array(&response->contentString, request.response_format, result.results_val, result.results_len, "");

	ov_memstack_unlock();
	return res;
}

OV_RESULT upload_tree(OV_INSTPTR_CTree_Upload pinst, cJSON* jsparent,
		OV_INSTPTR_ov_domain pparent) {

	OV_RESULT res = OV_ERR_OK;

	OV_INSTPTR_ov_class pclass = NULL;
	OV_INSTPTR_ov_object pobj = NULL;
	OV_INSTPTR_ov_class pclassobj = NULL;
	OV_STRING identifier = NULL;

	cJSON* jschild = NULL;
	cJSON* jscurrent = NULL;
	OV_STRING parentpath = NULL;

	//TODO: find more elegant solution to get path
	ov_memstack_lock();
	ov_string_setvalue(&parentpath,
			ov_path_getcanonicalpath(Ov_PtrUpCast(ov_object, pparent),
			VERSION_FOR_CTREE));
	ov_memstack_unlock();

	cJSON_ArrayForEach(jschild, jsparent)
	{
//		1. Getting identifier
		ov_string_setvalue(&identifier, jschild->string);
		if (!identifier) {
			return OV_ERR_BADPARAM;
		}

//		2. Getting class pointer
		jscurrent = cJSON_GetObjectItemCaseSensitive(jschild, FACTORYNAME);
		if (jscurrent == NULL) {
			ov_logfile_warning("malformed json at %s/%s", parentpath,
					jschild->string);
			continue;
		}
		OV_STRING factory = NULL;
		ov_string_setvalue(&factory, cJSON_GetStringValue(jscurrent));
		pclass = inverse_neutralpath(factory);
		if (pclass == NULL) {
			Upload_log(pinst, OV_MT_ERROR, OV_ERR_BADPARAM,
					"There is no class : %s", factory);
			continue;
		}

//		3. Creating Object
		res =
				ov_class_createobject(pclass,
						((OV_INSTPTR_ov_domain) ((pparent)
								+ (0
										* ((pparent)->__classinfo.is_of_class_ov_domain)))),
						identifier, ((OV_PLACEMENT_HINT) 0), ((void *) 0),
						((void *) 0), ((void *) 0),
						((OV_INSTPTR_ov_object*) &(pobj)));
		if (Ov_Fail(res)) {
			if (res == OV_ERR_ALREADYEXISTS) {
				//TODO: check if it is from same class
				OV_ELEMENT pelparent = { .elemtype = OV_ET_OBJECT, .pobj =
						Ov_StaticPtrCast(ov_object, pparent) };
				OV_ELEMENT pelobj = { .elemtype = OV_ET_OBJECT };
				ov_element_searchchild(&pelparent, &pelobj, identifier);
				pobj = pelobj.pobj;
				Upload_log(pinst, OV_MT_WARNING, res, "%s already exists",
						identifier);
			} else {
				return Upload_log(pinst, res, OV_MT_ERROR,
						"Could not create %s in %s", identifier, parentpath);
			}
		} else {
//			Upload_log_exit(pinst, OV_MT_INFO, res, "created %s/%s", parentpath,
//					identifier);
			ov_logfile_info("created %s/%s", parentpath, identifier);
		}

		//	4. Set Variables
		cJSON* jsvariables = cJSON_GetObjectItemCaseSensitive(jschild,
				"variables");
		if (jsvariables != NULL) {
			res = set_variable_values(pinst, jsvariables, pobj);

			if (Ov_OK(res))
				//			Upload_log_exit(pinst, OV_MT_INFO, res, "variables set.");
				ov_logfile_info("variables set.");
		}

		res = upload_tree(pinst, cJSON_GetObjectItem(jschild, CHILDRENNAME),
				Ov_StaticPtrCast(ov_domain, pobj));
		if (Ov_Fail(res)) {
			//TODO: give more info
			return Upload_log(pinst, OV_MT_ERROR, res,
					"Could not load tree. error at %s", parentpath);
		}
	}
	return res;
}

OV_RESULT upload_libraries(OV_INSTPTR_CTree_Upload pinst, const cJSON* jslibs) {
	cJSON* current = NULL;
	OV_INSTPTR_ov_library plib = NULL;
	OV_RESULT res = 0;

	cJSON_ArrayForEach(current, jslibs)
	{
		if (!cJSON_IsString(current)) {
			return OV_ERR_BADPARAM;
		}
		OV_STRING libname = cJSON_GetStringValue(current);
		plib = ov_library_search(libname);
		res = Ov_CreateObject(ov_library, plib, &(pdb->acplt), libname);

		//	2.3 check if loaded successfully?
		switch (res) {
		case OV_ERR_OK:
			break;
		case OV_ERR_ALREADYEXISTS:
			ov_logfile_info("Library %s exists", libname);
			break;
		default:
			return Upload_log(pinst,  OV_MT_ERROR, res,
					"Could not load library %s", libname);
		}
	}
	return res;
}

OV_RESULT link_objects(OV_INSTPTR_CTree_Upload pinst, cJSON* jsobj,
		OV_STRING objpath) {
	OV_RESULT res = OV_ERR_OK;
	cJSON* jslinks = NULL;
	cJSON* jslink = NULL;
	cJSON* jschildren = NULL;
	cJSON* jschild = NULL;
	cJSON* jscurrent = NULL;
	OV_INSTPTR_ov_association passoc = NULL;
	OV_INSTPTR_ov_object pparent = NULL;
	OV_INSTPTR_ov_object pchild = NULL;
	OV_INSTPTR_ov_object pobj = ov_path_getobjectpointer(objpath,
	VERSION_FOR_CTREE);

	if (jsobj == NULL || pobj == NULL || pinst == NULL)
		return OV_ERR_BADPARAM;

	jslinks = cJSON_GetObjectItem(jsobj, LINKSNAME);
	if (jslinks != NULL) {
		cJSON_ArrayForEach(jslink, jslinks)
		{
			jscurrent = cJSON_GetObjectItem(jslink, ASSOCNAME);
			if (!jscurrent) {
				ov_logfile_warning("malformed links at %s.%s : %s", objpath,
						jslink->string, ASSOCNAME);
				continue;
			}
			cJSON* jsasparent = NULL;
			cJSON* jsasparents = cJSON_GetObjectItem(jslink, LINKPARENTSNAME);
			if (!jsasparents) {
				ov_logfile_warning("malformed links at %s.%s : %s", objpath,
						jslink->string, LINKPARENTSNAME);
				continue;
			}
			cJSON* jsaschild = NULL;
			cJSON* jsaschildren = cJSON_GetObjectItem(jslink,
			LINKCHILDRENNAME);
			if (!jsaschildren) {
				ov_logfile_warning("malformed links at %s.%s: %s", objpath,
						jslink->string, LINKCHILDRENNAME);
				continue;
			}

			passoc = Ov_StaticPtrCast(ov_association,
					inverse_neutralpath(cJSON_GetStringValue(jscurrent)));
			if (!passoc) {
				ov_logfile_warning("couldnt find assoc at %s.%s: %s", objpath,
						jslink->string, ASSOCNAME);
				continue;
			}

			cJSON_ArrayForEach(jsasparent, jsasparents)
			{
				OV_STRING link_parent_path = NULL;
				if (ov_string_compare(cJSON_GetStringValue(jsasparent),
						"this")==OV_STRCMP_EQUAL)
					pparent = pobj;
				else {
					ov_string_setvalue(&link_parent_path,
							cJSON_GetStringValue(jsasparent));
					if (link_parent_path == NULL)
						continue;

					if (*link_parent_path == '~')
						link_parent_path = inverse_path2(pinst->v_path,
								link_parent_path);
					pparent = ov_path_getobjectpointer(link_parent_path,
					VERSION_FOR_CTREE);
				}
				if (pparent == NULL) {
					Upload_log(pinst, OV_MT_ERROR, OV_ERR_GENERIC, "%s does not exist", link_parent_path);
					continue;
					//			return OV_ERR_BADPARAM;
				}

				cJSON_ArrayForEach(jsaschild, jsaschildren)
				{
					OV_STRING path = NULL;
					ov_string_setvalue(&path, cJSON_GetStringValue(jsaschild));
					if (*path == '~')
						path = inverse_path2(pinst->v_path, path);

					if (ov_string_compare(cJSON_GetStringValue(jsaschild),
							"this") == OV_STRCMP_EQUAL)
						pchild = pobj;
					else
						pchild = ov_path_getobjectpointer(path,
						VERSION_FOR_CTREE);

					if (pchild == NULL) {
						Upload_log(pinst, OV_MT_ERROR, OV_ERR_GENERIC, "%s does not exist", path);
						continue;
						//			return OV_ERR_BADPARAM;
					}

					/*
					 * linking
					 */
					res = ov_association_link(passoc, pparent, pchild,
					OV_PMH_DEFAULT, NULL, OV_PMH_DEFAULT, NULL);
					if (Ov_OK(res))
						ov_logfile_info("%s linked with %s through %s",
								pparent->v_identifier, pchild->v_identifier,
								passoc->v_identifier);
					else {
						if (res == OV_ERR_ALREADYEXISTS) {
							ov_logfile_warning(
									"%s is already linked with %s through %s",
									pparent->v_identifier, pchild->v_identifier,
									passoc->v_identifier);
							res = OV_ERR_OK;
						} else {
							Upload_log(pinst, OV_MT_ERROR, OV_ERR_GENERIC,
									"%s can not be linked with %s through %s",
									pparent->v_identifier, pchild->v_identifier,
									passoc->v_identifier);
							res = OV_ERR_OK;
							//!!!
						}
					}
				}
			}
		}
	}
	jschildren = cJSON_GetObjectItem(jsobj, CHILDRENNAME);
	if (jschildren == NULL)
		return res;
	cJSON_ArrayForEach(jschild, jschildren)
	{
		OV_STRING childpath = NULL;
		ov_string_print(&childpath, "%s/%s", objpath, jschild->string);
		link_objects(pinst, jschild, childpath);

	}
	return res;
}

OV_RESULT CTree_Upload_execute(OV_INSTPTR_CTree_Upload pinst) {
	/*
	 * Init
	 */
	OV_RESULT res = OV_ERR_OK;
	cJSON * jsbase = pinst->v_cache.jsbase = NULL;
	cJSON * jslibs = pinst->v_cache.jslibs = NULL;
	cJSON * jstree = pinst->v_cache.jstree = NULL;
	//	cJSON * jspath = pinst->v_cache.jspath = NULL;
	cJSON * jscurrent = NULL;

	//1. parse input
	jsbase = cJSON_Parse(pinst->v_json);
	//1.1 check if file is ok
	if (jsbase == NULL) {
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL) {
			//			print_log(pinst, ov_logfile_error, "bad js file");
		}
		Upload_log(pinst, OV_ERR_BADPARAM, OV_MT_ERROR, "Bad json file");
		return OV_ERR_BADPARAM;

	}
	ov_logfile_info("parsed successfully");

	/*
	 * init path
	 */
	if (!pinst->v_path) {
		pinst->v_path = "";
		ov_string_setvalue(&pinst->v_path,
				cJSON_GetObjectItem(jsbase, PATHNAME)->valuestring);
	} else {
		strcpy(cJSON_GetObjectItem(jsbase, PATHNAME)->valuestring, pinst->v_path);
	}

	//   2. Load Libraries
	//   2.1 jsbase contains libraries?
	jslibs = cJSON_GetObjectItem(jsbase, "Libraries");
	if (jslibs == NULL) {
		ov_logfile_info("No libraries to load");
	}

	//  2.2 load
	res = upload_libraries(pinst, jslibs);
	//  2.3 successfully?
	if (Ov_Fail(res))
		if (res == OV_ERR_ALREADYEXISTS) {
			res = OV_ERR_OK;
		} else {
			Upload_log(pinst, OV_MT_ERROR, res,
					"Could not load dependent libraries");
			return res;
		}

	//	3. CreateObjects Iteratively
	OV_STRING root_factory = NULL;
	jstree = cJSON_GetObjectItem(jsbase, TREENAME);
	jscurrent = cJSON_GetObjectItem(jstree->child, FACTORYNAME);
	if (jscurrent == NULL) {
		Upload_log(pinst, OV_MT_ERROR, OV_ERR_BADPARAM, "malformed root factory");
		return OV_ERR_BADPARAM;
	}
	ov_string_setvalue(&root_factory, jscurrent->valuestring);
	OV_INSTPTR_ov_class proot_class = inverse_neutralpath(root_factory);

	OV_STRING root_path = NULL;
	ov_string_setvalue(&root_path,
			cJSON_GetStringValue(cJSON_GetObjectItem(jsbase, PATHNAME)));

	OV_STRING tmp = strrchr(root_path, '/');
	*(tmp)=0;
	OV_INSTPTR_ov_object proot = ov_path_getobjectpointer(root_path, VERSION_FOR_CTREE);
	*(tmp)='/';
	strcpy(jstree->child->string, tmp+1);

	if (proot != NULL) {
//		if (proot_class != Ov_GetParent(ov_instantiation, proot)) {
//			Upload_log(pinst, OV_MT_ERROR, "no object with path %s", root_path);
//			return OV_ERR_ALREADYEXISTS;
//		}
	} else {
		Upload_log(pinst, OV_MT_ERROR, res, "root doesnt exist");
		return OV_ERR_GENERIC;
	}


	upload_tree(pinst, jstree, Ov_StaticPtrCast(ov_domain, proot));

	//	4. Link
	res = link_objects(pinst, jstree->child, root_path);

	return res;
}

OV_DLLFNCEXPORT void CTree_Upload_typemethod(OV_INSTPTR_fb_functionblock pfb,
		OV_TIME *pltc) {
	/*
	 *   local variables
	 */
	OV_INSTPTR_CTree_Upload pinst = Ov_StaticPtrCast(CTree_Upload, pfb);

	/*
	 * Init
	 */
	pinst->v_ErrorMsg = "";
	pinst->v_result = OV_ERR_OK;

	OV_RESULT res = CTree_Upload_execute(pinst);
	switch (res) {
	case OV_ERR_OK:
		pinst->v_result = res;
		ov_logfile_info("Upload done.");
		break;
	case OV_ERR_BADPARAM:
		pinst->v_result = res;
		ov_logfile_error("Upload failed.");
		break;
	default:
		pinst->v_result = OV_ERR_GENERIC;
		ov_logfile_error("Upload failed. : %s", ov_result_getresulttext(res));
	}
	cJSON_free(pinst->v_cache.jsbase);
	pinst->v_path = NULL;
	pinst->v_json = NULL;
	return;
}

