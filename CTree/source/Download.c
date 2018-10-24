/******************************************************************************
 *
 *   FILE
 *   ----
 *   Download.c
 *
 *   History
 *   -------
 *   2018-02-23   File created by Zolboo Erdenebayar
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

	if(pre == NULL) return resstr;

	ov_string_setvalue(&resstr, pre);
	if(path == NULL) return resstr;
	ov_string_append(&resstr, path + 1);
	return resstr;
}
/*
 * from a/b searches for class b in library a
 */
OV_INSTPTR_ov_class inverse_neutralpath(const OV_STRING neutralpath) {
	OV_INSTPTR_ov_class pclass = NULL;
	pclass = ov_class_search(neutralpath);

	if(pclass != NULL) return pclass;

	OV_STRING path = NULL;
	ov_string_print(&path, "%s%s", "/acplt/", neutralpath);
	pclass = Ov_StaticPtrCast(ov_class,
		ov_path_getobjectpointer(path, VERSION_FOR_CTREE));
	if(pclass != NULL) {
		ov_string_setvalue(&path, NULL);
		return pclass;
	}

	ov_string_setvalue(&path, "/Libraries/");
	ov_string_append(&path, neutralpath);
	pclass = Ov_StaticPtrCast(ov_class,
		ov_path_getobjectpointer(path, VERSION_FOR_CTREE));
	ov_string_setvalue(&path, NULL);
	if(pclass != NULL) return pclass;

	return NULL;
}

OV_RESULT Download_log(OV_INSTPTR_CTree_Download pinst, OV_MSG_TYPE msg_type,
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

	if(msg_type == OV_MT_ERROR) {
		ov_string_print(&pinst->v_ErrorMsg, "%s ; %s ;",
			ov_result_getresulttext(result), msg);
	}
	return result;
}

OV_RESULT jsonToVarelement(OV_ELEMENT* value, const cJSON* jsvalue);

OV_RESULT jsonToVarvalue(OV_VAR_VALUE* pvalue, const cJSON* jsvalue);

