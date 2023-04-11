// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/unittest_lite_imports_nonlite.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_google_2fprotobuf_2funittest_5flite_5fimports_5fnonlite_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_google_2fprotobuf_2funittest_5flite_5fimports_5fnonlite_2eproto

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
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include "google/protobuf/unittest.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_google_2fprotobuf_2funittest_5flite_5fimports_5fnonlite_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_google_2fprotobuf_2funittest_5flite_5fimports_5fnonlite_2eproto {
  static const uint32_t offsets[];
};
namespace protobuf_unittest {
class TestLiteImportsNonlite;
struct TestLiteImportsNonliteDefaultTypeInternal;
extern TestLiteImportsNonliteDefaultTypeInternal _TestLiteImportsNonlite_default_instance_;
}  // namespace protobuf_unittest
PROTOBUF_NAMESPACE_OPEN
template<> ::protobuf_unittest::TestLiteImportsNonlite* Arena::CreateMaybeMessage<::protobuf_unittest::TestLiteImportsNonlite>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace protobuf_unittest {

// ===================================================================

class TestLiteImportsNonlite final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:protobuf_unittest.TestLiteImportsNonlite) */ {
 public:
  inline TestLiteImportsNonlite() : TestLiteImportsNonlite(nullptr) {}
  ~TestLiteImportsNonlite() override;
  explicit PROTOBUF_CONSTEXPR TestLiteImportsNonlite(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  TestLiteImportsNonlite(const TestLiteImportsNonlite& from);
  TestLiteImportsNonlite(TestLiteImportsNonlite&& from) noexcept
    : TestLiteImportsNonlite() {
    *this = ::std::move(from);
  }

  inline TestLiteImportsNonlite& operator=(const TestLiteImportsNonlite& from) {
    CopyFrom(from);
    return *this;
  }
  inline TestLiteImportsNonlite& operator=(TestLiteImportsNonlite&& from) noexcept {
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

  inline const std::string& unknown_fields() const {
    return _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString);
  }
  inline std::string* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<std::string>();
  }

