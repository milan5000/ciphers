#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

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
    // english letter frequency distribution
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
    printf("(1) Decrypt with parameter (2) Crack by brute force (3) Crack by frequency analysis \n");
    scanf("%d", &param_avail);

    if (param_avail == 1) {
        int n_shift;
        printf("Original input shift parameter: ");
        scanf("%d", &n_shift);

        n_shift %= 26;
        n_shift *= -1; // shift backwards to get original

        FILE *in_ptr, *out_ptr;
        in_ptr = fopen("./encrypted.txt", "r");
        out_ptr = fopen("./decrypted.txt", "w");

        if (in_ptr && out_ptr) {
            char ch;
            while ((ch = fgetc(in_ptr)) != EOF) {
                ch = shift_by_n(ch, n_shift);
                fputc(ch, out_ptr);
            }
            printf("Text decrypted.\n");
        } else {
            printf("Error occurred when opening a file.\n");
            exit(1);
        }

        fclose(in_ptr);
        fclose(out_ptr);

    } else if (param_avail == 2) {
        FILE *in_ptr, *out_ptr;
        in_ptr = fopen("./encrypted.txt", "r");
        out_ptr = fopen("./decrypted.txt", "w");

        if (in_ptr && out_ptr) {
            // get 30 sample characters
            char sample_ch[31];
            char term_ch;
            int idx = 0;

            while ((idx < 30) && ((sample_ch[idx] = fgetc(in_ptr)) != EOF) && (sample_ch[idx] != '\n')) {
                idx++;
            }
            term_ch = sample_ch[idx]; // junk iff idx = 30, otherwise one of EOF or \n depending on what was read
            // save it for use later when deciphering entire text
            sample_ch[idx] = '\0';

            printf("Sample text will be shown to you for each possible shift.\n");
            printf("Input 'y' to accept the current shift as correct, or any other character to continue.\n\n");

            // try shifts from 0-25
            int n_shift = 0;
            char accept;
            while (n_shift < 26) {
                printf("(Shift %.2d) %s | ", n_shift, sample_ch);
                scanf(" %c", &accept);
                if (accept == 'y') break;
                for (short i = 0; i < idx; i++) {
                    sample_ch[i] = shift_by_n(sample_ch[i], 1); 
                }
                n_shift++;
            }

            printf("\n");

            if (accept == 'y') {
                for (short i = 0; i < idx; i++) {
                    fputc(sample_ch[i], out_ptr);
                }

                //printf("%d", n_shift);

                if (idx == 30) { // more characters remain
                    char new_ch;
                    while ((new_ch = fgetc(in_ptr)) != EOF) {
                        //printf("%d %c => ", new_ch, new_ch);
                        new_ch = shift_by_n(new_ch, n_shift);
                        //printf("%d %c\n", new_ch, new_ch);
                        fputc(new_ch, out_ptr);
                    }
                } else if (term_ch == '\n') { // more characters remain, but a newline must be added first
                    fputc('\n', out_ptr);
                    char new_ch;
                    while ((new_ch = fgetc(in_ptr)) != EOF) {
                        new_ch = shift_by_n(new_ch, n_shift);
                        fputc(new_ch, out_ptr);
                    }
                } // else, EOF was reached, no more characters to read

                printf("Full text deciphered.\n");
            } else {
                printf("No satisfactory shift found.\n");
            }
        } else {
            printf("Error occurred when opening a file\n");
            exit(1);
        }

        fclose(in_ptr);
        fclose(out_ptr);
    } else if (param_avail == 3) {
        FILE *in_ptr, *out_ptr;
        in_ptr = fopen("./encrypted.txt", "r");
        out_ptr = fopen("./decrypted.txt", "w");

        if (in_ptr && out_ptr) {
            // Use the chi-square test for goodness of fit to evaluate the frequency distribution for each shift
            // The one which matches best to the english frequency distribution will be picked

            int best_chi_sq_shift = 0;
            double best_chi_sq = DBL_MAX;
            double sample_fd[26] = {0};
            int total_chars = 0;
            
            int ch;
            while ((ch = fgetc(in_ptr)) != EOF) {
                ch -= 'a';
                if ((ch >= 0) && (ch < 26)) {
                    sample_fd[ch] += 1.0;
                    total_chars++;
                }
            }
            for (int i = 0; i < 26; i++) {
                sample_fd[i] /= total_chars;
            }

            for (int sh = 0; sh < 26; sh++) {
                double chi_sq = 0.0;
                for (int i = 0; i < 26; i++) {
                    int shift_i = i - sh;
                    if (shift_i < 0) shift_i += 26;
                    chi_sq += pow(sample_fd[shift_i] - eng_fd[i], 2)/eng_fd[i];
                }
                if (chi_sq < best_chi_sq) {
                    best_chi_sq = chi_sq;
                    best_chi_sq_shift = sh;
                }
            }

            printf("Calculated shift: %d (X^2 = %e)\n", best_chi_sq_shift, best_chi_sq);

            rewind(in_ptr);

            char new_ch;
            while ((new_ch = fgetc(in_ptr)) != EOF) {
                new_ch = shift_by_n(new_ch, best_chi_sq_shift);
                fputc(new_ch, out_ptr);
            }

            printf("Text deciphered.\n");

        } else {
            printf("Error occurred when opening a file\n");
            exit(1);
        }

        fclose(in_ptr);
        fclose(out_ptr);
    }

    return(0);
}