OV_RESULT jsonToValue(OV_BYTE* value, const OV_VAR_TYPE type,
		const cJSON* jstrueval) {
	OV_RESULT result = OV_ERR_OK;
	cJSON * jselem = NULL;
	OV_INT i = 0;
	OV_UINT vecLen = 0;
	if(type & OV_VT_ISVECTOR) vecLen = cJSON_GetArraySize(jstrueval);
	switch (type) {
		case OV_VT_VOID:
			break;
		case OV_VT_BYTE:
			*((OV_BYTE*) value) = jstrueval->valueint;
			break;
		case OV_VT_BOOL:
			*((OV_BOOL*) value) = jstrueval->valueint;
			break;
		case OV_VT_INT:
			*((OV_INT*) value) = jstrueval->valueint;
			break;
		case OV_VT_UINT:
			*((OV_UINT*) value) = jstrueval->valueint;
			break;
		case OV_VT_SINGLE:
			*((OV_SINGLE*) value) = jstrueval->valuedouble;
			break;
		case OV_VT_DOUBLE:
			*((OV_DOUBLE*) value) = jstrueval->valuedouble;
			break;
		case OV_VT_STRING:
			if(cJSON_IsNull(jstrueval)) {
				*((OV_STRING*) value) = NULL;
				break;
			}
			*((OV_STRING*) value) = NULL;
			result = ov_string_setvalue(((OV_STRING*) value), jstrueval->valuestring);
			break;
		case OV_VT_TIME:
			ov_time_asciitotime(((OV_TIME*) value), jstrueval->valuestring);
			/*jstmp = jstrueval;
			 value->valueunion.val_time.secs =
			 cJSON_GetArrayItem(jsvalue, 0)->valueint;
			 value->valueunion.val_time.usecs =
			 cJSON_GetArrayItem(jsvalue, 1)->valueint;*/
			break;
		case OV_VT_TIME_SPAN:
			//		ov_time_asciitotimespan(&value->valueunion.val_time, jstrueval->valuestring);
			((OV_TIME_SPAN*) value)->secs =
					cJSON_GetArrayItem(jstrueval, 0)->valueint;
			((OV_TIME_SPAN*) value)->usecs =
					cJSON_GetArrayItem(jstrueval, 1)->valueint;
			break;
		case OV_VT_STATE:
		case OV_VT_STRUCT:
		case OV_VT_CTYPE:
			return OV_ERR_NOTIMPLEMENTED;
			break;
			//		vector
		case OV_VT_BYTE_VEC:
			Ov_SetDynamicVectorLength(((OV_BYTE_VEC* ) value), vecLen, BYTE);
			cJSON_ArrayForEach(jselem, jstrueval)
			{
				((OV_BYTE_VEC*) value)->value[i] = (OV_BYTE) jselem->valueint;
				i++;
			}
			break;
		case OV_VT_BOOL_VEC:
			Ov_SetDynamicVectorLength(((OV_BOOL_VEC* ) value), vecLen, BOOL);
			cJSON_ArrayForEach(jselem, jstrueval)
			{
				if(cJSON_IsBool(jselem))
					((OV_BOOL_VEC*) value)->value[i] = jselem->valueint;
				i++;
			}
			break;
		case OV_VT_INT_VEC:
			Ov_SetDynamicVectorLength(((OV_INT_VEC* ) value), vecLen, INT);
			cJSON_ArrayForEach(jselem, jstrueval)
			{
				((OV_INT_VEC*) value)->value[i] = jselem->valueint;
				i++;
			}
			break;
		case OV_VT_UINT_VEC:
			Ov_SetDynamicVectorLength(((OV_UINT_VEC* ) value), vecLen, UINT);
			cJSON_ArrayForEach(jselem, jstrueval)
			{
				((OV_UINT_VEC*) value)->value[i] = jselem->valueint;
				i++;
			}
			break;
		case OV_VT_SINGLE_VEC:
			Ov_SetDynamicVectorLength(((OV_SINGLE_VEC* ) value), vecLen, SINGLE);
			cJSON_ArrayForEach(jselem, jstrueval)
			{
				((OV_SINGLE_VEC*) value)->value[i] = jselem->valuedouble;
				i++;
			}
			break;
		case OV_VT_DOUBLE_VEC:
			Ov_SetDynamicVectorLength(((OV_DOUBLE_VEC* ) value), vecLen, DOUBLE);
			cJSON_ArrayForEach(jselem, jstrueval)
			{
				((OV_DOUBLE_VEC*) value)->value[i] = jselem->valuedouble;
				i++;
			}
			break;
		case OV_VT_STRING_VEC:
			Ov_SetDynamicVectorLength(((OV_STRING_VEC* ) value), vecLen, STRING);
			cJSON_ArrayForEach(jselem, jstrueval)
			{
				((OV_STRING_VEC*) value)->value[i] = NULL;
				result = ov_string_setvalue(&((OV_STRING_VEC*) value)->value[i],
					jselem->valuestring);
				i++;
			}
			break;
		case OV_VT_TIME_VEC:
			Ov_SetDynamicVectorLength(((OV_TIME_VEC* ) value), vecLen, TIME);
			cJSON_ArrayForEach(jselem, jstrueval)
			{
				ov_time_asciitotime(&((OV_TIME_VEC*) value)->value[i],
					jselem->valuestring);
				i++;
			}
			break;
		case OV_VT_TIME_SPAN_VEC:
			Ov_SetDynamicVectorLength(((OV_TIME_SPAN_VEC* ) value), vecLen,
				TIME_SPAN);
			cJSON_ArrayForEach(jselem, jstrueval)
			{
				((OV_TIME_SPAN_VEC*) value)->value[i].secs = cJSON_GetArrayItem(jselem,
					0)->valueint;
				((OV_TIME_SPAN_VEC*) value)->value[i].usecs = cJSON_GetArrayItem(jselem,
					1)->valueint;
				i++;
			}
			break;
		case OV_VT_STRUCT_VEC:
			result = OV_ERR_NOTIMPLEMENTED;
			break;

		case OV_VT_BOOL_PV:
		case OV_VT_INT_PV:
		case OV_VT_UINT_PV:
		case OV_VT_SINGLE_PV:
		case OV_VT_DOUBLE_PV:
		case OV_VT_STRING_PV:
		case OV_VT_TIME_PV:
		case OV_VT_TIME_SPAN_PV:

		case OV_VT_BOOL_PV_VEC:
		case OV_VT_INT_PV_VEC:
		case OV_VT_UINT_PV_VEC:
		case OV_VT_SINGLE_PV_VEC:
		case OV_VT_DOUBLE_PV_VEC:
		case OV_VT_STRING_PV_VEC:
		case OV_VT_TIME_PV_VEC:
		case OV_VT_TIME_SPAN_PV_VEC:
			return OV_ERR_NOTIMPLEMENTED;

		case OV_VT_ANY:
			((OV_ANY*) value)->state = cJSON_GetArrayItem(jstrueval, 1)->valueint;
			ov_time_asciitotime(&((OV_ANY*) value)->time,
				cJSON_GetArrayItem(jstrueval, 2)->valuestring);
			result = jsonToVarvalue(&((OV_ANY*) value)->value,
				cJSON_GetArrayItem(jstrueval, 0));
			break;
		case OV_VT_POINTER:
			return OV_ERR_NOTIMPLEMENTED;

		default:
			return OV_ERR_BADPARAM; //TODO: revise it
	}
	return result;
}

