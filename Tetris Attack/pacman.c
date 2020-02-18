#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <gl/gl.h>
#include "SOIL.h"
#include "pacman.h"
#include <time.h>

//=========================================================
// Tamanho da matriz do jogo
#define N 21
#define P 12
// Tamanho de cada bloco da matriz do jogo na tela
#define TAM 0.1f
//Funções que convertem a linha e coluna da matriz em uma coordenada de [-1,1]
#define MAT2X(j) ((j)*0.1f-1)
#define MAT2Y(i) (0.9-(i)*0.1f)

//=========================================================
// Estruturas usadas para controlar o jogo
struct TPoint
{
    int x,y;
};

const struct TPoint direcoes[4] = {{1,0},{0,1},{-1,0},{0,-1}}; // estrutura global

struct TCenario
{
    int mapa[N][P];
    int cont1;
    int pontos;
};

struct TPacman
{
    int x,y;
};

//==============================================================
// Texturas
//==============================================================

GLuint pecas[5];
GLuint grade;
GLuint pontuacao[10];
GLuint telaStart, telaGameOver;
GLuint fundo;
GLuint fundop;

static void desenhaSprite(float coluna,float linha, GLuint tex);
static GLuint carregaArqTextura(char *str);

// Função que carrega todas as texturas do jogo
void carregaTexturas()
{
    int i;
    char str[50];
    for(i=1; i<=5; i++)
    {
        sprintf(str,".//Sprites//peca%d.png",i);
        pecas[i-1] = carregaArqTextura(str);
    }
    sprintf(str,".//Sprites//grade.png");
    grade = carregaArqTextura(str);
    for(i=0; i<=9; i++)
    {
        sprintf(str,".//Sprites//sprite%d.png",i);
        pontuacao[i] = carregaArqTextura(str);
    }
    telaStart = carregaArqTextura(".//Sprites//start.png");
    fundo = carregaArqTextura(".//Sprites//spritemaisfoda.png");
    fundop = carregaArqTextura(".//Sprites//fundoplacar.png");
    telaGameOver = carregaArqTextura(".//Sprites//gameover.png");
}

// Função que carrega um arquivo de textura do jogo
static GLuint carregaArqTextura(char *str)
{
    // http://www.lonesock.net/soil.html
    GLuint tex = SOIL_load_OGL_texture
                 (
                     str,
                     SOIL_LOAD_AUTO,
                     SOIL_CREATE_NEW_ID,
                     SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y |
                     SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
                 );

    /* check for an error during the load process */
    if(0 == tex)
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }

    return tex;
}

// Função que recebe uma linha e coluna da matriz e um código
// de textura e desenha um quadrado na tela com essa textura
void desenhaSprite(float coluna,float linha, GLuint tex)
{
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);
    glVertex2f(coluna, linha);
    glTexCoord2f(1.0f,0.0f);
    glVertex2f(coluna+TAM, linha);
    glTexCoord2f(1.0f,1.0f);
    glVertex2f(coluna+TAM, linha+TAM);
    glTexCoord2f(0.0f,1.0f);
    glVertex2f(coluna, linha+TAM);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

}
void desenhaTipoTela(float x, float y, float tamanho, GLuint tex){

    glPushMatrix();

    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,1.0f); glVertex2f(x - tamanho, y + tamanho);
        glTexCoord2f(1.0f,1.0f); glVertex2f(x + tamanho, y + tamanho);
        glTexCoord2f(1.0f,0.0f); glVertex2f(x + tamanho, y - tamanho);
        glTexCoord2f(0.0f,0.0f); glVertex2f(x - tamanho, y - tamanho);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void desenhaTela(int tipo){
    if(tipo == 0)
        desenhaTipoTela(0, 0, 1.0, telaStart);
    else
        desenhaTipoTela(0, 0, 1.0, telaGameOver);
}

//==============================================================
// Cenario
//==============================================================

