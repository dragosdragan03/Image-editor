#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct imagine {
	char tip_imagine[10];
	int coloane; // numarul de coloane
	int linii; // numarul de linii
	int **matrice;
	int culoare; // numarul intensitatii culorii
};

struct selectie {
	char tip_imagine[10];
	int coloane; // numarul de coloane
	int linii; // numarul de linii
	int **matrice;
	int culoare;
	int linie_start; // de unde incepe indexul liniei
	int linie_stop;
	int coloana_start;
	int coloana_stop;
};

int minim(int x1, int x2)
{
	if (x1 < x2)
		return x1;
	else
		return x2;
}

int maxim(int x1, int x2)
{
	if (x1 > x2)
		return x1;
	else
		return x2;
}

void alocare(int ***matrice, int linii, int coloane)
{
	(*matrice) = (int **)malloc(linii * sizeof(int *));
	if ((*matrice) == NULL) {
		printf("Nu s-a putut aloca matricea\n");
	}

	for (int i = 0; i < linii; i++) {
		(*matrice)[i] = (int *)malloc(coloane * sizeof(int));
		if ((*matrice)[i] == NULL) {
			printf("Nu s-a putut aloca matricea");
		}
	}
}

void dealocare(struct imagine **img)
{
	for (int i = 0; i < (*img)->linii; i++)
		free((*img)->matrice[i]);

	free((*img)->matrice);

}

void dealocare_select(struct selectie **img)
{
	for (int i = 0; i < (*img)->linii; i++)
		free((*img)->matrice[i]);

	free((*img)->matrice);

}

void dealocare_matrice(int **mat, int linie_start, int linie_stop)
{
	for (int i = linie_start; i < linie_stop; i++)
		free(mat[i]);

	free(mat);
}

void load(struct imagine **img, char *filename)
{
	char cuvant[10000], sir[1000];
	int k = 0;
	FILE *in = fopen(filename, "r");
	if (!in) {
		printf("Failed to load %s\n", filename);
	} else {
		if ((*img))
			dealocare(img);

		while (fscanf(in, "%s", cuvant)) {
			// if (strcmp(cuvant, "#") == 0)
			// 	continue;
			while (cuvant[0] == '#') {
				char c = fgetc(in);
				while (c != '\n')
					c = fgetc(in);
				fscanf(in, "%s", cuvant);
			}
			// trece peste liniile care incep cu #
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
				continue;
			}
			char tip[10];
			strcpy(tip, (*img)->tip_imagine);
			if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0))
				(*img)->coloane = (*img)->coloane * 3;

			if (k > 2) {
				alocare(&(*img)->matrice, (*img)->linii, (*img)->coloane);
				int i, j;
				(*img)->matrice[0][0] = atoi(cuvant);
				for (int i = 0; i < (*img)->linii; i++)
					for (int j = 0; j < (*img)->coloane; j++) {
						if (i == 0 && j == 0)
							continue;
						fscanf(in, "%d", &(*img)->matrice[i][j]);
					}
				printf("Loaded %s\n", filename);
				break;
			}
		}
	}
	// for (int i = 0; i < (*img)->linii; i++) {
	// 	for (int j = 0; j < (*img)->coloane; j++) {
	// 		printf("%d ", (*img)->matrice[i][j]);
	// 	}
	// 	printf("\n");
	// }

}

