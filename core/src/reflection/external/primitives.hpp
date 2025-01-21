#pragma once

#include "reflection/reflect.hpp"

/* Non-const types */

IO_REFLECT_IMPL(iodine::u8, "iodine::u8");
IO_REFLECT_IMPL(iodine::u8*, "iodine::u8*");
IO_REFLECT_IMPL(iodine::u8**, "iodine::u8**");
IO_REFLECT_IMPL(iodine::u8&, "iodine::u8&");
IO_REFLECT_IMPL(iodine::u8&&, "iodine::u8&&");
IO_REFLECT_IMPL(iodine::u8[], "iodine::u8[]");

IO_REFLECT_IMPL(iodine::u16, "iodine::u16");
IO_REFLECT_IMPL(iodine::u16*, "iodine::u16*");
IO_REFLECT_IMPL(iodine::u16**, "iodine::u16**");
IO_REFLECT_IMPL(iodine::u16&, "iodine::u16&");
IO_REFLECT_IMPL(iodine::u16&&, "iodine::u16&&");
IO_REFLECT_IMPL(iodine::u16[], "iodine::u16[]");

IO_REFLECT_IMPL(iodine::u32, "iodine::u32");
IO_REFLECT_IMPL(iodine::u32*, "iodine::u32*");
IO_REFLECT_IMPL(iodine::u32**, "iodine::u32**");
IO_REFLECT_IMPL(iodine::u32&, "iodine::u32&");
IO_REFLECT_IMPL(iodine::u32&&, "iodine::u32&&");
IO_REFLECT_IMPL(iodine::u32[], "iodine::u32[]");

IO_REFLECT_IMPL(iodine::u64, "iodine::u64");
IO_REFLECT_IMPL(iodine::u64*, "iodine::u64*");
IO_REFLECT_IMPL(iodine::u64**, "iodine::u64**");
IO_REFLECT_IMPL(iodine::u64&, "iodine::u64&");
IO_REFLECT_IMPL(iodine::u64&&, "iodine::u64&&");
IO_REFLECT_IMPL(iodine::u64[], "iodine::u64[]");

IO_REFLECT_IMPL(iodine::i8, "iodine::i8");
IO_REFLECT_IMPL(iodine::i8*, "iodine::i8*");
IO_REFLECT_IMPL(iodine::i8**, "iodine::i8**");
IO_REFLECT_IMPL(iodine::i8&, "iodine::i8&");
IO_REFLECT_IMPL(iodine::i8&&, "iodine::i8&&");
IO_REFLECT_IMPL(iodine::i8[], "iodine::i8[]");

IO_REFLECT_IMPL(iodine::i16, "iodine::i16");
IO_REFLECT_IMPL(iodine::i16*, "iodine::i16*");
IO_REFLECT_IMPL(iodine::i16**, "iodine::i16**");
IO_REFLECT_IMPL(iodine::i16&, "iodine::i16&");
IO_REFLECT_IMPL(iodine::i16&&, "iodine::i16&&");
IO_REFLECT_IMPL(iodine::i16[], "iodine::i16[]");

IO_REFLECT_IMPL(iodine::i32, "iodine::i32");
IO_REFLECT_IMPL(iodine::i32*, "iodine::i32*");
IO_REFLECT_IMPL(iodine::i32**, "iodine::i32**");
IO_REFLECT_IMPL(iodine::i32&, "iodine::i32&");
IO_REFLECT_IMPL(iodine::i32&&, "iodine::i32&&");
IO_REFLECT_IMPL(iodine::i32[], "iodine::i32[]");

IO_REFLECT_IMPL(iodine::i64, "iodine::i64");
IO_REFLECT_IMPL(iodine::i64*, "iodine::i64*");
IO_REFLECT_IMPL(iodine::i64**, "iodine::i64**");
IO_REFLECT_IMPL(iodine::i64&, "iodine::i64&");
IO_REFLECT_IMPL(iodine::i64&&, "iodine::i64&&");
IO_REFLECT_IMPL(iodine::i64[], "iodine::i64[]");

IO_REFLECT_IMPL(iodine::f32, "iodine::f32");
IO_REFLECT_IMPL(iodine::f32*, "iodine::f32*");
IO_REFLECT_IMPL(iodine::f32**, "iodine::f32**");
IO_REFLECT_IMPL(iodine::f32&, "iodine::f32&");
IO_REFLECT_IMPL(iodine::f32&&, "iodine::f32&&");
IO_REFLECT_IMPL(iodine::f32[], "iodine::f32[]");

IO_REFLECT_IMPL(iodine::f64, "iodine::f64");
IO_REFLECT_IMPL(iodine::f64*, "iodine::f64*");
IO_REFLECT_IMPL(iodine::f64**, "iodine::f64**");
IO_REFLECT_IMPL(iodine::f64&, "iodine::f64&");
IO_REFLECT_IMPL(iodine::f64&&, "iodine::f64&&");
IO_REFLECT_IMPL(iodine::f64[], "iodine::f64[]");

IO_REFLECT_IMPL(iodine::byte, "iodine::byte");
IO_REFLECT_IMPL(iodine::byte*, "iodine::byte*");
IO_REFLECT_IMPL(iodine::byte**, "iodine::byte**");
IO_REFLECT_IMPL(iodine::byte&, "iodine::byte&");
IO_REFLECT_IMPL(iodine::byte&&, "iodine::byte&&");
IO_REFLECT_IMPL(iodine::byte[], "iodine::byte[]");

/* Const types */

