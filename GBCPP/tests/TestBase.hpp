//
//  TestBase.hpp
//  GroundBaseCPP
//
//  Created by Manuel Deneu on 03/11/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#ifndef TestBase_hpp
#define TestBase_hpp


// TestRuntime.cpp
bool testRuntime();

// TestBase.cpp

bool testString();
bool testDictionary();
bool testArray();
bool testLambdaString();

// TestRunLoop.cpp
bool testRunLoop();

bool testFDSource();

bool testJSON();


// TestVariant.cpp
bool testVariant();

bool testSharedMemory();

bool testPropertyList();

bool testUPCClient();
bool testUPCService();

bool testGBThread();

#endif /* TestBase_hpp */
