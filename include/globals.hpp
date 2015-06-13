// Copyright 2015 PsychoLama

#ifndef GLOBALS
#define GLOBALS

#include <QObject>
#include <QQuaternion>
#include <QVector3D>
#include <QtMath>

/*! \file globals.hpp
 * \brief Definitions globales
 *
 * Contient les macros utilitaires globales au projet.
*/

/*! \class QObject
 */

/*! \class QWidget
 * \extends QObject
 */

/*! \class QMainWindow
 * \extends QWidget
 */

/*! \class QOpenGLWidget
 * \extends QWidget
 */

#define FILE_MAGIC 0xB00B1E5

//! Déclare une variable membre privée
#define declare(TYPE, NAME) \
private:                    \
	TYPE m_##NAME;

#define setterBase(TYPE, NAME, COMMAND) \
	Q_SLOT TYPE NAME(TYPE val) {        \
		m_##NAME = val;                 \
		COMMAND return NAME();          \
	}
//! Déclare un setter pour une variable membre
#define setter(TYPE, NAME) setterBase(TYPE, NAME, )
//! Déclare un setter emettant un evenement
#define setterSig(TYPE, NAME, SIG) setterBase(TYPE, NAME, emit SIG(NAME());)

//! Déclare un getter pour une variable membre
#define getter(TYPE, NAME) \
	TYPE NAME() const { return m_##NAME; }

/*! \def prop
 * \brief Declare une propriété
 *
 * Déclare une variable membre avec le type TYPE et le nom NAME, ainsi qu'un getter et un setter du même nom.
 * Cette propriété est déclarée dans le système de meta-objet de Qt.
 */

/*! \def propSig
 * \brief Declare une propriété emettant un signal quand modifiée
 *
 * Fonctionne de manière similaire a prop, mais la classe emettra le signal SIG lorsque le getter est appelé.
 * Le signal prend la nouvelle valeur en paramètre.
 */

#define propBase(TYPE, NAME) declare(TYPE, NAME) public : getter(TYPE, NAME)
#define prop(TYPE, NAME)                        \
	Q_PROPERTY(TYPE NAME WRITE NAME READ NAME); \
	propBase(TYPE, NAME);                       \
	setter(TYPE, NAME)
#define propSig(TYPE, NAME, SIG)                                                \
	Q_PROPERTY(TYPE NAME WRITE NAME READ NAME NOTIFY SIG) propBase(TYPE, NAME); \
	setterSig(TYPE, NAME, SIG);                                                 \
	Q_SIGNALS:                                                                  \
	void SIG(const TYPE&);                                                             \
																				\
public:
//! Déclare une variable membre et un getter, mais pas de setter (RO: Read-only)
#define propRO(TYPE, NAME)           \
private:                             \
	Q_PROPERTY(TYPE NAME READ NAME); \
	propBase(TYPE, NAME)

QQuaternion fromEuler(const QVector3D& euler);
QVector3D* toEuler(const QQuaternion& quat);

/*! \def noinline
 * \brief Empeche une fonction d'être inlinée
 *
 * Cette macro force une fonction a ne pas être compilée en mode inline lorsque le compileur le permet.
 * Inliner des fonctions de petites taille peut constituer une optimisation, mais il est parfois préférable d'éviter
 * d'inliner des fonctions de grande taille comme Geometry::initProgram pour diminuer la taille du programme.
 */

#ifdef _MSC_VER
	#define noinline __declspec(noinline)
#else
	#ifdef __GNUG__
		#define noinline __attribute__((noinline))
	#else
		#define noinline
	#endif
#endif

#endif  // GLOBALS