IO_REFLECT_IMPL(const iodine::u8, "const iodine::u8");
IO_REFLECT_IMPL(const iodine::u8*, "const iodine::u8*");
IO_REFLECT_IMPL(const iodine::u8**, "const iodine::u8**");
IO_REFLECT_IMPL(const iodine::u8&, "const iodine::u8&");
IO_REFLECT_IMPL(const iodine::u8&&, "const iodine::u8&&");
IO_REFLECT_IMPL(const iodine::u8[], "const iodine::u8[]");

IO_REFLECT_IMPL(const iodine::u16, "const iodine::u16");
IO_REFLECT_IMPL(const iodine::u16*, "const iodine::u16*");
IO_REFLECT_IMPL(const iodine::u16**, "const iodine::u16**");
IO_REFLECT_IMPL(const iodine::u16&, "const iodine::u16&");
IO_REFLECT_IMPL(const iodine::u16&&, "const iodine::u16&&");
IO_REFLECT_IMPL(const iodine::u16[], "const iodine::u16[]");

IO_REFLECT_IMPL(const iodine::u32, "const iodine::u32");
IO_REFLECT_IMPL(const iodine::u32*, "const iodine::u32*");
IO_REFLECT_IMPL(const iodine::u32**, "const iodine::u32**");
IO_REFLECT_IMPL(const iodine::u32&, "const iodine::u32&");
IO_REFLECT_IMPL(const iodine::u32&&, "const iodine::u32&&");
IO_REFLECT_IMPL(const iodine::u32[], "const iodine::u32[]");

IO_REFLECT_IMPL(const iodine::u64, "const iodine::u64");
IO_REFLECT_IMPL(const iodine::u64*, "const iodine::u64*");
IO_REFLECT_IMPL(const iodine::u64**, "const iodine::u64**");
IO_REFLECT_IMPL(const iodine::u64&, "const iodine::u64&");
IO_REFLECT_IMPL(const iodine::u64&&, "const iodine::u64&&");
IO_REFLECT_IMPL(const iodine::u64[], "const iodine::u64[]");

IO_REFLECT_IMPL(const iodine::i8, "const iodine::i8");
IO_REFLECT_IMPL(const iodine::i8*, "const iodine::i8*");
IO_REFLECT_IMPL(const iodine::i8**, "const iodine::i8**");
IO_REFLECT_IMPL(const iodine::i8&, "const iodine::i8&");
IO_REFLECT_IMPL(const iodine::i8&&, "const iodine::i8&&");
IO_REFLECT_IMPL(const iodine::i8[], "const iodine::i8[]");

IO_REFLECT_IMPL(const iodine::i16, "const iodine::i16");
IO_REFLECT_IMPL(const iodine::i16*, "const iodine::i16*");
IO_REFLECT_IMPL(const iodine::i16**, "const iodine::i16**");
IO_REFLECT_IMPL(const iodine::i16&, "const iodine::i16&");
IO_REFLECT_IMPL(const iodine::i16&&, "const iodine::i16&&");
IO_REFLECT_IMPL(const iodine::i16[], "const iodine::i16[]");

IO_REFLECT_IMPL(const iodine::i32, "const iodine::i32");
IO_REFLECT_IMPL(const iodine::i32*, "const iodine::i32*");
IO_REFLECT_IMPL(const iodine::i32**, "const iodine::i32**");
IO_REFLECT_IMPL(const iodine::i32&, "const iodine::i32&");
IO_REFLECT_IMPL(const iodine::i32&&, "const iodine::i32&&");
IO_REFLECT_IMPL(const iodine::i32[], "const iodine::i32[]");

IO_REFLECT_IMPL(const iodine::i64, "const iodine::i64");
IO_REFLECT_IMPL(const iodine::i64*, "const iodine::i64*");
IO_REFLECT_IMPL(const iodine::i64**, "const iodine::i64**");
IO_REFLECT_IMPL(const iodine::i64&, "const iodine::i64&");
IO_REFLECT_IMPL(const iodine::i64&&, "const iodine::i64&&");
IO_REFLECT_IMPL(const iodine::i64[], "const iodine::i64[]");

IO_REFLECT_IMPL(const iodine::f32, "const iodine::f32");
IO_REFLECT_IMPL(const iodine::f32*, "const iodine::f32*");
IO_REFLECT_IMPL(const iodine::f32**, "const iodine::f32**");
IO_REFLECT_IMPL(const iodine::f32&, "const iodine::f32&");
IO_REFLECT_IMPL(const iodine::f32&&, "const iodine::f32&&");
IO_REFLECT_IMPL(const iodine::f32[], "const iodine::f32[]");

IO_REFLECT_IMPL(const iodine::f64, "const iodine::f64");
IO_REFLECT_IMPL(const iodine::f64*, "const iodine::f64*");
IO_REFLECT_IMPL(const iodine::f64**, "const iodine::f64**");
IO_REFLECT_IMPL(const iodine::f64&, "const iodine::f64&");
IO_REFLECT_IMPL(const iodine::f64&&, "const iodine::f64&&");
IO_REFLECT_IMPL(const iodine::f64[], "const iodine::f64[]");

IO_REFLECT_IMPL(const iodine::byte, "const iodine::byte");
IO_REFLECT_IMPL(const iodine::byte*, "const iodine::byte*");
IO_REFLECT_IMPL(const iodine::byte**, "const iodine::byte**");
IO_REFLECT_IMPL(const iodine::byte&, "const iodine::byte&");
IO_REFLECT_IMPL(const iodine::byte&&, "const iodine::byte&&");
IO_REFLECT_IMPL(const iodine::byte[], "const iodine::byte[]");