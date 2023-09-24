#include <malloc.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
int qtd_thread = 4;
int iteracoes = 2000;
int tamanho = 2048;

typedef struct {
  float **grid;
  float **newgrid;
} Dados_Thread;

int getNeighbors(float **grid, int i, int j) {
  int neighbors = 0;
  if (grid[((i - 1) + tamanho) % tamanho][j] == 1.0) // vizinho superior
    neighbors++;
  if (grid[((i - 1) + tamanho) % tamanho][((j - 1) + tamanho) % tamanho] ==
      1.0) // vizinho superior esquerdo
    neighbors++;
  if (grid[((i - 1) + tamanho) % tamanho][(j + 1) % tamanho] ==
      1.0) // vizinho superior direito
    neighbors++;
  if (grid[i][((j - 1) + tamanho) % tamanho] == 1.0) // vizinho esquerda
    neighbors++;
  if (grid[i][(j + 1) % tamanho] == 1.0) // viziho direito
    neighbors++;
  if (grid[(i + 1) % tamanho][((j - 1) + tamanho) % tamanho] ==
      1.0) // vizinho inferior esquerdo
    neighbors++;
  if (grid[(i + 1) % tamanho][j] == 1.0) // vizinho inferior
    neighbors++;
  if (grid[(i + 1) % tamanho][(j + 1) % tamanho] ==
      1.0) // vizinho inferior direito
    neighbors++;
  return neighbors;
}

void celulas_vivas50(float **grid, int k) {
  int cel_vivas = 0;
  int i;
  int j;
  for (i = 0; i < 50; i++) {
    for (j = 0; j < 50; j++) {
       printf("%.2f ", grid[i][j]);
      if (grid[i][j] == 1.0) {
        cel_vivas++;
      }
    }
  }
  printf("\nGeração %d: %d\n\n", k+1, cel_vivas);
}

int celulas_vivas2048(float **grid) {
  int cel_vivas = 0;
  int i;
  int j;
  for (i = 0; i < tamanho; i++) {
    for (j = 0; j < tamanho; j++) {
      // printf("%.2f ", grid[i][j]);
      if (grid[i][j] == 1.0) {
        cel_vivas++;
      }
    }
  }
  return cel_vivas; 
}

void *geracoes(float **grid, float **newgrid) {
  omp_set_num_threads(qtd_thread);
  int id, inicio, fim, ite, soma;
  float media;
  float **swap;
  
  #pragma omp parallel private(id, inicio, fim, ite, soma, media, swap) 
  {
    Dados_Thread *dados = (Dados_Thread *)malloc(sizeof(Dados_Thread));
    dados->grid = grid;
    dados->newgrid = newgrid;
    id = omp_get_thread_num();
    inicio = tamanho / qtd_thread * id;
    fim = inicio + tamanho / qtd_thread;
  
    for (ite = 0; ite < iteracoes; ite++) {
      for (int i = inicio; i < fim; i++) {
        for (int j = 0; j < tamanho; j++) {
          soma = getNeighbors(dados->grid, i, j);
          if (dados->grid[i][j] == 1 && (soma == 2 || soma == 3)) {
            media = soma / 8.0;
            dados->newgrid[i][j] = ceilf(media);
          } else if (dados->grid[i][j] == 0.0 && soma == 3) {
            media = soma / 8.0;
            dados->newgrid[i][j] = ceilf(media);
          } else
            dados->newgrid[i][j] = 0.0;
        }
      }
      swap = dados->grid;
      dados->grid = dados->newgrid;
      dados->newgrid = swap;
      if (id == 0)
        celulas_vivas50(dados->grid, ite);
        #pragma omp barrier
    }
  }
}

void inicializacao_da_grid(float **grid) {
  int lin = 1;
  int col = 1;
  // GLIDER
  grid[lin][col + 1] = 1.0;
  grid[lin + 1][col + 2] = 1.0;
  grid[lin + 2][col] = 1.0;
  grid[lin + 2][col + 1] = 1.0;
  grid[lin + 2][col + 2] = 1.0;

  lin = 10;
  col = 30;
  // R-pentomino
  grid[lin][col + 1] = 1.0;
  grid[lin][col + 2] = 1.0;
  grid[lin + 1][col] = 1.0;
  grid[lin + 1][col + 1] = 1.0;
  grid[lin + 2][col + 1] = 1.0;
}

int main(int argc, char **argv) {
  struct timeval inicio, final;
  long long tmili;

  float **grid = (float **)malloc(tamanho * sizeof(float *));
  float **newgrid = (float **)malloc(tamanho * sizeof(float *));
  int i = 0;

  while (i < tamanho) {
    grid[i] = (float *)malloc(tamanho * sizeof(float));
    newgrid[i] = (float *)malloc(tamanho * sizeof(float));
    i++;
  }

  inicializacao_da_grid(grid);

  gettimeofday(&inicio, NULL);

  geracoes(grid, newgrid);

  printf("Geração final de células vivas para grid 2048: %d\n", celulas_vivas2048(grid));

  gettimeofday(&final, NULL);
  tmili = (int) (1000 * (final.tv_sec - inicio.tv_sec) + (final.tv_usec - inicio.tv_usec) / 1000);
  printf("Tempo decorrido: %lld ms\n", tmili);
  return 0;
}
