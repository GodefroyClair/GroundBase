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



#include <GroundBase.h>

#include "../src/UPC/GBUPC_Private.h"

int main(int argc, const char * argv[])
{
    
	(void) argc;
	(void) argv;

    GBRuntimeEnableInvalidReleaseDebug(1);
    
    assert( GBUPCMessageHeaderSize == offsetof(GBUPCMessage, data));
    
    
    
    
	const GBSize ret = GBObjectIntrospection(1);

    //sleep( 4 ); // for instruments
	
    const GBSize total = GBObjectGetObjectsCount();
    
    assert( ret == total  );
    assert(ret == 0);


	return 0;
}
