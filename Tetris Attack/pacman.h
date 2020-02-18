
//=========================================================
typedef struct TPacman Pacman;
typedef struct TCenario Cenario;

//=========================================================
void carregaTexturas();
//=========================================================
Cenario* cenario_carrega();
void cenario_destroy(Cenario* cen);
void cenario_desenha(Cenario* cen);
void zerar(Cenario* cen);
void descePecas(Cenario* cen);
void sobeLinhas(Cenario* cen);
void randLinha(Cenario* cen);
void funcPecas(Cenario* cen, Pacman* pac);
//
void desenhaTela(int tipo);
int check(Cenario *cen);
////=========================================================
Pacman* pacman_create(int x, int y);
void pacman_destroy(Pacman *pac);
void pacman_desenha(Pacman *pac);
void pacman_AlteraDirecao(Pacman *pac, int direcao);
//
//////=========================================================
void troca_Pecas(Cenario* cen, Pacman* pac);
void terminaJogo(); //Funçao para terminar no jogo no pacman.c quando o sobe linhas chegar em seu maximo
