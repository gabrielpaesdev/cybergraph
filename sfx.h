#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

// Tons básicos para geração de som dinâmico
void play_tone(FILE *out, double freq, double duration);
void play_sweep(FILE *out, double start_freq, double end_freq, double duration);

// Eventos de som do jogo
void play_connect(void);
void play_disconnect(void);
void play_vicrou(void);
void play_vicsab(void);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_H
