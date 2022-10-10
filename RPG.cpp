//#include "stdafx.h" //depende da versão do visual studio
#include <iostream>
#include <string>
#include <time.h>
#include <windows.h> ///para o sleep()
#include <conio.h> ///para o getch()

using namespace std;

/*
DESENVOLVEDORES:
- Filipi Scalvin da Costa
- Luiz Carlos Braga
- Luiz Felipe Mantoani Fantini
- Vitor Alexandre Silveira
*/

using namespace std;

struct Arma {
	int dano_minimo;
	int dano_maximo;
};

struct Jogador {
	int nivel;
	int vida;
	Arma arma;
	// posição no mapa
	int posX;
	int posY;
};

struct Inimigo {
	string nome;
	int vida;
	Arma arma;
};

struct Bloco {
	bool pedra;
	bool inimigo;
	Inimigo* pInimigo;
};

struct Mapa {
	int altura;
	int largura;
	Bloco** blocos;
};

struct Fase {
	string nome;
	Mapa map;
	int nroInimigos;
	Inimigo* inimigos;
};

template<typename T>
bool Morreu(T personagem) {
	return personagem.vida <= 0;
}

template<typename Tata, typename Tdef>
Tdef Ataque(Tata atacante, Tdef defensor) {
	int intervalo_dano = atacante.arma.dano_maximo - atacante.arma.dano_minimo + 1;
	int dano = atacante.arma.dano_minimo + rand() % intervalo_dano;

	defensor.vida = defensor.vida - dano;

	return defensor;
}

void Combate(Inimigo& inimigo, Jogador& jog) {
	system("cls");
	while (!Morreu<Jogador>(jog)) {

		// TURNO DO JOGADOR
		jog = Ataque<Inimigo, Jogador>(inimigo, jog);
		cout << "O jogador atacou " << inimigo.nome << " e ele ficou com " << inimigo.vida << " de vida" << endl;
		if (Morreu<Inimigo>(inimigo)) {
			cout << "\nInimigo foi morto\n";
			char tecla = _getch();
			return;
		}

		// TURNO DO INIMIGO
		inimigo = Ataque<Jogador, Inimigo>(jog, inimigo);
		cout << "O " << inimigo.nome << " atacou o jogador e o deixou com " << jog.vida << " de vida" << endl << endl;
	}

	cout << "\nO jogador morreu! O jogo acabou!\n";
}

void GeraPosicao(int& X, int& Y, Mapa& map) {
	while (map.blocos[X][Y].pedra || map.blocos[X][Y].inimigo) {
		X = rand() % map.altura;
		Y = rand() % map.largura;
	}
}

Jogador CriarJogador(Mapa map) {
	Arma aJ = { 4, 10 };

	int posX = 0;
	int posY = 0;

	GeraPosicao(posX, posY, map);

	return { 1, 100, aJ, posX, posY };
}

Mapa CriarMapa(int altura, int largura) {
	Mapa map;

	map.altura = altura;
	map.largura = largura;

	// ALOCAÇÃO NA MEMÓRIA
	map.blocos = new Bloco * [altura];
	for (int i = 0; i < altura; i++) {
		map.blocos[i] = new Bloco[largura];
		for (int j = 0; j < largura; j++) { // inicialização
			map.blocos[i][j].pedra = true;
			map.blocos[i][j].inimigo = false;
		}
	}

	int qtdPedras = map.altura * map.largura * 20 / 100; // Área*20% = quantidade de pedras
	int qtdBlocos = map.altura * map.largura;

	int posX = rand() % altura;
	int posY = rand() % largura;

	while (qtdPedras < qtdBlocos) {
		int mov = rand() % 4;

		if (mov == 0 && posX + 1 < map.altura) { // CIMA
			posX++;
		}
		else if (mov == 1 && posY + 1 < map.largura) { // DIREITA
			posY++;
		}
		else if (mov == 2 && posX - 1 >= 0) { // BAIXO
			posX--;
		}
		else if (mov == 3 && posY - 1 >= 0) { // ESQUERDA
			posY--;
		}

		if (map.blocos[posX][posY].pedra)
			qtdBlocos--;
		map.blocos[posX][posY].pedra = false;
	}

	return map;
}