// Função que carrega os dados do cenário de um arquivo texto
Cenario *cenario_carrega()
{
    int i,j,check,aleat;
    Cenario *cen = malloc(sizeof(Cenario));
    if(cen == NULL){
        printf("Erro ao carregar cenario!\n");
        system("pause");
        exit(1);
    }
    aleat=13 + rand()%3;
    cen->pontos=0;
    cen->cont1=0;
    for(i=0;i<N;i++)
        for(j=0;j<P;j++)
        {
            if(i<aleat)
                cen->mapa[i][j]=0;
            else
                do
                {
                    check=0;
                    cen->mapa[i][j] = 1+rand() % 5;
                    if(j==0)
                        continue;
                    if(cen->mapa[i][j-1]==cen->mapa[i][j])
                        check=1;
                }
                while(cen->mapa[i-1][j]==cen->mapa[i][j] || check==1);
        }
    return cen;
}

// Libera os dados associados ao cenário
void cenario_destroy(Cenario *cen)
{
    free(cen);
}

//Percorre a matriz do jogo desenhando os sprites
void cenario_desenha(Cenario *cen)
{
    int i,j,placar[5];
    for(i=0;i<5;i++)
        placar[i]=0;
    placar[0]=cen->pontos/10000;
    placar[1]=(cen->pontos%10000)/1000;
    placar[2]=(cen->pontos%1000)/100;
    placar[3]=(cen->pontos%100)/10;
    placar[4]=cen->pontos%10;
    desenhaTipoTela(0.0,0.0,1.0,fundo);
    for(i=0;i<4;i++)
        desenhaTipoTela(MAT2X(i+2),-0.75,0.1,fundop);
    for(i=1; i<N; i++)
        for(j=0; j<P; j++)
        {
            if(cen->mapa[i][j] > 0)
                desenhaSprite(MAT2X(j+7),MAT2Y(i-1),pecas[cen->mapa[i][j]-1]);
        }
        for(j=0;j<5;j++)
        {
            if(placar[j] >= 0)
                desenhaSprite(MAT2X(j+1),-0.8,pontuacao[placar[j]]);
        }
}
//--------------Funcao para subir linhas--------------------------

void funcPecas(Cenario *cen, Pacman *pac){//----Funçao que reune todas----
    cen->cont1++;
    if(cen->cont1 >= 400){ // timer
        if(pac->y>1)
            pac->y-=1;
        sobeLinhas(cen);
        randLinha(cen);
        cen->cont1 = 0;
    }
}

void sobeLinhas(Cenario *cen)
{
    int i,j;
    for(i=0;i<N;i++)
    {
        for(j=0;j<P;j++)
            if(cen->mapa[i][j]!=0)
                cen->mapa[i-1][j]=cen->mapa[i][j];
    }
}
//--------------Funcao para gerar linhas aleatorias--------------
void randLinha(Cenario *cen)
{
    int j,check=0;
    srand(time(NULL));
    for(j=0; j<P; j++)
    {
        do
        {
            check=0;
            cen->mapa[N-1][j]= 1+rand() % 5;
            if(j<2)
                continue;
            if(cen->mapa[N-1][j]==cen->mapa[N-1][j-1] && cen->mapa[N-1][j]==cen->mapa[N-1][j-2])
                check=1;
        }
        while((cen->mapa[N-2][j]==cen->mapa[N-1][j] && cen->mapa[N-3][j]==cen->mapa[N-1][j]) || check==1);
    }
}
//---------------Funcao para quebrar peças--------------------
void zerar(Cenario *cen)
{
    int guarda[N][P];
    int i,j,cont,pos,c;
    for(i = 0; i<N; i++)
    {
        for(j=0; j<P; j++)
            guarda[i][j] = 0;
    }
    for(i=0; i<N; i++)
    {
        for(j=0; j<P; j++)
        {
            cont=0;
            if(cen->mapa[i][j]!=0)  //check nas linhas pontuacao
            {
                pos=j;
                for(c=j; cen->mapa[i][j]==cen->mapa[i][c] && c<P; c++)
                    cont++;
                if(cont>2)
                {
                    while(j<cont+pos)  //marcado 3 ou mais consecutivos
                    {
                        guarda[i][j]=1;
                        j++;
                    }
                if(cont==3)
                    cen->pontos+=50;
                if(cont==4)
                    cen->pontos+=100;
                if(cont==5)
                    cen->pontos+=150;
                if(cont>=6)
                    cen->pontos+=200;
                }
            }
        }
    }
//---------------------Colunas--------------------
    for(j=0;j<P;j++)
    {
        for(i=0;i<N;i++)
        {
            cont=0;
            if(cen->mapa[i][j]!=0)  //check nas linhas pontuacao
            {
                pos=i;
                for(c=i; cen->mapa[i][j]==cen->mapa[c][j] && c<N; c++)
                    cont++;
                if(cont>2)
                {
                    while(i<cont+pos)  //marcado 3 ou mais consecutivos
                    {
                        guarda[i][j]=1;
                        i++;
                    }
                if(cont==3)
                    cen->pontos+=50;
                if(cont==4)
                    cen->pontos+=100;
                if(cont==5)
                    cen->pontos+=150;
                if(cont>=6)
                    cen->pontos+=200;
                }
            }
        }
    }
//-------------------
    for(i=0;i<N;i++)
    {
        for(j=0;j<P;j++)
        {
            if(guarda[i][j]==1)
            {
                cen->mapa[i][j] = 0; //zerando
            }
        }
    }
}
//-------------------Direcionamento do jogador------------------
Pacman* pacman_create(int x, int y){
    Pacman* pac = malloc(sizeof(Pacman));
    if(pac == NULL){
        printf("Erro ao carregar grade!\n");
        system("pause");
        exit(1);
    }
    pac->x = x;
    pac->y = y;
    return pac;
}

