#include "Garage.h"

Garage  Garage::instance ;
Voiture Garage::projetEnCours;
Employe * Garage::Connected = NULL ; 

Garage::Garage(){

// on avait ajooute lors de la conception de l'etape 11 
//ajouteEmploye("Gavage","Cedric","ADMIN","Administratif");
// tout est deja cree on modifiera les valeurs max de size max ici au besoin 
	// imaginons si on doit gerer 11 options 

}

 void Garage::ajouteModele(const Modele & m){
 	modeles.insere(m);
 }
 void Garage::afficheModelesDisponibles() const{
 	//modeles.Affiche();
 	
 	for (int i=0;i<modeles.size();i++)
 	{
 		modeles[i].Affiche();
 	}
 }
 Modele Garage::getModele(int indice){
 	return modeles[indice];
 }


 void Garage::ajouteOption(const Option & o){
 	options.insere(o);
 }
 void Garage::afficheOptionsDisponibles() const{
 	//options.Affiche();
 	
 	for (int i=0;i<modeles.size();i++)
 	{
 		options[i].Affiche();
 	}
 }
 Option Garage::getOption(int indice){
 	return options[indice];
 }


 void Garage::ajouteClient(string nom,string prenom,string gsm)
 {
    int tmp =0; 
    for ( int i =0 ; i<clients.size();i++)
    {
        cout <<"tmp : "<< tmp << endl;
        if (clients[i].getNumero()==tmp+1)
        {
            tmp++;
            i=-1; // permet de repasser dans la boucle car le vec est pas trie , ca aurait ete plus simple si il l'etait
                //-1 car on va directeemtn la reincrementer de 1 en fin de boucle 
        }
    }
    cout <<"nbclient : "<< Intervenant::nombreClient << endl;
    if (tmp!=Intervenant::nombreClient-1)
    { 
        Client c(nom,prenom,tmp+1,gsm);
        clients.insere(c);
    }
    else 
    {
       Client c(nom,prenom,Intervenant::nombreClient,gsm);
       clients.insere(c);
       Intervenant::nombreClient++;
    }

 }
 void Garage::afficheClients() const{
 	clients.Affiche();
 }
 int Garage::supprimeClientParIndice(int ind){
    Iterateur<Contrat> it(Garage::getInstance().getVecteurContrat());

     for(it.reset(); !it.end(); it++)
    { 
      Contrat Cont = (Contrat)it;
      if (Cont.getAcheteur() == clients[ind].getNumero())
      {
        return 0 ;// pas ok  car possede un contrat a son nom

      }
    }
    Intervenant::nombreClient--;
    clients.retire(ind);
    return 1; 
 }
 int Garage::supprimeClientParNumero(int num)
 {
 	 Iterateur<Contrat> it(Garage::getInstance().getVecteurContrat());
    for (int i =0 ; i<clients.size() ;i++)
    {
        if (clients[i].getNumero()==num)
        {
            for(it.reset(); !it.end(); it++)
            { 
              Contrat Cont = (Contrat)it;
              if (Cont.getAcheteur() == clients[i].getNumero())
              {
                return 0 ;// pas ok  car possede un contrat a son nom

              }
            }
            Intervenant::nombreClient--;
            clients.retire(i);
            return 1;     
        }
    }
    return 0;
 }

 void Garage::ajouteEmploye(string nom,string prenom,string login,string fonction)
 {
    int tmp =0; 
    for ( int i =0 ; i<employes.size();i++)
    {
        if (employes[i].getNumero()==tmp+1)
        {
            tmp++;
            i=-1; // permet de repasser dans la boucle car le vec est pas trie , ca aurait ete plus simple si il l'etait
                //-1 car on va directeemtn la reincrementer de 1 en fin de boucle 
        }
    }

    if (tmp!=Intervenant::nombreEmploye-1)
    { 
        Employe e (nom ,prenom,tmp+1,login,fonction);
        employes.insere(e);
    }
    else 
    {
       Employe e (nom ,prenom,Intervenant::nombreEmploye,login,fonction); 
       employes.insere(e);
       Intervenant::nombreEmploye++;
    }


 }
 void Garage::afficheEmployes() const{
 	employes.Affiche();
 }
 int Garage::supprimeEmployeParIndice(int ind)
 {
     Iterateur<Contrat> it(Garage::getInstance().getVecteurContrat());

     for(it.reset(); !it.end(); it++)
    { 
      Contrat Cont = (Contrat)it;
      if (Cont.getVendeur() == employes[ind].getNumero())
      {
        return 0 ;// pas ok  car possede un contrat a son nom

      }
    }
    Intervenant::nombreEmploye--;
 	employes.retire(ind);
    return 1; 
}
int Garage::supprimeEmployeParNumero(int num)
{
     Iterateur<Contrat> it(Garage::getInstance().getVecteurContrat());
 	for (int i =0 ; i<employes.size() ;i++)
 	{
 		if (employes[i].getNumero()==num)
 		{
         	for(it.reset(); !it.end(); it++)
            { 
              Contrat Cont = (Contrat)it;
              if (Cont.getVendeur() == employes[i].getNumero())
              {
                return 0 ;// pas ok  car possede un contrat a son nom

              }
            }
            Intervenant::nombreEmploye--;
            employes.retire(i);
            return 1;     
 		}
 	}
    return 0;
 }

 // fonctions rajoute du au singleton 

 Garage& Garage::getInstance()
 {
    
    return instance ;
 }

 Voiture& Garage::getProjetEnCours()
 {

    return Garage::projetEnCours ;
 }
 void Garage::resetProjetEnCours()
 {
    Voiture V ;
    Garage::projetEnCours= V ;
 }


 int Garage::importeModeles(string nomFichier)
{   
     Modele M;

    char buffer[250];
    char * pchar;
  
    string NomTampon,ImgTampon;
    int puissanceTampon,moteurTampon;
    float prixTampon;
    

    ifstream fichier(nomFichier, ios::in);
    fichier.getline(buffer, 250);

    
    int i = 0;
    while(fichier.getline(buffer, 250))
    {   
      //ds l'ordre des lignes du fichiers

        pchar = strtok(buffer, ";,:");
        NomTampon = pchar;
        M.setNom(NomTampon);

        pchar = strtok(NULL, ";,:");
        puissanceTampon = atoi(pchar);
        M.setPuissance(puissanceTampon);

        pchar = strtok(NULL, ";,:");
        moteurTampon = atoi(pchar);
        M.setMoteur(moteurTampon);

        pchar = strtok(NULL, ";,:");
        ImgTampon = pchar;
        M.setImage(ImgTampon);

        pchar = strtok(NULL, ";,:");
        prixTampon = atof(pchar); // ascii to float 
        M.setPrixDeBase(prixTampon);

        Garage::getInstance().ajouteModele(M);

        i++;

    }

    return i;


}