void select_poza(struct imagine *img, struct selectie **aux, int c[])
{
	if (img->matrice) {
		if (c[0] * c[1] * c[2] * c[3] >= 0)// pentru a verifica ca nu este nicio coordonata negativa
		{
			if (c[0] <= img->coloane && c[2] <= img->coloane && c[1] <= img->linii && c[3] <= img->linii) {
				if ((*aux))
					dealocare_select(aux);
				strcpy((*aux)->tip_imagine, img->tip_imagine);
				(*aux)->culoare = img->culoare;
				(*aux)->coloane = abs(c[2] - c[0]);
				(*aux)->linii = abs(c[3] - c[1]);
				char tip[10];
				strcpy(tip, (*aux)->tip_imagine);

				if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0))
					(*aux)->coloane = (*aux)->coloane * 3;

				alocare(&(*aux)->matrice, (*aux)->linii, (*aux)->coloane);
				(*aux)->linie_start = minim(c[0], c[2]);
				(*aux)->coloana_start = minim(c[1], c[3]);
				(*aux)->linie_stop = maxim(c[0], c[2]);
				(*aux)->coloana_stop = maxim(c[1], c[3]);
				for (int i = 0; i < (*aux)->linii; i++)
					for (int j = 0; j < (*aux)->coloane; j++)
						(*aux)->matrice[i][j] = img->matrice[(*aux)->coloana_start + i][(*aux)->linie_start + j]; // am creat matricea selectata

				// for (int i = 0; i < (*aux)->linii; i++) {
				// 	for (int j = 0; j < (*aux)->coloane; j++) {
				// 		printf("%d ", (*aux)->matrice[i][j]);
				// 	}
				// 	printf("\n");
				// }
				// printf("%d %d %d %d\n", (*aux)->linie_start, (*aux)->coloana_start, (*aux)->linie_stop, (*aux)->coloana_stop);
				printf("Selected %d %d %d %d\n", c[0], c[1], c[2], c[3]);
			} else printf("Invalid coordinates\n");
		} else printf("Invalid coordinates\n");
	} else printf("No image loaded");

}

void histograma(struct imagine *img, int x, int y)
{
	if (img->matrice) {
		if (strcmp(img->tip_imagine, "P2") == 0 || strcmp(img->tip_imagine, "P5") == 0) {
			int frecv[256] = { 0 }, max_suma = -1;

			for (int i = 0; i < img->linii; i++)
				for (int j = 0; j < img->coloane; j++)
					frecv[img->matrice[i][j]]++;

			for (int i = 0; i < y; i++) {
				int s = 0;
				for (int j = (i * 256) / y; j < ((i + 1) * 256) / y; j++)
					s = s + frecv[j];
				if (max_suma < s)
					max_suma = s;

			}
			for (int i = 0; i < y; i++) {
				int s = 0;
				for (int j = (i * 256) / y; j < ((i + 1) * 256) / y; j++)
					s = s + frecv[j];
				printf("%d\t|\t", s * x / max_suma);
				for (int i = 0; i < s * x / max_suma; i++)
					printf("*");
				printf("\n");
			}

		} else printf("Black and white image needed\n");
	} else printf("No image loaded\n");

}

void egalizare(struct imagine **img)
{
	if ((*img)->matrice) {
		if (strcmp((*img)->tip_imagine, "P2") == 0 || strcmp((*img)->tip_imagine, "P5") == 0) {
			int frecv[256] = { 0 };
			int s = 0, aria = (*img)->coloane * (*img)->linii;
			for (int i = 0; i < (*img)->linii; i++)
				for (int j = 0; j < (*img)->coloane; j++)
					frecv[(*img)->matrice[i][j]]++;
			for (int i = 0; i < (*img)->linii; i++) {
				for (int j = 0; j < (*img)->coloane; j++) {
					s = 0;
					for (int p = 0; p < (*img)->matrice[i][j]; p++)
						s = s + frecv[p];

					(*img)->matrice[i][j] = 255 * s / aria;
				}

			}

			printf("Equalize done\n");
		} else printf("Black and white image needed\n");
	} else printf("No image loaded\n");
}

void salvare(struct imagine *img, char *filename)
{
	if (img->matrice) {
		FILE *out = fopen(filename, "w");
		int coloana = 0;
		fprintf(out, "%s\n", img->tip_imagine);
		char tip[10];
		strcpy(tip, img->tip_imagine);
		if ((strcmp(tip, "P3") == 0) || (strcmp(tip, "P6") == 0))
			coloana = img->coloane / 3;
		else
			coloana = img->coloane;
		fprintf(out, "%d %d\n%d\n", img->coloane, img->linii, img->culoare);
		for (int i = 0; i < img->linii; i++) {
			for (int j = 0; j < img->coloane; j++)
				fprintf(out, "%d ", img->matrice[i][j]);
			fprintf(out, "\n");
		}
		fclose(out);

		if (out)
			printf("Saved %s\n", filename);
	} else printf("No image loaded\n");


}

