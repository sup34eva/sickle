// Copyright 2015 PsychoLama

#ifndef FILELOADER_HPP
#define FILELOADER_HPP

#include <QString>
#include <QFile>

class Viewport;

/*! \brief Interface de gestion des fichiers
 *
 * Les classes implémentant cette interface permettent d'utiliser un certain format de fichier.
 */
class FileLoader {
	public:
		virtual ~FileLoader() {}
		virtual void save(Viewport*, const QString&) = 0;
		virtual void load(Viewport*, const QString&) = 0;
};

Q_DECLARE_INTERFACE(FileLoader, "com.sup3asc2.sickle.FileLoader/1.0")

/*! \brief Chargeur de fichier par defaut
 *
 * Gestionnaire de fichier pour le format WLD
 */
class DefaultFileLoader : public QObject, public FileLoader {
	Q_OBJECT
	Q_INTERFACES(FileLoader)
	public:
		virtual void save(Viewport*, const QString&);
		virtual void load(Viewport*, const QString&);
};

#endif  // FILELOADER_HPP
