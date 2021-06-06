#ifndef MENSAGEM_H
#define MENSAGEM_H


enum Tipo {
	DISPARA=0,
	DESLIGA=1,
	EXITO=2,
	ECO=3
};

enum Origem {
	ORQUESTRADOR=0,
	CLIENTE=1,
	SERVIDOR=2
};


struct {
    unsigned char tipo;
    unsigned char origem;
    unsigned char tamanho;
    unsigned char repeticoes;
} typedef mensagem_t;

// Por conveniência, não é necessário
void escreve_mensagem (mensagem_t* m,
		       unsigned char tipo,
		       unsigned char origem,
		       unsigned char tamanho,
		       unsigned char repeticoes){
	m->tipo = tipo;
	m->origem = origem;
	m->tamanho = tamanho;
	m->repeticoes = repeticoes;
}

#define TAMANHO_DA_MENSAGEM 4

#endif // MENSAGEM_H
