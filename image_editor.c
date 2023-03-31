// Copyright Dragan Dragos Ovidiu 313CAb 2022-2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

struct imagine {
	char tip_imagine[10];
	int coloane; // numarul de coloane
	int linii; // numarul de linii
	int **matrice;
	int culoare; // numarul intensitatii culorii
	int verificare;
};

struct select {
	char tip_imagine[10];
	int coloane; // numarul de coloane
	int linii; // numarul de linii
	int **matrice;
	int culoare;
	int linie_start; // coordonatele selectiei
	int linie_stop;
	int coloana_start;
	int coloana_stop;
};

int minim(int x1, int x2)
{
	// minimul dintre 2 numere
	if (x1 < x2)
		return x1;
	else
		return x2;
}

int maxim(int x1, int x2)
{
	// maximul dintre 2 numere
	if (x1 > x2)
		return x1;
	else
		return x2;
}

void dealocare(struct imagine **img)
{
	// dealoca matricea  din cadrul structurii imagine
	for (int i = 0; i < (*img)->linii; i++)
		free((*img)->matrice[i]);

	free((*img)->matrice);
}

void dealocare_select(struct select **aux)
{
	// dealoca matricea  din cadrul structurii select
	for (int i = 0; i < (*aux)->linii; i++)
		free((*aux)->matrice[i]);

	free((*aux)->matrice);
}

void alocare(int ***matrice, int linii, int coloane)
{
	// aloca dinamic o matrice
	(*matrice) = (int **)malloc(linii * sizeof(int *));
	if ((*matrice) == NULL)
		printf("Nu s-a putut aloca matricea");

	for (int i = 0; i < linii; i++) {
		(*matrice)[i] = (int *)malloc(coloane * sizeof(int));
		if ((*matrice)[i] == NULL)
			printf("Nu s-a putut aloca matricea");
	}
}

int inmultire(int chenar[3][3], int ker[3][3])
{
	// functie pentru a inmulti matricea kernel cu matricea unei culori
	int s = 0;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			s = s + chenar[i][j] * ker[i][j];
	return s;
}

int clamp(int x)
{
	if (x < 0)
		return 0;
	if (x > 255)
		return 255;
	return x;
}

int select_poza(struct imagine *img, struct select **aux, int c[], int ok)
{
	if (abs(c[0] - c[2]) > 0 && abs(c[1] - c[3]) > 0) {
		if (c[0] >= 0 && c[1] >= 0 && c[2] >= 0 && c[3] >= 0) {
			// pentru a verifica ca nu este nicio coordonata negativa
			char tip[10];
			strcpy(tip, img->tip_imagine);
			int ppm;
			if (strcmp(tip, "P3") == 0 || strcmp(tip, "P6") == 0)
				ppm = 3;
			else
				ppm = 1;
			if (ok == 1)
				ppm = 1;
			if (c[0] * ppm <= img->coloane && c[2] * ppm <= img->coloane &&
				c[1] <= img->linii && c[3] <= img->linii) {
				// verific daca coordonatele sunt valide
				if (img->verificare)
					dealocare_select(aux);
				strcpy((*aux)->tip_imagine, img->tip_imagine);
				(*aux)->culoare = img->culoare;
				(*aux)->coloane = abs(c[2] - c[0]); // retin numarul de coloane
				(*aux)->linii = abs(c[3] - c[1]); // retin numarul de linii

				if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0))
					(*aux)->coloane = (*aux)->coloane * ok;

				alocare(&(*aux)->matrice, (*aux)->linii, (*aux)->coloane);
				// retin coordonatele selectiei
				(*aux)->linie_start = minim(c[1], c[3]);
				(*aux)->linie_stop = maxim(c[1], c[3]);
				(*aux)->coloana_start = minim(c[0], c[2]);
				(*aux)->coloana_stop = maxim(c[0], c[2]);
				if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0)) {
					(*aux)->coloana_start = (*aux)->coloana_start * ok;
					(*aux)->coloana_stop = (*aux)->coloana_stop * ok;
				}
				for (int i = 0; i < (*aux)->linii; i++)
					for (int j = 0; j < (*aux)->coloane; j++) {
						int x = (*aux)->linie_start + i;
						int y = (*aux)->coloana_start + j;
						(*aux)->matrice[i][j] = img->matrice[x][y];
						// am creat matricea selectata
					}
				return 1;
			}
		}
	}
	return 0;
}

