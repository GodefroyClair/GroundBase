# **GroundBase**

The GroundBase library defines a base layer of functionality that is required for almost all applications. 
 It provides primitive classes, introduces several paradigms and is designed with these goals in mind:
 
 - Provide a small set of basic utility classes.
 - Make software development easier by introducing consistent conventions.
 - Provide a level of OS independence, to enhance portability.

# **API**
GroundBase Core is written in C, and an API is also available in C++.

# **GroundBase projects**
- libGroundBase : Library 
- SystemTests : a simple program to test if all assumptions made in the library about the underlying OS are correct.
- UnitTests : Unit tests, and a few integration tests that will be moved in an other separate project.
- IntegrationTests : Todo :)

# **Build & deploy**
- `make` + `make install` + `make test` should work. See DEPENDENCIES about missing stuffs.
- To install c++ headers : `make testCpp` + `make installCPP`

# **Run Tests**
- `./UnitTests` and `./UnitTestsCPP`

# **Generate C Documentation**
- doxygen doxyConfiguration
- Documentation will be generated inside 'Documentation' folder.


# **GroundBase classes and Modules**


## **GBObject**
Base class for most GroundBase's classes. Defines various  methods such as a constructor, retain/release operations, hash, etc.

## **GBString**
GroundBase's representation of a string. 

## **GBNumber**
Wrapper object for numeric values.

## **GBSet**
Ordered Set.

## **GBArray**
Ordered Array.

## **GBThread**
Mostly a wrapper around pthread mecanisms.

## **GBDictionary**
Hashmap associating a _key_ (GBString) and a _value_ (any GBObject).

## **GBXMLDocument**
A class for reading/writting Xml formatted content.

## **GBJSON**
a set of functions to load/save datas using Json format.

## **GBRunLoop**
A complete mecanism, built to perform async actions, notifications, etc. Many GroundBase classes are constructed and designed to use a runloop (Sockets , Timers , etc.)

## **GBTimer**
A timer class, to use on a GBRunLoop instance.

## **GBSocket**
A networking class, to use on a GBRunLoop instance.

## **GBSharedMem**
A class designed for reading/writting into shared memory.

## **GBUPCService - GBUPCClient**
These classes provide a lightweight mechanism for both interprocess and distant communication.  

## **GBProcessLock**
Acquires a system lock preventing other application instances from running simultaneously. 


#### Version history
- Creation : 14/07/2016
- Update for ver 5.4.1 : 17/01/2018
- Update for ver 5.4.9 : 10/03/2018


