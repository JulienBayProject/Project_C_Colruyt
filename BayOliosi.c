//	Bay Julien
//	Oliosi Ludovic
//	2 BI - a2
//	LPP - projet 
//	Liste chain�e
//	20/12/21

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

typedef struct article {
	int num;
	int alimentaire;
	char denomination[40];
	int rExtra;
	int rGrosseQuantite;
	float prix;
	int vidange;
	struct article *suivant;
}article;

typedef struct quantite {
	float quantite;
    struct article art;
    struct quantite *prochain;
} quantite;

typedef struct reductionXtra {
	int num;
	float pReduc;
	int quantiteMin;
	struct reductionXtra *next;
}reductionXtra;

typedef struct reductionGrosQuant {
	int num;
	float prix;
	int quantiteMin;
	struct reductionGrosQuant *suiv;
}reductionGrosQuant;

typedef struct scanning {
	int numArt;
	float val;
	struct scanning *scanApres;
}scanning;

main() {
	
///////// OUVERTURE DES FICHIERS /////////////

	FILE *fart, *fxtra, *fgq, *fcad, *fticket;
	fcad = fopen("BayOliosi_caddy.dat", "a+");
	fticket = fopen("BayOliosi_ticket.res", "w");
	
//////// D�claration des fonctions  ////////	
	// Lecture
	void lectureDesDonneesArticle(article*, article*, int *);
	void lectureDesDonneesExtra(reductionXtra*, reductionXtra*);
	void lectureGrosseQuantite(reductionGrosQuant*, reductionGrosQuant*);
	
	// Utilitaire
	int validiteBon(int, int, int);
	
	// Traitement de donn�es
	void ajoutArticle( int , int *, quantite *, int *, article *, quantite **, quantite *, quantite *, article *, int, float, scanning **);
	void ajoutVidange( float *, float, scanning **);
	void modificationCarte( char, int *, int *, scanning**);
	void recherchePrixDeBase(reductionGrosQuant *, reductionGrosQuant *, quantite  **, float *, float *, float *, float *);
	void calculPrix( quantite * , reductionXtra * , reductionXtra *, float *, int, float*, float*, float *, float*);
	void suppresionScanning( int , quantite **, quantite **, quantite **, scanning ** ,scanning **, scanning **, int *, int *, int *, float *, float *);
	float calculCaution(int);
	void ajoutBon( int , int , int , float , float * , scanning ** , int * );
	
	// Affichage 
	void affichageColruyt(FILE **);
	void afficheFin(FILE **, float, float, float, float, float, float, int, float, float, float);
	
	//Effet
	void my_delay(int);

///////// # ARTICLE ///////////
	// D�claration et initialisation
	int nbArticle = 0;
	article *courant, *debut, *suivant;
	debut = malloc(sizeof(article));
	courant = debut;
	
	// Lecture
	lectureDesDonneesArticle(courant, debut, &nbArticle);
	
///////// # EXTRA ///////////
	// D�claration et initialisation
	reductionXtra *current, *next, *start;
	start = malloc(sizeof(reductionXtra));
	current = start;
	
	// Lecture	
	lectureDesDonneesExtra(current, start);
		
///////// # GROSSE QUANTITE ///////////
	// D�claration et initialisation
	reductionGrosQuant *cour, *suiv, *deb;
	deb = malloc(sizeof(reductionGrosQuant));
	cour = deb;
	
	// Lecture
	lectureGrosseQuantite(cour, deb);

///////// TRAITEMENT ///////////
	// initialisation du ticket
	quantite *mnt, *proch, *prems, *iterator, *precedent; 
	prems = malloc(sizeof(quantite));
	mnt = prems;
	
	// initialisation scanning 	
	scanning *scanDeb, *scanNext, *scanCourant, *scanPrece;
	scanDeb = malloc (sizeof(scanning ));
	scanCourant = scanDeb;
	
	
	// D�claration et initialisation g�n�ral
	float totAlim = 0, totNonAlim =0, tot =0., vid , totVid = 0., prixDeBase, totMarchandise = 0.,valReducGQ, totReducGQ = 0., valReducCar, totReducCar = 0., pourReduc, cash, reducProf = 0.05, avantageProf, caut, poid, valBon, totBon =0.;
	int carteExtra = 0, carteProf = 0, compteur = 0, y = 1, i, a, m, j, rep, num, type, ok =0, nbArt = 1, scanEnCour = 1, choix, rappelAjoutCaisse;
	char code[3], carte, saisieClavier[21];
	
///////// LECTURE DU CADDY ///////////

	printf("Scanning en cours...  \n" );
	// lecture des articles et pr�paration du traitement
	while(scanEnCour == 1) {
		fscanf(fcad,"%3s", &code);
		
		// lecture d'un article
		if(strcmp (code, "art") == 0 ) {
			
			fscanf(fcad,"%4d", &num);
			// si plus grand que 9000 --> cat article au poids, on r�cup�re le poids
			if (num >= 9000 ) {
				fscanf(fcad,"%6f", &poid);
				// si l'article n'a pas de poids, on demande de l'indiqu�, ou si une balance est connect�e, le recup�rer
				if (poid == 0.) {
					// tant qu'on rentre pas un poids r�el combin� avec rewind qui vide le buffer entre deux entr�es
					while ( poid == 0.) {
						printf("Veuillez rentrer le poids de l'article\n");
						scanf("%f", &poid);
						rewind(stdin);
					}
				}
			}
			ajoutArticle( num, &compteur, prems, &nbArt, debut, &mnt, iterator, proch, courant, nbArticle, poid, &scanCourant);
			
		// si carte vidange		
		}else if (strcmp (code, "vid") ==0 ) {
			
			fscanf(fcad,"%6f", &vid);
			ajoutVidange( &totVid, vid, &scanCourant);
			// affichage du ticket vidange scann�
			printf("%3d  -  Vidange                            %6.2f\n", nbArt, vid);
			
		// si carte extra ou professionel	
		}else if (strcmp (code, "car") ==0 ){
			
			fscanf(fcad,"%c", &carte);
			modificationCarte(carte, &carteExtra, &carteProf , &scanCourant);
			// affichage de la carte
			printf( (carte == 'E') ? "%3d  -  Carte Extra\n" : "%3d  -  Carte Professionnel\n", nbArt);
		
		// si on supprime un article scann�	
		}else if (strcmp (code, "sup") == 0 ) {
			fscanf(fcad,"%d", &num);
			
			// on vide l'�cran (console) pour une meilleur lisibilit�
			system("cls");
			suppresionScanning( num, &iterator, &prems, &precedent, &scanNext , &scanDeb, &scanPrece, &carteExtra, &carteProf, &nbArt, &totVid, &totBon);
			
		// si on a termin� le scanning
		}else if (strcmp (code, "non") == 0 ) {
			printf("Cr�ation du ticket\n");
			scanEnCour = 0;
			free(scanCourant);
			break;
		
		//si on scanne un bon 
		}else if (strcmp (code, "bon") == 0 ) {
			fscanf(fcad,"%4d%2d%2d%5f", &a, &m, &j, &valBon);
			ajoutBon( a,  m, j, valBon, &totBon, &scanCourant, &nbArt);
		
		// si lecture fin de fichier caddy on passe en mode rappel
		}else if (strcmp (code, "fin") == 0 ) {
			rappelAjoutCaisse = 1;	
			nbArt--;
			
		// si erreur de lecture sauf pour la fin
		}else {
			printf("Code barre errone, veuillez reiterer\n");
			Beep(3500,500);
			nbArt--;
			
		}
		// une fois que le fichier caddy � �t� lu, on propose de rajouter des articles et/ou
		// on rappel de carte, ticket vidange si aucun n'a �t� scann� pdt la phase pr�c�dente
		if ( rappelAjoutCaisse >= 1 ) {
			printf("Faut il autre chose ?\n");
			
			// si pas de carte et/ou de ticket vidange scann�, un petit rappel au premier passage
			if ( carteExtra == 0 && carteProf == 0 && rappelAjoutCaisse == 1) 
				printf("Avez-vous une carte Extra ou Professionnel ?\n");
			if ( totVid == 0. && rappelAjoutCaisse == 1 ) 
				printf("Avez-vous un ticket vidange ?\n");
			rappelAjoutCaisse++;
			
			// on vide le tampon de lecture pour �viter que gets r�cup�re des �l�ments restant dans celui-ci
			rewind(stdin);
			// on �crit directement l'ajout � la fin du fichier et ainsi continu� le traitement pour cela
			// on repositionne le curseur pour l'�criture � la fin de fichier
			fseek(fcad, 0 , SEEK_END);
			//  on r�cup�re la saisie clavier
			gets(saisieClavier);
			// on copie dans le fichier
			fprintf(fcad,"\n%s", saisieClavier);
			// on replace le curseur avant la saisie clavier enregistr�e dans le fichier
			fseek(fcad, -strlen(saisieClavier) , SEEK_END);
		}
		
		// si l'adresse n'a pas encore �t� utilis� (erreur de lecture,...) on n'en cr�e pas une autre
		if ( scanCourant->numArt != NULL) {
			// ajout de l'adresse du scanning suivant
			scanNext=malloc(sizeof(scanning));
			scanCourant->scanApres = scanNext;
			scanCourant = scanNext;
			
			// confirmation audio du scanning, si on cr�e une nouvelle ad, s'est qu'il n'ya pas eu d'erreur
			Beep(1500,500);
		}
		
		// on compte le nombre d'article scann�
		nbArt++;
		
		// effet visuel du scanning
		if (rappelAjoutCaisse == 0)
			my_delay(1);
	}
	fclose(fcad);
	
	// Affichage du d�but du ticket
	affichageColruyt(&fticket);
	
///////// CALCUL + CREATION DU TICKET ///////////
	// pour chaque article scann� pr�sent dans la struct quantite, on recherche le prix de base en tenant compte des r�ductions + cr�ation visuel du ticket
	iterator = prems;
	while (iterator->prochain != NULL ) {
		// remise � 0 des valeurs pour les calculs d'un art
		valReducCar = 0.;
		valReducGQ = 0.;
		prixDeBase = 0.;
		caut = 0.;
		
		// on recherche le prix de base (si prix grosse quantit� ou normal)
		recherchePrixDeBase( cour, deb, &iterator,  &prixDeBase, &valReducGQ, &totReducGQ, &totMarchandise);
		
		// on r�cup�re la caution si l'article en dispose d'une
		caut = calculCaution(iterator->art.vidange) * (float)iterator->quantite;
		
		// affichage ticket avec adaptation si l'art � une caution ou si s'est un article au poid
		fprintf(fticket,"|  %4d | %-40s    |",iterator->art.num, iterator->art.denomination);
		fprintf(fticket,(caut != 0.) ? " %4.2f |" : "      |", caut);
		fprintf(fticket,(iterator->art.num >= 9000) ? "%5.3fkg| %6.2f  |" :"   %2.0f  | %6.2f  |",iterator->quantite, prixDeBase);
		
		// on effectue le calcul total d'un artcicle  (si ya carte extra ou non et de la quantit�) + calcul de r�duction		
		calculPrix( iterator, current, start, &prixDeBase, carteExtra, &pourReduc, &valReducCar, &totReducCar, &totAlim);        
		fprintf(fticket,"%6.2f |\n", prixDeBase);
		
		// on ajoute les r�ductions apr�s l'affichage
		prixDeBase += valReducCar;
		
		// calcul total avec r�duc et hors vidange
		tot += prixDeBase ;
		
		// affichage si r�duction grosse quantite octroy�e
		if ( valReducGQ != 0. )  
			fprintf(fticket,"|       |  Reduction sur vos achats en grande quantite| %6.2f (deja deduit)   |       |\n", valReducGQ);	
		
		// affichage si reduction carte extra octroy�e
		if ( valReducCar != 0.) 
			fprintf(fticket,"|       |  Reduction                                  |      |   %2.0f%% |         |%6.2f |\n", pourReduc , valReducCar);	
		
		// on ajoute les cautions si l'article en a
		totVid += caut;

		iterator = iterator->prochain;
	}
		
	// si carte profesionnel on soustrait l'avantage qu'ils ont droit
	if ( carteProf == 1 ) {
		avantageProf = tot * reducProf;
		tot *= ( 1. - reducProf );
	}
	
	// on retire les bons fournisseur
	tot -= totBon;

	// on retire les vidanges du montant total
	tot += totVid;
	
	// affichage de fin de ticket
	afficheFin(&fticket, tot, totAlim, totVid, totMarchandise, totReducCar, totReducGQ, carteProf, avantageProf, totVid, totBon);

		
///////// PAIEMENT ///////////
	// on vide la console pour la lisibilit�
	system("cls");

	// tant que le montant � payer n'est pas �gal � z�ro
	while (tot != 0.){
	printf("\n\nMontant : %6.2f �\nQuels moyen de paiement choissisez vous ? \n 1 - Ticket restaurant \n 2 - Bancontact \n 3 - Cash\n", tot);
	scanf("%d", &choix);
		switch (choix) {
			// opt ticket restaurant : on part du principe que la carte ticket resto du client � suffisament de fond
			case 1 :
				printf("Montant alimentaire : %6.2f\n", totAlim);
				tot -= totAlim;				
				break;
			// opt bancontact : on part du principe que la compte du client � suffisamennt de fond
			case 2 :
				printf("%6.2f ont ete deduit de votre compte\n", tot);
				tot = 0.;
				break;
			// opt cash 
			case 3 :
				printf("Entrer le montant du cash  \n");
				scanf("%f", &cash);
					printf("Montant a payer : %6.2f\nMontant recu : %6.2f\n", tot, cash);
				// si on re�oit plus que le total, on rend la monnaie	
				if ( cash >= tot) {
					cash -= tot;
					tot = 0.;
					printf( (cash > 0 ) ? "%6.2f de retour\n" : "", cash);
				// sinon on soustrait
				} else {
					tot -= cash;
				}
				// on tronque d�s que le montant restant est inf�rieux � 0.01
				if ( tot < 0.01)
					tot = 0.;
				break;

			default :
				printf("Veuillez selectionner une operation proposee\n");
				break;
		}
	}
	
///////// FIN DU TICKET ///////////
	printf("Merci et a bientot !!");
	// fermeture
	fclose(fcad);
	fclose(fticket);
}

