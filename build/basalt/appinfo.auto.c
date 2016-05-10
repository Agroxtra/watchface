#include "pebble_process_info.h"
#include "src/resource_ids.auto.h"

const PebbleProcessInfo __pbl_app_info __attribute__ ((section (".pbl_header"))) = {
  .header = "PBLAPP",
  .struct_version = { PROCESS_INFO_CURRENT_STRUCT_VERSION_MAJOR, PROCESS_INFO_CURRENT_STRUCT_VERSION_MINOR },
  .sdk_version = { PROCESS_INFO_CURRENT_SDK_VERSION_MAJOR, PROCESS_INFO_CURRENT_SDK_VERSION_MINOR },
  .process_version = { 1, 0 },
  .load_size = 0xb6b6,
  .offset = 0xb6b6b6b6,
  .crc = 0xb6b6b6b6,
  .name = "Watchface",
  .company = "martin.zoerfuss@kabelplus.at",
  .icon_resource_id = RESOURCE_ID_IMAGE_PREVIEW,
  .sym_table_addr = 0xA7A7A7A7,
  .flags = PROCESS_INFO_WATCH_FACE,
  .num_reloc_entries = 0xdeadcafe,
  .uuid = { 0x76, 0x75, 0xAB, 0xCE, 0x0F, 0x4F, 0x4E, 0x40, 0x91, 0xB5, 0x57, 0x6A, 0x42, 0x0F, 0x2A, 0x3E },
  .virtual_size = 0xb6b6
};
