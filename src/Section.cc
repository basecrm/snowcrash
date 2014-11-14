//
//  Section.cc
//  snowcrash
//
//  Created by Zdenek Nemec on 5/14/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#include "Section.h"

using namespace snowcrash;

std::string snowcrash::SectionName(const SectionType& section) {

    switch (section) {
        case ModelSectionType:
        case ModelBodySectionType:
            return "model";

        case RequestSectionType:
        case RequestBodySectionType:
            return "request";

        case ResponseSectionType:
        case ResponseBodySectionType:
            return "response";

        case BodySectionType:
        case DanglingBodySectionType:
            return "message-body";

        case SchemaSectionType:
        case DanglingSchemaSectionType:
            return "message-schema";

        case HeadersSectionType:
            return "headers";

        default:
            return "section";
    }
}

#define ENUM2STRING(ENUM) case ENUM: return #ENUM;

std::string SectionTypeToString(SectionType type) {

    switch (type) {

        ENUM2STRING(UndefinedSectionType)     
        ENUM2STRING(BlueprintSectionType)     
        ENUM2STRING(ResourceGroupSectionType) 
        ENUM2STRING(ResourceSectionType)      
        ENUM2STRING(ActionSectionType)        
        ENUM2STRING(RequestSectionType)       
        ENUM2STRING(RequestBodySectionType)   
        ENUM2STRING(ResponseSectionType)      
        ENUM2STRING(ResponseBodySectionType)  
        ENUM2STRING(ModelSectionType)         
        ENUM2STRING(ModelBodySectionType)     
        ENUM2STRING(BodySectionType)          
        ENUM2STRING(DanglingBodySectionType)  
        ENUM2STRING(SchemaSectionType)        
        ENUM2STRING(DanglingSchemaSectionType)
        ENUM2STRING(HeadersSectionType)       
        ENUM2STRING(ForeignSectionType)       
        ENUM2STRING(ParametersSectionType)
        ENUM2STRING(AttributesSectionType)
        ENUM2STRING(ParameterSectionType)     
        ENUM2STRING(ValuesSectionType)        
        ENUM2STRING(ValueSectionType)
    }

    return "<UNKNOWN>";

}

#undef ENUM2STRING 