char TipoBloco(Bloco posicao) {
	if (!posicao.inimigo && !posicao.pedra) return ' ';

	if (posicao.inimigo) return posicao.pInimigo->nome[0];

	return char(219);
}

Fase CriarFase(int nroInimigos, Inimigo* inimigos, int alturaMapa, int larguraMapa) {
	Fase fase;
	fase.nome = "Calabouço do Medo";
	fase.nroInimigos = nroInimigos;
	fase.inimigos = inimigos;

	cout << "Carregando Calabouço do Medo...";

	fase.map = CriarMapa(alturaMapa, larguraMapa);

	// POSICIONA INIMIGOS
	int cont = nroInimigos;
	while (cont > 0) {
		int x = 0;
		int y = 0;
		GeraPosicao(x, y, fase.map);
		cont--;
		fase.map.blocos[x][y].pInimigo = &fase.inimigos[cont];
		fase.map.blocos[x][y].inimigo = true;
	}

	return fase;
}

void ImprimeMapa(Mapa map, Jogador jog) {
	system("cls");
	for (int i = 0; i < map.altura; i++) {
		for (int j = 0; j < map.largura; j++) {
			if (i == jog.posX && j == jog.posY) {
				cout << char(157);
			}
			else {
				cout << TipoBloco(map.blocos[i][j]);
			}
		}
		cout << endl;
	}
}

void Movimentar(int& posX, int& posY, Mapa map) {
	cout << endl << "Escolha um:\n\tW - Cima\n\tA - Esquerda\n\tS - Baixo\n\tD - Direita\n\n";
	char tecla = _getch();
	if ((tecla == 'w' || tecla == 'W') && posX - 1 >= 0 && !map.blocos[posX - 1][posY].pedra) {
		posX--;
	}
	else if ((tecla == 'a' || tecla == 'A') && posY - 1 >= 0 && !map.blocos[posX][posY - 1].pedra) {
		posY--;
	}
	else if ((tecla == 's' || tecla == 'S') && posX + 1 < map.altura && !map.blocos[posX + 1][posY].pedra) {
		posX++;
	}
	else if ((tecla == 'd' || tecla == 'D') && posY + 1 < map.largura && !map.blocos[posX][posY + 1].pedra) {
		posY++;
	}
}

void JogarFase(Fase& fase, Jogador& jog) {
	while (!Morreu<Jogador>(jog) && fase.nroInimigos > 0) {
		ImprimeMapa(fase.map, jog);
		Movimentar(jog.posX, jog.posY, fase.map);
		if (fase.map.blocos[jog.posX][jog.posY].inimigo) {
			Combate(*fase.map.blocos[jog.posX][jog.posY].pInimigo, jog);
			if (Morreu<Inimigo>(*fase.map.blocos[jog.posX][jog.posY].pInimigo)) {
				fase.nroInimigos--;
				fase.map.blocos[jog.posX][jog.posY].inimigo = false;
			}
		}

	}
	if (fase.nroInimigos == 0)
		cout << "\n\nVocê completou a fase " << fase.nome << "! Parabéns!\n";
}

int main()
{
	srand(time(NULL));

	int altura, largura;
	int nInimigos;

	altura = 6;
	largura = 10;
	nInimigos = 5;

	Inimigo* inimigos = new Inimigo[nInimigos];

	Arma aI = { 1, 5 };

	inimigos[0] = { "Goblin", 20, aI };
	inimigos[1] = { "Goblerto", 30, aI };
	inimigos[2] = { "Gobo", 40, aI };
	inimigos[3] = { "Goblinio", 50, aI };
	inimigos[4] = { "Juca", 60, aI };

	Fase fase;
	Jogador jog;

	fase = CriarFase(nInimigos, inimigos, altura, largura);
	jog = CriarJogador(fase.map);


	cout << "Iniciando " << fase.nome << "..." << endl << endl;

	JogarFase(fase, jog);

	cout << endl << "Aperte qualquer tecla para sair." << endl << endl;

	char tecla = _getch();

	return 0;
}