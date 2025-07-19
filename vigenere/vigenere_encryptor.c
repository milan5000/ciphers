#include <stdio.h>
#include <stdlib.h>

int main() {
    char keyword[26];
    printf("Input keyword (lowercase letters only, max 25 characters): ");
    scanf("%25s", keyword);

    while (fgetc(stdin) != '\n'); // flush stdin if user entered >25 chars and/or chars after space

    // Check whether keyword is valid (lowercase only)
    if (keyword[0] == '\0') {
        printf("Keyword is blank; please try again\n");
        return 0;
    }

    for (int i = 0; i < 26; i++) {
        if (keyword[i] == '\0') break;
        if ((keyword[i] < 'a') || (keyword[i] > 'z')) {
            printf("Invalid characters in input; please try again\n");
            return 0;
        }
    }

    FILE *in_ptr, *out_ptr;
    in_ptr = fopen("./sample_text.txt", "r");
    out_ptr = fopen("./encrypted.txt", "w");

    if (in_ptr && out_ptr) {
        int ch;
        int curr_keyletter = 0;
        while ((ch = fgetc(in_ptr)) != EOF) {
            if ((ch >= 'a') && (ch <= 'z')) {
                ch += keyword[curr_keyletter] - 'a';
                if (ch < 'a') {
                    ch += 26;
                } else if (ch > 'z') {
                    ch -= 26;
                }
            }
            curr_keyletter++;
            if (keyword[curr_keyletter] == '\0') {
                curr_keyletter = 0;
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