OV_RESULT jsonToVarvalue(OV_VAR_VALUE* pvalue, const cJSON* jsvalue) {
	OV_RESULT result = OV_ERR_OK;
	cJSON * jstrueval = NULL;
	OV_VAR_TYPE type = 0;
	/* checking param */
	if(!pvalue || !jsvalue) return OV_ERR_BADPARAM;
	/* getting type */
	result = CTree_helper_strToOVType(&type,
		cJSON_GetArrayItem(jsvalue, VARTYPE_POS)->valuestring);
	if(result) return result;

	pvalue->vartype = type;
	/* getting jsvalue */
	jstrueval = cJSON_GetArrayItem(jsvalue, VARVAL_POS);
	/* writing value */
	result = jsonToValue(&pvalue->valueunion.val_byte, type, jstrueval);
	return result;
}

OV_RESULT jsonToVarelement(OV_ELEMENT* pelement, const cJSON* jsvalue) {
	OV_RESULT result = OV_ERR_OK;
	cJSON * jstrueval = NULL;
	OV_VAR_TYPE type = 0;
	/* checking param */
	if(!pelement || pelement->elemtype != OV_ET_VARIABLE || !jsvalue)
		return OV_ERR_BADPARAM;
	/* getting type */
	result = CTree_helper_strToOVType(&type,
		cJSON_GetArrayItem(jsvalue, VARTYPE_POS)->valuestring);
	if(result) return result;
	if(type != pelement->elemunion.pvar->v_vartype) return OV_ERR_BADPARAM;

//	OV_ANY val = { 0 };
//	if(pelement->elemunion.pvar->v_vartype == OV_ANY) {
//		jsonToValue(&val, OV_ANY, jstrueval);
//		Ov_SetAnyValue(pelement->pvalue, &val);
//	} else {
//		Ov_SetAnyValueByPointer(pelement->pvalue, pelement->elemunion.pvar->v_vartype,
//);
//	}


	/* getting jsvalue */
	jstrueval = cJSON_GetArrayItem(jsvalue, VARVAL_POS);
	/* writing value */
	result = jsonToValue(pelement->pvalue, type, jstrueval);
	if(result)
		ov_logfile_error("%s.%s: %s", pelement->pobj->v_identifier,
			pelement->elemunion.pvar->v_identifier, ov_result_getresulttext(result));
	return result;
}

OV_RESULT set_variable_values(OV_INSTPTR_CTree_Download pinst,
		cJSON* jsvariables, OV_INSTPTR_ov_object pobj) {
	/*
	 *	parameter and result objects
	 */
	OV_RESULT result = OV_ERR_OK;

	cJSON* jsvariable = NULL;

//TODO: check json value

	ov_memstack_lock();

	OV_ELEMENT parent = { 0 };
	parent.elemtype = OV_ET_OBJECT;
	parent.pobj = pobj;
	cJSON_ArrayForEach(jsvariable, jsvariables)
	{
		OV_ELEMENT child = { 0 };
		ov_element_searchpart(&parent, &child, OV_ET_VARIABLE, jsvariable->string);
		if(!child.elemunion.pvar) {
			ov_logfile_error("%s.%s not found", pobj->v_identifier,
				jsvariable->string);
			continue;
		}
		result = jsonToVarelement(&child, jsvariable);
		if(Ov_Fail(result)) {
			ov_logfile_error("%s.%s : %s", pobj->v_identifier, jsvariable->string,
				ov_result_getresulttext(result));
		}
	}

	ov_memstack_unlock();
	return OV_ERR_OK;
}

