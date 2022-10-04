#include <iostream>
#include <string>
#include <time.h>
#include <windows.h>

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
	bool semPedra;
	bool semInimigo;
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
bool morreu(T personagem) {
	if (personagem.vida < 0) {
		return true;
	}
	else {
		return false;
	}
}

template<typename Tata, typename Tdef>
Tdef ataque(Tata atacante, Tdef defensor) {
	//2 - 5
	int intervalo_dano = atacante.arma.dano_maximo - atacante.arma.dano_minimo + 1;
	int dano = atacante.arma.dano_minimo + rand() % intervalo_dano;

	defensor.vida = defensor.vida - dano;

	return defensor;
}

void jogar_fase(Jogador jog, Fase fase) {
	cout << "Começou " << fase.nome << endl << endl;

	for (int atual = 0; atual < 5; atual++) {
		while (!morreu(fase.inimigos[atual])) {
			jog = ataque(fase.inimigos[atual], jog);
			fase.inimigos[atual] = ataque(jog, fase.inimigos[atual]);

			cout << "O jogador atacou " << fase.inimigos[atual].nome << " e ele ficou com " << fase.inimigos[atual].vida << " de vida" << endl;
			cout << "O " << fase.inimigos[atual].nome << "atacou o jogador ao mesmo tempo e o deixou com " << jog.vida << " de vida" << endl;


			if (morreu(jog)) {
				cout << "O jogador morreu, o jogo acabou";
				return;
			}
		}

		cout << fase.inimigos[atual].nome << " foi morto" << endl << endl;
	}

	cout << "O jogador passou a fase";
}

Mapa CriarMapa(int altura, int largura) {
	Mapa mapa;
	mapa.altura = altura;
	mapa.largura = largura;

	// ALOCAÇÃO NA MEMÓRIA
	mapa.blocos = new Bloco*[altura];
	for (int i = 0; i < altura; i++) {
		mapa.blocos[i] = new Bloco[largura];
	}

	int qtdBlocks = altura*largura*20/100; // Área*20% = quantidade de pedras

	while (qtdBlocks > 0) {
		int linha = rand() % altura;
		int coluna = rand() % largura;

		if (mapa.blocos[linha][coluna].semPedra) { // não está bloqueado
			mapa.blocos[linha][coluna].semPedra = false;
			qtdBlocks--;
		}
	}

	return mapa;
}

bool PosicaoValida(int posX, int posY, Bloco* bloco, int altura, int largura) {
	return
		posX >= 0 && posX < altura && posY >= 0 && posY < largura && // endereço correto
		((bloco + posX) + posY)->semPedra && ((bloco + posX) + posY)->semInimigo && // sem inimigo e sem pedra
		(																	// verifica se tem saída
			posX + 1 < altura && ((bloco + posX + 1) + posY)->semPedra ||	// em cima
			posX - 1 >= 0 && ((bloco + posX - 1) + posY)->semPedra ||		// embaixo
			posY + 1 < largura && ((bloco + posX) + posY + 1)->semPedra ||	// direita
			posY - 1 >= 0 && ((bloco + posX) + posY + 1)->semPedra			// esquerda
		); 
}

void PosicionaInimigos(int nroInimigos, Fase &fase) {
	int cont = nroInimigos;
	while (cont > 0) {
		int x = rand() % fase.map.altura;
		int y = rand() % fase.map.largura;
		if (PosicaoValida(x, y, &fase.map.blocos[x][y], fase.map.altura, fase.map.largura)) {
			cont--;
			fase.map.blocos[x][y].pInimigo = &fase.inimigos[cont];
			fase.map.blocos[x][y].semInimigo = false;
		}
	}
}

char CharBloco(Bloco posicao) {
	if (posicao.semInimigo && posicao.semPedra) return ' ';

	return char(219);
}

Fase CriarFase(int nroInimigos, Inimigo* inimigos, int alturaMapa, int larguraMapa) {
	Fase fase;
	fase.nome = "Prototipo";
	fase.nroInimigos = nroInimigos;
	fase.inimigos = inimigos;

	fase.map = CriarMapa(alturaMapa, larguraMapa);

	PosicionaInimigos(nroInimigos, fase);

	return fase;
}

Jogador CriaJogador(Mapa map) {
	Arma aJ = { 4, 10 };

	int posX, posY;
	posX = 0; posY = 0;

	while (!PosicaoValida(posX, posY, &map.blocos[posX][posY], map.altura, map.largura)) {
		posX = rand() % map.altura;
		posY = rand() % map.largura;
	}

	return { 1, 100, aJ, posX, posY };
}

void ImprimeMapa(Mapa map, Jogador jog) {
	system("cls");
	for (int i = 0; i < map.altura; i++) {
		for (int j = 0; j < map.largura; j++) {
			if (i == jog.posX && j == jog.posY) {
				cout << "O";
			}
			else {
				cout << CharBloco(map.blocos[i][j]);
			}
		}
		cout << endl;
	}
}

void Movimentar(int &posX, int &posY, Mapa map) {
	char tecla;
	cout << endl << "Escolha um:\n\tW - Cima\n\tA - Esquerda\n\tS - Baixo\n\tD - Direita\n\n";
	cin >> tecla;
	if ((tecla == 'w' || tecla == 'W') && PosicaoValida(posX - 1, posY, &map.blocos[posX - 1][posY], map.altura, map.largura)) {
		posX--;
	}
	else if ((tecla == 'a' || tecla == 'A') && PosicaoValida(posX, posY - 1, &map.blocos[posX][posY - 1], map.altura, map.largura)) {
		posY--;
	}
	else if ((tecla == 's' || tecla == 'S') && PosicaoValida(posX + 1, posY, &map.blocos[posX + 1][posY], map.altura, map.largura)) {
		posX++;
	}
	else if ((tecla == 'd' || tecla == 'D') && PosicaoValida(posX, posY + 1, &map.blocos[posX][posY + 1], map.altura, map.largura)) {
		posY++;
	}
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
	inimigos[4] = { "Juca", 95, aI };

	Fase fase = CriarFase(nInimigos, inimigos, altura, largura);

	Jogador jog = CriaJogador(fase.map);

	while (jog.vida > 0) {
		ImprimeMapa(fase.map, jog);
		Movimentar(jog.posX,jog.posY,fase.map);
	}


	/*
	Arma aI = { 1, 5 };
	Arma aJ = { 4, 10 };

	Inimigo goblin1 = { "Goblin", 20, aI };
	Inimigo goblin2 = { "Goblerto", 30, aI };
	Inimigo goblin3 = { "Gobo", 40, aI };
	Inimigo goblin4 = { "Goblinio", 50, aI };
	Inimigo chefao = { "Juca", 95, aI };

	Fase fase;
	fase.nome = "Fase 1";
	fase.inimigos[0] = goblin1;
	fase.inimigos[1] = goblin2;
	fase.inimigos[2] = goblin3;
	fase.inimigos[3] = goblin4;
	fase.inimigos[4] = chefao;

	jogar_fase(jog, fase);
	*/
}