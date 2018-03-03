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

#include "CTree.h"
#include "libov/ov_macros.h"
#include "libov/ov_string.h"
#include "libov/ov_class.h"
#include "libov/ov_library.h"
#include "libov/ov_path.h"

#include "cJSON.h"

#include "libov/ov_debug.h"

#define VERSION_FOR_CTREE 	2

OV_RESULT upload_tree(cJSON* jsparent, OV_INSTPTR_ov_domain pparent) {

	OV_RESULT res = OV_ERR_OK;

	OV_INSTPTR_ov_class pclass = NULL;
	OV_INSTPTR_ov_domain pobj = NULL;
	OV_INSTPTR_ov_object pclassobj = NULL;
	OV_STRING identifier = NULL;

	cJSON* jschild = NULL;
	cJSON* jscurrent = NULL;

	cJSON_ArrayForEach(jschild, jsparent)
	{
		ov_string_setvalue(&identifier, jschild->string);
		if(!identifier){
			return OV_ERR_BADPARAM;
		}
		jscurrent = cJSON_GetObjectItemCaseSensitive(jschild, "factory");
		OV_STRING factory = cJSON_GetStringValue(jscurrent);

		pclassobj = ov_path_getobjectpointer(factory, VERSION_FOR_CTREE);
		pclass = Ov_StaticPtrCast(ov_class, pclassobj);
//		OV_UINT len = 0;
//		OV_STRING* fact = ov_string_split(factory, "/", &len);
//		OV_STRING class = NULL;
//		ov_string_setvalue(&class, fact[len-2]);
//		ov_string_append(class, "_");
//		ov_string_append(class, fact[len-1]);

		res = ov_class_createobject(pclass, ((OV_INSTPTR_ov_domain)((pparent)+(0*((pparent)->__classinfo.					\
			is_of_class_ov_domain)))),	\
			identifier, ((OV_PLACEMENT_HINT)0), ((void *)0), ((void *)0), ((void *)0), ((OV_INSTPTR_ov_object*)&(pobj)));
		if(Ov_Fail(res)){
			ov_logfile_error("couldnt create %s in %s", identifier, pparent->v_identifier);
			return res;
		}
		else{
			ov_logfile_info("%s created in %s", identifier, pparent->v_identifier);
		}

		res = upload_tree(cJSON_GetObjectItem(jschild, "children"), pobj);
		if (Ov_Fail(res)) {
			ov_logfile_error("couldnt load tree. error at %s", identifier);
			return res;
		}
	}
	return res;
}

OV_RESULT upload_libraries(const cJSON* jslibs) {
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
			ov_logfile_info("lib %s loaded\n", libname);
			break;
		case OV_ERR_ALREADYEXISTS:
			ov_logfile_info("lib %s already exists\n", libname);
			break;
		default:
			ov_logfile_error("couldnt load lib %s\n", libname);
			return res;
		}
	}
	return res;
}

OV_DLLFNCEXPORT void CTree_Upload_typemethod(OV_INSTPTR_fb_functionblock pfb,
		OV_TIME *pltc) {
	/*
	 *   local variables
	 */
	OV_INSTPTR_CTree_Upload pinst = Ov_StaticPtrCast(CTree_Upload, pfb);

//    ov_string_setvalue(&pinst->v_json, "{\"Path\":\"/TechUnits\", \"Tree\":{\"A\":{\"factory\":\"/acplt/CTree/Upload\",\"variables\":{},\"children\":{\"AA\":{\"factory\":\"/acplt/CTree/Upload\",\"variables\":{},\"children\":{}}}}},\"Links\":{},\"Libraries\":[\"CTree\",\"ksapi\"]}");

	OV_RESULT res;

	//1. parse input
	cJSON * jsbase = NULL;
	cJSON * jslibs = NULL;
	cJSON * jstree = NULL;
	cJSON * jspath = NULL;
	cJSON * current = NULL;

	jsbase = cJSON_Parse(pinst->v_json);
	//1.1 check if file is ok
	if (jsbase == NULL) {
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL) {
			ov_logfile_error("bad js file");
		}
    	pinst->v_return = OV_ERR_BADPARAM;
		return;
	}
	ov_logfile_info("parsed successfully");

//   2. Load Libraries
//   2.1 jsbase contains libraries?
	jslibs = cJSON_GetObjectItem(jsbase, "Libraries");
	if (jslibs == NULL) {
		ov_logfile_info("no libraries to load");
	}
//  2.2 load
	res = upload_libraries(jslibs);
//  2.3 successfully?
	if (Ov_Fail(res))
		if (res!=OV_ERR_ALREADYEXISTS) {
			pinst->v_return = res;
			return;
		}

//	3. CreateObjects Iterativly
	jstree = cJSON_GetObjectItem(jsbase, "Tree");

	OV_STRING rootpath = NULL;
	ov_string_setvalue(&rootpath, cJSON_GetStringValue(cJSON_GetObjectItem(jsbase, "Path")));
	OV_INSTPTR_ov_object proot = ov_path_getobjectpointer(rootpath, VERSION_FOR_CTREE);
	upload_tree(jstree, Ov_StaticPtrCast(ov_domain, proot));

	// Free memory
	ov_string_setvalue(&pinst->v_json, "");
	cJSON_Delete(jsbase);
	return;
}

