#include <fileloader.hpp>
#include <viewport.hpp>

QString DefaultFileLoader::name() {
	return tr("Sickle World");
}

QStringList DefaultFileLoader::extensions() {
	return QStringList({
						   "wld"
					   });
}

void DefaultFileLoader::save(Viewport* view, const QString& name) {
	QFile file(name);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out << static_cast<quint32>(FILE_MAGIC);  // Magic number
	out << static_cast<quint32>(FILE_VERSION);  // Sickle version

	auto format = QDataStream::Qt_5_4;  // File format
	out << static_cast<qint32>(format);
	out.setVersion(format);

	// Data
	out << *(view->camera());
	auto childList = view->findChildren<Actor*>();
	out << static_cast<quint32>(childList.size());
	for (auto obj : childList) {
		int type = QMetaType::type(obj->metaObject()->className());
		out << type;
		out << *obj;
	}
}

void DefaultFileLoader::load(Viewport* view, const QString& name) {
	QFile file(name);
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);

	quint32 magic;
	in >> magic;  // Magic number
	if(magic != FILE_MAGIC) {
		qWarning() << "Bad file format";
		return;
	}

	quint32 version;  // Sickle version
	in >> version;
	if(version != FILE_VERSION) {
		qWarning() << "Old file format";
		return;
	}

	qint32 format;  // File format
	in >> format;
	in.setVersion(format);

	view->clearLevel();

	// Data
	in >> *(view->camera());
	quint32 size;
	in >> size;
	view->makeCurrent();
	for (quint32 i = 0; i < size; i++) {
		int id;
		in >> id;
		auto obj = static_cast<Actor*>(QMetaType::create(id));
		qDebug() << "Restoring object of type" << QMetaType::typeName(id);
		in >> *obj;
		obj->setParent(view);
		view->childAdded(obj);
	}
	view->doneCurrent();
}
