
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
#include "libov/ov_library.h"


#include "cJSON.h"

#include "libov/ov_debug.h"

OV_RESULT upload_tree(cJSON* jsparent, OV_INSTPTR_ov_object pparent){
    OV_RESULT res;

	OV_INSTPTR_ov_class  pclass = NULL;
    OV_INSTPTR_CTree_Upload pobj = NULL;
	OV_STRING identifier = NULL;

	cJSON* child = NULL;
	cJSON* current = NULL;

	cJSON_ArrayForEach(child, jsparent)
	{
		identifier = child->string;

		current = cJSON_GetObjectItemCaseSensitive(child, "factory");
		OV_STRING factory = cJSON_GetStringValue(current);

		pclass = ov_class_search(factory);

//		Ov_CreateObject(CTree_Upload, pobj, pparent, identifier);

		res = upload_tree(cJSON_GetObjectItem(child, "children"), pobj);
		if (!res){
			Ov_Error("err");
			return res;
		}
	}
	return res;
}

OV_RESULT upload_libraries(const cJSON* libs)
{
		cJSON* current=NULL;
		OV_INSTPTR_ov_library plib = NULL;
		OV_RESULT res = 0;

		cJSON_ArrayForEach(current, libs)
	    {
	    	if(!cJSON_IsString(current)){
	    		//!!! can libname be anything else than string?
//	    		ov_string_append(&pinst->v_status, "bad lib name\n");
	    		return OV_ERR_BADPARAM;
	    	}
	    	OV_STRING libname = cJSON_GetStringValue(current);
	    	plib = ov_library_search(libname);
	    	res = Ov_CreateObject(ov_library, plib, &(pdb->acplt), libname);

	    	switch(res){
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

	//	2.3 check if loaded successfully?
//	    	ov_string_append(&pinst->v_status, lib);
	    }
		cJSON_free(current);
		return res;
}

OV_DLLFNCEXPORT void CTree_Upload_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_CTree_Upload pinst = Ov_StaticPtrCast(CTree_Upload, pfb);

    Ov_Info("test info");

//    ov_string_setvalue(&pinst->v_json, "{\"Path\":\"/TechUnits\", \"Tree\":{\"A\":{\"factory\":\"/acplt/CTree/Upload\",\"variables\":{},\"children\":{\"AA\":{\"factory\":\"/acplt/CTree/Upload\",\"variables\":{},\"children\":{}}}}},\"Links\":{},\"Libraries\":[\"CTree\",\"ksapi\"]}");
//

    OV_RESULT res;

    cJSON * libs = NULL;
    cJSON * tree = NULL;
    cJSON * path = NULL;
    cJSON * current = NULL;


    //1. parse input
    cJSON *file = cJSON_Parse(pinst->v_json);
    //1.1 check if file is ok
    if (file==NULL){
    	const char *error_ptr = cJSON_GetErrorPtr();
    	if(error_ptr!=NULL){
//    		Ov_Info((char *)error_ptr);
    		ov_string_append(&pinst->v_status, "1. bad param");
    	}
//    	pinst->v_return
		return;
    }
   	ov_string_append(&pinst->v_status, "1. parsed successfully\n");

//   2. Load Libraries
//   2.1 file contains libraries?
    libs = cJSON_GetObjectItem(file, "Libraries");
    if(libs==NULL){

    }
//    2.2 load
    res = upload_libraries(libs);
//    2.3 successfully?
    if(!res)
    {
    	pinst->v_return = res;
    	return;
    }

//	3. CreateObjects Iterativly
   	tree = cJSON_GetObjectItem(file, "Tree");
   	current = tree->child;

   	OV_STRING rootpath = cJSON_GetObjectItem(file, "Path");
   	OV_INSTPTR_ov_object proot = ov_path_getobjectpointer(rootpath, 2);
   	upload_tree(current, proot);

    // Free memory

    cJSON_Delete(current);
    cJSON_Delete(libs);
    cJSON_Delete(tree);
    cJSON_Delete(file);
    return;
}

