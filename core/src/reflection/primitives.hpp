#pragma once

#include "reflection/reflect.hpp"

/* Non-const types */

IO_REFLECT(iodine::u8, "iodine::u8");
IO_REFLECT(iodine::u8*, "iodine::u8*");
IO_REFLECT(iodine::u8**, "iodine::u8**");
IO_REFLECT(iodine::u8&, "iodine::u8&");
IO_REFLECT(iodine::u8&&, "iodine::u8&&");
IO_REFLECT(iodine::u8[], "iodine::u8[]");

IO_REFLECT(iodine::u16, "iodine::u16");
IO_REFLECT(iodine::u16*, "iodine::u16*");
IO_REFLECT(iodine::u16**, "iodine::u16**");
IO_REFLECT(iodine::u16&, "iodine::u16&");
IO_REFLECT(iodine::u16&&, "iodine::u16&&");
IO_REFLECT(iodine::u16[], "iodine::u16[]");

IO_REFLECT(iodine::u32, "iodine::u32");
IO_REFLECT(iodine::u32*, "iodine::u32*");
IO_REFLECT(iodine::u32**, "iodine::u32**");
IO_REFLECT(iodine::u32&, "iodine::u32&");
IO_REFLECT(iodine::u32&&, "iodine::u32&&");
IO_REFLECT(iodine::u32[], "iodine::u32[]");

IO_REFLECT(iodine::u64, "iodine::u64");
IO_REFLECT(iodine::u64*, "iodine::u64*");
IO_REFLECT(iodine::u64**, "iodine::u64**");
IO_REFLECT(iodine::u64&, "iodine::u64&");
IO_REFLECT(iodine::u64&&, "iodine::u64&&");
IO_REFLECT(iodine::u64[], "iodine::u64[]");

IO_REFLECT(iodine::i8, "iodine::i8");
IO_REFLECT(iodine::i8*, "iodine::i8*");
IO_REFLECT(iodine::i8**, "iodine::i8**");
IO_REFLECT(iodine::i8&, "iodine::i8&");
IO_REFLECT(iodine::i8&&, "iodine::i8&&");
IO_REFLECT(iodine::i8[], "iodine::i8[]");

IO_REFLECT(iodine::i16, "iodine::i16");
IO_REFLECT(iodine::i16*, "iodine::i16*");
IO_REFLECT(iodine::i16**, "iodine::i16**");
IO_REFLECT(iodine::i16&, "iodine::i16&");
IO_REFLECT(iodine::i16&&, "iodine::i16&&");
IO_REFLECT(iodine::i16[], "iodine::i16[]");

IO_REFLECT(iodine::i32, "iodine::i32");
IO_REFLECT(iodine::i32*, "iodine::i32*");
IO_REFLECT(iodine::i32**, "iodine::i32**");
IO_REFLECT(iodine::i32&, "iodine::i32&");
IO_REFLECT(iodine::i32&&, "iodine::i32&&");
IO_REFLECT(iodine::i32[], "iodine::i32[]");

IO_REFLECT(iodine::i64, "iodine::i64");
IO_REFLECT(iodine::i64*, "iodine::i64*");
IO_REFLECT(iodine::i64**, "iodine::i64**");
IO_REFLECT(iodine::i64&, "iodine::i64&");
IO_REFLECT(iodine::i64&&, "iodine::i64&&");
IO_REFLECT(iodine::i64[], "iodine::i64[]");

IO_REFLECT(iodine::f32, "iodine::f32");
IO_REFLECT(iodine::f32*, "iodine::f32*");
IO_REFLECT(iodine::f32**, "iodine::f32**");
IO_REFLECT(iodine::f32&, "iodine::f32&");
IO_REFLECT(iodine::f32&&, "iodine::f32&&");
IO_REFLECT(iodine::f32[], "iodine::f32[]");

IO_REFLECT(iodine::f64, "iodine::f64");
IO_REFLECT(iodine::f64*, "iodine::f64*");
IO_REFLECT(iodine::f64**, "iodine::f64**");
IO_REFLECT(iodine::f64&, "iodine::f64&");
IO_REFLECT(iodine::f64&&, "iodine::f64&&");
IO_REFLECT(iodine::f64[], "iodine::f64[]");

/* Const types */