int Garage::importeOptions(string nomFichier)
{
   Option Opt;

    char buffer[250];
    char * pchar;


    ifstream fichier(nomFichier, ios::in);
    fichier.getline(buffer, 250);//on le fait une fois , permet de passer l'entete 

    string codeTampon,intituleTampon;
    float prixTampon;

    

    int i = 0;
    while(fichier.getline(buffer, 250)) // prend la ligne numero 2 du coup 
    {
        pchar = strtok(buffer, ";,:"); // on precise une fois qu'on fais sur le buffer , pusi ca se fait automatiquemen qaund on precise NULL 
        codeTampon = pchar;
        Opt.setCode(codeTampon);

        pchar = strtok(NULL, ";,:");
        intituleTampon = pchar;
        Opt.setIntitule(intituleTampon);

        pchar = strtok(NULL, ";,:");
        prixTampon = atof(pchar);
        Opt.setPrix(prixTampon);

        Garage::getInstance().ajouteOption(Opt);

        i++;
    
    }

    return i;

}

Vecteur<Employe>& Garage::getVecteurEmploye()
{   
    return employes;
}

Vecteur<Client>& Garage::getVecteurClient() 
{
    return clients;
}






void Garage::Save(ofstream &fichier)const
{

    // on ns dmd d'enregistrer les numintervenant+ les employrs+les clients 
    int tampon;

    fichier.write((char*)&Intervenant::nombreClient,sizeof(int));
    fichier.write((char*)&Intervenant::nombreEmploye,sizeof(int));
    fichier.write((char*)&Contrat::NombreContrat,sizeof(int));

    tampon = employes.size();


    fichier.write((char*)&tampon, sizeof(int));

    for(int i = 0; i < tampon; i++)
    {
        employes[i].Save(fichier);
    }

    tampon = clients.size();

    fichier.write((char*)&tampon, sizeof(int));

    for(int i = 0; i < tampon; i++)
    {
        clients[i].Save(fichier);
    }

    tampon = contrats.size();

    fichier.write((char*)&tampon, sizeof(int));

    for(int i = 0; i < tampon; i++)
    {
        contrats[i].Save(fichier);
    }

    fichier.close();



}


