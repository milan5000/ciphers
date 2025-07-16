#include <stdio.h>
#include <stdlib.h>

int main() {
    int param_avail;
    printf("SELECT MODE\n");
    printf("(1) Decrypt with parameter (2) Attempt to crack without parameter\n");
    scanf("%d", &param_avail);

    if (param_avail == 1) {
        int n_shift;
        printf("Original input shift parameter: ");
        scanf("%d", &n_shift);

        n_shift %= 26;

        FILE *in_ptr, *out_ptr;
        in_ptr = fopen("./encrypted.txt", "r");
        out_ptr = fopen("./decrypted.txt", "w");

        if (in_ptr && out_ptr) {
            int ch;
            while ((ch = fgetc(in_ptr)) != EOF) {
                if ((ch >= 'a') && (ch <= 'z')) {
                    ch -= n_shift;
                    if (ch < 'a') {
                        ch += 26;
                    } else if (ch > 'z') {
                        ch -= 26;
                    }
                }
                fputc(ch, out_ptr);
            }
        } else {
            printf("Error occurred when opening a file\n");
            exit(1);
        }

        fclose(in_ptr);
        fclose(out_ptr);

    } else {
        printf("Unimplemented currently\n");
        exit(1);

        // TODO: add two ways of breaking the cipher:
        // 1. brute force:
        //   cycle through shifts 1-26, showing user sample of ~30 initial characters;
        //   user may select whichever looks correct, at which point entire file is deciphered;
        // 2. frequency analysis
        //   perform frequency analysis on the text and pick the best option
    }

    return(0);
}