void selectare(struct imagine *img, struct select *aux, char sir[])
{
	char *token = strtok(sir, " ");
	int c[4] = { 0 }, i = 0, ok = 0, ok1 = 3, ok2 = 1;
	if (strncmp(token, "ALL", 3) == 0) {
		c[0] = 0; //daca este SELECT ALL retin dimensiunile intregii poze
		c[1] = 0;
		c[2] = img->coloane;
		c[3] = img->linii;
		ok = 1;
	} else {
		while (token) {
			int l = strlen(token);
			for (int i = 0; i < l; i++)
				if (isalpha(token[i]) != 0)
					ok2 = 0;
			c[i] = atoi(token);
			token = strtok(NULL, " ");
			i++;
		} // retin dimensiunile
		ok = 3;
	}
	if (img->verificare) {
		if ((i == 4 || ok == 1) && ok2 == 1) {
			ok1 = select_poza(img, &aux, c, ok);
		} else {
			printf("Invalid command\n");
			return;
		}
	} else {
		printf("No image loaded\n");
	}

	if (ok == 1 && ok1 == 1) {
		printf("Selected ALL\n");
	} else if (ok1 == 1) {
		int min_lin = minim(c[0], c[2]);
		int min_col = minim(c[1], c[3]);
		int max_lin = maxim(c[0], c[2]);
		int max_col = maxim(c[1], c[3]);
		printf("Selected %d %d %d %d\n", min_lin, min_col, max_lin, max_col);
	} else if (ok1 == 0) {
		printf("Invalid set of coordinates\n");
	}
}

void load(struct imagine **img, struct select *aux, char *filename)
{
	char cuvant[10000];
	int k = 0, c[4] = { 0 };
	FILE *in = fopen(filename, "r");
	if (!in) {
		if ((*img)->verificare) {
			dealocare(img);
			dealocare_select(&aux);
			(*img)->verificare = 0;
		}
		printf("Failed to load %s\n", filename);
	} else {
		if ((*img)->verificare)
			dealocare(img);

		while (fscanf(in, "%s", cuvant)) {
			while (cuvant[0] == '#') {
				char c = fgetc(in);
				while (c != '\n')
					c = fgetc(in);
				fscanf(in, "%s", cuvant);
			} // trece peste liniile care incep cu #
			if (k == 0) {
				strcpy((*img)->tip_imagine, cuvant);
				k++;
				continue;
			} else if (k == 1) {
				(*img)->coloane = atoi(cuvant); // numarul de coloane
				fscanf(in, "%s", cuvant);
				(*img)->linii = atoi(cuvant); // numarul de linii
				k++;
				continue;
			} else if (k == 2) {
				(*img)->culoare = atoi(cuvant); // numarul intensitatii culorii
				k++;
			}
			char tip[10];
			strcpy(tip, (*img)->tip_imagine);
			if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0))
				(*img)->coloane = (*img)->coloane * 3;

			alocare(&(*img)->matrice, (*img)->linii, (*img)->coloane);
			if (strcmp(tip, "P2") == 0 || strcmp(tip, "P3") == 0) {
				for (int i = 0; i < (*img)->linii; i++)
					for (int j = 0; j < (*img)->coloane; j++)
						fscanf(in, "%d", &(*img)->matrice[i][j]);

			} else {
				unsigned char val;
				fread(&val, sizeof(unsigned char), 1, in);
				for (int i = 0; i < (*img)->linii; i++)
					for (int j = 0; j < (*img)->coloane; j++) {
						fread(&val, sizeof(unsigned char), 1, in);
						(*img)->matrice[i][j] = (int)val;
					}
			}
			printf("Loaded %s\n", filename);
			break;
		}
		c[0] = 0;
		c[1] = 0;
		c[2] = (*img)->coloane;
		c[3] = (*img)->linii;
		select_poza(*img, &aux, c, 1);
		(*img)->verificare = 1;
		fclose(in);
	}
}

