# Análise de Algoritimo: Dijkstra



## 📖 Descrição

Este projeto implementa o algoritmo de Dijkstra para encontrar o caminho mais curto em grafos. Ele inclui implementações em C e Python, além de ferramentas para análise e geração de grafos.

---

## 📂 Estrutura do Projeto

```
src/
	c/
		cJSON.c
		cJSON.h
		dijskra.c
	graphs/
		grafo_G_medio.json
		grafo_G_melhor.json
		grafo_G_pior.json
		grafo_M_medio.json
		grafo_M_melhor.json
		grafo_M_pior.json
		grafo_P_medio.json
		grafo_P_melhor.json
		grafo_P_pior.json
	python/
		analysis.ipynb
		dijkstra.ipynb
		graph_generator.ipynb
```

---

### 📁 Diretórios

- **src/c/**: Contém a implementação do algoritmo de Dijkstra em C e bibliotecas auxiliares.
- **src/graphs/**: Contém grafos em formato JSON para testes e análises.
- **src/python/**: Contém notebooks Jupyter para análise, implementação do algoritmo em Python e geração de grafos.

---

## 🛠️ Como Usar

### 💻 Implementação em C

1. Compile os arquivos C usando um compilador como `gcc`.
2. Execute o programa para calcular o caminho mais curto em um grafo.

### 🐍 Implementação em Python

1. Abra os notebooks Jupyter na pasta `src/python/`.
2. Execute os notebooks para análise, geração de grafos ou execução do algoritmo de Dijkstra.

---

## 📦 Dependências

### 🔧 C

- `gcc` para compilação.

### 🐍 Python

- Jupyter Notebook
- Bibliotecas Python necessárias (instale usando `pip`):
  - `numpy`
  - `matplotlib`

---

### 📚 Bibliotecas Utilizadas

#### cJSON

A biblioteca cJSON é utilizada para manipulação de arquivos JSON na implementação em C. Ela permite a leitura, escrita e análise de dados em formato JSON de maneira eficiente e simples. Para mais informações, visite o [repositório oficial do cJSON](https://github.com/DaveGamble/cJSON).

---

## 🤝 Contribuição

Sinta-se à vontade para contribuir com melhorias ou novas funcionalidades. Faça um fork do repositório e envie um pull request.

---

## 📝 Licença

Este projeto está licenciado sob a [MIT License](LICENSE).

---

## 🏫 Informações Adicionais

Este projeto foi realizado no 4º período do curso de Ciência da Computação da CESAR School para a disciplina de Teoria da Computação.
