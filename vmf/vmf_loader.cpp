#include <vmf_loader.hpp>
#include <viewport.hpp>

QString VMFFileLoader::name() {
	return tr("Hammer Map");
}

QStringList VMFFileLoader::extensions() {
	return QStringList({
						   "vmf"
					   });
}

void VMFFileLoader::save(Viewport* view, const QString& name) {
	Q_UNUSED(view);
	Q_UNUSED(name);
	// TODO
}

void VMFFileLoader::load(Viewport* view, const QString& name) {
	Q_UNUSED(view);
	Q_UNUSED(name);
	// TODO
}
