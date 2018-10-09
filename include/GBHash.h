/*
 * Copyright (c) 2016 FlyLab
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
//  GBHash.h
//  GroundBase
//
//  Created by Manuel Deneu on 01/09/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
 * \file GBHash.h
 * \brief  Hashing method.
 */
#ifndef GBHash_h
#define GBHash_h

#include <GBTypes.h>

// will return 0 if data is null or len is zero.
GBHashCode GBHashFunction(const char * data, size_t len);

#endif /* GBHash_h */
