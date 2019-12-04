#include "Volume.h"
#include <DebugUtils.h>
#include <string.h>
#include <vector>
#include <string>
#include <StoragePartitionLabel.h>

namespace storage {

namespace Volume {

bool MountSpiffsVolume( const char* _partition_label,
                        const char* _base_path,
                        const unsigned _max_files,
                        const bool _format_if_mount_failed ) {
    return true;
}

bool UnmountSpiffsVolume( const char* _partition_label ) {
    return true;
}

bool MountLittleFsVolume( const char* _partition_label, const char* _base_path, const bool _format_if_mount_failed ) {
    return true;
}

bool UmountLittleFsVolume( const char* _partition_label ) {
    return true;
}

void Init() {

}

void Deinit() {
    UmountLittleFsVolume( PartitionLabel::OPT );
    UmountLittleFsVolume( PartitionLabel::STORAGE );
}

bool FormatAllPartitions() {  // We must be sure that no one else read/write volume
    return true;
}

bool FormatPartition( const char* label ) {
    return true;
}

}  // namespace Volume

}  // namespace storage