void lectureDesDonneesArticle(article *courant, article *suivant, int * nbArt){
	// ouverture fichier contenant les articles disponnible � la vente
	FILE *fart;
	fart = fopen("BayOliosi_article.dat", "r");
	
	// lecture des donn�es
	fscanf(fart,"%4d", &courant->num);
	while(!feof(fart)) {
		fscanf(fart," %d ",&courant->alimentaire);
		fgets(courant->denomination, 40, fart);
		fscanf(fart," %d %d %5f %d", &courant->rExtra, &courant->rGrosseQuantite, &courant->prix, &courant->vidange);
		
		//cr�ation l'ad suivante de la structure	
		suivant=malloc(sizeof(article));
		courant->suivant = suivant;
		courant = suivant;
		courant->suivant = NULL;
		
		// r�cup�ration du nbre d'article
		(*nbArt)++;
		fscanf(fart,"%4d", &courant->num);
	}	
	//fermeture du fichier de lecture
	fclose(fart);
}

void lectureDesDonneesExtra(reductionXtra *courant, reductionXtra *suivant){
	// ouverture fichier contenant les informations des art qui ont une r�duction extra
	FILE *fxtra;
	fxtra = fopen("BayOliosi_extra.dat", "r");
	
	// lecture des donn�es
	fscanf(fxtra,"%4d", &courant->num);
	while(!feof(fxtra)){
		fscanf(fxtra," %4f %2d", &courant->pReduc, &courant->quantiteMin);
		
		//cr�ation l'ad suivante de la structure	
		suivant = malloc(sizeof(reductionXtra));
		courant->next = suivant;
		courant = suivant;
		courant->next = NULL;
		fscanf(fxtra,"%4d", &courant->num);		
	}
	//fermeture
	fclose(fxtra);
}

