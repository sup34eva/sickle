#ifndef TOOLS
#define TOOLS

#define declare(TYPE, NAME) private: TYPE m_ ## NAME; // Déclare une variable membre privée

#define setterBase(TYPE, NAME, COMMAND) Q_SLOT TYPE NAME(TYPE val) {m_ ## NAME = val;COMMAND return NAME();}
#define setter(TYPE, NAME) setterBase(TYPE, NAME,) // Déclare un setter pour une variable membre
#define setterSig(TYPE, NAME, SIG) setterBase(TYPE, NAME, emit SIG(NAME());) // Déclare un setter emettant un evenement

#define getter(TYPE, NAME) TYPE NAME() const {return m_ ## NAME;} // Déclare un getter pour une variable membre

#define propBase(TYPE, NAME) declare(TYPE, NAME) public: getter(TYPE, NAME)
#define prop(TYPE, NAME) Q_PROPERTY( TYPE NAME WRITE NAME READ NAME ); propBase(TYPE, NAME); setter(TYPE, NAME) // Déclare une variable membre, avec ses getter / setter
#define propSig(TYPE, NAME, SIG) Q_PROPERTY( TYPE NAME WRITE NAME READ NAME NOTIFY SIG) propBase(TYPE, NAME); setterSig(TYPE, NAME, SIG); Q_SIGNALS: void SIG(TYPE); public: // Déclare une variable membre, avec ses getter / setter et un signal emit a chaque modification
#define propRO(TYPE, NAME) private: Q_PROPERTY(TYPE NAME READ NAME); propBase(TYPE, NAME) // Déclare une variable membre et un getter, mais pas de setter (RO: Read-only)

#endif // TOOLS