typedef enum {
	CTREE_CHILDREN, CTREE_PARTS
} TYPE_OF_MEMBER;

OV_RESULT download_tree(OV_INSTPTR_CTree_Download pinst, cJSON* jsparent,
		OV_INSTPTR_ov_domain pparent, TYPE_OF_MEMBER type);

OV_RESULT download_children(OV_INSTPTR_CTree_Download pinst, cJSON* jsparent,
		OV_INSTPTR_ov_domain pparent) {
	return download_tree(pinst, jsparent, pparent, CTREE_CHILDREN);
}
OV_RESULT download_parts(OV_INSTPTR_CTree_Download pinst, cJSON* jsparent,
		OV_INSTPTR_ov_domain pparent) {
	return download_tree(pinst, jsparent, pparent, CTREE_PARTS);
}

OV_RESULT download_libraries(OV_INSTPTR_CTree_Download pinst,
		const cJSON* jslibs) {
	cJSON* jscurrent = NULL;
	OV_INSTPTR_ov_library plib = NULL;
	OV_RESULT res = 0;
	cJSON* jslibs_cpy = cJSON_Duplicate(jslibs, TRUE);

	OV_STRING libname = NULL;

	OV_UINT round = 0;
	OV_UINT maxRound = 100;
	OV_UINT deleted = 1;

	while (cJSON_GetArraySize(jslibs_cpy) && deleted && round++ < maxRound) {
		char * tmpStr = cJSON_Print(jslibs_cpy);
		ov_logfile_info("round: %u: %s", round, tmpStr);
		free(tmpStr);

		OV_UINT currentIndex = 0;
		deleted = 0;

		for (jscurrent =
				(jslibs_cpy != ((void *) 0)) ? (jslibs_cpy)->child : ((void *) 0);
				jscurrent != ((void *) 0);) {
			if(!cJSON_IsString(jscurrent)) {
				ov_string_setvalue(&libname, NULL);
				return OV_ERR_BADPARAM;
			}
			ov_string_setvalue(&libname, cJSON_GetStringValue(jscurrent));
			res = Ov_CreateObject(ov_library, plib, &(pdb->acplt), libname);

			//	2.3 check if loaded successfully?
			switch (res) {
				case OV_ERR_OK:
					ov_logfile_info("Library %s loaded", libname);
					jscurrent = jscurrent->next;
					cJSON_DeleteItemFromArray(jslibs_cpy, currentIndex);
					deleted++;
					break;
				case OV_ERR_ALREADYEXISTS:
					ov_logfile_info("Library %s exists", libname);
					jscurrent = jscurrent->next;
					cJSON_DeleteItemFromArray(jslibs_cpy, currentIndex);
					deleted++;
					break;
				default:
					jscurrent = jscurrent->next;
					currentIndex++;
			}
		}
	}
	if(cJSON_GetArraySize(jslibs_cpy)) {
		char * tmpStr = cJSON_Print(jslibs_cpy);
		Download_log(pinst, OV_MT_ERROR, OV_ERR_GENERIC,
			"%i Libraries couldnt loaded: %s", cJSON_GetArraySize(jslibs_cpy),
			tmpStr);
		free(tmpStr);
		cJSON_Delete(jslibs_cpy);
		return OV_ERR_GENERIC;
	}
	cJSON_Delete(jslibs_cpy);
	ov_string_setvalue(&libname, NULL);
	return 0;
}