  static const TestLiteImportsNonlite& default_instance() {
    return *internal_default_instance();
  }
  static inline const TestLiteImportsNonlite* internal_default_instance() {
    return reinterpret_cast<const TestLiteImportsNonlite*>(
               &_TestLiteImportsNonlite_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(TestLiteImportsNonlite& a, TestLiteImportsNonlite& b) {
    a.Swap(&b);
  }
  inline void Swap(TestLiteImportsNonlite* other) {
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
  void UnsafeArenaSwap(TestLiteImportsNonlite* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  TestLiteImportsNonlite* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<TestLiteImportsNonlite>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const TestLiteImportsNonlite& from);
  void MergeFrom(const TestLiteImportsNonlite& from);
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
  void SetCachedSize(int size) const;
  void InternalSwap(TestLiteImportsNonlite* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "protobuf_unittest.TestLiteImportsNonlite";
  }
  protected:
  explicit TestLiteImportsNonlite(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kMessageFieldNumber = 1,
    kMessageWithRequiredFieldNumber = 2,
  };
  // optional .protobuf_unittest.TestAllTypes message = 1;
  bool has_message() const;
  private:
  bool _internal_has_message() const;
  public:
  void clear_message();
  const ::protobuf_unittest::TestAllTypes& message() const;
  PROTOBUF_NODISCARD ::protobuf_unittest::TestAllTypes* release_message();
  ::protobuf_unittest::TestAllTypes* mutable_message();
  void set_allocated_message(::protobuf_unittest::TestAllTypes* message);
  private:
  const ::protobuf_unittest::TestAllTypes& _internal_message() const;
  ::protobuf_unittest::TestAllTypes* _internal_mutable_message();
  public:
  void unsafe_arena_set_allocated_message(
      ::protobuf_unittest::TestAllTypes* message);
  ::protobuf_unittest::TestAllTypes* unsafe_arena_release_message();

  // optional .protobuf_unittest.TestRequired message_with_required = 2;
  bool has_message_with_required() const;
  private:
  bool _internal_has_message_with_required() const;
  public:
  void clear_message_with_required();
  const ::protobuf_unittest::TestRequired& message_with_required() const;
  PROTOBUF_NODISCARD ::protobuf_unittest::TestRequired* release_message_with_required();
  ::protobuf_unittest::TestRequired* mutable_message_with_required();
  void set_allocated_message_with_required(::protobuf_unittest::TestRequired* message_with_required);
  private:
  const ::protobuf_unittest::TestRequired& _internal_message_with_required() const;
  ::protobuf_unittest::TestRequired* _internal_mutable_message_with_required();
  public:
  void unsafe_arena_set_allocated_message_with_required(
      ::protobuf_unittest::TestRequired* message_with_required);
  ::protobuf_unittest::TestRequired* unsafe_arena_release_message_with_required();

  // @@protoc_insertion_point(class_scope:protobuf_unittest.TestLiteImportsNonlite)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
    ::protobuf_unittest::TestAllTypes* message_;
    ::protobuf_unittest::TestRequired* message_with_required_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_google_2fprotobuf_2funittest_5flite_5fimports_5fnonlite_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// TestLiteImportsNonlite

// optional .protobuf_unittest.TestAllTypes message = 1;
inline bool TestLiteImportsNonlite::_internal_has_message() const {
  bool value = (_impl_._has_bits_[0] & 0x00000001u) != 0;
  PROTOBUF_ASSUME(!value || _impl_.message_ != nullptr);
  return value;
}
inline bool TestLiteImportsNonlite::has_message() const {
  return _internal_has_message();
}
inline const ::protobuf_unittest::TestAllTypes& TestLiteImportsNonlite::_internal_message() const {
  const ::protobuf_unittest::TestAllTypes* p = _impl_.message_;
  return p != nullptr ? *p : reinterpret_cast<const ::protobuf_unittest::TestAllTypes&>(
      ::protobuf_unittest::_TestAllTypes_default_instance_);
}
inline const ::protobuf_unittest::TestAllTypes& TestLiteImportsNonlite::message() const {
  // @@protoc_insertion_point(field_get:protobuf_unittest.TestLiteImportsNonlite.message)
  return _internal_message();
}
inline void TestLiteImportsNonlite::unsafe_arena_set_allocated_message(
    ::protobuf_unittest::TestAllTypes* message) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.message_);
  }
  _impl_.message_ = message;
  if (message) {
    _impl_._has_bits_[0] |= 0x00000001u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000001u;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:protobuf_unittest.TestLiteImportsNonlite.message)
}
inline ::protobuf_unittest::TestAllTypes* TestLiteImportsNonlite::release_message() {
  _impl_._has_bits_[0] &= ~0x00000001u;
  ::protobuf_unittest::TestAllTypes* temp = _impl_.message_;
  _impl_.message_ = nullptr;
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
inline ::protobuf_unittest::TestAllTypes* TestLiteImportsNonlite::unsafe_arena_release_message() {
  // @@protoc_insertion_point(field_release:protobuf_unittest.TestLiteImportsNonlite.message)
  _impl_._has_bits_[0] &= ~0x00000001u;
  ::protobuf_unittest::TestAllTypes* temp = _impl_.message_;
  _impl_.message_ = nullptr;
  return temp;
}
inline ::protobuf_unittest::TestAllTypes* TestLiteImportsNonlite::_internal_mutable_message() {
  _impl_._has_bits_[0] |= 0x00000001u;
  if (_impl_.message_ == nullptr) {
    auto* p = CreateMaybeMessage<::protobuf_unittest::TestAllTypes>(GetArenaForAllocation());
    _impl_.message_ = p;
  }
  return _impl_.message_;
}
inline ::protobuf_unittest::TestAllTypes* TestLiteImportsNonlite::mutable_message() {
  ::protobuf_unittest::TestAllTypes* _msg = _internal_mutable_message();
  // @@protoc_insertion_point(field_mutable:protobuf_unittest.TestLiteImportsNonlite.message)
  return _msg;
}
inline void TestLiteImportsNonlite::set_allocated_message(::protobuf_unittest::TestAllTypes* message) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.message_);
  }
  if (message) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(message));
    if (message_arena != submessage_arena) {
      message = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, message, submessage_arena);
    }
    _impl_._has_bits_[0] |= 0x00000001u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000001u;
  }
  _impl_.message_ = message;
  // @@protoc_insertion_point(field_set_allocated:protobuf_unittest.TestLiteImportsNonlite.message)
}

