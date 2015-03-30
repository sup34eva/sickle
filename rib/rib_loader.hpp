#ifndef VMFLOADER_HPP
#define VMFLOADER_HPP

#include <QtPlugin>
#include <fileloader.hpp>

class RIBFileLoader : public QObject, public FileLoader {
	Q_OBJECT
	Q_INTERFACES(FileLoader)
	Q_PLUGIN_METADATA(IID "com.sup3asc2.sickle.RIBLoader" FILE "rib_loader.json")
	public:
		virtual void save(Viewport*, const QString&);
		virtual void load(Viewport*, const QString&);
};

#endif // VMFLOADER_HPP
