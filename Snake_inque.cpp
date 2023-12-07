#include <iostream>
#include "raylib.h"
#include <deque> //tipo de conteiner (array dinamico)
#include <raymath.h> //vetor2add

using namespace std;
//#define PlayRaylibSound PlaySound

Color azul = { 135,206,235,255 }; //cores do meu projeto
Color roxoEscuro = { 75,0,130,255 };

int tamCelula = 25; //tamanho de cada celula na grade(tela)
int numCelula = 20; //numero de celulas
int borda = 75;

double ultimoUpTempo = 0;

bool ElementoNoDeque(Vector2 elemento, deque<Vector2> deque) {//evita a comida ser gerada encima da cobra
    for(unsigned int i = 0; i < deque.size();i++) {
        if (Vector2Equals(deque[i], elemento)) {//compara os valores de (deque e elemento)
            return true;//retorna true se forem iguais
        }
    }
    return false;
}

bool EventoTrigg(double intervalo) {//essa funçao vai ser chamada 60 vezes por segundo
    double tempoAtual = GetTime();//atribui o tempo decorrido em seundos a variavel
    if(tempoAtual - ultimoUpTempo >= intervalo){//compara o tempo com minha variavel
        ultimoUpTempo = tempoAtual;
        return true;
    }
    return false;
}

class Cobra {
public :
    deque<Vector2> corpo = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };//valores iniciais da cobra
    Vector2 direcao = { 1,0 };//direcao inicial 

    bool addCorpo = false;//sempre q for true, adiciona mais um segmento a cobra

    void Draw() {
        for (unsigned int i = 0;i< corpo.size();i++) {
            //aparencia da cobra 
            float x = corpo[i].x;
            float y = corpo[i].y;
            Rectangle segmento = Rectangle{ borda + x * tamCelula, borda + y * tamCelula, (float)tamCelula, (float)tamCelula };
            DrawRectangleRounded(segmento, 0.5, 6, roxoEscuro);
        }
    }

    void Atualiza() {

        corpo.push_front(Vector2Add(corpo[0], direcao));//adiciona um elemento | vecttor2add(nova cordenada)
        if (addCorpo == true) {//sempre q atualizar ele verifica se AddCorpo recebeu true
            
            addCorpo = false;//depois de adicionado ele retorna para false, esperando a proxima colisao com comida
        }
        else {
            corpo.pop_back();//tira o ultimo elemento da cobra
            
        }
        //ele esta sempre atualizando, tira 1 e coloca 1, quando for true ele vai adicionar 1, mas n vai tirar 1.(corpo cresce)
   
    }

    void reset() {// reposiciona a cobra para o tamanho e posicao inicial 
        corpo = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
        direcao = { 1,0 };
    }
};

class Comida {

public :
    Vector2 posicao;//vetor de "matriz"(x e y) fornecida pello raymath.h
    Texture2D textura;//variavel que representa uma imagem

    Comida(deque<Vector2> corpoCobra) {//construtor da classe comida tem como paramentro um deque<x e Y> que reprensenta o corpo da cobra
        Image imagem = LoadImage("graficos/estrela.png");//procura a imagem
        textura = LoadTextureFromImage(imagem);//carrega a imagem e retorna uma estrutura 2D(renderiza mais rapido)
        UnloadImage(imagem);//descarrega os recursos usados na imagem(lata de lixo)
        posicao = PosicaoRandomica(corpoCobra);//cria a comida em uma posicao aleatoria (corpo da cobra como parametro)(proposito de comparaçao)
    }

    ~Comida() {//destrutor de comida
        UnloadTexture(textura);
    }

    void Draw(){//desenha a comida
        DrawTexture(textura, borda + posicao.x * tamCelula, borda + posicao.y * tamCelula, WHITE);
    }

    Vector2 GeradorCelulaRandom() {//cria a celula com um vallor radom de 0 a numcelula -1 para X e Y
        float x = GetRandomValue(0, numCelula - 1);
        float y = GetRandomValue(0, numCelula - 1);
        return Vector2{ x,y };
    }

    Vector2 PosicaoRandomica(deque<Vector2> corpoCobra) {//posiciona a celula evitando o corpo da cobra
        
        Vector2 posicao = GeradorCelulaRandom();//posicao vai receber x e y do gerador
        while (ElementoNoDeque(posicao, corpoCobra)){//enquanto x e y de ambas forem diferentes ele cria a comida
            posicao = GeradorCelulaRandom();
        }
        return posicao;
    }
};

class Jogo {//responsavel por centralizar os recursos do jogo
public:
    Cobra cobra = Cobra();
    Comida comida = Comida(cobra.corpo);

    bool rodando = true;
    int pontuacao = 0;
    int record = 0;

    Sound somComida;
    Sound somParede;
    

