#ifndef GLOBALS
#define GLOBALS

/*! \file globals.h
    \brief Definitions globales

    Contient les macros utilitaires globales au projet.
*/

#define declare(TYPE, NAME) private: TYPE m_ ## NAME; //! Déclare une variable membre privée

#define setterBase(TYPE, NAME, COMMAND) Q_SLOT TYPE NAME(TYPE val) {m_ ## NAME = val;COMMAND return NAME();}
#define setter(TYPE, NAME) setterBase(TYPE, NAME,) //! Déclare un setter pour une variable membre
#define setterSig(TYPE, NAME, SIG) setterBase(TYPE, NAME, emit SIG(NAME());) //! Déclare un setter emettant un evenement

#define getter(TYPE, NAME) TYPE NAME() const {return m_ ## NAME;} //! Déclare un getter pour une variable membre

/*! \def prop
 * \brief Declare une propriété
 * Déclare une variable membre avec le type TYPE et le nom NAME, ainsi qu'un getter et un setter du même nom.
 * Cette propriété est déclarée dans le système de meta-objet de Qt.
 */

/*! \def propSig
 * \brief Declare une propriété emettant un signal quand modifiée
 * Fonctionne de manière similaire a prop, mais la classe emettra le signal SIG lorsque le getter est appelé.
 * Le signal prend la nouvelle valeur en paramètre.
 */

#define propBase(TYPE, NAME) declare(TYPE, NAME) public: getter(TYPE, NAME)
#define prop(TYPE, NAME) Q_PROPERTY( TYPE NAME WRITE NAME READ NAME ); propBase(TYPE, NAME); setter(TYPE, NAME)
#define propSig(TYPE, NAME, SIG) Q_PROPERTY( TYPE NAME WRITE NAME READ NAME NOTIFY SIG) propBase(TYPE, NAME); setterSig(TYPE, NAME, SIG); Q_SIGNALS: void SIG(TYPE); public:
#define propRO(TYPE, NAME) private: Q_PROPERTY(TYPE NAME READ NAME); propBase(TYPE, NAME) //! Déclare une variable membre et un getter, mais pas de setter (RO: Read-only)

#endif // GLOBALS