OV_RESULT link_objects(OV_INSTPTR_CTree_Download pinst, cJSON* jsobj,
		const OV_STRING objpath) {
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

	if(jsobj == NULL || pobj == NULL || pinst == NULL) return OV_ERR_BADPARAM;

	jslinks = cJSON_GetObjectItem(jsobj, LINKSNAME);
	if(jslinks != NULL) {
		cJSON_ArrayForEach(jslink, jslinks)
		{
			jscurrent = cJSON_GetObjectItem(jslink, ASSOCNAME);
			if(!jscurrent) {
				ov_logfile_warning("malformed links at %s.%s : %s", objpath,
					jslink->string,
					ASSOCNAME);
				continue;
			}
			cJSON* jsasparent = NULL;
			cJSON* jsasparents = cJSON_GetObjectItem(jslink,
			LINKPARENTSNAME);
			if(!jsasparents) {
				ov_logfile_warning("malformed links at %s.%s : %s", objpath,
					jslink->string,
					LINKPARENTSNAME);
				continue;
			}
			cJSON* jsaschild = NULL;
			cJSON* jsaschildren = cJSON_GetObjectItem(jslink,
			LINKCHILDRENNAME);
			if(!jsaschildren) {
				ov_logfile_warning("malformed links at %s.%s: %s", objpath,
					jslink->string,
					LINKCHILDRENNAME);
				continue;
			}

			passoc = Ov_StaticPtrCast(ov_association,
				inverse_neutralpath(cJSON_GetStringValue(jscurrent)));
			if(!passoc) {
				ov_logfile_warning("couldnt find assoc at %s.%s: %s", objpath,
					jslink->string, ASSOCNAME);
				continue;
			}

			OV_STRING link_parent_path = NULL;
			cJSON_ArrayForEach(jsasparent, jsasparents)
			{
				if(ov_string_compare(cJSON_GetStringValue(jsasparent),
					"this") == OV_STRCMP_EQUAL)
					pparent = pobj;
				else {
					ov_string_setvalue(&link_parent_path,
						cJSON_GetStringValue(jsasparent));
					if(link_parent_path == NULL) continue;

					if(*link_parent_path == '~')
						link_parent_path = inverse_path2(pinst->v_path, link_parent_path);
					pparent = ov_path_getobjectpointer(link_parent_path,
					VERSION_FOR_CTREE);
				}
				if(pparent == NULL) {
					Download_log(pinst, OV_MT_ERROR, OV_ERR_GENERIC, "%s does not exist",
						link_parent_path);
					continue;
					//			return OV_ERR_BADPARAM;
				}

				OV_STRING path = NULL;
				cJSON_ArrayForEach(jsaschild, jsaschildren)
				{
					ov_string_setvalue(&path, cJSON_GetStringValue(jsaschild));
					if(*path == '~') path = inverse_path2(pinst->v_path, path);

					if(ov_string_compare(cJSON_GetStringValue(jsaschild),
						"this") == OV_STRCMP_EQUAL)
						pchild = pobj;
					else
						pchild = ov_path_getobjectpointer(path,
						VERSION_FOR_CTREE);

					if(pchild == NULL) {
						Download_log(pinst, OV_MT_ERROR, OV_ERR_GENERIC,
							"%s does not exist", path);
						continue;
						//			return OV_ERR_BADPARAM;
					}

					/*
					 * linking
					 */
					res = ov_association_link(passoc, pparent, pchild,
					OV_PMH_DEFAULT, NULL, OV_PMH_DEFAULT, NULL);
					if(Ov_OK(res))
						ov_logfile_info("%s linked with %s through %s",
							pparent->v_identifier, pchild->v_identifier,
							passoc->v_identifier);
					else {
						if(res == OV_ERR_ALREADYEXISTS) {
							ov_logfile_warning("%s is already linked with %s through %s",
								pparent->v_identifier, pchild->v_identifier,
								passoc->v_identifier);
							res = OV_ERR_OK;
						} else {
							Download_log(pinst, OV_MT_ERROR, OV_ERR_GENERIC,
								"%s can not be linked with %s through %s",
								pparent->v_identifier, pchild->v_identifier,
								passoc->v_identifier);
							res = OV_ERR_OK;
							//!!!
						}
					}
				}
				ov_string_setvalue(&path, NULL);
			}
			ov_string_setvalue(&link_parent_path, NULL);
		}
	}
	jschildren = cJSON_GetObjectItem(jsobj, CHILDRENNAME);
	if(jschildren) {
		OV_STRING childpath = NULL;
		cJSON_ArrayForEach(jschild, jschildren)
		{
			ov_string_print(&childpath, "%s/%s", objpath, jschild->string);
			link_objects(pinst, jschild, childpath);

		}
		ov_string_setvalue(&childpath, NULL);
	}

	jschildren = cJSON_GetObjectItem(jsobj, PARTSNAME);
	if(jschildren) {
		OV_STRING partpath = NULL;
		cJSON_ArrayForEach(jschild, jschildren)
		{
			ov_string_print(&partpath, "%s.%s", objpath, jschild->string);
			link_objects(pinst, jschild, partpath);

		}
		ov_string_setvalue(&partpath, NULL);
	}
	return res;
}