void lectureGrosseQuantite(reductionGrosQuant *courant, reductionGrosQuant *suivant){
	// ouverture fichier contenant les informations des art qui ont une r�duction grosse quantite
	FILE *fgq;
	fgq = fopen("BayOliosi_quantite.dat", "r");
	
	// lecture
	fscanf(fgq,"%4d", &courant->num);
	while(!feof(fgq)) {
		fscanf(fgq," %5f %2d", &courant->prix, &courant->quantiteMin);
		
		//cr�ation l'adresse suivante de la structure
		suivant = malloc(sizeof(reductionGrosQuant));
		courant->suiv = suivant;
		courant = suivant; 
		courant->suiv = NULL;
		fscanf(fgq,"%4d", &courant->num);
	} 
	// fermeture du fichier
	fclose(fgq);
}

void affichageColruyt(FILE **fticket){
	
	fprintf(*fticket,"*--------------------------------------------------------------------------------------*\n");
	fprintf(*fticket,"|                                                                       ____           |\n");
	fprintf(*fticket,"|                                                                      /   / meilleurs |\n");
	fprintf(*fticket,"|         ������  ������  ��      ������  ��    �� ��    �� ��������  /___/  prix      | \n");
	fprintf(*fticket,"|        ��      ��    �� ��      ��   �� ��    ��  ��  ��     ��                      |\n");
	fprintf(*fticket,"|        ��      ��    �� ��      ������  ��    ��   ����      ��                      |\n");
	fprintf(*fticket,"|        ��      ��    �� ��      ��   �� ��    ��    ��       ��                      |\n");
	fprintf(*fticket,"|         ������  ������  ������� ��   ��  ������     ��       ��                      |\n");
	fprintf(*fticket,"|                                                                                      |\n");
	fprintf(*fticket,"|       %s   %s                    Vos caissiers :                      |\n", __DATE__, __TIME__);
	fprintf(*fticket,"|                                                 Bay Julien, Oliosi Ludovic           |\n");
	fprintf(*fticket,"*--------------------------------------------------------------------------------------*\n");
	fprintf(*fticket,"| N art | Denomination                                | Vid  | Quant | Prix U. | Total |\n");
	fprintf(*fticket,"|-------|---------------------------------------------|------|-------|---------|-------|\n");
	
}