// optional .protobuf_unittest.TestRequired message_with_required = 2;
inline bool TestLiteImportsNonlite::_internal_has_message_with_required() const {
  bool value = (_impl_._has_bits_[0] & 0x00000002u) != 0;
  PROTOBUF_ASSUME(!value || _impl_.message_with_required_ != nullptr);
  return value;
}
inline bool TestLiteImportsNonlite::has_message_with_required() const {
  return _internal_has_message_with_required();
}
inline const ::protobuf_unittest::TestRequired& TestLiteImportsNonlite::_internal_message_with_required() const {
  const ::protobuf_unittest::TestRequired* p = _impl_.message_with_required_;
  return p != nullptr ? *p : reinterpret_cast<const ::protobuf_unittest::TestRequired&>(
      ::protobuf_unittest::_TestRequired_default_instance_);
}
inline const ::protobuf_unittest::TestRequired& TestLiteImportsNonlite::message_with_required() const {
  // @@protoc_insertion_point(field_get:protobuf_unittest.TestLiteImportsNonlite.message_with_required)
  return _internal_message_with_required();
}
inline void TestLiteImportsNonlite::unsafe_arena_set_allocated_message_with_required(
    ::protobuf_unittest::TestRequired* message_with_required) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.message_with_required_);
  }
  _impl_.message_with_required_ = message_with_required;
  if (message_with_required) {
    _impl_._has_bits_[0] |= 0x00000002u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000002u;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:protobuf_unittest.TestLiteImportsNonlite.message_with_required)
}
inline ::protobuf_unittest::TestRequired* TestLiteImportsNonlite::release_message_with_required() {
  _impl_._has_bits_[0] &= ~0x00000002u;
  ::protobuf_unittest::TestRequired* temp = _impl_.message_with_required_;
  _impl_.message_with_required_ = nullptr;
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
inline ::protobuf_unittest::TestRequired* TestLiteImportsNonlite::unsafe_arena_release_message_with_required() {
  // @@protoc_insertion_point(field_release:protobuf_unittest.TestLiteImportsNonlite.message_with_required)
  _impl_._has_bits_[0] &= ~0x00000002u;
  ::protobuf_unittest::TestRequired* temp = _impl_.message_with_required_;
  _impl_.message_with_required_ = nullptr;
  return temp;
}
inline ::protobuf_unittest::TestRequired* TestLiteImportsNonlite::_internal_mutable_message_with_required() {
  _impl_._has_bits_[0] |= 0x00000002u;
  if (_impl_.message_with_required_ == nullptr) {
    auto* p = CreateMaybeMessage<::protobuf_unittest::TestRequired>(GetArenaForAllocation());
    _impl_.message_with_required_ = p;
  }
  return _impl_.message_with_required_;
}
inline ::protobuf_unittest::TestRequired* TestLiteImportsNonlite::mutable_message_with_required() {
  ::protobuf_unittest::TestRequired* _msg = _internal_mutable_message_with_required();
  // @@protoc_insertion_point(field_mutable:protobuf_unittest.TestLiteImportsNonlite.message_with_required)
  return _msg;
}
inline void TestLiteImportsNonlite::set_allocated_message_with_required(::protobuf_unittest::TestRequired* message_with_required) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.message_with_required_);
  }
  if (message_with_required) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(message_with_required));
    if (message_arena != submessage_arena) {
      message_with_required = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, message_with_required, submessage_arena);
    }
    _impl_._has_bits_[0] |= 0x00000002u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000002u;
  }
  _impl_.message_with_required_ = message_with_required;
  // @@protoc_insertion_point(field_set_allocated:protobuf_unittest.TestLiteImportsNonlite.message_with_required)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace protobuf_unittest

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_google_2fprotobuf_2funittest_5flite_5fimports_5fnonlite_2eproto