void crop(struct imagine **img, struct selectie *aux)
{
	if ((*img)->matrice) {
		if (aux->matrice) {
			dealocare(img);
			(*img)->coloane = aux->coloane;
			(*img)->linii = aux->linii;
			alocare(&(*img)->matrice, aux->linii, aux->coloane);
			for (int i = 0; i < aux->linii; i++)
				for (int j = 0; j < aux->coloane; j++)
					(*img)->matrice[i][j] = aux->matrice[i][j];
		}
		printf("Image cropped\n");
	} else printf("No image loaded\n");
}

// void copie_matrice(struct imagine *img, int ***a)
// {
// 	alocare(a, img->linii, img->coloane);
// 	for (int i = 0; i < img->linii; i++)
// 		for (int j = 0; j < img->coloane; j++)
// 			(*a)[i][j] = img->matrice[i][j];
// }

int inmultire(int chenar[3][3], int ker[3][3])
{
	int s = 0;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			s = s + chenar[i][j] * ker[i][j];
		}
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

void edge(struct imagine **img, struct selectie *aux)
{
	if ((strcmp((*img)->tip_imagine, "P3") == 0) || (strcmp((*img)->tip_imagine, "P6") == 0)) {
		int **cop;
		int lin_start, lin_stop, col_start, col_stop;
		int ker[3][3] = { { -1, -1, -1 }, { -1, 8, -1 }, { -1, -1, -1 } };
		int rosu[3][3] = { 0 }, verde[3][3] = { 0 }, albastru[3][3] = { 0 };
		if (aux->matrice) {
			lin_start = aux->linie_start;
			lin_stop = aux->linie_stop;
			col_start = aux->coloana_start;
			col_stop = aux->coloana_stop;
		} else { // pentru toata matricea
			lin_start = 0;
			lin_stop = (*img)->linii;
			col_start = 0;
			col_stop = (*img)->coloane;
		}

		alocare(&cop, lin_stop - lin_start, col_stop - col_start);

		if (lin_start == 0)
			lin_start = 1;
		else if (lin_stop = (*img)->linii)
			lin_stop = lin_stop - 1;
		else if (col_start == 0)
			col_start = 3;
		else if (col_stop == (*img)->coloane);
		col_stop = col_stop - 3;

		for (int i = lin_start; i < lin_stop; i++) {
			for (int j = col_start; j < col_stop; j = j + 3) {
				for (int k = 0; k < 3; k++) { // liniile matricei copie
					for (int l = 0; l < 3; l++) {
						rosu[k][l] = (*img)->matrice[i - 1 + k][j - 3 + l * 3];
						verde[k][l] = (*img)->matrice[i - 1 + k][j - 2 + l * 3];
						albastru[k][l] = (*img)->matrice[i - 1 + k][j - 1 + l * 3];
					}
				}
				cop[i][j] = clamp(inmultire(rosu, ker));
				cop[i][j + 1] = clamp(inmultire(verde, ker));
				cop[i][j + 2] = clamp(inmultire(albastru, ker));
			}
		}
		for (int i = lin_start; i < lin_stop; i++)
			for (int j = col_start; j < col_stop; j++)
				(*img)->matrice[i][j] = cop[i][j];
		dealocare_matrice(cop, lin_start, lin_stop);
		printf("APPLY EDGE done");
	} else printf("Easy, Charlie Chaplin");
}

