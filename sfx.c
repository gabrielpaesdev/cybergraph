#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sfx.h"
#define SAMPLE_RATE 44100
#define PI_2 6.283185307179586

void play_tone(FILE *out, double freq, double duration) {
    int total_samples = (int)(SAMPLE_RATE * duration);
    double step = (freq * PI_2) / SAMPLE_RATE;
    for (int i = 0; i < total_samples; i++) {
        double wave = sin(i * step);
        double decay = 1.0 - ((double)i / total_samples);
        fputc((unsigned char)(wave * decay * 127.0 + 128), out);
    }
}

void play_sweep(FILE *out, double start_freq, double end_freq, double duration) {
    int total_samples = (int)(SAMPLE_RATE * duration);
    double phase = 0.0;
    for (int i = 0; i < total_samples; i++) {
        double progress = (double)i / total_samples;
        double freq = start_freq + (end_freq - start_freq) * progress;
        phase += (freq * PI_2) / SAMPLE_RATE;
        double wave = sin(phase);
        double decay = 1.0 - progress;
        fputc((unsigned char)(wave * decay * 127.0 + 128), out);
    }
}

void generate_dual_pluck(FILE *out, double f1, double f2) {
    double duration1 = 0.33;
    double duration2 = 0.38;
    double delay = 0.08;

    int total_samples = (int)(SAMPLE_RATE * (delay + duration2));
    int delay_samples = (int)(SAMPLE_RATE * delay);
    double step1 = (f1 * PI_2) / SAMPLE_RATE;
    double step2 = (f2 * PI_2) / SAMPLE_RATE;
    for (int i = 0; i < total_samples; i++) {
        double wave1 = 0.0;
        double wave2 = 0.0;
        if (i < (int)(SAMPLE_RATE * duration1)) {
            double decay1 = exp(-6.0 * i / SAMPLE_RATE);
            wave1 = sin(i * step1) * decay1;
        }
        if (i >= delay_samples) {
            int j = i - delay_samples;
            if (j < (int)(SAMPLE_RATE * duration2)) {
                double decay2 = exp(-6.0 * j / SAMPLE_RATE);
                wave2 = sin(j * step2) * decay2;
            }
        }
        double mix = (wave1 + wave2) * 0.5;
        mix = mix * 0.9;
        fputc((unsigned char)(mix * 127.0 + 128), out);
    }
}

void generate_pluck_sequence(FILE *out, const double *freqs, const double *delays,
                              const double *durations, const double *decay_rates,
                              const double *amps, int n) {
    double total_duration = 0.0;
    for (int k = 0; k < n; k++) {
        double end = delays[k] + durations[k];
        if (end > total_duration) total_duration = end;
    }

    int total_samples = (int)(SAMPLE_RATE * total_duration);
    double *steps = malloc(n * sizeof(double));
    int *delay_samples = malloc(n * sizeof(int));
    int *dur_samples = malloc(n * sizeof(int));

    for (int k = 0; k < n; k++) {
        steps[k] = (freqs[k] * PI_2) / SAMPLE_RATE;
        delay_samples[k] = (int)(SAMPLE_RATE * delays[k]);
        dur_samples[k] = (int)(SAMPLE_RATE * durations[k]);
    }

    for (int i = 0; i < total_samples; i++) {
        double mix = 0.0;
        for (int k = 0; k < n; k++) {
            if (i >= delay_samples[k]) {
                int j = i - delay_samples[k];
                if (j < dur_samples[k]) {
                    double decay = exp(-decay_rates[k] * j / SAMPLE_RATE);
                    mix += sin(j * steps[k]) * decay * amps[k];
                }
            }
        }
        mix = mix / sqrt((double)n);

        if (mix > 1.0) mix = 1.0;
        if (mix < -1.0) mix = -1.0;

        fputc((unsigned char)(mix * 127.0 + 128), out);
    }

    free(steps);
    free(delay_samples);
    free(dur_samples);
}

void play_connect(void) {
    FILE *out = popen("aplay -q -c 1 -r 44100 -f U8 2>/dev/null", "w");
    if (out) {
        generate_dual_pluck(out, 523.25, 783.99);
        pclose(out);
    }
}

void play_disconnect(void) {
    FILE *out = popen("aplay -q -c 1 -r 44100 -f U8 2>/dev/null", "w");
    if (out) {
        generate_dual_pluck(out, 783.99, 523.25);
        pclose(out);
    }
}

void play_vicrou(void) {
    FILE *out = popen("aplay -q -c 1 -r 44100 -f U8 2>/dev/null", "w");
    if (!out) return;

    double freqs1[]   = {523.25, 659.25, 783.99, 1046.50, 1318.51};
    double delays1[]  = {0.00,   0.09,   0.18,   0.27,    0.36};
    double durs1[]    = {0.35,   0.35,   0.35,   0.40,    0.45};
    double decays1[]  = {5.5,    5.5,    5.0,    4.5,     3.5};
    double amps1[]    = {0.8,    0.8,    0.85,   0.9,     0.95};
    generate_pluck_sequence(out, freqs1, delays1, durs1, decays1, amps1, 5);

    double freqs2[]  = {1046.50, 1318.51, 1567.98};
    double delays2[] = {0.0, 0.0, 0.0};
    double durs2[]   = {0.9, 0.9, 0.9};
    double decays2[] = {1.8, 1.8, 1.8};
    double amps2[]   = {0.7, 0.7, 0.7};
    generate_pluck_sequence(out, freqs2, delays2, durs2, decays2, amps2, 3);

    pclose(out);
}

void play_vicsab(void) {
    FILE *out = popen("aplay -q -c 1 -r 44100 -f U8 2>/dev/null", "w");
    if (!out) return;

    double freqs1[]  = {783.99, 622.25, 493.88, 369.99};
    double delays1[] = {0.00,   0.10,   0.20,   0.30};
    double durs1[]   = {0.30,   0.30,   0.30,   0.35};
    double decays1[] = {6.0,    6.0,    6.0,    5.5};
    double amps1[]   = {0.85,   0.8,    0.8,    0.85};
    generate_pluck_sequence(out, freqs1, delays1, durs1, decays1, amps1, 4);

    double freqs2[]  = {233.08, 277.18, 349.23};
    double delays2[] = {0.0, 0.0, 0.0};
    double durs2[]   = {0.5, 0.5, 0.5};
    double decays2[] = {3.0, 3.0, 3.0};
    double amps2[]   = {0.7, 0.7, 0.7};
    generate_pluck_sequence(out, freqs2, delays2, durs2, decays2, amps2, 3);

    play_sweep(out, 220.0, 55.0, 0.6);

    pclose(out);
}
