#include "SOIL.h"
#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <time.h>
#include "pacman.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
/*GRUPO PJG
 Pedro Leale - Matr�cula 11921BCC008
 Jo�o Batista de Sousa Paula - Matr�cula 11911BCC008
 Guilherme Alves Carvalho - Matr�cula 11921BCC016
 */


// Define as vari�veis e fun��es que ir�o controlar o jogo
Cenario *cen;
Pacman *pac;
int iniciou_jogo = 0;
int contador;
void desenhaJogo();
void iniciaJogo();
void terminaJogo();

//fun�ao que cria e configura a janela de desenho OpenGL
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;


    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          650,
                          650,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    // Chama a fun��o que inicializa o jogo
    iniciaJogo();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();
            // Esse la�o controla cada frame do jogo.
            // Desenhar o jogo aqui
            desenhaJogo();

            glPopMatrix();

            SwapBuffers(hDC);

            Sleep(16.67);
        }
    }
    // Saiu do la�o que desenha os frames?
    // Ent�o o jogo acabou.
    terminaJogo();

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}
// Fun��o que verifica se o teclado foi pressionado
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case
        WM_KEYDOWN:
        {
                switch (wParam)
                {
                    case VK_ESCAPE: //Pressionou ESC?
                        PostQuitMessage(0);
                        break;
                    case VK_RIGHT: //Pressionou seta direita?
                        pacman_AlteraDirecao(pac,0);
                        break;
                    case VK_DOWN: //Pressionou seta para baixo?
                        pacman_AlteraDirecao(pac,1);
                        break;
                    case VK_LEFT: //Pressionou seta esquerda?
                        pacman_AlteraDirecao(pac,2);
                        break;
                    case VK_UP: //Pressionou seta para cima?
                        pacman_AlteraDirecao(pac,3);
                        break;
                    case VK_SPACE: //Pressionou espa�o?
                        if(iniciou_jogo==1)
                            troca_Pecas(cen,pac);
                        break;
                    case 'P':
                        if(iniciou_jogo == 2){
                            terminaJogo();
                            iniciaJogo();
                        }
                        iniciou_jogo = 1;
                        break;
                }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
// Fun��o que configura o OpenGL
void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);

    // Chama a fun��o que carrega as texturas do jogo
    carregaTexturas();
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering


}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

// ===================================================================
// Fun��es que controlam e desenham o jogo
// ===================================================================

// Fun��o que desenha cada componente do jogo
void desenhaJogo(){
    if(iniciou_jogo == 0){
        desenhaTela(0);
        return;
    }
    if(check(cen)){ //verifica se perdeu
        cenario_desenha(cen); //desenha pe�as
        pacman_desenha(pac); //desenha grade
        zerar(cen); //quebra as pe�as
        descePecas(cen); //des�e as pe�as
        funcPecas(cen,pac); // sobelinha + randlinha + timer
    }else{
        contador++;
        contador=contador%100;
            if(contador<40){
                desenhaTela(1);
            }else
                desenhaTela(0);
        iniciou_jogo=2;
    }
}

// Fun��o que inicia o mapa do jogo e as posi��es iniciais dos personagens
void iniciaJogo(){
    srand(time(NULL));
    contador=0;
    cen = cenario_carrega();
    pac = pacman_create(5,17);
}

// Fun��o que libera os dados do jogo
void terminaJogo(){
    pacman_destroy(pac);
    cenario_destroy(cen);
}