void sharpen(struct imagine **img, struct selectie *aux)
{
	if ((strcmp((*img)->tip_imagine, "P3") == 0) || (strcmp((*img)->tip_imagine, "P6") == 0)) {
		int **cop;
		int lin_start, lin_stop, col_start, col_stop;
		int ker[3][3] = { { 0, -1, 0}, { -1, 5, -1 }, { 0, -1, 0 } };
		int rosu[3][3] = { 0 }, verde[3][3] = { 0 }, albastru[3][3] = { 0 };
		if (aux->matrice) {
			lin_start = aux->linie_start;
			lin_stop = aux->linie_stop;
			col_start = aux->coloana_start;
			col_stop = aux->coloana_stop;
		} else { // pentru toata matricea
			lin_start = 0;
			lin_stop = (*img)->linii;
			col_start = 0;
			col_stop = (*img)->coloane;
		}

		alocare(&cop, lin_stop - lin_start, col_stop - col_start);

		if (lin_start == 0)
			lin_start = 1;
		else if (lin_stop = (*img)->linii)
			lin_stop = lin_stop - 1;
		else if (col_start == 0)
			col_start = 3;
		else if (col_stop == (*img)->coloane);
		col_stop = col_stop - 3;

		for (int i = lin_start; i < lin_stop; i++) {
			for (int j = col_start; j < col_stop; j = j + 3) {
				for (int k = 0; k < 3; k++) { // liniile matricei copie
					for (int l = 0; l < 3; l++) {
						rosu[k][l] = (*img)->matrice[i - 1 + k][j - 3 + l * 3];
						verde[k][l] = (*img)->matrice[i - 1 + k][j - 2 + l * 3];
						albastru[k][l] = (*img)->matrice[i - 1 + k][j - 1 + l * 3];
					}
				}
				cop[i][j] = inmultire(rosu, ker);
				cop[i][j + 1] = inmultire(verde, ker);
				cop[i][j + 2] = inmultire(albastru, ker);
			}
		}
		for (int i = lin_start; i < lin_stop; i++)
			for (int j = col_start; j < col_stop; j++)
				(*img)->matrice[i][j] = cop[i][j];
		printf("APPLY SHARPEN done\n");
	} else printf("Easy, Charlie Chaplin\n");
}

int main(void)
{
	char sir[100];
	struct imagine *img = NULL;

	img = malloc(sizeof(struct imagine));
	if (img == NULL) {
		printf("Could not alloc the struct");
	}

	struct selectie *mat = NULL;

	mat = malloc(sizeof(struct selectie));
	if (mat == NULL) {
		printf("Could not alloc the struct");
	}

	while (fgets(sir, 100, stdin)) {
		char *token = strtok(sir, " ");

		if (strcmp(token, "LOAD") == 0) {
			token = strtok(NULL, " ");
			token[strlen(token) - 1] = '\0';
			load(&img, token);
			continue;
		}

		if (strcmp(token, "SELECT") == 0) {
			token = strtok(NULL, " ");
			int coordonate[4], i = 0;
			if (strcmp(token, "ALL\n") == 0) {
				coordonate[0] = 0;
				coordonate[1] = 0;
				coordonate[2] = img->coloane;
				coordonate[3] = img->linii;
			} else {
				while (token != NULL) {
					coordonate[i] = atoi(token);
					token = strtok(NULL, " ");
					i++;
				}
			}
			select_poza(img, &mat, coordonate);\
				continue;
		}

		if (strcmp(token, "HISTOGRAM") == 0) {
			int x = atoi(strtok(NULL, " "));
			int y = atoi(strtok(NULL, " "));
			histograma(img, x, y);
			continue;
		}

		if (strcmp(token, "EQUALIZE\n") == 0)
			egalizare(&img);

		if (strcmp(token, "CROP\n") == 0)
			crop(&img, mat);

		if (strcmp(token, "APPLY") == 0) {
			token = strtok(NULL, " ");
			if (strcmp(token, "EDGE\n") == 0) {
				edge(&img, mat);
				continue;
			}
			if (strcmp(token, "SHARPEN\n") == 0) {
				sharpen(&img, mat);
				continue;
			}
			if (strcmp(token, "BLUR\n") == 0) {

				continue;
			}
			if (strcmp(token, "GAUSSIAN_BLUR\n") == 0) {

				continue;
			}
			printf("APPLY parameter invalid");
			continue;
		}

		if (strcmp(token, "SAVE") == 0) {
			token = strtok(NULL, " ");
			token[strlen(token) - 1] = '\0';
			salvare(img, token);
			continue;
		}

		if (strcmp(token, "EXIT\n") == 0) {
			if (img->matrice) {
				dealocare(&img);
			} else printf("No image loaded\n");
			break;
		}

	}

}
// 1 2 3 1 2 3 1 2 3
// 4 5 6 4 5 6 4 5 6
// 7 8 9 7 8 9 7 8 9