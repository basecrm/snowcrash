//
//  DataStructureParser.h
//  snowcrash
//
//  Created by Boguslaw Mista on 16/11/14.
//  Copyright (c) 2014 Base. All rights reserved.
//

#ifndef SNOWCRASH_DATASTRUCTURE_H
#define SNOWCRASH_DATASTRUCTURE_H

#include "HTTP.h"
#include "SectionParser.h"
#include "ParametersParser.h"
#include "PayloadParser.h"
#include "RegexMatch.h"

namespace snowcrash {

    /** Keyword Data Structure matching regex */
    const char* const KeywordDataStructureHeaderRegex = "^[[:blank:]]*[Ss]tructure[[:blank:]]+" SYMBOL_IDENTIFIER "[[:blank:]]*$";

    /** Internal type alias for Collection iterator of DataStructure */
    typedef Collection<DataStructure>::const_iterator DataStructuresIterator;

    /**
     * Data Structure Section processor
     */
    template<>
    struct SectionProcessor<DataStructure> : public SectionProcessorBase<DataStructure> {

        static MarkdownNodeIterator processSignature(const MarkdownNodeIterator& node,
                                                     const MarkdownNodes& siblings,
                                                     SectionParserData& pd,
                                                     SectionLayout& layout,
                                                     const ParseResultRef<DataStructure>& out) {

            CaptureGroups captureGroups;
            mdp::ByteBuffer subject, remaining;

            subject = GetFirstLine(node->text, remaining);
            TrimString(subject);
            
            if (RegexCapture(subject, KeywordDataStructureHeaderRegex, captureGroups, 3)) {
                out.node.name = captureGroups[1];
            }

            TrimString(out.node.name);

            if (pd.exportSourceMap()) {
                if (!out.node.name.empty()) {
                    out.sourceMap.name.sourceMap = node->sourceMap;
                }
            }

            if (!remaining.empty()) {
                out.node.description += remaining;

                if (pd.exportSourceMap()) {
                    out.sourceMap.description.sourceMap.append(node->sourceMap);
                }
            }

            return ++MarkdownNodeIterator(node);
        }

        static MarkdownNodeIterator processNestedSection(const MarkdownNodeIterator& node,
                                                         const MarkdownNodes& siblings,
                                                         SectionParserData& pd,
                                                         const ParseResultRef<DataStructure>& out) {

            SectionType sectionType = pd.sectionContext();
            MarkdownNodeIterator cur = node;
            std::stringstream ss;

            mdp::CharactersRangeSet sourceMap = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceData);

            switch (sectionType) {
                case MembersSectionType:
                {
                    ParseResultRef<Parameters> parameters(out.report, out.node.members, out.sourceMap.members);
                    return ParametersParser::parse(node, siblings, pd, parameters);
                }

                case SampleSectionType:
                case SampleBodySectionType:
                {
                    IntermediateParseResult<Payload> payload(out.report);

                    cur = PayloadParser::parse(node, siblings, pd, payload);

                    checkPayload(sectionType, sourceMap, payload.node, out);

                    out.node.sample = payload.node;

                    if (pd.exportSourceMap()) {
                        out.sourceMap.sample = payload.sourceMap;
                    }

                    break;
                }

                default:
                    break;
            }

            return cur;
        }

        static bool isUnexpectedNode(const MarkdownNodeIterator& node,
                                     SectionType sectionType) {

            if (SectionProcessor<Asset>::sectionType(node) != UndefinedSectionType) {
                return true;
            }

            return SectionProcessorBase<DataStructure>::isUnexpectedNode(node, sectionType);
        }

        static MarkdownNodeIterator processUnexpectedNode(const MarkdownNodeIterator& node,
                                                          const MarkdownNodes& siblings,
                                                          SectionParserData& pd,
                                                          SectionType& sectionType,
                                                          const ParseResultRef<DataStructure>& out) {

            SectionType assetType = SectionProcessor<Asset>::sectionType(node);

            if (assetType != UndefinedSectionType) {

                // WARN: Ignoring section
                std::stringstream ss;
                mdp::CharactersRangeSet sourceMap = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceData);

                ss << "Ignoring " << SectionName(assetType) << " list item, ";
                ss << SectionName(assetType) << " list item is expected to be indented by 4 spaces or 1 tab";

                out.report.warnings.push_back(Warning(ss.str(),
                                                      IgnoringWarning,
                                                      sourceMap));

                return ++MarkdownNodeIterator(node);
            }

            return SectionProcessorBase<DataStructure>::processUnexpectedNode(node, siblings, pd, sectionType, out);
        }

        static SectionType sectionType(const MarkdownNodeIterator& node) {

            if (node->type == mdp::HeaderMarkdownNodeType
                && !node->text.empty()) {

                mdp::ByteBuffer subject = node->text;
                TrimString(subject);

                if (RegexMatch(subject, KeywordDataStructureHeaderRegex)) {

                    return DataStructureSectionType;
                }
            }

            return UndefinedSectionType;
        }

        static SectionType nestedSectionType(const MarkdownNodeIterator& node) {

            SectionType nestedType = UndefinedSectionType;

            // Check if parameters section
            nestedType = SectionProcessor<Parameters>::sectionType(node);

            if (nestedType != UndefinedSectionType) {
                return nestedType;
            }

            // Check if payload section
            nestedType = SectionProcessor<Payload>::sectionType(node);

            if (nestedType != UndefinedSectionType) {
                return nestedType;
            }

            return UndefinedSectionType;
        }

        static SectionTypes nestedSectionTypes() {
            SectionTypes nested, types;

            // Payload & descendants
            nested.push_back(SampleBodySectionType);
            nested.push_back(SampleSectionType);

            types = SectionProcessor<Payload>::nestedSectionTypes();
            nested.insert(nested.end(), types.begin(), types.end());

            return nested;
        }

        static void finalize(const MarkdownNodeIterator& node,
                             SectionParserData& pd,
                             const ParseResultRef<DataStructure>& out) {
        }

        /**
         *  \brief  Check & report payload validity.
         *  \param  sectionType A section of the payload.
         *  \param  sourceMap   Payload signature source map.
         *  \param  payload     The payload to be checked.
         *  \param  out         The Data Structure to which payload belongs to.
         */
        static void checkPayload(SectionType sectionType,
                                 const mdp::CharactersRangeSet sourceMap,
                                 const Payload& payload,
                                 const ParseResultRef<DataStructure>& out) {


            if (!out.node.sample.body.empty()) {

                // WARN: Duplicate payload
                std::stringstream ss;
                ss << SectionName(sectionType) << " sample `" << payload.name << "`";
                ss << " already defined` - overwriting";

                out.report.warnings.push_back(Warning(ss.str(),
                                                      DuplicateWarning,
                                                      sourceMap));
            }
        }
    };

    /** DataStructure Section Parser */
    typedef SectionParser<DataStructure, HeaderSectionAdapter> DataStructureParser;
}

#endif
