#ifndef FILELOADER_HPP
#define FILELOADER_HPP

#include <QString>
#include <QFile>

class Viewport;

/*! \brief Interface de gestion des fichiers
 *
 * Les classes implémentant cette interface permettent d'utiliser un certain format de fichier.
 */
class FileLoaderInterface {
	public:
		virtual ~FileLoaderInterface() {}
		virtual QString name() = 0;
		virtual QStringList extensions() = 0;
		virtual void save(Viewport*, const QString&) = 0;
		virtual void load(Viewport*, const QString&) = 0;
};

Q_DECLARE_INTERFACE(FileLoaderInterface, "com.sup3asc2.sickle.FileLoaderInterface/1.0")

class DefaultFileLoader : public QObject, public FileLoaderInterface {
	Q_OBJECT
	Q_INTERFACES(FileLoaderInterface)
	public:
		virtual QString name();
		virtual QStringList extensions();
		virtual void save(Viewport*, const QString&);
		virtual void load(Viewport*, const QString&);
};

#endif // FILELOADER_HPP
