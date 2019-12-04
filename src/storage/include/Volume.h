#pragma once

namespace storage {

namespace Volume {
static constexpr auto m_basePath = "/vol";
static constexpr auto m_optPath = "/opt";

void Init();
void Deinit();

bool FormatAllPartitions();
bool FormatPartition( const char* label );

}  // namespace Volume

}  // namespace storage
