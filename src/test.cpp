#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "chunk.hpp"
#include "constants.hpp"
#include "context.hpp"
#include "type.hpp"
#include <memory>

TEST_CASE("Type", "[Type]") {
  TypePtr integer_type = std::make_shared<PrimitiveType>();
  CHECK(integer_type->get_size() == WORD_BYTES);
  CHECK(integer_type->get_type_category() == Type::PRIMITIVE);
  CHECK(integer_type->get_primitive_type() == Type::INT);
}

TEST_CASE("Context type retreival", "[Type][Context]") {
  Context context;
  TypePtr type = context.register_type(
      "type1", std::shared_ptr<PrimitiveType>(new PrimitiveType()));
  CHECK(type->get_type_category() == Type::PRIMITIVE);
  CHECK(type->get_primitive_type() == Type::INT);
  TypePtr type_2 = context.resolve_type("type1");
  CHECK(type_2->get_type_category() == Type::PRIMITIVE);
  CHECK(type_2->get_primitive_type() == Type::INT);
}

TEST_CASE("Chunk", "[Chunk][Type]") {
  Context context;
  TypePtr integer_type = std::make_shared<PrimitiveType>();
  ChunkPtr a = context.register_chunk("a", integer_type);
  CHECK(a->get_type() == integer_type);
  CHECK(a->get_chunk_type() == Chunk::GLOBAL);
  CHECK(a->get_offset() == 0);
}

TEST_CASE("Register and resolve Chunk", "[Chunk][Type]") {
  Context context;
  TypePtr integer_type = std::make_shared<PrimitiveType>();
  auto a_1 = context.register_chunk("a", integer_type);
  auto a_2 = context.resolve_chunk("a");
  CHECK(a_1 == a_2);
  CHECK(a_1->get_type() == integer_type);
  CHECK(a_1->get_chunk_type() == Chunk::GLOBAL);
  CHECK(a_1->get_offset() == 0);
  auto b_1 = context.register_chunk("b", integer_type);
  auto b_2 = context.resolve_chunk("b");
  CHECK(b_1 == b_2);
  CHECK(b_1->get_type() == integer_type);
  CHECK(b_1->get_chunk_type() == Chunk::GLOBAL);
  CHECK(b_1->get_offset() == integer_type->get_size());
}

TEST_CASE("Scoping types", "[Scope][Type]") {
  TypePtr type1 = std::shared_ptr<PrimitiveType>(new PrimitiveType());
  TypePtr type2 = std::shared_ptr<PrimitiveType>(new PrimitiveType());
  TypePtr type3 = std::shared_ptr<PrimitiveType>(new PrimitiveType());
  Context context;
  INFO("Scope 0");
  context.register_type("type", type1);
  CHECK(context.resolve_type("type") == type1);
  context.new_scope();
  INFO("Scope 1");
  context.register_type("type", type2);
  CHECK(context.resolve_type("type") == type2);
  context.new_scope();
  INFO("Scope 2");
  context.register_type("type", type3);
  CHECK(context.resolve_type("type") == type3);
  context.del_scope();
  INFO("Scope 1");
  CHECK(context.resolve_type("type") == type2);
  context.del_scope();
  INFO("Scope 0");
  CHECK(context.resolve_type("type") == type1);
}

TEST_CASE("Scoping chunks", "[Scope][Chunk]") {
  TypePtr type = std::shared_ptr<PrimitiveType>(new PrimitiveType());
  Context context;
  INFO("Scope 0");
  auto chunk0 = context.register_chunk("chunk", type);
  unsigned chunksize = chunk0->get_type()->get_size();
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk0);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 0 * chunksize);
  context.new_scope();
  INFO("Scope 0-1");
  auto chunk1 = context.register_chunk("chunk", type);
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk1);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 1 * chunksize);
  context.new_scope();
  INFO("Scope 0-1-2");
  auto chunk2 = context.register_chunk("chunk", type);
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk2);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 2 * chunksize);
  context.del_scope();
  INFO("Scope 0-1");
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk1);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 1 * chunksize);
  auto chunk3 = context.register_chunk("another_chunk", type);
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("another_chunk") == chunk3);
  CHECK(context.resolve_chunk("another_chunk")->get_offset() == 2 * chunksize);
  context.new_scope();
  INFO("Scope 0-1-2");
  auto chunk4 = context.register_chunk("chunk", type);
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk4);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 3 * chunksize);
  CHECK(context.resolve_chunk("another_chunk") == chunk3);
  CHECK(context.resolve_chunk("another_chunk")->get_offset() == 2 * chunksize);
  context.del_scope();
  INFO("Scope 0-1");
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk1);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 1 * chunksize);
  CHECK(context.resolve_chunk("another_chunk") == chunk3);
  CHECK(context.resolve_chunk("another_chunk")->get_offset() == 2 * chunksize);
  context.del_scope();
  INFO("Scope 0");
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk0);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 0 * chunksize);
}

TEST_CASE("Scoping chunks 2", "[Scope][Chunk]") {
  TypePtr type = std::shared_ptr<PrimitiveType>(new PrimitiveType());
  Context context;
  INFO("Scope 0");
  auto chunk0 = context.register_chunk("chunk", type);
  unsigned chunksize = chunk0->get_type()->get_size();
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk0);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 0 * chunksize);
  context.new_scope();
  INFO("Scope 0-1");
  auto chunk1 = context.register_chunk("chunk", type);
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk1);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 1 * chunksize);
  context.new_scope();
  INFO("Scope 0-1-2");
  auto chunk2 = context.register_chunk("chunk", type);
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk2);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 2 * chunksize);
  context.del_scope();
  INFO("Scope 0-1");
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk1);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 1 * chunksize);
  context.new_scope();
  INFO("Scope 0-1-2");
  auto chunk3 = context.register_chunk("chunk", type);
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk3);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 2 * chunksize);
  context.del_scope();
  INFO("Scope 0-1");
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk1);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 1 * chunksize);
  context.del_scope();
  INFO("Scope 0");
  CAPTURE(context.get_stack_size());
  CHECK(context.resolve_chunk("chunk") == chunk0);
  CHECK(context.resolve_chunk("chunk")->get_offset() == 0 * chunksize);
}
TEST_CASE("Loading and storing chunks to registers", "[Load][Store][Chunk]") {}