void afficheFin(FILE** fticket, float tot, float alim, float vid, float totMarch, float totReducCar, float totReducGQ, int carteProf, float avantageP, float totVid, float totBon) {
	// valeur absolue du total r�duction carte pour l'affichage
	totReducCar = fabs(totReducCar); // fabs -> donne la valeur absolue pour double ou float
	fprintf(*fticket,"|       |                                                                      |       |\n");
	if ( totVid != 0.) {
		fprintf(*fticket,"|       |                                          Total vidange               |%6.2f |\n", totVid);
	}
	if ( totReducGQ != 0.) {
		fprintf(*fticket,"|       |Votre reduction totale sur vos achats en grande quantite : %6.2f     |       |\n", totReducGQ);
		fprintf(*fticket,"|       | (deja calculee dans le montant total)                                |       |\n");
	}
	fprintf(*fticket,"|       |                                                                      |       |\n");
	if ( totReducCar != 0.)
		fprintf(*fticket,"|       |Reduction totale avec Extra : %6.2f                                  |       |\n", totReducCar);
		
	if ( totBon != 0.)
		fprintf(*fticket,"|       |Reduction totale avec bon   : %6.2f                                  |       |\n", totBon);
		
	if ( carteProf == 1)
		fprintf(*fticket,"|       |Avantage Professionel       : %6.2f                                  |       |\n", avantageP);
	fprintf(*fticket,"|       |                                                                      |       |\n");
	fprintf(*fticket,"|--------------------------------------------------------------------------------------|\n");
	fprintf(*fticket,"| FOOD %8.2f | TOTAL MARCHANDISE %8.2f | TOTAL VIDANGE %6.2f| A PAYER  %8.2f |\n", alim, totMarch, vid, tot);
	fprintf(*fticket,"*--------------------------------------------------------------------------------------*\n");

}

