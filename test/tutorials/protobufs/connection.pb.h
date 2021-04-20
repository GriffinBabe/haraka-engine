// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: connection.proto

#ifndef PROTOBUF_INCLUDED_connection_2eproto
#define PROTOBUF_INCLUDED_connection_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/extension_set.h> // IWYU pragma: export
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/repeated_field.h> // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_connection_2eproto

namespace protobuf_connection_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
    static const ::google::protobuf::internal::ParseTableField entries[];
    static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
    static const ::google::protobuf::internal::ParseTable schema[1];
    static const ::google::protobuf::internal::FieldMetadata field_metadata[];
    static const ::google::protobuf::internal::SerializationTable
        serialization_table[];
    static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
} // namespace protobuf_connection_2eproto
namespace tutorial {
class Connection;
class ConnectionDefaultTypeInternal;
extern ConnectionDefaultTypeInternal _Connection_default_instance_;
} // namespace tutorial
namespace google {
namespace protobuf {
    template <>
    ::tutorial::Connection*
    Arena::CreateMaybeMessage<::tutorial::Connection>(Arena*);
} // namespace protobuf
} // namespace google
namespace tutorial {

// ===================================================================

class Connection : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tutorial.Connection)
                                                       */
{
public:
    Connection();
    virtual ~Connection();

    Connection(const Connection& from);

    inline Connection& operator=(const Connection& from)
    {
        CopyFrom(from);
        return *this;
    }
#if LANG_CXX11
    Connection(Connection&& from) noexcept : Connection()
    {
        *this = ::std::move(from);
    }

    inline Connection& operator=(Connection&& from) noexcept
    {
        if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
            if (this != &from)
                InternalSwap(&from);
        }
        else {
            CopyFrom(from);
        }
        return *this;
    }
#endif
    inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const
    {
        return _internal_metadata_.unknown_fields();
    }
    inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields()
    {
        return _internal_metadata_.mutable_unknown_fields();
    }

    static const ::google::protobuf::Descriptor* descriptor();
    static const Connection& default_instance();

    static void InitAsDefaultInstance(); // FOR INTERNAL USE ONLY
    static inline const Connection* internal_default_instance()
    {
        return reinterpret_cast<const Connection*>(
            &_Connection_default_instance_);
    }
    static constexpr int kIndexInFileMessages = 0;

    void Swap(Connection* other);
    friend void swap(Connection& a, Connection& b)
    {
        a.Swap(&b);
    }

    // implements Message ----------------------------------------------

    inline Connection* New() const final
    {
        return CreateMaybeMessage<Connection>(NULL);
    }

    Connection* New(::google::protobuf::Arena* arena) const final
    {
        return CreateMaybeMessage<Connection>(arena);
    }
    void CopyFrom(const ::google::protobuf::Message& from) final;
    void MergeFrom(const ::google::protobuf::Message& from) final;
    void CopyFrom(const Connection& from);
    void MergeFrom(const Connection& from);
    void Clear() final;
    bool IsInitialized() const final;

    size_t ByteSizeLong() const final;
    bool MergePartialFromCodedStream(
        ::google::protobuf::io::CodedInputStream* input) final;
    void SerializeWithCachedSizes(
        ::google::protobuf::io::CodedOutputStream* output) const final;
    ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
        bool deterministic, ::google::protobuf::uint8* target) const final;
    int GetCachedSize() const final
    {
        return _cached_size_.Get();
    }

private:
    void SharedCtor();
    void SharedDtor();
    void SetCachedSize(int size) const final;
    void InternalSwap(Connection* other);

private:
    inline ::google::protobuf::Arena* GetArenaNoVirtual() const
    {
        return NULL;
    }
    inline void* MaybeArenaPtr() const
    {
        return NULL;
    }

public:
    ::google::protobuf::Metadata GetMetadata() const final;

    // nested types ----------------------------------------------------

    // accessors -------------------------------------------------------

    // required string username = 1;
    bool has_username() const;
    void clear_username();
    static const int kUsernameFieldNumber = 1;
    const ::std::string& username() const;
    void set_username(const ::std::string& value);
#if LANG_CXX11
    void set_username(::std::string&& value);
#endif
    void set_username(const char* value);
    void set_username(const char* value, size_t size);
    ::std::string* mutable_username();
    ::std::string* release_username();
    void set_allocated_username(::std::string* username);

    // optional string password = 2;
    bool has_password() const;
    void clear_password();
    static const int kPasswordFieldNumber = 2;
    const ::std::string& password() const;
    void set_password(const ::std::string& value);
#if LANG_CXX11
    void set_password(::std::string&& value);
#endif
    void set_password(const char* value);
    void set_password(const char* value, size_t size);
    ::std::string* mutable_password();
    ::std::string* release_password();
    void set_allocated_password(::std::string* password);

    // optional int32 instance = 3;
    bool has_instance() const;
    void clear_instance();
    static const int kInstanceFieldNumber = 3;
    ::google::protobuf::int32 instance() const;
    void set_instance(::google::protobuf::int32 value);

    // @@protoc_insertion_point(class_scope:tutorial.Connection)
private:
    void set_has_username();
    void clear_has_username();
    void set_has_password();
    void clear_has_password();
    void set_has_instance();
    void clear_has_instance();

    ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
    ::google::protobuf::internal::HasBits<1> _has_bits_;
    mutable ::google::protobuf::internal::CachedSize _cached_size_;
    ::google::protobuf::internal::ArenaStringPtr username_;
    ::google::protobuf::internal::ArenaStringPtr password_;
    ::google::protobuf::int32 instance_;
    friend struct ::protobuf_connection_2eproto::TableStruct;
};
// ===================================================================

