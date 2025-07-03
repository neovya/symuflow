#pragma once
#ifndef SensorsManagerH
#define SensorsManagerH

#include <string>
#include <vector>
#include <map>
#include <deque>

#include <float.h>

class Reseau;
class Tuyau;
class Connexion;
class AbstractSensor;
class PonctualSensor;
class EdieSensor;
class BlueToothSensor;
class LongitudinalSensor;
class Vehicule;
class TypeVehicule;

namespace boost {
    namespace serialization {
        class access;
    }
}


/*===========================================================================================*/
/* Classe de gestion des capteurs                                                            */
/*===========================================================================================*/

class SensorsManager
{
public:
        // Constructeur
        SensorsManager();
        SensorsManager(Reseau *pReseau, double dbPeriodeAgregation, double dbT0, bool bGlobal = false);

        // Destructeur
        ~SensorsManager();
        
private:

    Reseau              *m_pReseau;

    double              m_dbPeriodeAgregation;              // P�riode d'agr�gation des capteurs (en s)   
    double              m_dbT0;                             // Instant de d�marrage de la premi�re p�riode d'acquisition

    int                 m_nPeriode;                         // Num�ro de la p�riode courante

    int                 m_nNbInst;                          // Nombre d'instant d�fini dans les tableaux du nombre de 
                                                            // v�hicule ayant franchi le capteur (il est inf�rieur ou �gal �  m_dbPeriodeAgregation / PasDeTemps

public:
    std::vector<AbstractSensor*>        m_LstCapteurs;                  // Liste des capteurs g�r�s par cette instance de SensorsManager

public:

    PonctualSensor* AddCapteurPonctuel(const char* strNom, Tuyau *Tuyau, double dbPosition);
    int     AddCapteurLongitudinal(char* strNom, Tuyau *Tuyau, double dbPosDebut, double dbPosFin);
    int     AddCapteurMFD(char* strNom, const std::vector<Tuyau*> & Tuyaux, const std::vector<double> & dbPosDebut, const std::vector<double> & dbPosFin,
        const std::vector<int> & eTuyauType, bool bIsMFD);
    int     AddCapteurBlueTooth(char* strNom, Connexion *pNode, const std::map<Tuyau*, double> & mapPositions);
    int     AddCapteurReservoir(char* strNom, const std::vector<Tuyau*> & Tuyaux);

    bool    Init();
    void    Terminate();
    void    CalculInfoCapteurs(double dbInstant);
    void    AddChgtVoie(double  dbInstant, Tuyau * pTuyau, double dbPosition, int nVoieOrigine, int nVoieDestination); // gestion du comptage des changements de voie

    void    AddMesoVehicle(double dbInstant, Vehicule * pVeh, Tuyau * pLink, Tuyau * pDownstreamLink, double dbLengthInLink = DBL_MAX); // Prise en compte d'un v�hicule qui sort d'un troncon meso

    void    UpdateTabNbVeh();   
    double  GetDebitMoyen(PonctualSensor *pCpt, int nVoie);
    double  GetNbVehTotal(PonctualSensor *pCpt, int nVoie);
    double  GetRatioTypeVehicule(PonctualSensor *pCpt, int nVoie, TypeVehicule *pTV);

    AbstractSensor* GetCapteur(int nInd){return m_LstCapteurs[nInd];}

	int		GetPeriode() {return m_nPeriode;}
	void	SetPeriode(int nPeriode) {m_nPeriode = nPeriode;}

    double  GetPeriodeAgregation(){return m_dbPeriodeAgregation;}

private:
	bool CapteurExists(AbstractSensor *pCpt);

public:
	AbstractSensor * GetCapteurFromID(const std::string & sID);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

/*==============================================================================================*/
/* wrapper pour g�rer de fa�on transparente les diff�rents objets SensorsManager correspondants */
/* � chaque type de capteur (evolution n�90)                                                    */
/*==============================================================================================*/
class SensorsManagers {
public:
    SensorsManagers();
    SensorsManagers(Reseau *pReseau, double dbPeriodeAgregation, double dbT0, double dbPeriodeAgregationEdie, double dbT0Edie,
        double dbPeriodeAgregationLong, double dbT0Long, double dbPeriodeAgregationMFD, double dbT0MFD,
        double dbPeriodeAgregationBT, double dbT0BT, double dbT0Reservoir);
    ~SensorsManagers();

    PonctualSensor* AddCapteurPonctuel(char* strNom, Tuyau *Tuyau, double dbPosition);
    int     AddCapteurLongitudinal(char* strNom, Tuyau *Tuyau, double dbPosDebut, double dbPosFin);
    int     AddCapteurMFD(char* strNom, const std::vector<Tuyau*> & Tuyaux, const std::vector<double> & dbPosDebut, const std::vector<double> & dbPosFin,
        const std::vector<int> & eTuyauType, bool bIsMFD);
    int     AddCapteurBlueTooth(char* strNom, Connexion *pNode, const std::map<Tuyau*, double> & mapPositions);
    int     AddCapteurReservoir(char* strNom, const std::vector<Tuyau*> & Tuyaux);
    

    void Init();
    void Terminate();

    void CalculInfoCapteurs(double dbInstSimu);

    void AddMesoVehicle(double dbInstant, Vehicule * pVeh, Tuyau * pLink, Tuyau * pDownstreamLink, double dbLengthInLink = DBL_MAX); // Prise en compte d'un v�hicule qui sort d'un troncon meso

    std::vector<AbstractSensor*>            GetAllSensors();
    std::vector<int>                        GetPeriodes();
    void                                    SetPeriodes(const std::vector<int> & periods);

    // Accesseurs pour les capteurs ponctuels
    double                                  GetPeriodeAgregationPonctuels();
    std::vector<AbstractSensor*>            &GetCapteursPonctuels();
    PonctualSensor*                         GetCapteurPonctuel(size_t i);
    SensorsManager*                         GetGestionCapteursPonctuels();

    // Accesseurs pour les capteurs d'Edie
    std::deque<EdieSensor*>                 GetCapteursEdie();

    // Accesseur pour les capteurs longitudinaux
    std::vector<AbstractSensor*>            &GetCapteursLongitudinaux();
    SensorsManager*                         GetGestionCapteursLongitudinaux();

    // Accesseur pour les capteurs BlueTooth
    SensorsManager*                         GetGestionCapteursBT();

	// Accesseur pour les capteurs Edie
	SensorsManager*                         GetGestionCapteursEdie();

    // Accesseur pour les capteurs MFD
    SensorsManager*                         GetGestionCapteursMFD();

	// Accesseur pour le capteur global
	SensorsManager*							GetGestionCapteurGlobal();

private:
    SensorsManager *m_pGestionCapteursPonctuels;
    SensorsManager *m_pGestionCapteursEdie;
    SensorsManager *m_pGestionCapteursMFD;
    SensorsManager *m_pGestionCapteursReservoir;
    SensorsManager *m_pGestionCapteursLongitudinaux;
    SensorsManager *m_pGestionCapteursBT;
	SensorsManager *m_pGestionCapteurGlobal;

    std::vector<SensorsManager*> m_GestionsCapteurs;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // SensorsManagerH