IO_REFLECT(const iodine::u8, "const iodine::u8");
IO_REFLECT(const iodine::u8*, "const iodine::u8*");
IO_REFLECT(const iodine::u8**, "const iodine::u8**");
IO_REFLECT(const iodine::u8&, "const iodine::u8&");
IO_REFLECT(const iodine::u8&&, "const iodine::u8&&");
IO_REFLECT(const iodine::u8[], "const iodine::u8[]");

IO_REFLECT(const iodine::u16, "const iodine::u16");
IO_REFLECT(const iodine::u16*, "const iodine::u16*");
IO_REFLECT(const iodine::u16**, "const iodine::u16**");
IO_REFLECT(const iodine::u16&, "const iodine::u16&");
IO_REFLECT(const iodine::u16&&, "const iodine::u16&&");
IO_REFLECT(const iodine::u16[], "const iodine::u16[]");

IO_REFLECT(const iodine::u32, "const iodine::u32");
IO_REFLECT(const iodine::u32*, "const iodine::u32*");
IO_REFLECT(const iodine::u32**, "const iodine::u32**");
IO_REFLECT(const iodine::u32&, "const iodine::u32&");
IO_REFLECT(const iodine::u32&&, "const iodine::u32&&");
IO_REFLECT(const iodine::u32[], "const iodine::u32[]");

IO_REFLECT(const iodine::u64, "const iodine::u64");
IO_REFLECT(const iodine::u64*, "const iodine::u64*");
IO_REFLECT(const iodine::u64**, "const iodine::u64**");
IO_REFLECT(const iodine::u64&, "const iodine::u64&");
IO_REFLECT(const iodine::u64&&, "const iodine::u64&&");
IO_REFLECT(const iodine::u64[], "const iodine::u64[]");

IO_REFLECT(const iodine::i8, "const iodine::i8");
IO_REFLECT(const iodine::i8*, "const iodine::i8*");
IO_REFLECT(const iodine::i8**, "const iodine::i8**");
IO_REFLECT(const iodine::i8&, "const iodine::i8&");
IO_REFLECT(const iodine::i8&&, "const iodine::i8&&");
IO_REFLECT(const iodine::i8[], "const iodine::i8[]");

IO_REFLECT(const iodine::i16, "const iodine::i16");
IO_REFLECT(const iodine::i16*, "const iodine::i16*");
IO_REFLECT(const iodine::i16**, "const iodine::i16**");
IO_REFLECT(const iodine::i16&, "const iodine::i16&");
IO_REFLECT(const iodine::i16&&, "const iodine::i16&&");
IO_REFLECT(const iodine::i16[], "const iodine::i16[]");

IO_REFLECT(const iodine::i32, "const iodine::i32");
IO_REFLECT(const iodine::i32*, "const iodine::i32*");
IO_REFLECT(const iodine::i32**, "const iodine::i32**");
IO_REFLECT(const iodine::i32&, "const iodine::i32&");
IO_REFLECT(const iodine::i32&&, "const iodine::i32&&");
IO_REFLECT(const iodine::i32[], "const iodine::i32[]");

IO_REFLECT(const iodine::i64, "const iodine::i64");
IO_REFLECT(const iodine::i64*, "const iodine::i64*");
IO_REFLECT(const iodine::i64**, "const iodine::i64**");
IO_REFLECT(const iodine::i64&, "const iodine::i64&");
IO_REFLECT(const iodine::i64&&, "const iodine::i64&&");
IO_REFLECT(const iodine::i64[], "const iodine::i64[]");

IO_REFLECT(const iodine::f32, "const iodine::f32");
IO_REFLECT(const iodine::f32*, "const iodine::f32*");
IO_REFLECT(const iodine::f32**, "const iodine::f32**");
IO_REFLECT(const iodine::f32&, "const iodine::f32&");
IO_REFLECT(const iodine::f32&&, "const iodine::f32&&");
IO_REFLECT(const iodine::f32[], "const iodine::f32[]");

IO_REFLECT(const iodine::f64, "const iodine::f64");
IO_REFLECT(const iodine::f64*, "const iodine::f64*");
IO_REFLECT(const iodine::f64**, "const iodine::f64**");
IO_REFLECT(const iodine::f64&, "const iodine::f64&");
IO_REFLECT(const iodine::f64&&, "const iodine::f64&&");
IO_REFLECT(const iodine::f64[], "const iodine::f64[]");