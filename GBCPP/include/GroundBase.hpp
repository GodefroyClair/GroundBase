/*
 * Copyright (c) 2017 FlyLab
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

#ifndef GroundBase_hpp
#define GroundBase_hpp

/**
 * \file GroundBase.hpp
 * \brief GroundBase C++ API is a header only library built atop C GroundBase.
 * 
 * GroundBase C++ API is a header only library built atop C GroundBase.
 * It's sole purpose is to (mostly) get rid of all these Retain/Release calls, and also to profide closures/lambda for all the callbacks mechanims.
 *
 * General naming rules:
 *
 * - A global GB:: namespace is placed around every Classes/Methods. So for example the GBRunLoop C object becomes GB::RunLoop, GBObject becomes GB::Object, etc.
 * - A new GB::Variant class is added to act as a `cluster class` around GBString/GBNumber/GBDictionary/GBArrays/ etc.
 * - Most methods are guarranted not to throw any exceptions. A notable exception to these are methods that will later call user defined callbacks. For Example GB::RunLoop's run method is not exception free guarranted since it may later invoke sources or timers callbacks that could throw an exception. Be careful!
 */

/*!
 * @discussion Main Namespace for C++ GroundBase API.
 */
namespace GB {}





#endif /* GroundBase_hpp */
