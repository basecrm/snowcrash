//
//  test-DataStructureParser.cc
//  snowcrash
//
//  Created by Boguslaw Mista on 16/11/14.
//  Copyright (c) 2014 Base. All rights reserved.
//

#include "snowcrashtest.h"
#include "DataStructureParser.h"

using namespace snowcrash;
using namespace snowcrashtest;

const mdp::ByteBuffer DataStructureFixture = \
"# Structure Name\n\n"\
"Structure Description\n\n"\
"+ Members\n"\
"    + id (required, number, `42`) ... Resource Id\n"\
"\n"
"+ Sample\n\n"\
"   Hello World\n";

TEST_CASE("Recognize Data Structure section block with keyword", "[datastructure]")
{
    mdp::ByteBuffer source = \
    "# Structure Name\n\n"\
    "Description\n";
    
    mdp::MarkdownParser markdownParser;
    mdp::MarkdownNode markdownAST;
    markdownParser.parse(source, markdownAST);
    
    REQUIRE(!markdownAST.children().empty());
    SectionType sectionType = SectionProcessor<DataStructure>::sectionType(markdownAST.children().begin());
    REQUIRE(sectionType == DataStructureSectionType);
}

TEST_CASE("Parse data structure with members", "[datastructure]")
{
    mdp::ByteBuffer source = \
    "# Structure Name\n"\
    "+ Members\n"\
    "    + id (required, number, `42`) ... Resource Id\n"\
    "\n";
    
    ParseResult<DataStructure> ds;
    SectionParserHelper<DataStructure, DataStructureParser>::parse(source, DataStructureSectionType, ds, ExportSourcemapOption);
    
    REQUIRE(ds.report.error.code == Error::OK);
    REQUIRE(ds.report.warnings.empty());

    REQUIRE(ds.node.name == "Name");
    REQUIRE(ds.node.description.empty());
    
    REQUIRE(ds.node.members.size() == 1);
    REQUIRE(ds.node.members[0].name == "id");
    REQUIRE(ds.node.members[0].description == "Resource Id");
    REQUIRE(ds.node.members[0].type == "number");
    REQUIRE(ds.node.members[0].defaultValue.empty());
    REQUIRE(ds.node.members[0].exampleValue == "42");
    REQUIRE(ds.node.members[0].values.empty());
    
    REQUIRE(ds.sourceMap.name.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.name.sourceMap[0].location == 0);
    REQUIRE(ds.sourceMap.name.sourceMap[0].length == 17);
    REQUIRE(ds.sourceMap.description.sourceMap.empty());

    REQUIRE(ds.sourceMap.members.collection.size() == 1);
    REQUIRE(ds.sourceMap.members.collection[0].name.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.members.collection[0].name.sourceMap[0].location == 33);
    REQUIRE(ds.sourceMap.members.collection[0].name.sourceMap[0].length == 44);
}

TEST_CASE("Parse data structure with sample", "[datastructure]")
{
    mdp::ByteBuffer source = \
    "# Structure Name\n"\
    "+ Sample\n\n"\
    "   Hello World\n";
    
    ParseResult<DataStructure> ds;
    SectionParserHelper<DataStructure, DataStructureParser>::parse(source, DataStructureSectionType, ds, ExportSourcemapOption);
    
    REQUIRE(ds.report.error.code == Error::OK);
    //CHECK(ds.report.warnings.empty());
    
    REQUIRE(ds.node.name == "Name");
    REQUIRE(ds.node.description.empty());
    
    REQUIRE(ds.node.members.empty());
    
    REQUIRE(ds.node.sample.name.empty());
    REQUIRE(ds.node.sample.description.empty());
    REQUIRE(ds.node.sample.parameters.empty());
    REQUIRE(ds.node.sample.headers.empty());
    REQUIRE(ds.node.sample.body == "Hello World\n");
    REQUIRE(ds.node.sample.schema.empty());
    
    REQUIRE(ds.sourceMap.name.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.name.sourceMap[0].location == 0);
    REQUIRE(ds.sourceMap.name.sourceMap[0].length == 17);
    REQUIRE(ds.sourceMap.description.sourceMap.empty());
    
    REQUIRE(ds.sourceMap.members.collection.empty());

    REQUIRE(ds.sourceMap.sample.name.sourceMap.empty());
    REQUIRE(ds.sourceMap.sample.description.sourceMap.empty());
    REQUIRE(ds.sourceMap.sample.parameters.collection.empty());
    REQUIRE(ds.sourceMap.sample.headers.collection.empty());
    REQUIRE(ds.sourceMap.sample.body.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.sample.body.sourceMap[0].location == 30);
    REQUIRE(ds.sourceMap.sample.body.sourceMap[0].length == 12);
}

TEST_CASE("Parsing Data Structure", "[datastructure]")
{
    ParseResult<DataStructure> ds;
    SectionParserHelper<DataStructure, DataStructureParser>::parse(DataStructureFixture, DataStructureSectionType, ds, ExportSourcemapOption);
    
    REQUIRE(ds.report.error.code == Error::OK);
    //CHECK(ds.report.warnings.empty());
    
    REQUIRE(ds.node.name == "Name");
    REQUIRE(ds.node.description == "Structure Description\n\n");

    REQUIRE(ds.node.members.size() == 1);
    REQUIRE(ds.node.members[0].name == "id");
    REQUIRE(ds.node.members[0].description == "Resource Id");
    REQUIRE(ds.node.members[0].type == "number");
    REQUIRE(ds.node.members[0].defaultValue.empty());
    REQUIRE(ds.node.members[0].exampleValue == "42");
    REQUIRE(ds.node.members[0].values.empty());

    REQUIRE(ds.node.sample.name.empty());
    REQUIRE(ds.node.sample.description.empty());
    REQUIRE(ds.node.sample.parameters.empty());
    REQUIRE(ds.node.sample.headers.empty());
    REQUIRE(ds.node.sample.body == "Hello World\n");
    REQUIRE(ds.node.sample.schema.empty());
    
    REQUIRE(ds.sourceMap.name.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.name.sourceMap[0].location == 0);
    REQUIRE(ds.sourceMap.name.sourceMap[0].length == 18);
    REQUIRE(ds.sourceMap.description.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.description.sourceMap[0].location == 18);
    REQUIRE(ds.sourceMap.description.sourceMap[0].length == 23);
    
    REQUIRE(ds.sourceMap.members.collection.size() == 1);
    REQUIRE(ds.sourceMap.members.collection[0].name.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.members.collection[0].name.sourceMap[0].location == 57);
    REQUIRE(ds.sourceMap.members.collection[0].name.sourceMap[0].length == 44);
    
    REQUIRE(ds.sourceMap.sample.name.sourceMap.empty());
    REQUIRE(ds.sourceMap.sample.description.sourceMap.empty());
    REQUIRE(ds.sourceMap.sample.parameters.collection.empty());
    REQUIRE(ds.sourceMap.sample.headers.collection.empty());
    REQUIRE(ds.sourceMap.sample.body.sourceMap.size() == 1);
    REQUIRE(ds.sourceMap.sample.body.sourceMap[0].location == 115);
    REQUIRE(ds.sourceMap.sample.body.sourceMap[0].length == 12);
}

