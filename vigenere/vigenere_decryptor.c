#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

void print_mem_err() {
    printf("Error occurred when allocating memory\n");
    exit(1);
}

char shift_by_n(int original, int n_shift) {
    // note: original is cast to int to avoid overflow
    // during the shifting process
    n_shift %= 26;
    if ((original >= 'a') && (original <= 'z')) {
        original += n_shift;
        if (original < 'a') {
            original += 26;
        } else if (original > 'z') {
            original -= 26;
        }
    }
    return original;
}

int main() {
    const double eng_fd[26] = {
        14810/40000.,  2715/40000.,  4943/40000.,  7874/40000.,
        21912/40000.,  4200/40000.,  3693/40000., 10795/40000.,
        13318/40000.,   188/40000.,  1257/40000.,  7253/40000.,
         4761/40000., 12666/40000., 14003/40000.,  3316/40000.,
          205/40000., 10977/40000., 11450/40000., 16587/40000.,
         5246/40000.,  2019/40000.,  3819/40000.,   315/40000.,
         3853/40000.,   128/40000.,
    };

    int param_avail;
    printf("SELECT MODE\n");
    printf("(1) Decrypt with keyword (2) Crack with keyword length (3) Crack by Kasiski examination \n");
    scanf("%d", &param_avail);

    if (param_avail == 1) {
        char keyword[26];
        printf("Original keyword (lowercase letters only, max 25 characters): ");
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
        in_ptr = fopen("./encrypted.txt", "r");
        out_ptr = fopen("./decrypted.txt", "w");

        if (in_ptr && out_ptr) {
            int ch;
            int curr_keyletter = 0;
            while ((ch = fgetc(in_ptr)) != EOF) {
                if ((ch >= 'a') && (ch <= 'z')) {
                    ch -= (keyword[curr_keyletter] - 'a');
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
    } else if (param_avail == 2) {
        printf("mode currently unsupported");
        /*
        int keyword_len;
        printf("Original keyword length (max 25): ");
        scanf("%d", &keyword_len);

        if ((keyword_len < 1) || (keyword_len > 25)) {
            printf("WARNING: length out of bounds\n");
            if (keyword_len < 1) {
                keyword_len = 1;
            } else {
                keyword_len = 25;
            }
        }

        // let n = keyword_len. We perform frequency analysis on the 1st, (n+1)th, (2n+1)th, ... letters as a group,
        // then 2nd, (n+2)th, and so on, as we would with the Caesar cipher.
        // This way we can guess each letter of the keyword individually.
        // After we guess all letters of the keyword we may decode as above.

        FILE *in_ptr, *out_ptr;
        in_ptr = fopen("./encrypted.txt", "r");
        out_ptr = fopen("./decrypted.txt", "w");

        if (in_ptr && out_ptr) {
            int *best_chi_sq_shift = (int *) calloc(keyword_len, sizeof(int));
            if (best_chi_sq_shift == NULL) {
                fclose(in_ptr);
                fclose(out_ptr);
                print_mem_err();
            }

            double **sample_fd_list = (double **) calloc(keyword_len, sizeof(double *));
            if (sample_fd_list == NULL) {
                free(best_chi_sq_shift);
                fclose(in_ptr);
                fclose(out_ptr);
                print_mem_err();
            }
            for (int i = 0; i < keyword_len; i++) {
                sample_fd_list[i] = (double *) calloc(26, sizeof(double));
                if (sample_fd_list[i] == NULL) {
                    free(best_chi_sq_shift);
                    fclose(in_ptr);
                    fclose(out_ptr);
                    print_mem_err();
                }
            }

            int total_chars = 0;
            
            int ch;
            int keyword_index = 0;
            int debug_print = 0;
            while ((ch = fgetc(in_ptr)) != EOF) {
                ch -= 'a';
                if ((ch >= 0) && (ch < 26)) {
                    if (debug_print < 20) {
                        printf("Letter %c goes to keyword index %d\n", ch + 'a', keyword_index);
                        debug_print++;
                    }
                    sample_fd_list[keyword_index][ch] += 1.0;
                    keyword_index++;
                    if (keyword_index >= keyword_len) {
                        keyword_index = 0;
                        total_chars++;
                    }
                }
            }
            printf("total_chars = %d, keyword_index = %d\n", total_chars, keyword_index);

            if (total_chars == 0) {
                printf("Sample text is not long enough to analyze.\n");
                free(best_chi_sq_shift);
                for (int i = 0; i < keyword_len; i++) {
                    free(sample_fd_list[i]);
                }
                free(sample_fd_list);
                fclose(in_ptr);
                fclose(out_ptr);
                return 0;
            } else {
                for (int i = 0; i < keyword_index; i++) {
                    for (int j = 0; j < 26; j++) {
                        sample_fd_list[i][j] /= (total_chars + 1);
                    }
                }
                for (int i = keyword_index; i < keyword_len; i++) {
                    for (int j = 0; j < 26; j++) {
                        sample_fd_list[i][j] /= total_chars;
                    }
                }
            }

            printf("\nCalculating correct shifts...\n");

            for (int i = 0; i < keyword_len; i++) {
                int best_shift = 0;
                double best_chi_sq = DBL_MAX;
                for (int sh = 0; sh < 26; sh++) {
                    double chi_sq = 0.0;
                    for (int j = 0; j < 26; j++) {
                        int shift_j = j - sh;
                        if (shift_j < 0) shift_j += 26;
                        chi_sq += pow(sample_fd_list[i][shift_j] - eng_fd[j], 2)/eng_fd[j];
                    }
                    if (chi_sq < best_chi_sq) {
                        best_chi_sq = chi_sq;
                        best_shift = sh;
                    }
                }
                best_chi_sq_shift[i] = best_shift;
                printf("Letter %.2d = %c (X^2 = %e)\n", (i+1), ('z' - best_shift + 1), best_chi_sq);
            }

            rewind(in_ptr);

            char new_ch;
            keyword_index = 0;
            while ((new_ch = fgetc(in_ptr)) != EOF) {
                new_ch = shift_by_n(new_ch, best_chi_sq_shift[keyword_index]);
                fputc(new_ch, out_ptr);
                keyword_index++;
                if (keyword_index >= keyword_len) {
                    keyword_index = 0;
                }
            }

            printf("\nText deciphered.\n");

            free(best_chi_sq_shift);
            for (int i = 0; i < keyword_len; i++) {
                free(sample_fd_list[i]);
            }
            free(sample_fd_list);
        } else {
            printf("Error occurred when opening a file\n");
            exit(1);
        }

        fclose(in_ptr);
        fclose(out_ptr);
        */
    } else if (param_avail == 3) {
        printf("Mode currently unsupported\n");
    }

    return(0);

}
