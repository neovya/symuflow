#pragma once
#ifndef giratoireH
#define giratoireH

#include "BriqueDeConnexion.h"

class TuyauMicro;
class Reseau;
class VoieMicro;
struct GrpPtsConflitTraversee;

// Structure permettant d'associer un couple de voies origine destination
// � un groupe de travers�es correspondant
struct GroupeTraverseeEntreeGir {
    VoieMicro * pVoieEntree;
    VoieMicro * pVoieInterne;
    GrpPtsConflitTraversee * pGrpTra;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

// Structure permettant d'associer un couple voie interne - troncon de sortie
// � un groupe de travers�es correspondant
struct GroupeTraverseeSortieGir {
    TuyauMicro*  pTuyauSortie;
    VoieMicro*   pVoieInterne;
    GrpPtsConflitTraversee * pGrpTra;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

// Structure de stockage des coefficients d'insertion d�finis manuellement
struct CoeffsInsertion {

    Tuyau * pTuyauEntree;
    Tuyau * pTuyauSortie;
    int nVoieEntree;
    std::vector<double> coefficients;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

// Structure de stockage du tirage d'un choix de voie d'insertion dans le giratoire
struct MouvementsInsertion {
    Tuyau * pTuyauSortie;
    Voie *  pVoieEntree;
    std::map<int, boost::shared_ptr<MouvementsSortie> > mvtsInsertion;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

class Giratoire : public BriqueDeConnexion
{
public:
    // Constructeurs, destructeurs et assimil�s
    Giratoire() ; // Constructeur par d�faut    
    Giratoire(char *strID, double dbVitMax, std::string strRevetement,  double dbTagreg, double dbGamma, double dbMu, int nVoie, double dbLargeurVoie, char cType, int nm, double dbBeta, double dbBetaInt, double dbBetaOut, bool    bTraversees, Reseau *pReseau);
    ~Giratoire(); // Destructeur

private:

    // Variables caract�ristiques du giratoire
    char        m_strNom[128];                  // Identifiant

    Reseau*     m_pReseau;

    std::string m_strRevetement;               // Rev�tement de l'anneau

    double      m_dbTagreg;                     // P�riode d'agr�gation
    double      m_dbGamma;
    double      m_dbMu;

    double      m_dbLargeurVoie;                // Largeur de la couronne
    int         m_nVoie;                        // Nombre de voie de l'anneau
    char        m_cType;                        // Type (urbain, p�ri-urbain, urbain-dense)
    
    double      m_dbBeta;                       // Probabilit� pour un v�hicule voulant s'ins�rer de d�tecter un v�hicule sortant du giratoire
    double      m_dbBetaInt;                    // Probabilit� pour un v�hicule voulant s'ins�rer d'�tre g�n� par les v�hicules en amont sur la voie interne en r�gime fluide
                                                // (uniquement dans le cas d'un giratoire multivoie)
    double      m_dbBetaOut;                    // Probabilit� pour un v�hicule voulant traverser l'anneau externe pour sortir sur la voie de gauche de détecter qu'un véhicule sur l'anneau externe sort avant lui sur la voie de droite, ne gênant pas sa traversée de l'anneau
                                                // (uniquement dans le cas d'un giratoire multivoie)

    std::deque<TuyauMicro*>     m_LstTAmAv;     // Liste des tuyaux amont et aval du giratoire ordonn�s dans le sens du parcours
    std::deque<TuyauMicro*>     m_LstTInt;      // Liste des tuyaux internes du giratoire

    std::deque<TempsCritique>  m_LstTf;            // Liste des temps d'insertion
    std::deque<TempsCritique>  m_LstTt;            // Liste des temps de travers�e    

    std::vector<CoeffsInsertion>    m_LstCoeffsInsertion;   // Liste des coefficients d'insertion d�finis manuellement

    std::vector<GroupeTraverseeEntreeGir> m_LstGrpTraEntree;   // Liste des goupes de travers�es pour l'entr�e sur le giratoire
    std::vector<GroupeTraverseeSortieGir> m_LstGrpTraSortie;   // Liste des goupes de travers�es pour la sortie du giratoire

    // Variables de simulation
public:

    void    AddTuyauAmAv(TuyauMicro* pT){m_LstTAmAv.push_back(pT);};
    void    AddTf(TypeVehicule *pTV, double dbTf, double dbTt);

    void Init();        // Initialisation du giratoire
    void FinInit();     // Fin d'initialisation du giratoire

    char*   GetLabel(){return m_strNom;};    
    double  GetBeta(){return m_dbBeta;};
    double  GetBetaInt(){return m_dbBetaInt;};
    double  GetBetaOut(){return m_dbBetaOut;};

    int     GetNbVoie(){return m_nVoie;};    

    virtual bool    GetTuyauxInternesRestants( TuyauMicro *pTInt, Tuyau *pTAv, std::vector<Tuyau*> &dqTuyaux);
    virtual bool    GetTuyauxInternes( Tuyau *pTAm, Tuyau *pTAv, std::vector<Tuyau*> &dqTuyaux);
	virtual bool    GetTuyauxInternes( Tuyau *pTAm, int nVAm, Tuyau *pTAv, int nVAv, std::vector<Tuyau*> &dqTuyaux);
    virtual std::set<Tuyau*> GetAllTuyauxInternes( Tuyau *pTAm, Tuyau *pTAv );
    const std::deque<TuyauMicro*>& GetTuyauxInternes() {return m_LstTInt;}

    virtual void    CalculTraversee(Vehicule *pVeh, double dbInstant, std::vector<int> & vehiculeIDs);

    // Gestion de l'insertion sur les giratoires � plusieurs voies
    bool	GetCoeffsMouvementsInsertion(Vehicule * pVeh, Voie *pVAm, Tuyau *pTAv, std::map<int, boost::shared_ptr<MouvementsSortie> > &mapCoeffs);
    // Ajout de coefficients manuels (prioritaires sur les coefficients calcul�s par d�faut)
    void    AddCoefficientInsertion(Tuyau * pTuyauEntree, int nVoieEntree, Tuyau * pTuyauSortie, const std::vector<double> & coeffs);

	// calcul particulier du barycentre pour les briques de type Giratoire
	virtual Point*	CalculBarycentre();

    virtual double GetAdditionalPenaltyForUpstreamLink(Tuyau* pTuyauAmont);

    // Fonctions relatives � la simulation des giratoires    

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif
