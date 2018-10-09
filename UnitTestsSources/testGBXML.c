//
//  testGBXML.c
//  GroundBase
//
//  Created by Manuel Deneu on 23/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <string.h>
#include <stdlib.h>
#include <GBXMLDocument.h>
#include <GBNumber.h>
#include <stdio.h>
#include "testGBXML.h"




const char xmlContent[] =
"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?><catalogue><produit reference=\"GSEA6F\"><intitule>Stylo Developpez.com</intitule><prix>2.00</prix></produit><produit reference=\"MJIZ8F\"><intitule>T-Shirt Developpez.com</intitule><prix>8.00</prix></produit><produit reference=\"IHRC24\"><intitule>Sweat Developpez.com</intitule><prix>10.00</prix></produit></catalogue>";



static void testNode(const GBXMLNode* node, const char* intituleStr, const char* strToTest)
{

    
    GBString* n =  GBStringInitWithCStr("intitule");
    const GBXMLNode* intitule = GBXMLNodeGetChildrenByName(node, n);
    GBRelease(n);
    
    assert(intitule);
    char* c = GBXMLNodeGetContent(intitule);
    assert( strcmp( c , intituleStr) == 0);
    free(c);
    
    n =  GBStringInitWithCStr("prix");
    const GBXMLNode* prix = GBXMLNodeGetChildrenByName(node, n);
    GBRelease(n);
    
    assert(prix);
    c = GBXMLNodeGetContent(prix);
    assert( strcmp( c , strToTest) == 0);
    free(c);
}
void testGBXMLParse()
{
    printf("--------Test GBXML Parse --------\n");
    
    GBXMLDocument* doc = GBXMLDocumentInitWithBuffer( xmlContent , strlen(xmlContent));
    assert(doc);
    
    const GBXMLNode* root =  GBXMLDocumentGetRootNode(doc);
    assert(root);
    
    const GBString* rootName = GBXMLNodeGetName(root);
    assert(rootName );
    assert(GBStringEqualsCStr(rootName, "catalogue"));
    assert(GBXMLNodeGetNumChildren( root ) == 3);
    
    //
    
    const GBXMLNode* child1 = GBXMLNodeGetFirstChildren( root );
    assert(child1);
    
    const GBString* child1Name = GBXMLNodeGetName(child1);
    assert(child1Name );
    assert(GBStringEqualsCStr(child1Name, "produit"));
    assert(GBXMLNodeHasProperty(child1, "reference"));
    
    char* prop = GBXMLNodeGetProperty(child1, "reference");
    assert(strcmp( prop , "GSEA6F") == 0);
    free(prop);
    
    testNode(child1,"Stylo Developpez.com", "2.00");

    //
    
    const GBXMLNode* child2 = GBXMLNodeGetNext(child1);
    assert(child2);
    
    const GBString* child2Name = GBXMLNodeGetName(child2);
    assert(child2Name );
    assert(GBStringEqualsCStr(child2Name, "produit"));
    assert(GBXMLNodeHasProperty(child2, "reference"));
    
    prop = GBXMLNodeGetProperty(child2, "reference");
    assert(strcmp( prop , "MJIZ8F") == 0);
    free(prop);
    testNode(child2, "T-Shirt Developpez.com","8.00");
   
    //
    
    const GBXMLNode* child3 = GBXMLNodeGetNext(child2);
    assert(child3);
    
    const GBString* child3Name = GBXMLNodeGetName( child3 );
    assert(child3Name );
    assert(GBStringEqualsCStr(child3Name, "produit"));
    assert(GBXMLNodeHasProperty(child3, "reference"));
    
    prop = GBXMLNodeGetProperty(child3, "reference");
    assert(strcmp( prop , "IHRC24") == 0);
    free(prop);
    
    testNode(child3,"Sweat Developpez.com", "10.00");

    GBRelease(doc);
        
}



void testGBXMLSave()
{
    printf("--------Test GBXML Save --------\n");
    
    GBXMLDocument* doc = GBXMLDocumentInit();
    assert(doc);
    GBXMLNode* root = GBXMLDocumentCreateRootNode(doc, "root");
    
    assert(root);
    
    assert( GBXMLNodeAddProperty(GBXMLNodeAddNewChild(root, "test-node" , NULL), "Prop-name","Prop-val" ) );
    
    
    const GBString* fileName = GBStringInitWithCStr("file.xml");
    assert(GBXMLDocumentSaveCopy(doc, fileName));
    GBRelease(fileName);
    GBRelease(doc);
    
    
}

void testGBXMLLoad()
{
    printf("--------Test GBXML Load --------\n");
    
    const GBString* fileName = GBStringInitWithCStr("file.xml");
    GBXMLDocument* doc =GBXMLDocumentInitWithFile(fileName);
    GBRelease(fileName);
    
    assert(doc);
    
    const GBXMLNode* root = GBXMLDocumentGetRootNode(doc);
    assert( root );
    const GBXMLNode* testNode = GBXMLNodeGetFirstChildren(root);
    assert( testNode );
    
    char* prop = GBXMLNodeGetProperty(testNode, "Prop-name");
    assert( prop);
    assert(strcmp(prop, "Prop-val") == 0);
    free(prop);
    GBRelease(doc);
}