void ajoutArticle( int num, int *compteur, quantite *prems, int *nbArt, article *debut, quantite **mnt, quantite *iterator, quantite *proch, article *courant, int nbArticle, float poid, scanning ** scanCourant) {
	
	int y,i, ok = 0;
	
	// on parcourt les articles pr�sent dans la liste ticket (sauf si s'est le 1er art scann�)
	if ( compteur != 0) {
		iterator = prems;
		for ( y = 0 ; y < (*compteur) ; y++ ) {
			if ( iterator->art.num == num) {
				// si l'article est d�j� pr�sent dans la liste ticket, on incr�mente la quantit� ou on rajoute le poid si art au poid
				if ( num >= 9000){
					iterator->quantite += poid;
				}
				else
					iterator->quantite++;
				// confirmation de passage dans cette condition 
				ok = 1;
				(*compteur)++;
				// affichage de l'article scann�
				printf("%3d  -  %-40s  %6.2f\n", *nbArt, iterator->art.denomination, iterator->art.prix);
				break;
			}
			if ( iterator->prochain != NULL)
				iterator = iterator->prochain;
			else 
				break;
		}
	} 
	// si pas de confirmation de passage ci dessus, il faut cr�er une nouvelle struct quantit�
	if ( ok == 0 ) {
		// on parcourt la liste d'article
		courant = debut;	
		for ( i = 1 ; i <= nbArticle ; i++ ) { 							
			
			// si le n� art == � l'article
			if ( num == courant->num) {
				// cr�ation de la struct
				(*mnt)->art = *courant;	
				// si l'article est au poids, on lui donne son poid sinon la quantite est a 1
				if ( num >= 9000){
					(*mnt)->quantite = poid;
					(*scanCourant)->val = poid;
				}
				else
					(*mnt)->quantite = 1.;
				
				proch = malloc(sizeof(quantite));
				(*mnt)->prochain = proch;
				
 				// affichage de l'article scann�
				printf("%3d  -  %-40s  %6.2f\n", *nbArt, (*mnt)->art.denomination, (*mnt)->art.prix);
				
				(*mnt) = proch;
				(*mnt)->prochain = NULL;
				(*compteur)++;
				break;
			}
			courant = courant->suivant;
		}
	}
	// ajout du scanning
	(*scanCourant)->numArt = num;
}

