//
//  TestBase.cpp
//  GroundBaseCPP
//
//  Created by Manuel Deneu on 03/11/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "TestBase.hpp"


#include <unistd.h>
#include <iostream>
#include <GroundBase.hpp>
#include <GBObject.hpp>
#include <GBString.hpp>
#include <GBDictionary.hpp>
#include <GBArray.hpp>
#include <GBRunLoop.hpp>
#include <GBTimer.hpp>




bool testRuntime()
{
    
    GB::Timer timer(nullptr , 0);
    assert(IsKindOfClass(timer.getGBRef(), GBTimerClass) );
    assert(timer.getClass() == GBTimerClass);
    assert(timer.isKindOf(GBTimerClass) );
    
    
    GB::String str;
    
    assert(IsKindOfClass(str.getGBRef(), GBStringClass) );
    assert(str.getClass() == GBStringClass);
    assert(str.isKindOf(GBStringClass) );
    
    return true;
}