    /*Jogo() {
        InitAudioDevice();
        somComida = LoadSound("sons/eat.mp3");
        somParede = LoadSound("sons/wall.mp3");
    }

    ~Jogo() {
        UnloadSound(somComida);
        UnloadSound(somParede);
        CloseAudioDevice();
    }*/

    void Draw() {//desenha a cobra e comida na tela
        cobra.Draw();
        comida.Draw();
    }

    void Atualiza() {
        if (rodando) {//se rodando for true 
            cobra.Atualiza();
            ColisaoComida();   //atualiza esta sempre rodando, verifica cada um
            colisaoParede();
            ColisaoRabo();
        }
        
    }

    void ColisaoComida() {//verifica se a cobra chegou na mesma posicao da comida
        if (Vector2Equals(cobra.corpo[0], comida.posicao)) {
            //compara a posicao da cobra e da comida, se for igual gera uma nova posicao para comida
            comida.posicao = comida.PosicaoRandomica(cobra.corpo);
            cobra.addCorpo = true;//verifica se ouve colisao e adiciona +1 segmento ao corpo da cobra
            pontuacao ++;
            //PlayRaylibSound(somComida);
            
        }
    }

    void colisaoParede() {//verifica se o corpo da cobra ultrapassou o limite da tela(grade) 
        if (cobra.corpo[0].x == numCelula || cobra.corpo[0].x == -1) {
            fimDoJogo();
        }
        if (cobra.corpo[0].y == numCelula || cobra.corpo[0].y == -1) {
            fimDoJogo();
        }
    }

    void fimDoJogo() {//se ouve colisao com a parede ou o corpo, reinincia o jogo na posicao inicial (ate um event-key)
        cout << "Fim Do Jogo" << endl;
        cobra.reset();
        comida.posicao = comida.PosicaoRandomica(cobra.corpo);
        rodando = false;

        if (pontuacao > record) {
            record = pontuacao;
        }
        
        pontuacao = 0;
        //PlayRaylibSound(somParede);
    }

    void ColisaoRabo() {
        deque<Vector2> corpoSemCabeca = cobra.corpo;
        corpoSemCabeca.pop_front();
        if(ElementoNoDeque(cobra.corpo[0], corpoSemCabeca)) {
            fimDoJogo();
        }
    }

   
};

int main()
{
    
    std::cout << "Ininciando...\n";
    //tamanhho da tela se baseia no tamanho da celula * a quantitdade => cria uma grade
    InitWindow(2*borda + tamCelula * numCelula, 2*borda + tamCelula * numCelula, "jogo da Cobrinha - inque");//inicia a janela X, Y, um nome
    
    SetTargetFPS(60);//padroniza a taixa de atualizaçao de quadros

    Jogo jogo = Jogo();//chama a classe jogo q gerencia as outras classes 

    while (WindowShouldClose() == false) {//loop principal do jogo

        BeginDrawing();

        if (EventoTrigg(0.2)) {//chama a funçao passando como paramentro a velocidade em mileseugndos
            jogo.Atualiza();//atualiza a velocidade da cobra
        }

        if (IsKeyPressed(KEY_UP) && jogo.cobra.direcao.y != 1) {//garante q n faca o movimentto contrario
            jogo.cobra.direcao = { 0,-1 };//atualiza a direcao da cobra
            jogo.rodando = true;
        }
        if (IsKeyPressed(KEY_DOWN) && jogo.cobra.direcao.y != -1) {
            jogo.cobra.direcao = { 0, 1 };
            jogo.rodando = true; //sempre q uma seta é clicada ele inicia o jogo (caso tenha perdido)
        }
        if (IsKeyPressed(KEY_RIGHT) && jogo.cobra.direcao.x != -1) {
            jogo.cobra.direcao = { 1, 0 };
            jogo.rodando = true;
        }
        if (IsKeyPressed(KEY_LEFT) && jogo.cobra.direcao.x != 1) {
            jogo.cobra.direcao = { -1, 0 };
            jogo.rodando = true;
        }

        //desenhando o jogo na tela(fundo)
        ClearBackground(azul);
        DrawRectangleLinesEx(Rectangle{(float)borda-5,(float)borda-5, (float)tamCelula*numCelula+10, (float)tamCelula*numCelula+10}, 5, roxoEscuro);
        
        //(text,posX,posY,fontsize,cor)
        DrawText("Retro Snake - (Inque)", borda - 5, 20, 30, roxoEscuro);
        DrawText(TextFormat("%d", jogo.pontuacao), borda - 5, borda + tamCelula * numCelula + 10, 30, roxoEscuro);
        DrawText(TextFormat("Recorde atual - %d", jogo.record),tamCelula * numCelula - 193, borda + tamCelula * numCelula + 10, 30, roxoEscuro);

       //chama a comida e cobra (desenha na tela)
        jogo.Draw();
        

        EndDrawing();


    }



    CloseWindow();
}