void ajoutVidange( float * totVid,  float vid, scanning ** scanCourant) {
	// ajout des vidanges
	*totVid -= vid;
	
	// ajout du scanning
	(*scanCourant)->numArt = -1;
	(*scanCourant)->val = vid;
}

void modificationCarte(char carte, int * carteExtra, int * carteProf, scanning** scanCourant) {
	
	// si carte extra, on passe � true que la variable correspondante sinon s'est une carte prof donc toutes les r�ductions
	if ( carte == 'E') {
		*carteExtra = 1;
	}else {
		*carteProf = 1;	
		*carteExtra = 1;
	} 
	// ajout du scanning	
	(*scanCourant)->numArt = -2;
}

void recherchePrixDeBase(reductionGrosQuant * cour, reductionGrosQuant * deb, quantite ** iterator, float * prixDeBase, float * valReducGQ, float * totValReducGQ , float * totMarchandise) {
	// si l'art � un prix en grosse quantite
	if ( (*iterator)->art.rGrosseQuantite == 1 ) {
		// recherche du prix grosse quantite
		cour = deb;
		while ( cour->suiv != NULL ) {
			if ( cour->num == (*iterator)->art.num) {
				// si la quantite est suffisante pour l'acc�s � la r�duction
				if( (*iterator)->quantite >= cour->quantiteMin ) 
					// prix de grosse quantit�
					*prixDeBase = cour->prix;
				// sinon prix normal	
				else
					*prixDeBase = (*iterator)->art.prix;
				break;	
			}
			cour = cour->suiv;
		}
	// art sans grosse reduc -> prix normal
	} else
		*prixDeBase = (*iterator)->art.prix;
	
	//r�cup�ration r�duction pour affichage
	*valReducGQ = ((*iterator)->art.prix - *prixDeBase) * (*iterator)->quantite;
	*totValReducGQ += *valReducGQ;
	
	//calcul prix hors reduc pour l'affichage
	*totMarchandise += ((*iterator)->art.prix * (*iterator)->quantite);
}

