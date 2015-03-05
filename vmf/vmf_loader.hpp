#ifndef VMFLOADER_HPP
#define VMFLOADER_HPP

#include <QtPlugin>
#include <fileloader.hpp>

class VMFFileLoader : public QObject, public FileLoaderInterface {
	Q_OBJECT
	Q_INTERFACES(FileLoaderInterface)
	Q_PLUGIN_METADATA(IID "com.sup3asc2.sickle.VMFLoader" FILE "vmf_loader.json")
	public:
		virtual void save(Viewport*, const QString&);
		virtual void load(Viewport*, const QString&);
};

#endif // VMFLOADER_HPP