void histograma(struct imagine *img, int c[], int contor)
{
	if (img->verificare) {
		if (c[0] >= 0 && c[1] >= 0 && contor == 2) {
			char tip[10];
			strcpy(tip, img->tip_imagine);
			if (strcmp(tip, "P2") == 0 || strcmp(tip, "P5") == 0) {
				int frecv[256] = { 0 }, max_suma = -1;

				for (int i = 0; i < img->linii; i++)
					for (int j = 0; j < img->coloane; j++)
						frecv[img->matrice[i][j]]++;

				for (int i = 0; i < c[1]; i++) {
					int s = 0;
					int x = (i * 256) / c[1];
					for (int j = x; j < ((i + 1) * 256) / c[1]; j++)
						s = s + frecv[j];
					// aflu suma maxima a frecventelor pixelilor
					if (max_suma < s)
						max_suma = s;
				}
				for (int i = 0; i < c[1]; i++) {
					int s = 0;
					int x = (i * 256) / c[1];
					for (int j = x; j < ((i + 1) * 256) / c[1]; j++)
						s = s + frecv[j];
					printf("%d\t|\t", s * c[0] / max_suma);
					for (int i = 0; i < s * c[0] / max_suma; i++)
						printf("*");
					printf("\n");
				}
			} else {
				printf("Black and white image needed\n");
			}
		} else {
			printf("Invalid command\n");
		}
	} else {
		printf("No image loaded\n");
	}
}

void egalizare(struct imagine **img, struct select **aux)
{
	if ((*img)->verificare) {
		char tip[10];
		strcpy(tip, (*img)->tip_imagine);
		if (strcmp(tip, "P2") == 0 || strcmp(tip, "P5") == 0) {
			int frecv[256] = { 0 };
			double aria = (*img)->coloane * (*img)->linii;
			// calculez frecventa fiecarui pixel
			for (int i = 0; i < (*img)->linii; i++)
				for (int j = 0; j < (*img)->coloane; j++)
					frecv[(*img)->matrice[i][j]]++;

			for (int i = 0; i < (*img)->linii; i++) {
				for (int j = 0; j < (*img)->coloane; j++) {
					double s = 0;
					// fac suma frecventelor pixelilor
					// pana la pixelul respectiv, inclusiv
					for (int p = 0; p <= (*img)->matrice[i][j]; p++)
						s = s + frecv[p];
					double prod1 = (double)255 * s;
					double prod = (double)prod1 / aria;
					(*img)->matrice[i][j] = clamp(round(prod));
				}
			}
			int lin_start = (*aux)->linie_start;
			int lin_stop = (*aux)->linie_stop;
			int col_start = (*aux)->coloana_start;
			int col_stop = (*aux)->coloana_stop;
			for (int i = lin_start; i < lin_stop; i++)
				for (int j = col_start; j < col_stop; j++) {
					int x = i - (*aux)->linie_start;
					int y = j - (*aux)->coloana_start;
					(*aux)->matrice[x][y] = (*img)->matrice[i][j];
				}

			printf("Equalize done\n");
		} else {
			printf("Black and white image needed\n");
		}
	} else {
		printf("No image loaded\n");
	}
}

void crop(struct imagine **img, struct select *aux)
{
	if ((*img)->verificare) {
		// dealoc matricea initiala pentru a o aloca cu dimensiunile selectate
		dealocare(img);
		(*img)->coloane = aux->coloane;
		(*img)->linii = aux->linii;
		alocare(&(*img)->matrice, aux->linii, aux->coloane);
		for (int i = 0; i < aux->linii; i++)
			for (int j = 0; j < aux->coloane; j++)
				(*img)->matrice[i][j] = aux->matrice[i][j];
		aux->coloana_start = 0;
		aux->coloana_stop = aux->coloane;
		aux->linie_start = 0;
		aux->linie_stop = aux->linii;
		printf("Image cropped\n");
	} else {
		printf("No image loaded\n");
	}
}

