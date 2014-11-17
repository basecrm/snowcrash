//
//  test-DataStructuresParser.cc
//  snowcrash
//
//  Created by Boguslaw Mista on 17/11/14.
//  Copyright (c) 2014 Base. All rights reserved.
//

#include "snowcrashtest.h"
#include "DataStructuresParser.h"

using namespace snowcrash;
using namespace snowcrashtest;

const mdp::ByteBuffer DataStructuresFixture = \
"# Data Structures\n\n"\
"Data Structures Description\n\n"\
"## Structure Name\n\n"\
"Structure Description\n\n"\
"+ Members\n"\
"    + id (required, number, `42`) ... Resource Id\n"\
"\n"
"+ Sample\n\n"\
"   Hello World\n";

TEST_CASE("Recognize Data Structures section block", "[data_structures]")
{
    mdp::ByteBuffer source = \
    "# Data Structures\n\n"\
    "Description\n";
    
    mdp::MarkdownParser markdownParser;
    mdp::MarkdownNode markdownAST;
    markdownParser.parse(source, markdownAST);
    
    REQUIRE(!markdownAST.children().empty());
    SectionType sectionType = SectionProcessor<DataStructures>::sectionType(markdownAST.children().begin());
    REQUIRE(sectionType == DataStructuresSectionType);
}
 
TEST_CASE("Parse canonical data structures", "[data_structures]")
{
    ParseResult<DataStructures> ds;
    SectionParserHelper<DataStructures, DataStructuresParser>::parse(DataStructuresFixture, DataStructuresSectionType, ds, ExportSourcemapOption);
    
    REQUIRE(ds.report.error.code == Error::OK);
    
    REQUIRE(ds.node.description == "Data Structures Description\n\n");
    REQUIRE(ds.node.dataStructures.size() == 1);

    REQUIRE(ds.node.dataStructures[0].name == "Name");
    REQUIRE(ds.node.dataStructures[0].description == "Structure Description\n\n");

    REQUIRE(ds.node.dataStructures[0].members.size() == 1);
    REQUIRE(ds.node.dataStructures[0].members[0].name == "id");
    REQUIRE(ds.node.dataStructures[0].members[0].description == "Resource Id");
    REQUIRE(ds.node.dataStructures[0].members[0].type == "number");
    REQUIRE(ds.node.dataStructures[0].members[0].defaultValue.empty());
    REQUIRE(ds.node.dataStructures[0].members[0].exampleValue == "42");
    REQUIRE(ds.node.dataStructures[0].members[0].values.empty());

    REQUIRE(ds.node.dataStructures[0].sample.name.empty());
    REQUIRE(ds.node.dataStructures[0].sample.description.empty());
    REQUIRE(ds.node.dataStructures[0].sample.parameters.empty());
    REQUIRE(ds.node.dataStructures[0].sample.headers.empty());
    REQUIRE(ds.node.dataStructures[0].sample.body == "Hello World\n");
    REQUIRE(ds.node.dataStructures[0].sample.schema.empty());
    
    REQUIRE(ds.sourceMap.description.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.description.sourceMap[0].location == 19);
    REQUIRE(ds.sourceMap.description.sourceMap[0].length == 29);

    REQUIRE(ds.sourceMap.dataStructures.collection.size() == 1);
    
    REQUIRE(ds.sourceMap.dataStructures.collection[0].name.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.dataStructures.collection[0].name.sourceMap[0].location == 48);
    REQUIRE(ds.sourceMap.dataStructures.collection[0].name.sourceMap[0].length == 19);
    REQUIRE(ds.sourceMap.dataStructures.collection[0].description.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.dataStructures.collection[0].description.sourceMap[0].location == 67);
    REQUIRE(ds.sourceMap.dataStructures.collection[0].description.sourceMap[0].length == 23);

    REQUIRE(ds.sourceMap.dataStructures.collection[0].members.collection.size() == 1);
    REQUIRE(ds.sourceMap.dataStructures.collection[0].members.collection[0].name.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.dataStructures.collection[0].members.collection[0].name.sourceMap[0].location == 106);
    REQUIRE(ds.sourceMap.dataStructures.collection[0].members.collection[0].name.sourceMap[0].length == 44);

    REQUIRE(ds.sourceMap.dataStructures.collection[0].sample.name.sourceMap.empty());
    REQUIRE(ds.sourceMap.dataStructures.collection[0].sample.description.sourceMap.empty());
    REQUIRE(ds.sourceMap.dataStructures.collection[0].sample.parameters.collection.empty());
    REQUIRE(ds.sourceMap.dataStructures.collection[0].sample.headers.collection.empty());
    REQUIRE(ds.sourceMap.dataStructures.collection[0].sample.body.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.dataStructures.collection[0].sample.body.sourceMap[0].location == 164);
    REQUIRE(ds.sourceMap.dataStructures.collection[0].sample.body.sourceMap[0].length == 12);
}