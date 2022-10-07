/**
 * @file windowsrng.h
 * @brief A random bit of code from stackoverflow that seems to generate random numbers.
 * 
 */

#include <cstdlib>
#include <wincrypt.h>

class RandomSequence
{
  HCRYPTPROV hProvider;
public:
  RandomSequence(void) : hProvider(NULL) {
    if (FALSE == CryptAcquireContext(&hProvider, NULL, NULL, PROV_RSA_FULL, 0)) {
      // failed, should we try to create a default provider?
      if (NTE_BAD_KEYSET == GetLastError()) {
        if (FALSE == CryptAcquireContext(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
          // ensure the provider is NULL so we could use a backup plan
          hProvider = NULL;
        }
      }
    }
  }

  ~RandomSequence(void) {
    if (NULL != hProvider) {
      CryptReleaseContext(hProvider, 0U);
    }
  }

  BOOL generate(BYTE* buf, DWORD len) {
    if (NULL != hProvider) {
      return CryptGenRandom(hProvider, len, buf);
    }
    return FALSE;
  }
};