int Garage::Load(ifstream &fichier)
{

    int taille, IntTampon= 0, nbadmin=0 ;
    string tampon;   

    fichier.read((char*)&Intervenant::nombreClient, sizeof(int));
    fichier.read((char*)&Intervenant::nombreEmploye, sizeof(int));
    fichier.read((char*)&Contrat::NombreContrat, sizeof(int));

    fichier.read((char*)&IntTampon, sizeof(float));
    Employe Emp;
    for(int i = 0; i <IntTampon; i++)
    {
        Emp.Load(fichier);

        employes.insere(Emp);
        if (Emp.getFonction()=="Administratif") nbadmin++ ; 
    }

    fichier.read((char*)&IntTampon, sizeof(float));
    Client Cli;
    for(int i = 0; i < IntTampon; i++)
    {
        Cli.Load(fichier);

        clients.insere(Cli);
    }

    fichier.read((char*)&IntTampon, sizeof(float));
    Contrat cont;
    for(int i = 0; i < IntTampon; i++)
    {
        cont.Load(fichier);

        contrats.insere(cont);
    }

    fichier.close();


    return nbadmin ;




}

void Garage::parDefaut()
{
    
    Employe Emp("admin", "admin", Intervenant::nombreEmploye, "admin", "Administratif");

    Emp.setMotDePasse("admin123");

    employes.insere(Emp);

    Intervenant::nombreEmploye++ ;

  
}


void Garage::ajouteContrat(int numemp,int numcli,string Nompro)
{
    Contrat Cont;
    int tmp =0; 
    for ( int i =0 ; i<contrats.size();i++)
    {
        if (contrats[i].getNumero()==tmp+1)
        {
            tmp++;
            i=-1; // permet de repasser dans la boucle car le vec est pas trie , ca aurait ete plus simple si il l'etait
                //-1 car on va directeemtn la reincrementer de 1 en fin de boucle 
        }
    }

    if (tmp!=Contrat::NombreContrat-1)
    { 
        Contrat Cont(tmp+1, numemp, numcli, Nompro);
        contrats.insere(Cont);
    }
    else 
    {
       Contrat Cont(Contrat::NombreContrat, numemp, numcli, Nompro); 
       contrats.insere(Cont);
       Contrat::NombreContrat++;
    }

    

    
}


void Garage::supprimeContratParIndice(int ind)
{
     Contrat::NombreContrat--;
    contrats.retire(ind);
}

void Garage::supprimeContratParNumero(int num)
{
    for (int i =0 ; i<contrats.size() ;i++)
    {
        if (contrats[i].getNumero()==num)
        {
            Contrat::NombreContrat--;
            contrats.retire(i);       
        }
    }
}


Vecteur<Contrat>& Garage::getVecteurContrat()
{
    return contrats;
}
