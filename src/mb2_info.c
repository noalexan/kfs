#include "mb2_info.h"
////////////////////////////////////////////////////////////
// Globals

multiboot_info_t *mb2info = NULL;

// Principal Header
const struct multiboot_header mb2_header
    __attribute__((used, section(".multiboot2_header"), aligned(8))) = {
        .magic         = MULTIBOOT2_HEADER_MAGIC,
        .architecture  = 0,
        .header_length = sizeof(struct multiboot_header) +
                         sizeof(struct multiboot2_header_tag_information_request) +
                         sizeof(struct multiboot2_header_tag_end),
        .checksum = 0x100000000 - (MULTIBOOT2_HEADER_MAGIC + 0 +
                                   (sizeof(struct multiboot_header) +
                                    sizeof(struct multiboot2_header_tag_information_request) +
                                    sizeof(struct multiboot2_header_tag_end)))};

// Tag information request
const struct multiboot2_header_tag_information_request mb2_tag_info_req
    __attribute__((used, section(".multiboot2_header"), aligned(8))) = {
        .type     = 1,
        .flags    = 0,
        .size     = sizeof(struct multiboot2_header_tag_information_request),
        .requests = {MULTIBOOT_TAG_TYPE_MMAP} // 6 = memory map
};

// End Tag
const struct multiboot2_header_tag_end mb2_tag_end
    __attribute__((used, section(".multiboot2_header"), aligned(8))) = {
        .type = 0, .flags = 0, .size = sizeof(struct multiboot2_header_tag_end)};

////////////////////////////////////////////////////////////
// Functions

void mb2_init(uint32_t magic, uint32_t mbi_addr)
{
	/* Make sure the magic number matches for memory mapping*/
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		kpanic("Invalid magic number: expected 0x%x | used 0x%x\n", MULTIBOOT2_BOOTLOADER_MAGIC,
		       magic);
	mb2info = (multiboot_info_t *)mbi_addr;
}

void *mb2_get_tag(uint32_t tag)
{

	// struct multiboot_tag* tag = (struct multiboot_tag*)&mb2info->tags[0];

	// while (tag->type != 0) {

	// }
}
