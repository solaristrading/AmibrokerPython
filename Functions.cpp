////////////////////////////////////////////////////
// Functions.cpp
// Sample functions implementation file for example AmiBroker plug-in
//
// Copyright (C)2001 Tomasz Janeczko, amibroker.com
// All rights reserved.
//
// Last modified: 2001-09-24 TJ
// 
// You may use this code in your own projects provided that:
//
// 1. You are registered user of AmiBroker
// 2. The software you write using it is for personal, noncommercial use only
//
// For commercial use you have to obtain a separate license from Amibroker.com
//
////////////////////////////////////////////////////

#include "stdafx.h"
#include "Plugin.h"
#include <Python.h>

// Helper function

int SkipEmptyValues( int nSize, float *Src, float *Dst )
{
	int i;

	for( i = 0; i < nSize && IS_EMPTY( Src[ i ] ); i++ )
	{
		Dst[ i ] = EMPTY_VAL;
	}

	return i;
}


///////////////////////////////////////////
// Each AFL function has the following prototype:
//
// AmiVar FunctionName( int NumArgs, AmiVar *ArgsTable )
//
// You can define as many functions as you want.
// To make them visible you should add them to the function
// table at the bottom of this file
//
///////////////////////////////////////////////


// First Test
AmiVar VPythonHelloWorld( int NumArgs, AmiVar *ArgsTable )
{
	PyRun_SimpleString("open(r'c:\\trade\\hello_from_python.txt', 'w').write('Hello!')");
	return gSite.AllocArrayResult();
}

// Second Test
AmiVar VPythonRunPyFile( int NumArgs, AmiVar *ArgsTable )
{
	char* filename = ArgsTable[0].string;
	FILE* fp = _Py_fopen(filename, "r");
	long rc = -1;
	if(fp != NULL)
	{
		rc = PyRun_SimpleFile(fp, "r");
		if(rc == -1)
		{
			if(PyErr_Occurred())
			{
				PyObject *pyObStr, *ptype, *pvalue, *ptraceback;
				PyErr_Fetch(&ptype, &pvalue, &ptraceback);
				pyObStr = PyObject_Str(pvalue);
				PyObject* pyStr = PyUnicode_AsEncodedString(pyObStr, "utf-8", "Error ~");
				const char *strExcType = PyBytes_AS_STRING(pyStr);

				AmiVar result;
				result.type = VAR_STRING;
				result.string = (char *)gSite.Alloc(strlen(strExcType)+1);
				result.string = strdup(strExcType);

				Py_XDECREF(pyObStr);
				Py_XDECREF(pyStr);
				Py_XDECREF(ptype);
				Py_XDECREF(pvalue);
				Py_XDECREF(ptraceback);

				PyErr_Clear();
			
				return result;
			}
		}
	}
	
	AmiVar result;
	result.val = rc;
	result.type = VAR_FLOAT;
	//AmiVar result;
	//result.type = VAR_STRING;
	//result.string = (char *)gSite.Alloc(2+1);
	//result.string = strdup("Ok");
	
	return result;
}




/////////////////////////////////////////////
// Function table now follows
//
// You have to specify each function that should be
// visible for AmiBroker.
// Each entry of the table must contain:
// "Function name", { FunctionPtr, <no. of array args>, <no. of string args>, <no. of float args>, <no. of default args>, <pointer to default values table float *>

FunctionTag gFunctionTable[] = {
								"PythonHelloWorld",	{ VPythonHelloWorld, 0, 0, 0, 0, NULL }, 
								"PythonRunPyFile",	{ VPythonRunPyFile, 0, 1, 0, 0, NULL }
};

int gFunctionTableSize = sizeof( gFunctionTable )/sizeof( FunctionTag );
