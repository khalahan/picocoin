
#include "picocoin-config.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <jansson.h>
#include <glib.h>
#include <ccoin/script.h>
#include <ccoin/core.h>
#include "libtest.h"

static void test_script(bool is_valid,GString *scriptSig, GString *scriptPubKey,
			unsigned int idx, const char *scriptSigEnc,
			const char *scriptPubKeyEnc)
{
	struct bp_tx tx;
	static const unsigned int test_flags =
		SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC;

	bp_tx_init(&tx);

	bool rc;
	rc = bp_script_verify(scriptSig, scriptPubKey, &tx, 0,
			      test_flags, SIGHASH_NONE);
	if (rc != is_valid) {
		fprintf(stderr,
			"script: %sis_valid test %u failed\n"
			"script: [\"%s\", \"%s\"]\n",
			is_valid ? "" : "!",
			idx, scriptSigEnc, scriptPubKeyEnc);
		assert(rc == is_valid);
	}

	bp_tx_free(&tx);
}

static void runtest(bool is_valid, const char *basefn)
{
	char *fn = test_filename(basefn);
	json_t *tests = read_json(fn);
	assert(json_is_array(tests));

	unsigned int idx;
	for (idx = 0; idx < json_array_size(tests); idx++) {
		json_t *test = json_array_get(tests, idx);
		assert(json_is_array(test));

		const char *scriptSigEnc =
			json_string_value(json_array_get(test, 0));
		const char *scriptPubKeyEnc =
			json_string_value(json_array_get(test, 1));
		assert(scriptSigEnc != NULL);
		assert(scriptPubKeyEnc != NULL);

		GString *scriptSig = parse_script_str(scriptSigEnc);
		GString *scriptPubKey = parse_script_str(scriptPubKeyEnc);
		assert(scriptSig != NULL);
		assert(scriptPubKey != NULL);

		test_script(is_valid, scriptSig, scriptPubKey,
			    idx, scriptSigEnc, scriptPubKeyEnc);

		g_string_free(scriptSig, TRUE);
		g_string_free(scriptPubKey, TRUE);
	}

	json_decref(tests);
	free(fn);
}

int main (int argc, char *argv[])
{
	runtest(true, "script_valid.json");
	runtest(false, "script_invalid.json");
	return 0;
}

