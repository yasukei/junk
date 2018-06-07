#include <stdio.h>
#include <stdint.h>

typedef struct _ELFHeader ELFHeader;
struct _ELFHeader
{
	uint8_t EI_MAG0;
	uint8_t EI_MAG1;
	uint8_t EI_MAG2;
	uint8_t EI_MAG3;
	uint8_t EI_CLASS;
	uint8_t EI_DATA;
	uint8_t EI_VERSION;
	uint8_t EI_OSABI;
	uint8_t EI_ABIVERSION;
	uint8_t EI_PAD[7];

	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;

	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;

	uint16_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phetsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

int main(void)
{
	printf("hello loader\n");
	return 0;
}
