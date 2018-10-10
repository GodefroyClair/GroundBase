/*
 * Copyright (c) 2016-2018 FlyLab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
//
//  GroundBase.h
//  GroundBase
//
//  Created by Manuel Deneu on 31/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
 * \file GroundBase.h
 */

/*! \mainpage GroundBase library
 *
 * \section intro_sec Introduction
 *
 * GroundBase is an implementation library code for FlyKit project.
 * 
 *
 * \section C-API C API guidelines
 *
 * - 'Object oriented' API. GBObject methods all begin with 'GBType'DoSomething (for example GBStringIsEmpty, or GBRunLoopRun) and their first parameter is
 always a 'GBType' instance (for example GBString* myStr = ... ; if( GBStringIsEmpty(myStr)) ... ).
 * - GBObjects are allocated on the head, and you can manage their lifecycle using GBRetain/GBRelease calls. Every created Objects instances using a method with
 'Create' or 'Init' in its name must be released, or memory leaks will occur.
 *
 * Check GBObject File Reference for more informations about GroundBase's runtime system.
 *
 * \section CPP-API C++ API guidelines
 * The GroundBase C++ API is a header only implementation wrapper built around C API. Its main purposes are to provide bridging functions with the STL, and provide (almost)
 automatic GBRetain/GBrelease call cycles.
 * See GroundBase.hpp for more informations.
 *
 */

#ifndef GroundBase_h
#define GroundBase_h

// current ver 5.5.3
#define GROUNDBASE_VERSION_MAJ   (int) 5
#define GROUNDBASE_VERSION_MIN   (int) 5
#define GROUNDBASE_VERSION_PATCH (int) 3


#include <GBObject.h>
#include <GBTypes.h>



#endif /* GroundBase_h */
