/*
   TEST CASE	: NAND Flash write read 
Description 	: Write random data to the NAND Flash and read back the same data 
Author		: Chandu
 */
#include <config.h>
#include <common.h>
#include <command.h>
#include <linux/types.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <nand.h>

#define NAND_PROGMABLE_AREA 		0x80000
#define MAX_BAD_BLK_LIMIT   		40
#define BUF_LEN				2048

extern nand_info_t nand_info[];

static int check_nand_bad_block(nand_info_t * nand);

int do_nand_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct nand_chip *nand_chip = nand_info[0].priv;
	nand_info_t *nand;
	int count;

	int ret = 0;
	/*********************** ERASE - start ***************************************/
	struct erase_info instr;
	ulong start;
	ulong end = 0x8000000;
	ulong ofs = 0;
	ulong block;
	ulong off;
	size_t size;
	int i = 0;
	int test_passed = 1;
	int p_test_passed = 1;
	u_char *read_buff = (u_char *) malloc(BUF_LEN);
	u_char *write_buff = (u_char *) malloc(BUF_LEN) ;

	nand = &nand_info[nand_curr_device];

	/* Added to check nand bad block */
	ret = check_nand_bad_block(nand);
	if (ret != 0) {
		printf("FAILED\n");
		return -1;
	}

	start = 3;
	end = 10;

	printf("Running Erase Test...Please wait...");
	printf("Start Block : %d, End Block : %d\n", (int)start, (int)end);

	for (i = 0; i < BUF_LEN; i++) {
		write_buff[i] = (unsigned char)i;
	}

	for (block = start; block <= end; ofs++, block++) {
		instr.addr = block << nand_chip->phys_erase_shift;
		instr.len = 1 << nand_chip->phys_erase_shift;
		off = instr.addr;
		size = instr.len;
		ret = nand_erase(nand, off, size);
		if (ret) {
			if (!nand_block_isbad(nand, off)) {
				printf("erase failed at block %d\n", (int)block);
				p_test_passed = test_passed = 0;
			}
			continue;
		}
	}
	/*********************** ERASE - end ***************************************/
	if (p_test_passed) {
		printf("PASSED\n");
	} else
		printf("FAILED\n");

	p_test_passed = 1;
	printf("Running Write test...Please wait...");
	printf("Start Block : %d, End Block : %d\n", (int)start, (int)end);
	for (block = start; block <= end; ofs++, block++) {

		instr.addr = block << nand_chip->phys_erase_shift;
		instr.len = 1 << nand_chip->phys_erase_shift;
		off = instr.addr;
		size = instr.len;
		ret = nand_write(nand, off, &size, (u_char *) write_buff);

		if (ret) {
			printf("Error writing to NAND: ret = %d\n", ret);
			p_test_passed = test_passed = 0;
			continue;
		}
	}

	if (p_test_passed) {
		printf("PASSED\n");
	} else
		printf("FAILED\n");

	p_test_passed = 1;
	printf("Running Read test...Please wait...");
	printf("Start Block : %d, End Block : %d\n", (int)start, (int)end);

	for (block = start; block <= end; ofs++, block++) {

		instr.addr = block << nand_chip->phys_erase_shift;
		instr.len = 1 << nand_chip->phys_erase_shift;
		off = instr.addr;
		size = instr.len;
		nand_read(nand, off, &size, (u_char *) read_buff);

		for (i = 0; i < BUF_LEN; i++) {
			if (read_buff[i] != (unsigned char)i) {
				if (!nand_block_isbad(nand, off)) {
					//printf("write-read failed at offset 0x%x (0x%x)\n", (unsigned int)(block * 0x20000 + i), read_buff[i]);
					p_test_passed = test_passed = 1;
				}
				break;
			}
		}
	}

	if (p_test_passed) {
		printf("PASSED\n");
	} else {
		printf("FAILED\n");
	}

	/*********************** ERASE - start ***************************************/
	printf("Erase nand after completing the Nand write read test... \n");
	{
		struct erase_info instr;
		ulong start;
		ulong end = 0x8000000;
		ulong ofs = 0;
		ulong block;
		ulong off, size;
		int i = 0;
		u_char *write_buff = (u_char *) malloc(BUF_LEN);

		nand = &nand_info[nand_curr_device];

		start = 3;
		end = 10;

		printf("Running Erase Test...Please wait...");
		printf("Start Block : %d, End Block : %d\n", (int)start, (int)end);

		for (i = 0; i < BUF_LEN; i++) {
			write_buff[i] = (unsigned char)i;
		}
		for (block = start; block <= end; ofs++, block++) {
			instr.addr = block << nand_chip->phys_erase_shift;
			instr.len = 1 << nand_chip->phys_erase_shift;
			off = instr.addr;
			size = instr.len;
			ret = nand_erase(nand, off, size);
			if (ret) {
				if (!nand_block_isbad(nand, off)) {
					printf("erase failed at block %d\n", (int)block);
					p_test_passed = test_passed = 0;
				}
				continue;
			}
		}
	}
	/*********************** ERASE - end ***************************************/
	printf("Nand Test Completed... %s\n", ((test_passed) ? "PASS" : "FAIL"));
	return 0;
}

static int check_nand_bad_block(nand_info_t * nand)
{
	unsigned int off;
	unsigned int bad_blk_cnt = 0;
	for (off = 0; off < nand->size; off += nand->erasesize) {
		if (nand_block_isbad(nand, off)) {
			printf("  %08lx\n", (long unsigned int)off);
			bad_blk_cnt++;

			if (off <= NAND_PROGMABLE_AREA) {
				printf("ERROR: NAND bad block exists in programmable area (offset %08lx)\n", (long unsigned int)off);
				return -1;
			}
		}

		if (bad_blk_cnt >= MAX_BAD_BLK_LIMIT) {
			printf("ERROR: Bad NAND Block Count exceeds MAX_BAD_BLK_LIMIT (%d)\n", MAX_BAD_BLK_LIMIT);
			return -1;
		}
	}
	printf("Bad NAND Block Count %d\n", bad_blk_cnt);
	return 0;
}


U_BOOT_CMD(
		nandtest, 2,  1, do_nand_test,
		"TESTING NAND\n",
		" - writes the STR into NAND read back same "
	  );
