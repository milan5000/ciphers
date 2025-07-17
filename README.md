# ciphers

## How to Use

Encryption/decryption programs for different types of cipher are available in the corresponding directory. Use your favorite compiler to test them out. It is recommended to use the C99 standard; other standards have not been tested.

Sample text is provided in `sample_text.txt`, with lowercase characters and whitespace only, and `sample_text_ascii.txt`, with alphanumeric characters, punctuation, and whitespace (text source: [Wikipedia](https://en.wikipedia.org/wiki/Numerical_analysis)). Encrypted text appears in `encrypted.txt` and attempts to decrypt it appear in `decrypted.txt`.

## Available Ciphers

### Caesar Cipher

A simple Caesar cipher. Encryption with a user-provided shift parameter, then decryption with knowledge of that parameter, is available.

Caesar-encoded text can also be deciphered without the shift parameter through 2 other methods:

1. Brute force: cycles through a sample of the text, shifted through all 26 possibilities; the user selects which is correct to decode the entire file.

2. Frequency analysis: assuming the file is sufficiently "average" English text, performs a chi-square goodness of fit test (Pearson's $\chi^2$ test) to identify which shift parameter applied to the text's letter frequency distribution produces the best match for the standard English letter frequency distribution.* The file is then decoded with the identified parameter.

All 3 methods are available by running `caesar/caesar_decryptor.c`.

*Source used: [https://pi.math.cornell.edu/~mec/2003-2004/cryptography/subs/frequencies.html]

### Vigenère Cipher

Coming soon!
