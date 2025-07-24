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

    int mode_param;
    printf("Choose whether to (1, default) encrypt or (2) decrypt: ");
    scanf("%d", &mode_param);

    FILE *in_ptr, *out_ptr;
    if (mode_param == 2) {
        in_ptr = fopen("./encrypted.txt", "r");
        out_ptr = fopen("./decrypted.txt", "w");
    } else {
        in_ptr = fopen("./sample_text.txt", "r");
        out_ptr = fopen("./encrypted.txt", "w");
    }

    if (in_ptr && out_ptr) {
        // construct key table
        // note: i/j treated as interchangeable

        char key_table[5][5];
        char *key_line = (char *) key_table;
        int visited[25] = { };
        int indices_map[26] = { };
        int table_idx = 0;

        for (int i = 0; i < 26; i++) {
            if (keyword[i] == '\0') break;
            if (keyword[i] < 'j') {
                if (visited[keyword[i] - 'a'] == 0) {
                    visited[keyword[i] - 'a'] = 1;
                    key_line[table_idx] = keyword[i];
                    indices_map[keyword[i] - 'a'] = table_idx;
                    table_idx++;
                }
            } else {
                if (visited[keyword[i] - 'a' - 1] == 0) {
                    visited[keyword[i] - 'a' - 1] = 1;
                    if (keyword[i] == 'j') {
                        key_line[table_idx] = 'i';
                    } else {
                        key_line[table_idx] = keyword[i];
                    }
                    indices_map[keyword[i] - 'a'] = table_idx;
                    table_idx++;
                }
            }
        }

        for (int i = 0; i < 25; i++) {
            if (visited[i] == 0) {
                if ('j' - 'a' > i) {
                    key_line[table_idx] = 'a' + i;
                    indices_map[i] = table_idx;
                } else {
                    key_line[table_idx] = 'a' + i + 1;
                    indices_map[i + 1] = table_idx;
                }
                table_idx++;
            }
        }

        // make sure i and j both map to the same index
        if (indices_map['i'-'a']) {
            indices_map['j'-'a'] = indices_map['i'-'a'];
        }
        indices_map['i'-'a'] = indices_map['j'-'a'];

        if (table_idx != 25) {
            printf("There's a bug in your code buddy\n");
            fclose(in_ptr);
            fclose(out_ptr);
            exit(1);
        }
        
        int ch1;
        int ch2;
        int ch_extra = '0';
        int ch1_loc;
        int ch2_loc;
        int ch_buf[256];
        int ch_buf_pt = 0;
        int cont = 1;
        while (cont) {
            if (ch_extra != '0') {
                ch1 = ch_extra;
                ch_extra = '0';
            } else {
                while (((ch1 = fgetc(in_ptr)) != EOF) && ((ch1 < 'a') || (ch1 > 'z'))) {
                    fputc(ch1, out_ptr);
                };
            }

            if (ch1 == EOF) {
                cont = 0;
            } else { // apply encryption/decryption rules
                while (((ch2 = fgetc(in_ptr)) != EOF) && ((ch2 < 'a') || (ch2 > 'z'))) {
                    ch_buf[ch_buf_pt] = ch2;
                    ch_buf_pt++;
                }
                // rule 1: add 'q' (or 'x') if ch2 EOF or same as ch1
                if (mode_param == 2) {
                    if (ch2 == EOF) {
                        // should never happen - correct playfair encryption
                        // will produce an even number of encrypted characters
                        // so we report the error and quit
                        printf("Unexpected odd number of encrypted characters\n");
                        fclose(in_ptr);
                        fclose(out_ptr);
                        exit(1);
                    }
                    // note, we skip the rest of the stuff done during encryption
                    // since rule 1 doesn't apply
                    // junk characters can be cleaned after decryption
                } else {
                    if (ch2 == EOF) {
                        cont = 0;
                        ch2 = 'q';
                        if (ch1 == 'q') ch2 = 'x';
                    } else if (ch2 == ch1) {
                        ch_extra = ch2;
                        ch2 = 'q';
                        if (ch1 == 'q') ch2 = 'x';
                    }
                }

                ch1_loc = indices_map[ch1 - 'a'];
                ch2_loc = indices_map[ch2 - 'a'];
                if (ch1_loc - (ch1_loc % 5) == ch2_loc - (ch2_loc % 5)) {
                    // same row so apply rule 2:
                    // for encryption: replace with letters to immediate right, wrap around if needed
                    // for decryption: opposite direction
                    if (mode_param == 2) {
                        ch1_loc--;
                        if ((ch1_loc + 5) % 5 == 4) ch1_loc += 5;
                        ch2_loc--;
                        if ((ch2_loc + 5) % 5 == 4) ch2_loc += 5;
                        // +5 is added to avoid the edge case where ch1_loc or ch2_loc = -1
                        // then -1 % 5 == -1 in C, not 4 as intended, and the index
                        // does not get correctly moved back into the original row
                    } else {
                        ch1_loc++;
                        if (ch1_loc % 5 == 0) ch1_loc -= 5;
                        ch2_loc++;
                        if (ch2_loc % 5 == 0) ch2_loc -= 5;
                    }
                } else if (ch1_loc % 5 == ch2_loc % 5) {
                    // same column so apply rule 3:
                    // for encryption: replace with letters immediately below, wrap around if needed
                    // for decryption: opposite direction
                    if (mode_param == 2) {
                        ch1_loc -= 5;
                        if (ch1_loc < 0) ch1_loc += 25;
                        ch2_loc -= 5;
                        if (ch2_loc < 0) ch2_loc += 25;
                    } else {
                        ch1_loc += 5;
                        if (ch1_loc >= 25) ch1_loc -= 25;
                        ch2_loc += 5;
                        if (ch2_loc >= 25) ch2_loc -= 25;
                    }
                } else {
                    // letters aren't on same row or column
                    // apply rule 4: replace with letters at other corners of rectangle
                    // formed by the original letters
                    // same for encryption/decryption
                    int temp = ch2_loc % 5;
                    ch2_loc = ch2_loc - temp + (ch1_loc % 5);
                    ch1_loc = ch1_loc - (ch1_loc % 5) + temp;
                }

                fputc(key_line[ch1_loc], out_ptr);
                for (int i = 0; i < ch_buf_pt; i++) {
                    fputc(ch_buf[i], out_ptr);
                }
                ch_buf_pt = 0;
                fputc(key_line[ch2_loc], out_ptr);
            }
        }
    } else {
        printf("Error occurred when opening a file\n");
        exit(1);
    }

    fclose(in_ptr);
    fclose(out_ptr);

    return(0);
}