int prelucrare(struct imagine **img, struct select **aux, int ker[3][3], int n)
{
	char tip[10];
	strcpy(tip, (*img)->tip_imagine);
	if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0)) {
		int lin_start, lin_stop, col_start, col_stop;
		int rosu[3][3], verde[3][3], albastru[3][3];
		lin_start = (*aux)->linie_start;
		lin_stop = (*aux)->linie_stop;
		col_start = (*aux)->coloana_start;
		col_stop = (*aux)->coloana_stop;

		int cop = lin_start;
		int cop2 = lin_stop;
		int cop3 = col_start;
		int cop4 = col_stop;

		// verific dimensiunile pentru a evita bordarea marginilor pozei
		if (lin_start == 0)
			lin_start = 1;
		if (lin_stop == (*img)->linii)
			lin_stop = lin_stop - 1;
		if (col_start == 0)
			col_start = 3;
		if (col_stop == (*img)->coloane)
			col_stop = col_stop - 3;

		for (int i = lin_start; i < lin_stop; i++) {
			for (int j = col_start; j < col_stop; j = j + 3) {
				for (int k = 0; k < 3; k++) { // liniile matricei copie
					for (int l = 0; l < 3; l++) {
						int x = j + l * 3;
						// retin in fiecare matrice (rosu, verde,albastru)
						// pixelii corespunzatori culorii
						rosu[k][l] = (*img)->matrice[i - 1 + k][x - 3];
						verde[k][l] = (*img)->matrice[i - 1 + k][x - 2];
						albastru[k][l] = (*img)->matrice[i - 1 + k][x - 1];
					}
				}
				int x = i - cop;
				int y = j - cop3;
				int red, green, blue;
				red = clamp(round(inmultire(rosu, ker) / n));
				green = clamp(round(inmultire(verde, ker) / n));
				blue = clamp(round(inmultire(albastru, ker) / n));
				(*aux)->matrice[x][y] = red;
				(*aux)->matrice[x][y + 1] = green;
				(*aux)->matrice[x][y + 2] = blue;
			}
		}
		// copiez in matricea initiala valorile prelucrate
		for (int i = cop; i < cop2; i++)
			for (int j = cop3; j < cop4; j++)
				(*img)->matrice[i][j] = (*aux)->matrice[i - cop][j - cop3];
		return 1;
	}
	printf("Easy, Charlie Chaplin\n");
	return 0;
}

void apply(struct imagine *img, struct select *aux, char *token)
{
	if (img->verificare) {
		if (!token) { // verific daca mai exista vreun cuvant dupa APPLY
			printf("Invalid command\n");
			// apelez functia corespunzatoare inputului
		} else if (strcmp(token, "EDGE\n") == 0) {
			int ker[3][3] = { {-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1} };
			if (prelucrare(&img, &aux, ker, 1) == 1)
				printf("APPLY EDGE done\n");

		} else if (strcmp(token, "SHARPEN\n") == 0) {
			int ker[3][3] = { { 0, -1, 0}, { -1, 5, -1 }, { 0, -1, 0 } };
			if (prelucrare(&img, &aux, ker, 1) == 1)
				printf("APPLY SHARPEN done\n");

		} else if (strcmp(token, "BLUR\n") == 0) {
			int ker[3][3] = { {1, 1, 1}, {1, 1, 1}, {1, 1, 1} };
			if (prelucrare(&img, &aux, ker, 9) == 1)
				printf("APPLY BLUR done\n");

		} else if (strcmp(token, "GAUSSIAN_BLUR\n") == 0) {
			int ker[3][3] = { { 1, 2, 1}, { 2, 4, 2 }, { 1, 2, 1  } };
			if (prelucrare(&img, &aux, ker, 16) == 1)
				printf("APPLY GAUSSIAN_BLUR done\n");

		} else {
			printf("APPLY parameter invalid\n");
		}
	} else {
		printf("No image loaded\n");
	}
}

