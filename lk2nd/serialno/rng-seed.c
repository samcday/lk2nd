// SPDX-License-Identifier: BSD-3-Clause

#include <boot.h>
#include <debug.h>
#include <libfdt.h>
#include <scm.h>
#include <string.h>
#include <sys/types.h>

#define RNG_SEED_BYTES 64

static int lk2nd_rng_seed_dt_update(void *dtb, const char *cmdline,
				    enum boot_type boot_type)
{
	uintptr_t rngseed[RNG_SEED_BYTES / sizeof(uintptr_t)];
	unsigned int i;
	int offset, ret;

	if (boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	/* ENABLE_KASLRSEED_SUPPORT is our proxy for scm_random() working here. */
	for (i = 0; i < ARRAY_SIZE(rngseed); i++) {
		ret = scm_random(&rngseed[i], sizeof(rngseed[i]));
		if (ret) {
			dprintf(CRITICAL, "ERROR: Cannot generate rng-seed\n");
			memset(rngseed, 0, sizeof(rngseed));
			return 0;
		}
	}

	offset = fdt_path_offset(dtb, "/chosen");
	if (offset < 0)
		goto out;

	ret = fdt_setprop(dtb, offset, "rng-seed", rngseed, sizeof(rngseed));
	if (ret < 0)
		dprintf(CRITICAL, "ERROR: Cannot update chosen node [rng-seed] - 0x%x\n", ret);

out:
	memset(rngseed, 0, sizeof(rngseed));
	return 0;
}
DEV_TREE_UPDATE(lk2nd_rng_seed_dt_update);