OV_RESULT download_tree(OV_INSTPTR_CTree_Download pinst, cJSON* jsparent,
		OV_INSTPTR_ov_domain pparent, TYPE_OF_MEMBER type) {

	OV_RESULT res = OV_ERR_OK;

	OV_INSTPTR_ov_class pclass = NULL;
	OV_INSTPTR_ov_object pobj = NULL;
//	OV_INSTPTR_ov_class pclassobj = NULL;
	OV_STRING identifier = NULL;

	cJSON* jschild = NULL;
	cJSON* jscurrent = NULL;
	OV_STRING parentpath = NULL;
	OV_STRING elementpath = NULL;

	/*
	 *	for parts
	 */
	OV_PATH path;
	OV_ELEMENT element;

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
		if(!identifier) {
			return OV_ERR_BADPARAM;
		}

		switch (type) {
			case CTREE_CHILDREN:
//		2. Getting class pointer
				jscurrent = cJSON_GetObjectItemCaseSensitive(jschild, FACTORYNAME);
				if(jscurrent == NULL) {
					ov_logfile_warning("malformed json at %s/%s", parentpath,
						jschild->string);
					continue;
				}
				// OV_STRING factory = NULL;
				// ov_string_setvalue(&factory, cJSON_GetStringValue(jscurrent));
				pclass = inverse_neutralpath(cJSON_GetStringValue(jscurrent));
				if(pclass == NULL) {
					Download_log(pinst, OV_MT_ERROR, OV_ERR_BADPARAM,
						"There is no class : %s", cJSON_GetStringValue(jscurrent));
					continue;
				}

//		3. Creating Object
				res = ov_class_createobject(pclass,
					((OV_INSTPTR_ov_domain) ((pparent)
							+ (0 * ((pparent)->__classinfo.is_of_class_ov_domain)))),
					identifier, ((OV_PLACEMENT_HINT) 0), ((void *) 0), ((void *) 0),
					((void *) 0), ((OV_INSTPTR_ov_object*) &(pobj)));
				if(Ov_Fail(res)) {
					if(res == OV_ERR_ALREADYEXISTS) {
						//TODO: check if it is from same class
						OV_ELEMENT pelparent = { .elemtype = OV_ET_OBJECT, .pobj =
								Ov_StaticPtrCast(ov_object, pparent) };
						OV_ELEMENT pelobj = { .elemtype = OV_ET_OBJECT };
						ov_element_searchchild(&pelparent, &pelobj, identifier);
						pobj = pelobj.pobj;
						Download_log(pinst, OV_MT_WARNING, res, "%s already exists",
							identifier);
					} else {
						//TODO: free identifier
						res = Download_log(pinst, res, OV_MT_ERROR,
							"Could not create %s in %s", identifier, parentpath);
						ov_string_setvalue(&identifier, NULL);
						ov_string_setvalue(&parentpath, NULL);
						return res;
					}
				} else {
//			Download_log_exit(pinst, OV_MT_INFO, res, "created %s/%s", parentpath,
//					identifier);
					ov_logfile_info("created %s/%s", parentpath, identifier);
				}
				break;
			case CTREE_PARTS:
				ov_string_print(&elementpath, "%s.%s", parentpath, identifier);
				/*
				 *	instructions
				 */
				element.pobj = NULL;
				ov_memstack_lock();
				if(Ov_Fail(
					ov_path_resolve(&path, NULL, elementpath, VERSION_FOR_CTREE))) {
					/*
					 *	bad path
					 */
					ov_memstack_unlock();
					if(pobj == NULL) {
						Download_log(pinst, OV_MT_ERROR, OV_ERR_GENERIC,
							"couldnt find part %s.%s", parentpath, identifier);
						ov_string_setvalue(&identifier, NULL);
						ov_string_setvalue(&parentpath, NULL);
						ov_string_setvalue(&elementpath, NULL);
						return OV_ERR_BADPARAM;
					}
				}
				element = path.elements[path.size - 1];
				ov_memstack_unlock();
				pobj = element.pobj;
				break;
			default:
				Download_log(pinst, OV_MT_ERROR, OV_ERR_BADPARAM, "bad CTREE type");
				ov_string_setvalue(&identifier, NULL);
				ov_string_setvalue(&parentpath, NULL);
				ov_string_setvalue(&elementpath, NULL);
				return OV_ERR_BADPARAM;
		}

//	4. Set Variables
		cJSON* jsvariables = cJSON_GetObjectItemCaseSensitive(jschild, "variables");
		if(jsvariables != NULL) {
			res = set_variable_values(pinst, jsvariables, pobj);

			if(Ov_OK(res))
			//			Download_log_exit(pinst, OV_MT_INFO, res, "variables set.");
				ov_logfile_info("variables set.");
		}

		res = download_children(pinst, cJSON_GetObjectItem(jschild, CHILDRENNAME),
			Ov_StaticPtrCast(ov_domain, pobj));
		res = download_parts(pinst, cJSON_GetObjectItem(jschild, PARTSNAME),
			Ov_StaticPtrCast(ov_domain, pobj));

		if(Ov_Fail(res)) {
			//TODO: give more info
			ov_string_setvalue(&identifier, NULL);
			ov_string_setvalue(&parentpath, NULL);
			ov_string_setvalue(&elementpath, NULL);
			return Download_log(pinst, OV_MT_ERROR, res,
				"Could not load tree. error at %s", parentpath);
		}
	}
	ov_string_setvalue(&identifier, NULL);
	ov_string_setvalue(&parentpath, NULL);
	ov_string_setvalue(&elementpath, NULL);
	return res;
}