void rotire90_select(struct select **aux, int n)
{
	// rotesc o selectie greyscale cu 90 de grade spre dreapta
	for (int i = 0; i < n / 2; i++) {
		for (int j = i; j < n - i - 1; j++) {
			int cop = (*aux)->matrice[i][j];
			(*aux)->matrice[i][j] = (*aux)->matrice[n - j - 1][i];
			(*aux)->matrice[n - j - 1][i] = cop;
		}
		for (int j = i; j < n - i - 1; j++) {
			int x = n - j - 1;
			int cop = (*aux)->matrice[x][i];
			(*aux)->matrice[x][i] = (*aux)->matrice[n - i - 1][x];
			(*aux)->matrice[n - i - 1][x] = cop;
		}
		for (int j = i; j < n - i - 1; j++) {
			int x = n - i - 1;
			int cop = (*aux)->matrice[x][n - j - 1];
			(*aux)->matrice[x][n - j - 1] = (*aux)->matrice[j][x];
			(*aux)->matrice[j][x] = cop;
		}
	}
}

void rotire90_selectcolor(struct select **aux, int n)
{
	// rotesc o selectie color spre dreapta
	for (int i = 0; i < n / 2; i++) {
		for (int j = 3 * i; j < 3 * (n - i - 1); j = j + 3) {
			// interschimba cadranele din coltul stanga sus si coltul
			// dreapta jos al patratului curent.
			int x = n - j / 3 - 1;
			int cop = (*aux)->matrice[i][j];
			(*aux)->matrice[i][j] = (*aux)->matrice[x][i * 3]; // pentru rosu
			(*aux)->matrice[x][i * 3] = cop;

			cop = (*aux)->matrice[i][j + 1];// pentru verde
			(*aux)->matrice[i][j + 1] = (*aux)->matrice[x][i * 3 + 1];
			(*aux)->matrice[x][i * 3 + 1] = cop;

			cop = (*aux)->matrice[i][j + 2]; // pentru albastru
			(*aux)->matrice[i][j + 2] = (*aux)->matrice[x][i * 3 + 2];
			(*aux)->matrice[x][i * 3 + 2] = cop;
		}
		for (int j = 3 * i; j < 3 * (n - i - 1); j = j + 3) {
			// interschimba cadranele din coltul stanga jos si coltul
			// dreapta sus al patratului curent.
			int x = n - j / 3 - 1;
			int y = 3 * n - j;
			int cop = (*aux)->matrice[x][i * 3];// pentru rosu
			(*aux)->matrice[x][i * 3] = (*aux)->matrice[n - i - 1][y - 3];
			(*aux)->matrice[n - i - 1][y - 3] = cop;

			cop = (*aux)->matrice[x][i * 3 + 1];// pentru verde
			(*aux)->matrice[x][i * 3 + 1] = (*aux)->matrice[n - i - 1][y - 2];
			(*aux)->matrice[n - i - 1][y - 2] = cop;

			cop = (*aux)->matrice[x][i * 3 + 2];// pentru albastru
			(*aux)->matrice[x][i * 3 + 2] = (*aux)->matrice[n - i - 1][y - 1];
			(*aux)->matrice[n - i - 1][y - 1] = cop;
		}
		for (int j = 3 * i; j < 3 * (n - i - 1); j = j + 3) {
			// interschimba cadranele din coltul dreapta sus si coltul stanga
			// jos al patratului curent.
			int y = 3 * n - i * 3;
			int x = 3 * n - j;
			int cop = (*aux)->matrice[n - i - 1][x - 3];// pentru rosu
			(*aux)->matrice[n - i - 1][x - 3] = (*aux)->matrice[j / 3][y - 3];
			(*aux)->matrice[j / 3][y - 3] = cop;

			cop = (*aux)->matrice[n - i - 1][x - 2];// pentru verde
			(*aux)->matrice[n - i - 1][x - 2] = (*aux)->matrice[j / 3][y - 2];
			(*aux)->matrice[j / 3][y - 2] = cop;

			cop = (*aux)->matrice[n - i - 1][x - 1];// pentru albastru
			(*aux)->matrice[n - i - 1][x - 1] = (*aux)->matrice[j / 3][y - 1];
			(*aux)->matrice[j / 3][y - 1] = cop;
		}
	}
}

