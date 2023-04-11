// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/unittest_lazy_dependencies.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_google_2fprotobuf_2funittest_5flazy_5fdependencies_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_google_2fprotobuf_2funittest_5flazy_5fdependencies_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021004 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_bases.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "google/protobuf/unittest_lazy_dependencies_custom_option.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_google_2fprotobuf_2funittest_5flazy_5fdependencies_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_google_2fprotobuf_2funittest_5flazy_5fdependencies_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_2eproto;
namespace protobuf_unittest {
namespace lazy_imports {
class ImportedMessage;
struct ImportedMessageDefaultTypeInternal;
extern ImportedMessageDefaultTypeInternal _ImportedMessage_default_instance_;
class MessageCustomOption;
struct MessageCustomOptionDefaultTypeInternal;
extern MessageCustomOptionDefaultTypeInternal _MessageCustomOption_default_instance_;
class MessageCustomOption2;
struct MessageCustomOption2DefaultTypeInternal;
extern MessageCustomOption2DefaultTypeInternal _MessageCustomOption2_default_instance_;
}  // namespace lazy_imports
}  // namespace protobuf_unittest
PROTOBUF_NAMESPACE_OPEN
template<> ::protobuf_unittest::lazy_imports::ImportedMessage* Arena::CreateMaybeMessage<::protobuf_unittest::lazy_imports::ImportedMessage>(Arena*);
template<> ::protobuf_unittest::lazy_imports::MessageCustomOption* Arena::CreateMaybeMessage<::protobuf_unittest::lazy_imports::MessageCustomOption>(Arena*);
template<> ::protobuf_unittest::lazy_imports::MessageCustomOption2* Arena::CreateMaybeMessage<::protobuf_unittest::lazy_imports::MessageCustomOption2>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace protobuf_unittest {
namespace lazy_imports {

// ===================================================================

class ImportedMessage final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:protobuf_unittest.lazy_imports.ImportedMessage) */ {
 public:
  inline ImportedMessage() : ImportedMessage(nullptr) {}
  ~ImportedMessage() override;
  explicit PROTOBUF_CONSTEXPR ImportedMessage(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  ImportedMessage(const ImportedMessage& from);
  ImportedMessage(ImportedMessage&& from) noexcept
    : ImportedMessage() {
    *this = ::std::move(from);
  }

  inline ImportedMessage& operator=(const ImportedMessage& from) {
    CopyFrom(from);
    return *this;
  }
  inline ImportedMessage& operator=(ImportedMessage&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance);
  }
  inline ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const ImportedMessage& default_instance() {
    return *internal_default_instance();
  }
  static inline const ImportedMessage* internal_default_instance() {
    return reinterpret_cast<const ImportedMessage*>(
               &_ImportedMessage_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(ImportedMessage& a, ImportedMessage& b) {
    a.Swap(&b);
  }
  inline void Swap(ImportedMessage* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(ImportedMessage* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  ImportedMessage* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<ImportedMessage>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const ImportedMessage& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const ImportedMessage& from) {
    ImportedMessage::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ImportedMessage* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "protobuf_unittest.lazy_imports.ImportedMessage";
  }
  protected:
  explicit ImportedMessage(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kLazyMessageFieldNumber = 1,
  };
  // optional .protobuf_unittest.lazy_imports.LazyMessage lazy_message = 1;
  bool has_lazy_message() const;
  private:
  bool _internal_has_lazy_message() const;
  public:
  void clear_lazy_message();
  const ::protobuf_unittest::lazy_imports::LazyMessage& lazy_message() const;
  PROTOBUF_NODISCARD ::protobuf_unittest::lazy_imports::LazyMessage* release_lazy_message();
  ::protobuf_unittest::lazy_imports::LazyMessage* mutable_lazy_message();
  void set_allocated_lazy_message(::protobuf_unittest::lazy_imports::LazyMessage* lazy_message);
  private:
  const ::protobuf_unittest::lazy_imports::LazyMessage& _internal_lazy_message() const;
  ::protobuf_unittest::lazy_imports::LazyMessage* _internal_mutable_lazy_message();
  public:
  void unsafe_arena_set_allocated_lazy_message(
      ::protobuf_unittest::lazy_imports::LazyMessage* lazy_message);
  ::protobuf_unittest::lazy_imports::LazyMessage* unsafe_arena_release_lazy_message();

  // @@protoc_insertion_point(class_scope:protobuf_unittest.lazy_imports.ImportedMessage)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
    ::protobuf_unittest::lazy_imports::LazyMessage* lazy_message_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_google_2fprotobuf_2funittest_5flazy_5fdependencies_2eproto;
};
// -------------------------------------------------------------------

class MessageCustomOption final :
    public ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase /* @@protoc_insertion_point(class_definition:protobuf_unittest.lazy_imports.MessageCustomOption) */ {
 public:
  inline MessageCustomOption() : MessageCustomOption(nullptr) {}
  explicit PROTOBUF_CONSTEXPR MessageCustomOption(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  MessageCustomOption(const MessageCustomOption& from);
  MessageCustomOption(MessageCustomOption&& from) noexcept
    : MessageCustomOption() {
    *this = ::std::move(from);
  }

  inline MessageCustomOption& operator=(const MessageCustomOption& from) {
    CopyFrom(from);
    return *this;
  }
  inline MessageCustomOption& operator=(MessageCustomOption&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance);
  }
  inline ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const MessageCustomOption& default_instance() {
    return *internal_default_instance();
  }
  static inline const MessageCustomOption* internal_default_instance() {
    return reinterpret_cast<const MessageCustomOption*>(
               &_MessageCustomOption_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(MessageCustomOption& a, MessageCustomOption& b) {
    a.Swap(&b);
  }
  inline void Swap(MessageCustomOption* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(MessageCustomOption* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  MessageCustomOption* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<MessageCustomOption>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyFrom;
  inline void CopyFrom(const MessageCustomOption& from) {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyImpl(*this, from);
  }
  using ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeFrom;
  void MergeFrom(const MessageCustomOption& from) {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeImpl(*this, from);
  }
  public:

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "protobuf_unittest.lazy_imports.MessageCustomOption";
  }
  protected:
  explicit MessageCustomOption(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:protobuf_unittest.lazy_imports.MessageCustomOption)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
  };
  friend struct ::TableStruct_google_2fprotobuf_2funittest_5flazy_5fdependencies_2eproto;
};
// -------------------------------------------------------------------

class MessageCustomOption2 final :
    public ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase /* @@protoc_insertion_point(class_definition:protobuf_unittest.lazy_imports.MessageCustomOption2) */ {
 public:
  inline MessageCustomOption2() : MessageCustomOption2(nullptr) {}
  explicit PROTOBUF_CONSTEXPR MessageCustomOption2(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  MessageCustomOption2(const MessageCustomOption2& from);
  MessageCustomOption2(MessageCustomOption2&& from) noexcept
    : MessageCustomOption2() {
    *this = ::std::move(from);
  }

  inline MessageCustomOption2& operator=(const MessageCustomOption2& from) {
    CopyFrom(from);
    return *this;
  }
  inline MessageCustomOption2& operator=(MessageCustomOption2&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance);
  }
  inline ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const MessageCustomOption2& default_instance() {
    return *internal_default_instance();
  }
  static inline const MessageCustomOption2* internal_default_instance() {
    return reinterpret_cast<const MessageCustomOption2*>(
               &_MessageCustomOption2_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(MessageCustomOption2& a, MessageCustomOption2& b) {
    a.Swap(&b);
  }
  inline void Swap(MessageCustomOption2* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(MessageCustomOption2* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  MessageCustomOption2* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<MessageCustomOption2>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyFrom;
  inline void CopyFrom(const MessageCustomOption2& from) {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyImpl(*this, from);
  }
  using ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeFrom;
  void MergeFrom(const MessageCustomOption2& from) {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeImpl(*this, from);
  }
  public:

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "protobuf_unittest.lazy_imports.MessageCustomOption2";
  }
  protected:
  explicit MessageCustomOption2(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:protobuf_unittest.lazy_imports.MessageCustomOption2)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
  };
  friend struct ::TableStruct_google_2fprotobuf_2funittest_5flazy_5fdependencies_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// ImportedMessage

// optional .protobuf_unittest.lazy_imports.LazyMessage lazy_message = 1;
inline bool ImportedMessage::_internal_has_lazy_message() const {
  bool value = (_impl_._has_bits_[0] & 0x00000001u) != 0;
  PROTOBUF_ASSUME(!value || _impl_.lazy_message_ != nullptr);
  return value;
}
inline bool ImportedMessage::has_lazy_message() const {
  return _internal_has_lazy_message();
}
inline const ::protobuf_unittest::lazy_imports::LazyMessage& ImportedMessage::_internal_lazy_message() const {
  const ::protobuf_unittest::lazy_imports::LazyMessage* p = _impl_.lazy_message_;
  return p != nullptr ? *p : reinterpret_cast<const ::protobuf_unittest::lazy_imports::LazyMessage&>(
      ::protobuf_unittest::lazy_imports::_LazyMessage_default_instance_);
}
inline const ::protobuf_unittest::lazy_imports::LazyMessage& ImportedMessage::lazy_message() const {
  // @@protoc_insertion_point(field_get:protobuf_unittest.lazy_imports.ImportedMessage.lazy_message)
  return _internal_lazy_message();
}
inline void ImportedMessage::unsafe_arena_set_allocated_lazy_message(
    ::protobuf_unittest::lazy_imports::LazyMessage* lazy_message) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.lazy_message_);
  }
  _impl_.lazy_message_ = lazy_message;
  if (lazy_message) {
    _impl_._has_bits_[0] |= 0x00000001u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000001u;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:protobuf_unittest.lazy_imports.ImportedMessage.lazy_message)
}
inline ::protobuf_unittest::lazy_imports::LazyMessage* ImportedMessage::release_lazy_message() {
  _impl_._has_bits_[0] &= ~0x00000001u;
  ::protobuf_unittest::lazy_imports::LazyMessage* temp = _impl_.lazy_message_;
  _impl_.lazy_message_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::protobuf_unittest::lazy_imports::LazyMessage* ImportedMessage::unsafe_arena_release_lazy_message() {
  // @@protoc_insertion_point(field_release:protobuf_unittest.lazy_imports.ImportedMessage.lazy_message)
  _impl_._has_bits_[0] &= ~0x00000001u;
  ::protobuf_unittest::lazy_imports::LazyMessage* temp = _impl_.lazy_message_;
  _impl_.lazy_message_ = nullptr;
  return temp;
}
inline ::protobuf_unittest::lazy_imports::LazyMessage* ImportedMessage::_internal_mutable_lazy_message() {
  _impl_._has_bits_[0] |= 0x00000001u;
  if (_impl_.lazy_message_ == nullptr) {
    auto* p = CreateMaybeMessage<::protobuf_unittest::lazy_imports::LazyMessage>(GetArenaForAllocation());
    _impl_.lazy_message_ = p;
  }
  return _impl_.lazy_message_;
}
inline ::protobuf_unittest::lazy_imports::LazyMessage* ImportedMessage::mutable_lazy_message() {
  ::protobuf_unittest::lazy_imports::LazyMessage* _msg = _internal_mutable_lazy_message();
  // @@protoc_insertion_point(field_mutable:protobuf_unittest.lazy_imports.ImportedMessage.lazy_message)
  return _msg;
}
inline void ImportedMessage::set_allocated_lazy_message(::protobuf_unittest::lazy_imports::LazyMessage* lazy_message) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.lazy_message_);
  }
  if (lazy_message) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(lazy_message));
    if (message_arena != submessage_arena) {
      lazy_message = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, lazy_message, submessage_arena);
    }
    _impl_._has_bits_[0] |= 0x00000001u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000001u;
  }
  _impl_.lazy_message_ = lazy_message;
  // @@protoc_insertion_point(field_set_allocated:protobuf_unittest.lazy_imports.ImportedMessage.lazy_message)
}

// -------------------------------------------------------------------

// MessageCustomOption

// -------------------------------------------------------------------

// MessageCustomOption2

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace lazy_imports
}  // namespace protobuf_unittest

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_google_2fprotobuf_2funittest_5flazy_5fdependencies_2eproto
