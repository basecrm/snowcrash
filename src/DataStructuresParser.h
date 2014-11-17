//
//  DataStructuresParser.h
//  snowcrash
//
//  Created by Boguslaw Mista on 17/11/14.
//  Copyright (c) 2014 Base. All rights reserved.
//

#ifndef SNOWCRASH_DATASTRUCTURESPARSER_H
#define SNOWCRASH_DATASTRUCTURESPARSER_H

#include "SectionParser.h"
#include "DataStructureParser.h"
#include "RegexMatch.h"

namespace snowcrash {

    const char* const DataStructuresHeaderRegex = "^[[:blank:]]*[Dd]ata[[:blank:]]+[Ss]tructures[[:blank:]]*$";

    /** Internal type alias for Collection iterator of DataStructure */
    typedef Collection<DataStructure>::const_iterator DataStructureIterator;

    /**
     * DataStructures Section processor
     */
    template<>
    struct SectionProcessor<DataStructures> : public SectionProcessorBase<DataStructures> {

        static MarkdownNodeIterator processSignature(const MarkdownNodeIterator& node,
                                                     const MarkdownNodes& siblings,
                                                     SectionParserData& pd,
                                                     SectionLayout& layout,
                                                     const ParseResultRef<DataStructures>& out) {
            if (!isRootHeader(node)) {
                MarkdownNodeIterator cur = node;
                SectionType nestedType = nestedSectionType(cur);

                // DataStructure only, parse as exclusive nested sections
                if (nestedType != UndefinedSectionType) {
                    layout = ExclusiveNestedSectionLayout;
                    return cur;
                }
            }

            return ++MarkdownNodeIterator(node);
        }

        static MarkdownNodeIterator processNestedSection(const MarkdownNodeIterator& node,
                                                         const MarkdownNodes& siblings,
                                                         SectionParserData& pd,
                                                         const ParseResultRef<DataStructures>& out) {

            if (pd.sectionContext() == DataStructureSectionType) {

                IntermediateParseResult<DataStructure> ds(out.report);

                MarkdownNodeIterator cur = DataStructureParser::parse(node, siblings, pd, ds);

                DataStructureIterator duplicate = findDataStructure(out.node.dataStructures, ds.node);

                if (duplicate != out.node.dataStructures.end()) {
                    // WARN: Duplicate data structure
                    mdp::CharactersRangeSet sourceMap = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceData);
                    out.report.warnings.push_back(Warning("the data structure '" + ds.node.name + "' is already defined",
                                                          DuplicateWarning,
                                                          sourceMap));
                }

                out.node.dataStructures.push_back(ds.node);

                if (pd.exportSourceMap()) {
                    out.sourceMap.dataStructures.collection.push_back(ds.sourceMap);
                }

                return cur;
            }

            return node;
        }

        static MarkdownNodeIterator processUnexpectedNode(const MarkdownNodeIterator& node,
                                                          const MarkdownNodes& siblings,
                                                          SectionParserData& pd,
                                                          SectionType& lastSectionType,
                                                          const ParseResultRef<DataStructures>& out) {

            return ++MarkdownNodeIterator(node);
        }

        static SectionType sectionType(const MarkdownNodeIterator& node) {

            if (node->type == mdp::HeaderMarkdownNodeType
                && !node->text.empty()) {

                mdp::ByteBuffer subject = node->text;
                TrimString(subject);

                if (RegexMatch(subject, DataStructuresHeaderRegex)) {
                    return DataStructuresSectionType;
                }
            }

            return UndefinedSectionType;
        }

        static SectionType nestedSectionType(const MarkdownNodeIterator& node) {

            // Return DataStructureSectionType or UndefinedSectionType
            return SectionProcessor<DataStructure>::sectionType(node);
        }

        static SectionTypes nestedSectionTypes() {
            SectionTypes nested;

            // DataStructure & descendants
            nested.push_back(DataStructureSectionType);
            SectionTypes types = SectionProcessor<DataStructure>::nestedSectionTypes();
            nested.insert(nested.end(), types.begin(), types.end());

            return nested;
        }

        /** Finds a resource in collection by its name */
        static DataStructureIterator findDataStructure(const DataStructureCollection& collection,
                                                 const DataStructure& ds) {
            return std::find_if(collection.begin(),
                                collection.end(),
                                std::bind2nd(MatchName<DataStructure>(), ds));
        }

        static bool isRootHeader(const MarkdownNodeIterator& node) {
            const MarkdownNode& cur = *node;

            return (cur.type == mdp::HeaderMarkdownNodeType &&
                    cur.data == 1 &&
                    cur.hasParent() &&
                    cur.parent().type == mdp::RootMarkdownNodeType);
        }
    };

    /** Data Structures Section Parser */
    typedef SectionParser<DataStructures, HeaderSectionAdapter> DataStructuresParser;
}

#endif
