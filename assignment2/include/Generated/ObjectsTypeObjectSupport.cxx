// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file ObjectsTypeObjectSupport.cxx
 * Source file containing the implementation to register the TypeObject representation of the described types in the IDL file
 *
 * This file was generated by the tool fastddsgen.
 */

#include "ObjectsTypeObjectSupport.hpp"

#include <mutex>
#include <string>

#include <fastcdr/xcdr/external.hpp>
#include <fastcdr/xcdr/optional.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/xtypes/common.hpp>
#include <fastdds/dds/xtypes/type_representation/ITypeObjectRegistry.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObject.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObjectUtils.hpp>

#include "Objects.hpp"


using namespace eprosima::fastdds::dds::xtypes;

// TypeIdentifier is returned by reference: dependent structures/unions are registered in this same method
void register_Objects_type_identifier(
        TypeIdentifierPair& type_ids_Objects)
{

    ReturnCode_t return_code_Objects {eprosima::fastdds::dds::RETCODE_OK};
    return_code_Objects =
        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
        "Objects", type_ids_Objects);
    if (eprosima::fastdds::dds::RETCODE_OK != return_code_Objects)
    {
        StructTypeFlag struct_flags_Objects = TypeObjectUtils::build_struct_type_flag(eprosima::fastdds::dds::xtypes::ExtensibilityKind::APPENDABLE,
                false, false);
        QualifiedTypeName type_name_Objects = "Objects";
        eprosima::fastcdr::optional<AppliedBuiltinTypeAnnotations> type_ann_builtin_Objects;
        eprosima::fastcdr::optional<AppliedAnnotationSeq> ann_custom_Objects;
        CompleteTypeDetail detail_Objects = TypeObjectUtils::build_complete_type_detail(type_ann_builtin_Objects, ann_custom_Objects, type_name_Objects.to_string());
        CompleteStructHeader header_Objects;
        header_Objects = TypeObjectUtils::build_complete_struct_header(TypeIdentifier(), detail_Objects);
        CompleteStructMemberSeq member_seq_Objects;
        {
            TypeIdentifierPair type_ids_x;
            ReturnCode_t return_code_x {eprosima::fastdds::dds::RETCODE_OK};
            return_code_x =
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
                "anonymous_sequence_int32_t_unbounded", type_ids_x);

            if (eprosima::fastdds::dds::RETCODE_OK != return_code_x)
            {
                return_code_x =
                    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
                    "_int32_t", type_ids_x);

                if (eprosima::fastdds::dds::RETCODE_OK != return_code_x)
                {
                    EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                            "Sequence element TypeIdentifier unknown to TypeObjectRegistry.");
                    return;
                }
                bool element_identifier_anonymous_sequence_int32_t_unbounded_ec {false};
                TypeIdentifier* element_identifier_anonymous_sequence_int32_t_unbounded {new TypeIdentifier(TypeObjectUtils::retrieve_complete_type_identifier(type_ids_x, element_identifier_anonymous_sequence_int32_t_unbounded_ec))};
                if (!element_identifier_anonymous_sequence_int32_t_unbounded_ec)
                {
                    EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION, "Sequence element TypeIdentifier inconsistent.");
                    return;
                }
                EquivalenceKind equiv_kind_anonymous_sequence_int32_t_unbounded = EK_COMPLETE;
                if (TK_NONE == type_ids_x.type_identifier2()._d())
                {
                    equiv_kind_anonymous_sequence_int32_t_unbounded = EK_BOTH;
                }
                CollectionElementFlag element_flags_anonymous_sequence_int32_t_unbounded = 0;
                PlainCollectionHeader header_anonymous_sequence_int32_t_unbounded = TypeObjectUtils::build_plain_collection_header(equiv_kind_anonymous_sequence_int32_t_unbounded, element_flags_anonymous_sequence_int32_t_unbounded);
                {
                    SBound bound = 0;
                    PlainSequenceSElemDefn seq_sdefn = TypeObjectUtils::build_plain_sequence_s_elem_defn(header_anonymous_sequence_int32_t_unbounded, bound,
                                eprosima::fastcdr::external<TypeIdentifier>(element_identifier_anonymous_sequence_int32_t_unbounded));
                    if (eprosima::fastdds::dds::RETCODE_BAD_PARAMETER ==
                            TypeObjectUtils::build_and_register_s_sequence_type_identifier(seq_sdefn, "anonymous_sequence_int32_t_unbounded", type_ids_x))
                    {
                        EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                            "anonymous_sequence_int32_t_unbounded already registered in TypeObjectRegistry for a different type.");
                    }
                }
            }
            StructMemberFlag member_flags_x = TypeObjectUtils::build_struct_member_flag(eprosima::fastdds::dds::xtypes::TryConstructFailAction::DISCARD,
                    false, false, false, false);
            MemberId member_id_x = 0x00000000;
            bool common_x_ec {false};
            CommonStructMember common_x {TypeObjectUtils::build_common_struct_member(member_id_x, member_flags_x, TypeObjectUtils::retrieve_complete_type_identifier(type_ids_x, common_x_ec))};
            if (!common_x_ec)
            {
                EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION, "Structure x member TypeIdentifier inconsistent.");
                return;
            }
            MemberName name_x = "x";
            eprosima::fastcdr::optional<AppliedBuiltinMemberAnnotations> member_ann_builtin_x;
            ann_custom_Objects.reset();
            CompleteMemberDetail detail_x = TypeObjectUtils::build_complete_member_detail(name_x, member_ann_builtin_x, ann_custom_Objects);
            CompleteStructMember member_x = TypeObjectUtils::build_complete_struct_member(common_x, detail_x);
            TypeObjectUtils::add_complete_struct_member(member_seq_Objects, member_x);
        }
        {
            TypeIdentifierPair type_ids_y;
            ReturnCode_t return_code_y {eprosima::fastdds::dds::RETCODE_OK};
            return_code_y =
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
                "anonymous_sequence_int32_t_unbounded", type_ids_y);

            if (eprosima::fastdds::dds::RETCODE_OK != return_code_y)
            {
                return_code_y =
                    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
                    "_int32_t", type_ids_y);

                if (eprosima::fastdds::dds::RETCODE_OK != return_code_y)
                {
                    EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                            "Sequence element TypeIdentifier unknown to TypeObjectRegistry.");
                    return;
                }
                bool element_identifier_anonymous_sequence_int32_t_unbounded_ec {false};
                TypeIdentifier* element_identifier_anonymous_sequence_int32_t_unbounded {new TypeIdentifier(TypeObjectUtils::retrieve_complete_type_identifier(type_ids_y, element_identifier_anonymous_sequence_int32_t_unbounded_ec))};
                if (!element_identifier_anonymous_sequence_int32_t_unbounded_ec)
                {
                    EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION, "Sequence element TypeIdentifier inconsistent.");
                    return;
                }
                EquivalenceKind equiv_kind_anonymous_sequence_int32_t_unbounded = EK_COMPLETE;
                if (TK_NONE == type_ids_y.type_identifier2()._d())
                {
                    equiv_kind_anonymous_sequence_int32_t_unbounded = EK_BOTH;
                }
                CollectionElementFlag element_flags_anonymous_sequence_int32_t_unbounded = 0;
                PlainCollectionHeader header_anonymous_sequence_int32_t_unbounded = TypeObjectUtils::build_plain_collection_header(equiv_kind_anonymous_sequence_int32_t_unbounded, element_flags_anonymous_sequence_int32_t_unbounded);
                {
                    SBound bound = 0;
                    PlainSequenceSElemDefn seq_sdefn = TypeObjectUtils::build_plain_sequence_s_elem_defn(header_anonymous_sequence_int32_t_unbounded, bound,
                                eprosima::fastcdr::external<TypeIdentifier>(element_identifier_anonymous_sequence_int32_t_unbounded));
                    if (eprosima::fastdds::dds::RETCODE_BAD_PARAMETER ==
                            TypeObjectUtils::build_and_register_s_sequence_type_identifier(seq_sdefn, "anonymous_sequence_int32_t_unbounded", type_ids_y))
                    {
                        EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                            "anonymous_sequence_int32_t_unbounded already registered in TypeObjectRegistry for a different type.");
                    }
                }
            }
            StructMemberFlag member_flags_y = TypeObjectUtils::build_struct_member_flag(eprosima::fastdds::dds::xtypes::TryConstructFailAction::DISCARD,
                    false, false, false, false);
            MemberId member_id_y = 0x00000001;
            bool common_y_ec {false};
            CommonStructMember common_y {TypeObjectUtils::build_common_struct_member(member_id_y, member_flags_y, TypeObjectUtils::retrieve_complete_type_identifier(type_ids_y, common_y_ec))};
            if (!common_y_ec)
            {
                EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION, "Structure y member TypeIdentifier inconsistent.");
                return;
            }
            MemberName name_y = "y";
            eprosima::fastcdr::optional<AppliedBuiltinMemberAnnotations> member_ann_builtin_y;
            ann_custom_Objects.reset();
            CompleteMemberDetail detail_y = TypeObjectUtils::build_complete_member_detail(name_y, member_ann_builtin_y, ann_custom_Objects);
            CompleteStructMember member_y = TypeObjectUtils::build_complete_struct_member(common_y, detail_y);
            TypeObjectUtils::add_complete_struct_member(member_seq_Objects, member_y);
        }
        {
            TypeIdentifierPair type_ids_objects_number;
            ReturnCode_t return_code_objects_number {eprosima::fastdds::dds::RETCODE_OK};
            return_code_objects_number =
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
                "_int32_t", type_ids_objects_number);

            if (eprosima::fastdds::dds::RETCODE_OK != return_code_objects_number)
            {
                EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                        "objects_number Structure member TypeIdentifier unknown to TypeObjectRegistry.");
                return;
            }
            StructMemberFlag member_flags_objects_number = TypeObjectUtils::build_struct_member_flag(eprosima::fastdds::dds::xtypes::TryConstructFailAction::DISCARD,
                    false, false, false, false);
            MemberId member_id_objects_number = 0x00000002;
            bool common_objects_number_ec {false};
            CommonStructMember common_objects_number {TypeObjectUtils::build_common_struct_member(member_id_objects_number, member_flags_objects_number, TypeObjectUtils::retrieve_complete_type_identifier(type_ids_objects_number, common_objects_number_ec))};
            if (!common_objects_number_ec)
            {
                EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION, "Structure objects_number member TypeIdentifier inconsistent.");
                return;
            }
            MemberName name_objects_number = "objects_number";
            eprosima::fastcdr::optional<AppliedBuiltinMemberAnnotations> member_ann_builtin_objects_number;
            ann_custom_Objects.reset();
            CompleteMemberDetail detail_objects_number = TypeObjectUtils::build_complete_member_detail(name_objects_number, member_ann_builtin_objects_number, ann_custom_Objects);
            CompleteStructMember member_objects_number = TypeObjectUtils::build_complete_struct_member(common_objects_number, detail_objects_number);
            TypeObjectUtils::add_complete_struct_member(member_seq_Objects, member_objects_number);
        }
        CompleteStructType struct_type_Objects = TypeObjectUtils::build_complete_struct_type(struct_flags_Objects, header_Objects, member_seq_Objects);
        if (eprosima::fastdds::dds::RETCODE_BAD_PARAMETER ==
                TypeObjectUtils::build_and_register_struct_type_object(struct_type_Objects, type_name_Objects.to_string(), type_ids_Objects))
        {
            EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                    "Objects already registered in TypeObjectRegistry for a different type.");
        }
    }
}

