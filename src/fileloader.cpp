// Copyright 2015 PsychoLama

#include <fileloader.hpp>
#include <viewport.hpp>

void DefaultFileLoader::save(Viewport* view, const QString& name) {
	QFile file(name);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out << static_cast<quint32>(FILE_MAGIC);  // Magic number
	quint32 revision = staticMetaObject.method(staticMetaObject.indexOfMethod("save")).revision();
	out << revision;  // Sickle version

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

	quint32 revision = staticMetaObject.method(staticMetaObject.indexOfMethod("save")).revision();
	quint32 version;
	in >> version;
	if(version < revision) {
		qWarning() << "Older file format";
		return;
	} else if (version > revision) {
		qWarning() << "Newer file format";
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

QDataStream& operator<<(QDataStream& stream, const QObject& obj) {
	auto metaObject = obj.metaObject();
	quint32 count = metaObject->propertyCount();
	stream << count;
	for (quint32 i = 0; i < count; ++i) {
		auto prop = metaObject->property(i);
		qDebug() << "Saving " << prop.name();
		if (static_cast<QMetaType::Type>(prop.type()) != QMetaType::QObjectStar) {
			stream << QString(prop.name()) << obj.property(prop.name());
		}
	}
	return stream;
}

QDataStream& operator>>(QDataStream& stream, QObject& obj) {
	quint32 count;
	stream >> count;
	for (quint32 i = 0; i < count; ++i) {
		QString name;
		stream >> name;
		qDebug() << "Restoring " << name;
		QVariant value;
		stream >> value;
		obj.setProperty(name.toLatin1().data(), value);
	}
	return stream;
}
