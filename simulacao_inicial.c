#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define NUM_PARTICLES (1024 * 1024 * 10)
#define DT 0.01 // Time step

// Antonio Costa Satiro de Souza 10723636

typedef struct
{
    double x, y, z;    // Posição
    double vx, vy, vz; // Velocidade
} Particle;

int main()
{

    Particle *particles = (Particle *)malloc(NUM_PARTICLES * sizeof(Particle));
    double *vy = (double *)malloc((size_t)NUM_PARTICLES * sizeof(double));
    if (!particles)
    {
        perror("malloc falhou");
        return 1;
    }
    long long i;

    // Inicializa partículas
    printf("Inicializando partículas...\n");

#pragma omp parallel for

    for (i = 0; i < NUM_PARTICLES; i++)
    {
        particles[i].x = (double)rand() / RAND_MAX * 100.0;
        particles[i].y = (double)rand() / RAND_MAX * 100.0;
        particles[i].z = (double)rand() / RAND_MAX * 100.0;
        particles[i].vx = (double)rand() / RAND_MAX * 10.0 - 5.0;
        particles[i].vy = (double)rand() / RAND_MAX * 10.0 - 5.0;
        particles[i].vz = (double)rand() / RAND_MAX * 10.0 - 5.0;
    }
    printf("Partículas inicializadas.\n");

    double start_time, end_time;
    start_time = omp_get_wtime();

    // --- Bloco a ser otimizado ---
    printf("Simulando um passo (versão inicial)...\n");
    const double GRAVITY = -9.81;

    // 1. Atualiza Posições
#pragma omp parallel sections
    {
#pragma omp section
        {
            for (long long j = 0; j < NUM_PARTICLES; j++)
            {
                particles[j].x += particles[j].vx * DT;
                particles[j].y += particles[j].vy * DT;
                particles[j].z += particles[j].vz * DT;
            }
        }
#pragma omp section
        {
            for (long long j = 0; j < NUM_PARTICLES; j++)
                vy[j] = particles[j].vy + GRAVITY * DT;
        }
    }
// Aplica as novas velocidades (após a barreira implícita das sections)
#pragma omp parallel for
    for (i = 0; i < NUM_PARTICLES; i++)
        particles[i].vy = vy[i];

    // --- Fim do Bloco ---
    end_time = omp_get_wtime();
    printf("\nSimulação concluída.\n");

    printf("Tempo de execução (simulação): %.4f segundos\n", end_time - start_time);
    free(particles);
    return 0;
}