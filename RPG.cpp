#include <iostream>
#include <string>
#include <time.h>
#include <windows.h>
#include <conio.h> ///para o getch()
#include <time.h> ///para o time()

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
		((bloco + posX) + posY)->semPedra && ((bloco + posX) + posY)->semInimigo && // sem inimigo e sem pedra
		(																	// verifica se tem saída
			posX + 1 < altura && ((bloco + posX + 1) + posY)->semPedra ||	// em cima
			posX - 1 >= 0 && ((bloco + posX - 1) + posY)->semPedra ||		// embaixo
			posY + 1 < largura && ((bloco + posX) + posY + 1)->semPedra ||	// direita
			posY - 1 >= 0 && ((bloco + posX) + posY + 1)->semPedra			// esquerda
		); 
}

Fase CriarFase(int nroInimigos, Inimigo* inimigos, int alturaMapa, int larguraMapa) {
	Fase fase;
	fase.nome = "Prototipo";
	fase.nroInimigos = nroInimigos;
	fase.inimigos = inimigos;

	fase.map = CriarMapa(alturaMapa, larguraMapa);

	int cont = nroInimigos;
	while (cont > 0) {
		int x = rand() % alturaMapa;
		int y = rand() % larguraMapa;
		if (PosicaoValida(x, y, &fase.map.blocos[x][y], alturaMapa, larguraMapa)) {
			cont--;
			fase.map.blocos[x][y].pInimigo = &fase.inimigos[cont];
			fase.map.blocos[x][y].semInimigo = false;
		}
	}
	for (int i = 0; i < alturaMapa; i++) {
		for (int j = 0; j < larguraMapa; j++) {
			if (fase.map.blocos[i][j].semInimigo) {
				if (fase.map.blocos[i][j].semPedra) {
					cout << " ";
				}
				else {
					cout << char(219);
				}
			}
		}
		cout << endl;
	}
	cout << fase.nome << endl;

	return fase;

}

Jogador CriaJogador(int alt, int larg, Mapa map) {
	Arma aJ = { 4, 10 };

	int posX, posY;
	posX = 0; posY = 0;

	while (!PosicaoValida(posX, posY, &map.blocos[posX][posY], alt, larg)) {
		posX = rand() % alt;
		posY = rand() % larg;
	}

	return { 1, 100, aJ, posX, posY };
}

int main()
{
	///ALERTA: NÃO MODIFICAR O TRECHO DE CÓDIGO, A SEGUIR.
	//COMANDOS PARA QUE O CURSOR NÃO FIQUE PISCANDO NA TELA
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO     cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
	COORD coord; // reposiciona o cursor
	srand(time(NULL));
	///ALERTA: NÃO MODIFICAR O TRECHO DE CÓDIGO, ACIMA.

	coord.X = 0;    coord.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

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

	Jogador jog = CriaJogador(altura, largura, fase.map);

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