// ===================================================================

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif // __GNUC__
// Connection

// required string username = 1;
inline bool Connection::has_username() const
{
    return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Connection::set_has_username()
{
    _has_bits_[0] |= 0x00000001u;
}
inline void Connection::clear_has_username()
{
    _has_bits_[0] &= ~0x00000001u;
}
inline void Connection::clear_username()
{
    username_.ClearToEmptyNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited());
    clear_has_username();
}
inline const ::std::string& Connection::username() const
{
    // @@protoc_insertion_point(field_get:tutorial.Connection.username)
    return username_.GetNoArena();
}
inline void Connection::set_username(const ::std::string& value)
{
    set_has_username();
    username_.SetNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
    // @@protoc_insertion_point(field_set:tutorial.Connection.username)
}
#if LANG_CXX11
inline void Connection::set_username(::std::string&& value)
{
    set_has_username();
    username_.SetNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
        ::std::move(value));
    // @@protoc_insertion_point(field_set_rvalue:tutorial.Connection.username)
}
#endif
inline void Connection::set_username(const char* value)
{
    GOOGLE_DCHECK(value != NULL);
    set_has_username();
    username_.SetNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
        ::std::string(value));
    // @@protoc_insertion_point(field_set_char:tutorial.Connection.username)
}
inline void Connection::set_username(const char* value, size_t size)
{
    set_has_username();
    username_.SetNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
        ::std::string(reinterpret_cast<const char*>(value), size));
    // @@protoc_insertion_point(field_set_pointer:tutorial.Connection.username)
}
inline ::std::string* Connection::mutable_username()
{
    set_has_username();
    // @@protoc_insertion_point(field_mutable:tutorial.Connection.username)
    return username_.MutableNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Connection::release_username()
{
    // @@protoc_insertion_point(field_release:tutorial.Connection.username)
    if (!has_username()) {
        return NULL;
    }
    clear_has_username();
    return username_.ReleaseNonDefaultNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Connection::set_allocated_username(::std::string* username)
{
    if (username != NULL) {
        set_has_username();
    }
    else {
        clear_has_username();
    }
    username_.SetAllocatedNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited(), username);
    // @@protoc_insertion_point(field_set_allocated:tutorial.Connection.username)
}

// optional string password = 2;
inline bool Connection::has_password() const
{
    return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Connection::set_has_password()
{
    _has_bits_[0] |= 0x00000002u;
}
inline void Connection::clear_has_password()
{
    _has_bits_[0] &= ~0x00000002u;
}
inline void Connection::clear_password()
{
    password_.ClearToEmptyNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited());
    clear_has_password();
}
inline const ::std::string& Connection::password() const
{
    // @@protoc_insertion_point(field_get:tutorial.Connection.password)
    return password_.GetNoArena();
}
inline void Connection::set_password(const ::std::string& value)
{
    set_has_password();
    password_.SetNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
    // @@protoc_insertion_point(field_set:tutorial.Connection.password)
}
#if LANG_CXX11
inline void Connection::set_password(::std::string&& value)
{
    set_has_password();
    password_.SetNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
        ::std::move(value));
    // @@protoc_insertion_point(field_set_rvalue:tutorial.Connection.password)
}
#endif
inline void Connection::set_password(const char* value)
{
    GOOGLE_DCHECK(value != NULL);
    set_has_password();
    password_.SetNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
        ::std::string(value));
    // @@protoc_insertion_point(field_set_char:tutorial.Connection.password)
}
inline void Connection::set_password(const char* value, size_t size)
{
    set_has_password();
    password_.SetNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
        ::std::string(reinterpret_cast<const char*>(value), size));
    // @@protoc_insertion_point(field_set_pointer:tutorial.Connection.password)
}
inline ::std::string* Connection::mutable_password()
{
    set_has_password();
    // @@protoc_insertion_point(field_mutable:tutorial.Connection.password)
    return password_.MutableNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Connection::release_password()
{
    // @@protoc_insertion_point(field_release:tutorial.Connection.password)
    if (!has_password()) {
        return NULL;
    }
    clear_has_password();
    return password_.ReleaseNonDefaultNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Connection::set_allocated_password(::std::string* password)
{
    if (password != NULL) {
        set_has_password();
    }
    else {
        clear_has_password();
    }
    password_.SetAllocatedNoArena(
        &::google::protobuf::internal::GetEmptyStringAlreadyInited(), password);
    // @@protoc_insertion_point(field_set_allocated:tutorial.Connection.password)
}

// optional int32 instance = 3;
inline bool Connection::has_instance() const
{
    return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Connection::set_has_instance()
{
    _has_bits_[0] |= 0x00000004u;
}
inline void Connection::clear_has_instance()
{
    _has_bits_[0] &= ~0x00000004u;
}
inline void Connection::clear_instance()
{
    instance_ = 0;
    clear_has_instance();
}
inline ::google::protobuf::int32 Connection::instance() const
{
    // @@protoc_insertion_point(field_get:tutorial.Connection.instance)
    return instance_;
}
inline void Connection::set_instance(::google::protobuf::int32 value)
{
    set_has_instance();
    instance_ = value;
    // @@protoc_insertion_point(field_set:tutorial.Connection.instance)
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__

// @@protoc_insertion_point(namespace_scope)

} // namespace tutorial

// @@protoc_insertion_point(global_scope)

#endif // PROTOBUF_INCLUDED_connection_2eproto
