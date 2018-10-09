/*
 * Copyright (c) 2016 FlyLab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
//
//  main.c
//  TestGroundBase
//
//  Created by Manuel Deneu on 14/07/2016.
//

#include <stdio.h>
#include <unistd.h> // read
#include <assert.h>
#include <string.h> // strcmp

#include <GroundBase.h>

#include "testClone.h"
#include "testGBObject.h"
#include "TestGBString.h"
#include "testGBDictionary.h"
#include "testGBNumber.h"
#include "testGBJSON.h"
#include "testGBSet.h"
#include "testGBXML.h"

#include "testGBRunLoop.h"
#include "testGBFDSource.h"

#include "testGBTimer.h"
#include "testGBTimerV2.h"

//#include "testGBUPCService.h"
//#include "testGBUPCClient.h"
#include "testUPCBase.h"
#include "testGBSocket.h"

#include "testGBBinCoder.h"
#include "testThread.h"

#include "testGBPropertyList.h"

#include "testAsyncCall.h"
#include "testGBSharedMem.h"
#include "testSourceAndDispatch.h"

#include "testGBNumber.h"

int main()
{
    printf("GroundBase Test suite\n");
    printf("\n");
    printf("Version %i.%i.%i\n" , GROUNDBASE_VERSION_MAJ , GROUNDBASE_VERSION_MIN , GROUNDBASE_VERSION_PATCH);
    printf("\n");
    
#ifdef DEBUG
    printf("Runs in DEBUG mode\n");
#else
    printf("Runs in RELEASE mode\n");
#endif
    
    printf("\n");
	

    GBRuntimeEnableInvalidReleaseDebug(1);
    
    //Passive tests

    testGBObject();
    testGBObject2();
    
    testGBObjectInternals();
    testGBObjectOwnership();

    testGBStringStatic();
    
    testGBString();
    
    testGBString2();
    testGBString3();
 
    testGBString4();

    
    testGBNumber();
    
    testGBDictionary();
    testGBDictionary2();
    testGBSet();
    testGBArray();
    testGBList();
    
    
    testGBJSONParse();
    testGBJSONSave();
    
    testGBXMLParse();
    
    testGBXMLSave();
    testGBXMLLoad();
    
    testClone();
    
    testGBPropertyWrite();
    
    testGBPropertyList();
    
    testGBSocket();
    
    testGBRunLoop();
    testGBFDSource();

    testGBBinCoder();
    testGBBinCoder2();
    testGBBinCoder3();
    testGBBinCoder4();
    testGBBinCoder5();
    testGBBinCoder6();
    

    testUPCClient();
    testUPCService();
    

    testAsyncCall();

    testAsyncCall2();

 
    testGBSharedMem(); // will fork

    testGBFDSource2();
    
    testSourceAndDispatch();
     
    
    
    // time consumming tests
    
    testGBTimerV2();
    
    testGBTimer();
    testGBTimer2();
    testGBTimer3();
    testGBTimerImpl();
    
    testThread();
 
    testThread2(); // NOT WORKING
    
    //testGBUPCClient();
    //testGBUPCService();



	const GBSize ret = GBObjectIntrospection(1);
#ifndef DEBUG
    sleep( 4 ); // for instruments
#endif
    const GBSize total = GBObjectGetObjectsCount();
    
    assert( ret == total  );
    assert(ret == 0);


	return 0;
}