void calculPrix(quantite * iterator, reductionXtra * current, reductionXtra *start, float * prixDeBase, int carteExtra, float * pourRed, float *valReducE, float * totValReducE, float * totAlim){
	
	// calcul normal 	
	*prixDeBase *= iterator->quantite ;	
	// si l'article courant dispose actuellement d'une reduc extra et si la carte a �t� scann�e
	if ( iterator->art.rExtra == 1 && carteExtra == 1 ) {
		// on recherche alors la r�duction dans la liste des R extra
		current = start;
		// on cherche la r�duction
		while ( current->next != NULL )  {
			if ( current->num == iterator->art.num) {
				// si la quantite est suffisante pour l'acc�s � la r�duction
				if( iterator->quantite >= current->quantiteMin ) {
					*valReducE = (*prixDeBase *  (1 - current->pReduc)) - (*prixDeBase ) ;
					*totValReducE += *valReducE;
					*pourRed = 100*current->pReduc;
					break;
				}
			}
			current = current->next;
		}
	} 
	
	// si il est alimentaire 
	if ( iterator->art.alimentaire == 1 )
			*totAlim += *prixDeBase ;  
}

void suppresionScanning( int num, quantite **iterator, quantite **prems, quantite **precedent, scanning **scanNext ,scanning **scanDeb, scanning **scanPrece, int *carteExtra, int *carteProf, int *nbArt, float * totVid, float * totBon ) {
	int i, y;
	// condition pour eviter une erreur si on rentre un indice n�gatif ou sup�rieur au nombre d'article scann�
	if ( num >= 1 && num <= *nbArt-1 ){
		(*iterator) = *prems;
		(*scanNext) = *scanDeb;
		// on se deplace jusqu'a l'art � supprimer
		for ( i = 1 ; i< num ; i++) {
			*scanPrece = *scanNext;
			(*scanNext) = (*scanNext)->scanApres;
		}
		// si vidange on d�duit sa valeur
		if ( (*scanNext)->numArt == -1 ) {
			*totVid -= (*scanNext)->val;
			
		// si carte ( remise � false des deux carte vu que la carte P octrois les Reduc E aussi 
		}else if ( (*scanNext)->numArt == -2) {
			*carteExtra = 0;
			*carteProf = 0;
			
		// si c'est un bon on deduit �a valeur
		}else if ( (*scanNext)->numArt == -3){
			*totBon -= (*scanNext)->val;
		
		// sinon c'est un article
		} else {
			// on recherche l'adresse de la struct quantite correspondante � l'art scann� � supprimer
			while( (*scanNext)->numArt != (*iterator)->art.num ) {
				*precedent = *iterator;
				(*iterator) = (*iterator)->prochain;
			}
			// si la quantit� = 1 on supprime l'adresse 
			if ( (*iterator)->quantite == 1.  || (*iterator)->quantite - (*scanNext)->val == 0.) {
				
				//  si c'est le 1er de la liste
				if ( (*iterator) == *prems) {
					*prems = (*prems)->prochain;
					
				// si s'est le dernier
				} else if ( (*iterator)->prochain == NULL ) {
					(*precedent)->prochain = NULL;
				
				} else {
					(*precedent)->prochain = (*iterator)->prochain;
				}
				
			// si la quantite/poids > de 1 on diminue la quantite / on soustrait le poids	
			} else 	{
				if ( (*iterator)->art.num >= 9000 )
					(*iterator)->quantite -= (*scanNext)->val ;
				else 
					(*iterator)->quantite--; 
			}
			--*nbArt;
		}
		// on efface l'adresse du scan en fonction de sa position 
		if ( (*scanNext) == *scanDeb)
			*scanDeb = (*scanDeb)->scanApres;
		else if ( (*scanNext)->scanApres == NULL ) 
			(*scanPrece)->scanApres = NULL;
		else 
			(*scanPrece)->scanApres = (*scanNext)->scanApres;
		
		// actualisation de l'affichage
		printf("\nrecap apr�s suppression\n");
		(*scanNext) = *scanDeb;
		y = 1;
		// pour chaque scan, on affiche les informations correspondante en fonction de c'est une carte, bon, vidange, article
		while ((*scanNext)->scanApres != NULL ) {
			if ( (*scanNext)->numArt == -1 ) {
				printf("%3d  -  Vidange                            %6.2f\n", y, (*scanNext)->val);
			
			} else if ((*scanNext)->numArt == -2){
				if ( *carteProf == 1 ){
					printf( "%3d  -  Carte Professionnel\n", y);
				} else if ( *carteExtra == 1) {
					printf( "%3d  -  Carte Extra\n", y);
				} else {
					y--;
				}
			} else if ((*scanNext)->numArt == -3){
				printf("%3d  -  Bon accepte       %5.2f", y ,  (*scanNext)->val);
			}else {
				(*iterator) = *prems;
				// on recherche l'�l�ment correspondant au scanning
				while( (*scanNext)->numArt != (*iterator)->art.num ) {
					(*iterator) = (*iterator)->prochain;
				}
				// affichage 
				printf("%3d  -  %-40s  %6.2f\n", y,  (*iterator)->art.denomination, (*iterator)->art.prix);
			}
			y++;
			(*scanNext) = (*scanNext)->scanApres;
		}
	} else {
		printf("        suppresion impossible\n");
		*nbArt--;
	}
}