void pacman_AlteraDirecao(Pacman *pac, int direcao){ //condiçao para verificar se é fora da matriz
    if(pac->x + direcoes[direcao].x <P-1 && pac->x + direcoes[direcao].x >=0 && pac->y + direcoes[direcao].y <N && pac->y + direcoes[direcao].y >0){
      pac->x += direcoes[direcao].x;
      pac->y += direcoes[direcao].y;
    }
}

void troca_Pecas(Cenario *cen, Pacman *pac){
    int aux;
    if(cen->mapa[pac->y][pac->x]!=0 || cen->mapa[pac->y][pac->x+1]!=0 || cen->mapa[pac->y][pac->x]!=cen->mapa[pac->y][pac->x+1]){
        aux=cen->mapa[pac->y][pac->x];
        cen->mapa[pac->y][pac->x]=cen->mapa[pac->y][pac->x+1];
        cen->mapa[pac->y][pac->x+1]=aux;
    }
}

void pacman_desenha(Pacman *pac){
    float linha, coluna;
    linha = pac->y;
    coluna = pac->x;
    desenhaSprite(MAT2X(coluna+7),MAT2Y(linha-1), grade);
    desenhaSprite(MAT2X(coluna+8),MAT2Y(linha-1), grade);
}

void pacman_destroy(Pacman *pac){
    free(pac);
}

int check(Cenario *cen){
    int j;
    for(j =0; j<P;j++){
        if(cen->mapa[0][j]!=0) // condição pra terminar o jogo quando chegar no topo da tela
            return 0;
    }
    return 1;
}

//void descePecas(Cenario *cen){
//    int i,j,l,cont,aux;
//    for(i=0;i<N-1;i++){
//        for(j=0;j<P;j++){
//            if(cen->mapa[i][j]!=0 && cen->mapa[i+1][j]==0){
//                l=i+1; cont=0;
//                while(cen->mapa[l][j]==0){
//                    cont++;
//                    l++;
//                }
//                aux=i;
//                while(cont>0){
//                    for(l=aux;cen->mapa[l][j]!=0;l--){
//                        cen->mapa[l+1][j]=cen->mapa[l][j];
//                    }
//                    cen->mapa[l+1][j]=0;
//                    cont--; aux++;
//                }
//            }
//        }
//    }
//}

void descePecas(Cenario *cen){
    int i,j,aux;
    for(i=0;i<N-1;i++){
        for(j=0;j<P;j++){
            if(cen->mapa[i][j]!=0 && cen->mapa[i+1][j] == 0){
                aux=cen->mapa[i][j];
                cen->mapa[i][j]=cen->mapa[i+1][j];
                cen->mapa[i+1][j]=aux;
            }
        }
    }
}