int rotire_select(struct imagine *img, struct select *aux, char *grad, int p2)
{
	int ok = 0;
	char tip[10];
	strcpy(tip, img->tip_imagine);

	// verific ce fel de rotire trebuie sa se efectueze pe o portiune selectata
	if ((strcmp(grad, "90\n") == 0) || (strcmp(grad, "-270\n") == 0)) {
		if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0))
			rotire90_selectcolor(&aux, p2);
		else
			rotire90_select(&aux, p2);
		ok = 1;

	} else if ((strcmp(grad, "-90\n") == 0) || (strcmp(grad, "270\n") == 0)) {
		if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0)) {
			rotire90_selectcolor(&aux, p2);
			rotire90_selectcolor(&aux, p2);
			rotire90_selectcolor(&aux, p2);
		} else {
			rotire90_select(&aux, p2);
			rotire90_select(&aux, p2);
			rotire90_select(&aux, p2);
		}
		ok = 1;
	} else if (strcmp(grad, "180\n") == 0 || strcmp(grad, "-180\n") == 0) {
		if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0)) {
			rotire90_selectcolor(&aux, p2);
			rotire90_selectcolor(&aux, p2);
		} else {
			rotire90_select(&aux, p2);
			rotire90_select(&aux, p2);
		}
		ok = 1;
	} else if (strcmp(grad, "360\n") == 0 || strcmp(grad, "-360\n") == 0 ||
		strcmp(grad, "0\n") == 0) {
		ok = 1;
	}
	return ok;
}

void rotire90_pozanoncolor(struct imagine **img, struct select **aux)
{  // rotesc o poza greyscale la dreapta cu 90 de grade
	dealocare_select(aux);
	alocare(&(*aux)->matrice, (*img)->coloane, (*img)->linii);
	(*aux)->coloane = (*img)->linii;
	(*aux)->linii = (*img)->coloane;
	(*aux)->coloana_start = 0;
	(*aux)->coloana_stop = (*img)->linii;
	// fac pentru poza noncolor deci nu trebuie sa inmultesc cu 3 coloanele
	(*aux)->linie_start = 0;
	(*aux)->linie_stop = (*img)->coloane;
	for (int i = 0; i < (*aux)->linii; i++)// rotesc matricea auxiliara
		for (int j = 0; j < (*aux)->coloane; j++)
			(*aux)->matrice[i][j] = (*img)->matrice[(*img)->linii - 1 - j][i];

	// dealoc poza initiala
	dealocare(img);
	// aloc o matrice cu dimensiunile bune
	alocare(&(*img)->matrice, (*aux)->linii, (*aux)->coloane);
	(*img)->linii = (*aux)->linii;
	(*img)->coloane = (*aux)->coloane;
	for (int i = 0; i < (*img)->linii; i++)
		for (int j = 0; j < (*img)->coloane; j++)
			(*img)->matrice[i][j] = (*aux)->matrice[i][j];
}

void rotire90_pozacolor(struct imagine **img, struct select **aux)
{	// efectuez o rotire la stanga pentru  poza color cu 90 de grade
	dealocare_select(aux);
	(*aux)->coloane = (*img)->linii * 3;
	(*aux)->linii = (*img)->coloane / 3;
	(*aux)->coloana_start = 0;
	(*aux)->coloana_stop = (*img)->linii * 3; // nu trebuie sa inmultesc cu 3
	(*aux)->linie_start = 0; //                  (poza greyscale)
	(*aux)->linie_stop = (*img)->coloane / 3;
	// aloc o matrice de dimensiunile potrivite
	alocare(&(*aux)->matrice, (*aux)->linii, (*aux)->coloane);

	for (int i = 0; i < (*aux)->linii; i++) {
		int y = (*img)->coloane - i * 3;
		for (int j = 0; j < (*aux)->coloane; j += 3)
			(*aux)->matrice[i][j] = (*img)->matrice[j / 3][y - 3];
		for (int j = 1; j < (*aux)->coloane; j += 3)
			(*aux)->matrice[i][j] = (*img)->matrice[j / 3][y - 2];
		for (int j = 2; j < (*aux)->coloane; j += 3)
			(*aux)->matrice[i][j] = (*img)->matrice[j / 3][y - 1];
	}
	dealocare(img);// dealoc matricea veche
	// aloc o matrice de dimensiunile bune
	alocare(&(*img)->matrice, (*aux)->linii, (*aux)->coloane);
	(*img)->linii = (*aux)->linii;
	(*img)->coloane = (*aux)->coloane;
	for (int i = 0; i < (*img)->linii; i++)//copiez in matricea imaginea rotita
		for (int j = 0; j < (*img)->coloane; j++)
			(*img)->matrice[i][j] = (*aux)->matrice[i][j];
}