OV_RESULT CTree_Download_execute(OV_INSTPTR_CTree_Download pinst) {
	/*
	 * Init
	 */
	OV_RESULT res = OV_ERR_OK;
	cJSON * jsbase = NULL;
	cJSON * jslibs = pinst->v_cache.jslibs = NULL;
	cJSON * jstree = pinst->v_cache.jstree = NULL;
//	cJSON * jspath = pinst->v_cache.jspath = NULL;
	cJSON * jscurrent = NULL;

//1. parse input
	pinst->v_cache.jsbase = cJSON_Parse(pinst->v_json);
	jsbase = pinst->v_cache.jsbase;
//1.1 check if file is ok
	if(jsbase == NULL) {
		const char *error_ptr = cJSON_GetErrorPtr();
		if(error_ptr != NULL) {
			//			print_log(pinst, ov_logfile_error, "bad js file");
		}
		Download_log(pinst, OV_ERR_BADPARAM, OV_MT_ERROR, "Bad json file");
		return OV_ERR_BADPARAM;

	}
	ov_logfile_info("parsed successfully");

	/*
	 * init path
	 */
	if(!pinst->v_path) {
		pinst->v_path = NULL;
		ov_string_setvalue(&pinst->v_path,
			cJSON_GetObjectItem(jsbase, PATHNAME)->valuestring);
	} else {
		cJSON_DeleteItemFromObjectCaseSensitive(jsbase, PATHNAME);
		cJSON_AddStringToObject(jsbase, PATHNAME, pinst->v_path);
	}

//   2. Load Libraries
//   2.1 jsbase contains libraries?
	jslibs = cJSON_GetObjectItem(jsbase, "Libraries");
	if(jslibs == NULL) {
		ov_logfile_info("No libraries to load");
	}

//  2.2 load
	res = download_libraries(pinst, jslibs);
//  2.3 successfully?
	if(Ov_Fail(res)) {
		if(res == OV_ERR_ALREADYEXISTS) {
			res = OV_ERR_OK;
		} else {
			Download_log(pinst, OV_MT_ERROR, res,
				"Could not load dependent libraries");
			return res;
		}
	}

//	3. CreateObjects Iteratively
	jstree = cJSON_GetObjectItem(jsbase, TREENAME);
	jscurrent = cJSON_GetObjectItem(jstree->child, FACTORYNAME);
	if(jscurrent == NULL) {
		Download_log(pinst, OV_MT_ERROR, OV_ERR_BADPARAM, "malformed root factory");
		return OV_ERR_BADPARAM;
	}
//	OV_INSTPTR_ov_class proot_class = inverse_neutralpath(jscurrent->valuestring);

	OV_STRING root_path = NULL;
	ov_string_setvalue(&root_path,
		cJSON_GetStringValue(cJSON_GetObjectItem(jsbase, PATHNAME)));

//geting obj
	OV_STRING tmp = strrchr(root_path, '/');
	if(strchr(tmp, '.')) tmp = strrchr(root_path, '.');
	char tmpchar = *tmp;

	*(tmp) = 0;
	OV_INSTPTR_ov_object proot = NULL;
	if(strlen(root_path)) {
		proot = ov_path_getobjectpointer(root_path, VERSION_FOR_CTREE);
		free(jstree->child->string);
		jstree->child->string = malloc(ov_string_getlength(tmp + 1) + 1);
		strcpy(jstree->child->string, tmp + 1);
	} else
		proot = (OV_INSTPTR_ov_object) &pdb->root;

	if(proot == NULL) {
//		if(!pinst->v_force) {
		Download_log(pinst, OV_MT_ERROR, res, "root doesnt exist %s", root_path);
		*(tmp) = tmpchar;
		ov_string_setvalue(&root_path, NULL);
		return OV_ERR_GENERIC;
//		} else {
//			OV_PATH resolvedPath;
//			ov_path_resolve(&resolvedPath, NULL, root_path, VERSION_FOR_CTREE);
//			OV_UINT len = 0;
//			OV_STRING_VEC identifiers = ov_string_split(root_path, "/", &len);
//			if(resolvedPath) {
//
//			}
	}
//		if (proot_class != Ov_GetParent(ov_instantiation, proot)) {
//			Download_log(pinst, OV_MT_ERROR, "no object with path %s", root_path);
//			return OV_ERR_ALREADYEXISTS;
//		}

	*(tmp) = tmpchar;

	if(tmpchar == '/')
		res = download_children(pinst, jstree, Ov_StaticPtrCast(ov_domain, proot));
	else if(tmpchar == '.')
		res = download_parts(pinst, jstree, Ov_StaticPtrCast(ov_domain, proot));

	/* beta */
//	download_tree_beta(pinst, jstree, Ov_StaticPtrCast(ov_domain, proot));
//	4. Link
	res = link_objects(pinst, jstree->child, root_path);

// 5. Free
	ov_string_setvalue(&root_path, NULL);
	cJSON_Delete(pinst->v_cache.jsbase);
	ov_string_setvalue(&pinst->v_path, NULL);
	ov_string_setvalue(&pinst->v_json, NULL);
	return res;
}

OV_DLLFNCEXPORT void CTree_Download_typemethod(OV_INSTPTR_fb_functionblock pfb,
		OV_TIME *pltc) {
	/*
	 *   local variables
	 */
	OV_INSTPTR_CTree_Download pinst = Ov_StaticPtrCast(CTree_Download, pfb);

	/*
	 * Init
	 */
	ov_string_setvalue(&pinst->v_ErrorMsg, NULL);
	pinst->v_result = OV_ERR_OK;

	OV_RESULT res = CTree_Download_execute(pinst);
	switch (res) {
		case OV_ERR_OK:
			pinst->v_result = res;
			ov_logfile_info("Download done.");
			break;
		case OV_ERR_BADPARAM:
			pinst->v_result = res;
			ov_logfile_error("Download failed.");
			break;
		default:
			pinst->v_result = OV_ERR_GENERIC;
			ov_logfile_error("Download failed. : %s", ov_result_getresulttext(res));
	}
	return;
}
