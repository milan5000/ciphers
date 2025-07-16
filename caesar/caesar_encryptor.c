#include <stdio.h>
#include <stdlib.h>

int main() {
    int n_shift;
    printf("Input shift parameter: ");
    scanf("%d", &n_shift);

    n_shift %= 26;

    FILE *in_ptr, *out_ptr;
    in_ptr = fopen("./sample_text.txt", "r");
    out_ptr = fopen("./encrypted.txt", "w");

    if (in_ptr && out_ptr) {
        int ch;
        while ((ch = fgetc(in_ptr)) != EOF) {
            if ((ch >= 'a') && (ch <= 'z')) {
                ch += n_shift;
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
    return(0);
}