int rotire_poza(struct imagine *img, struct select *aux, char *grad)
{
	int ok = 0;
	char tip[10];
	strcpy(tip, img->tip_imagine);
	// verific ce fel de rotire trebuie sa se efectueze pe intreaga poza
	if ((strcmp(grad, "-90\n") == 0) || (strcmp(grad, "270\n") == 0)) {
		if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0)) {
			rotire90_pozacolor(&img, &aux);
		} else {
			rotire90_pozanoncolor(&img, &aux);
			rotire90_pozanoncolor(&img, &aux);
			rotire90_pozanoncolor(&img, &aux);
		}
		ok = 1;

	} else if ((strcmp(grad, "90\n") == 0) || (strcmp(grad, "-270\n") == 0)) {
		if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0)) {
			rotire90_pozacolor(&img, &aux);
			rotire90_pozacolor(&img, &aux);
			rotire90_pozacolor(&img, &aux);
		} else {
			rotire90_pozanoncolor(&img, &aux);
		}
		ok = 1;
	} else if (strcmp(grad, "180\n") == 0 || strcmp(grad, "-180\n") == 0) {
		if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0)) {
			rotire90_pozacolor(&img, &aux);
			rotire90_pozacolor(&img, &aux);
		} else {
			rotire90_pozanoncolor(&img, &aux);
			rotire90_pozanoncolor(&img, &aux);
		}
		ok = 1;
	} else if (strcmp(grad, "360\n") == 0 || strcmp(grad, "-360\n") == 0 ||
		strcmp(grad, "0\n") == 0) {
		ok = 1;
	}
	return ok;
}

void rotire(struct imagine **img, struct select *aux, char *grad)
{
	if ((*img)->verificare) { // functie pentru apelarea functiei ROTATE
		int lin_start, lin_stop, col_start, col_stop;
		lin_start = aux->linie_start;
		lin_stop = aux->linie_stop;
		col_start = aux->coloana_start;
		col_stop = aux->coloana_stop;

		int p1 = lin_stop - lin_start;
		int p2 = col_stop - col_start;

		char tip[10];
		strcpy(tip, (*img)->tip_imagine);
		if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0))
			p2 = p2 / 3;

		int ok = 0;
		if ((*img)->coloane == aux->coloane && (*img)->linii == aux->linii) {
			ok = rotire_poza((*img), aux, grad); // rotire pentru toata poza
		} else {
			if (p1 == p2) { //rotire pentru o selectie patratica
				ok = rotire_select((*img), aux, grad, p2);
				for (int i = lin_start; i < lin_stop; i++)
					for (int j = col_start; j < col_stop; j++) {
						int linie = i - lin_start;
						int coloana = j - col_start;
						(*img)->matrice[i][j] = aux->matrice[linie][coloana];

					} // copiez in matricea initiala rotirea efectuata
			} else {
				printf("The selection must be square\n");
			}
		}

		if (ok == 0)
			printf("Unsupported rotation angle\n");
		else
			printf("Rotated %s", grad);

	} else {
		printf("No image loaded\n");
	}
}