float calculCaution(int type) {
	// quantit� fixe, les bi�res se vendant par pack
	float caution[] = { 0. , 4.50 , 0.30 , 0.60 }; 
	return caution[type];
}

void my_delay(int i){    
    clock_t start,end;
    start=clock();
    while(((end=clock())-start)<=i*CLOCKS_PER_SEC);
}

int validiteBon(int j, int m, int a) {
	int calculJour, calculBon, valide;
	
	// on recup�re l'heure du system via la lib time.h
	time_t dateDuJour = time( NULL );
	// on transfert les valeurs dans la struct tm pour la date du jour
	struct tm *dateJour = localtime(&dateDuJour);
	// on ajoute au mois et � l'ann�e pour tomb� juste -->La majorit� des syst�mes utilisent le 01/01/1970 � 00:00:00 UTC comme r�f�rence.
	dateJour->tm_mon += 1;
	dateJour->tm_year += 1900;

	// on calcule le total de l'ann�e, mois, jour   
	calculJour = dateJour->tm_mday + dateJour->tm_mon + dateJour->tm_year;
	calculBon = j + m + a;
	
	// si le bon � un montant = ou sup�rieur on l'acception, sinon non
	if ( calculJour <= calculBon )
		valide = 1;
    else 
    	valide = 0;
    return valide;
}

void ajoutBon( int a, int m, int j, float valBon, float * totBon, scanning ** scanCourant, int * nbArt) {
	int rep;
	// on test la validit� du bon 
	rep = validiteBon(j,m, a);
	// si il est bon on le conserve
	if ( rep == 1 ) {
		printf("%3d  -  Bon accepte                         %5.2f\n",*nbArt, valBon);
		(*scanCourant)->numArt = -3;
		(*scanCourant)->val = valBon;
		*totBon += valBon;
	// sinon on l'ignore + alerte
	} else {
		printf("bon refuse\n");
		(*nbArt)--;
			Beep(3500,500);
	}
}