void salvare(struct imagine *img, char *filename, int ok)
{
	if (img->verificare) {
		FILE *out = fopen(filename, "w");
		int coloana = 0;
		char tip[10];
		strcpy(tip, img->tip_imagine);

		if (ok) {// daca trebuie in ascii
			if ((strcmp(tip, "P5") == 0))
				strcpy(img->tip_imagine, "P2");

			if ((strcmp(tip, "P6") == 0))
				strcpy(img->tip_imagine, "P3");

			fprintf(out, "%s\n", img->tip_imagine);

			if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0))
				coloana = img->coloane / 3;
			else
				coloana = img->coloane;

			fprintf(out, "%d %d\n%d\n", coloana, img->linii, img->culoare);
			for (int i = 0; i < img->linii; i++) { // afisare in ascii
				for (int j = 0; j < img->coloane; j++)
					fprintf(out, "%d ", img->matrice[i][j]);
				fprintf(out, "\n");
			}
		} else { // daca trebuie sa fie in binar
			if ((strcmp(tip, "P2") == 0))
				strcpy(img->tip_imagine, "P5");

			if ((strcmp(tip, "P3") == 0))
				strcpy(img->tip_imagine, "P6");

			fprintf(out, "%s\n", img->tip_imagine);

			if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0))
				coloana = img->coloane / 3;
			else
				coloana = img->coloane;

			fprintf(out, "%d %d\n%d\n", coloana, img->linii, img->culoare);

			for (int i = 0; i < img->linii; i++) { // afisare in binar
				for (int j = 0; j < img->coloane; j++)
					fwrite(&img->matrice[i][j], sizeof(unsigned char), 1, out);
			}
		}
		fclose(out);
		if (out)
			printf("Saved %s\n", filename);
	} else {
		printf("No image loaded\n");
	}
}

int main(void)
{
	char sir[100];
	struct imagine *img = NULL; // aloc dinamic structurile
	struct select *mat = NULL;
	img = malloc(sizeof(struct imagine));
	mat = malloc(sizeof(struct select));
	if (!img || !mat) // programare defensiva
		printf("Could not alloc the struct\n");
	img->verificare = 0;
	while (fgets(sir, 100, stdin)) { // citesc cat timp sunt randuri de citit
		char sir1[50];
		strcpy(sir1, sir);
		char *token = strtok(sir, " ");
		if (strcmp(token, "LOAD") == 0) {
			token = strtok(NULL, " "); // stochez in token numele fisierului
			token[strlen(token) - 1] = '\0';
			load(&img, mat, token);
			continue;
		}
		if (strncmp(token, "SELECT", 6) == 0) {
			char sir2[50] = "";
			int l = strlen(token) + 1;
			strcpy(sir2, sir1 + l); // copiez in sir2 restul de caractere
			selectare(img, mat, sir2);
			continue;
		}
		if (strncmp(token, "HISTOGRAM", 9) == 0) {
			token = strtok(NULL, " ");
			int c[2] = { -1, -1 }, i = 0;
			while (token) {
				c[i] = atoi(token);
				token = strtok(NULL, " ");
				i++;
			} // retin in vectorul v, x stelute si y binuri
			histograma(img, c, i);
			continue;
		}
		if (strcmp(token, "EQUALIZE\n") == 0) {
			egalizare(&img, &mat);
			continue;
		}
		if (strcmp(token, "CROP\n") == 0) {
			crop(&img, mat);
			continue;
		}
		if (strncmp(token, "APPLY", 5) == 0) {
			token = strtok(NULL, " "); // retin in token operatia ceruta
			apply(img, mat, token);
			continue;
		}
		if (strcmp(token, "ROTATE") == 0) {
			token = strtok(NULL, " ");
			rotire(&img, mat, token);
			continue;
		}
		if (strcmp(token, "SAVE") == 0) {
			int ok = 0;
			token = strtok(NULL, " ");
			char *verificare = strtok(NULL, " ");
			if (!verificare) // verific sa vad daca dupa SAVE este "ascii"
				token[strlen(token) - 1] = '\0';
			else if (strcmp(verificare, "ascii\n") == 0)
				ok = 1;
			salvare(img, token, ok);
			continue;
		}
		if (strncmp(token, "EXIT", 4) == 0) {
			if (img->verificare) {
				dealocare(&img);
				dealocare_select(&mat);
			} else {
				printf("No image loaded\n");
			}
			free(img);
			free(mat);// dealoc tot ce am alocat dinamic
			break;
		}
		printf("Invalid command\n");
	}
	return 0;
}
